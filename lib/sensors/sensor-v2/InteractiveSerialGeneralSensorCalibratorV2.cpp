#include "InteractiveSerialGeneralSensorCalibratorV2.h"
#include "Arduino.h"

InteractiveSerialGeneralSensorCalibratorV2::InteractiveSerialGeneralSensorCalibratorV2() {
    _serial = NULL;
    _timeout = 10000;
    _readInterval = 1000;
    _lastReadTime = 0;
    _continuousReading = false;
    _state = STATE_IDLE;
    _isFinished = false;
    _eepromAddress = 0;

    _readCallback = NULL;
    _readCallbackWithContext = NULL;
    _callbackContext = NULL;
    _sensorValuePtr = NULL;

    _sensorContext.sensor = nullptr;
    _sensorContext.valueKey = nullptr;

    _calibrationType = CALIBRATION_NONE;
    _isCalibrated = false;
    _inCalibrationMode = false;

    _maxCalibPoints = 5;
    _numCalibPoints = 0;

    _scale = 1.0;
    _slope = 1.0;
    _offset = 0.0;

    _interpolationMethod = 0;
    _polynomialDegree = 2;

    _calibPoints = NULL;
    _polynomialCoeffs = NULL;

    _menuPrompt = "\n===== MENU =====";
    _menuOptions = NULL;
    _numMenuOptions = 0;

    strcpy(_displayUnits, "");
    _displayPrecision = 3;

    _inputMode = INPUT_MODE_NONE;
    _inputBuffer = "";
    _inputComplete = false;
    _inputStartTime = 0;

    _inputDefaultFloat = 0.0f;
    _inputDefaultInt = 0;
    _inputDefaultBool = false;
    _inputDefaultChar = '\0';
    _inputDefaultString = "";

    _activeSensorName = nullptr;
    _activeSensorValueLabel = nullptr;

    _completedCallback = nullptr;
    _completedCallbackContext = nullptr;

    initializeBuffers();
}

InteractiveSerialGeneralSensorCalibratorV2::~InteractiveSerialGeneralSensorCalibratorV2() {
    cleanupMemory();

    if (_activeSensorName) {
        free(_activeSensorName);
        _activeSensorName = nullptr;
    }

    if (_activeSensorValueLabel) {
        free(_activeSensorValueLabel);
        _activeSensorValueLabel = nullptr;
    }
}

void InteractiveSerialGeneralSensorCalibratorV2::begin(Stream *serialPtr, unsigned long timeout, int eepromAddress) {
    _serial = serialPtr;
    _timeout = timeout;
    _eepromAddress = eepromAddress;
    _isFinished = false;

#if defined(ARDUINO_ARCH_ESP8266) || defined(ARDUINO_ARCH_ESP32)
    EEPROM.begin(512);
#endif

    if (_calibPoints == NULL || _polynomialCoeffs == NULL) {
        initializeBuffers();
    }

    _state = STATE_MENU;
    printMenu();
}

bool InteractiveSerialGeneralSensorCalibratorV2::isActive() const {
    return _state != STATE_IDLE;
}

bool InteractiveSerialGeneralSensorCalibratorV2::isFinished() const {
    return _isFinished;
}

void InteractiveSerialGeneralSensorCalibratorV2::setSensorReadCallback(SensorReadCallback callback) {
    _readCallback = callback;
    _readCallbackWithContext = NULL;
    _callbackContext = NULL;
}

void InteractiveSerialGeneralSensorCalibratorV2::setSensorReadCallbackWithContext(SensorReadCallbackWithContext callback, void *context) {
    _readCallbackWithContext = callback;
    _callbackContext = context;
    _readCallback = NULL;
}

void InteractiveSerialGeneralSensorCalibratorV2::setSensorValueContext(BaseSensV2 *sensor, const char *valueKey) {
    _sensorContext.sensor = sensor;
    _sensorContext.valueKey = valueKey;
}

void InteractiveSerialGeneralSensorCalibratorV2::setSensorValuePointer(float *valuePtr) {
    _sensorValuePtr = valuePtr;
}

void InteractiveSerialGeneralSensorCalibratorV2::setSensorInfo(const char *sensorName, const char *valueLabel) {
    if (_activeSensorName) {
        free(_activeSensorName);
    }

    if (_activeSensorValueLabel) {
        free(_activeSensorValueLabel);
    }

    _activeSensorName = sensorName ? strdup(sensorName) : nullptr;
    _activeSensorValueLabel = valueLabel ? strdup(valueLabel) : nullptr;
}

void InteractiveSerialGeneralSensorCalibratorV2::setCompletedCallback(CalibrationCompletedCallback callback, void *callbackContext) {
    _completedCallback = callback;
    _completedCallbackContext = callbackContext;
}

void InteractiveSerialGeneralSensorCalibratorV2::setEEPROMAddress(int address) {
    _eepromAddress = address;
}

int InteractiveSerialGeneralSensorCalibratorV2::getEEPROMAddress() const {
    return _eepromAddress;
}

float InteractiveSerialGeneralSensorCalibratorV2::readRawValue() {
    if (_sensorContext.sensor && _sensorContext.valueKey) {
        return _sensorContext.sensor->getFloatValue(_sensorContext.valueKey);
    } else if (_readCallbackWithContext != NULL) {
        return _readCallbackWithContext(_callbackContext);
    } else if (_readCallback != NULL) {
        return _readCallback();
    } else if (_sensorValuePtr != NULL) {
        return *_sensorValuePtr;
    }
    return 0.0f;
}

float InteractiveSerialGeneralSensorCalibratorV2::readCalibratedValue() {
    float rawValue = readRawValue();

    if (!_isCalibrated) {
        return rawValue;
    }

    switch (_calibrationType) {
        case CALIBRATION_ONE_POINT:
            return rawValue * _scale;

        case CALIBRATION_TWO_POINT:
            return _slope * rawValue + _offset;

        case CALIBRATION_MULTI_POINT:
            if (_interpolationMethod == 0) {
                return linearInterpolate(rawValue);
            } else {
                return polynomialInterpolate(rawValue);
            }

        default:
            return rawValue;
    }
}

void InteractiveSerialGeneralSensorCalibratorV2::setReadInterval(unsigned long interval) {
    _readInterval = interval;
}

void InteractiveSerialGeneralSensorCalibratorV2::enableContinuousReading(bool enable) {
    _continuousReading = enable;
}

void InteractiveSerialGeneralSensorCalibratorV2::update() {
    unsigned long currentTime = millis();

    if (_continuousReading && (currentTime - _lastReadTime >= _readInterval)) {
        readSensor();
        _lastReadTime = currentTime;
    }

    if (_state == STATE_WAITING_FOR_INPUT) {
        processSerialInput();
        return;
    }

    if (_serial && _serial->available() > 0 && _inputMode == INPUT_MODE_NONE) {
        char cmd = _serial->read();
        processCommand(cmd);
    }
}

bool InteractiveSerialGeneralSensorCalibratorV2::calibrateOnePoint(float knownValue, float rawValue) {
    if (_calibPoints == NULL || _polynomialCoeffs == NULL) {
        initializeBuffers();
    }

    if (rawValue < 0) {
        rawValue = readRawValue();
    }

    if (rawValue == 0) {
        rawValue = 0.0001;
    }

    if (knownValue == 0) {
        return false;
    }

    _scale = knownValue / rawValue;
    _calibrationType = CALIBRATION_ONE_POINT;
    _isCalibrated = true;

    clearCalibrationPoints();
    addCalibrationPoint(knownValue, rawValue);

    return true;
}

bool InteractiveSerialGeneralSensorCalibratorV2::calibrateTwoPoint(float knownValue1, float rawValue1, float knownValue2, float rawValue2) {
    if (_calibPoints == NULL || _polynomialCoeffs == NULL) {
        initializeBuffers();
    }

    if (rawValue1 == rawValue2) {
        return false;
    }

    _slope = (knownValue2 - knownValue1) / (rawValue2 - rawValue1);
    _offset = knownValue1 - _slope * rawValue1;
    _calibrationType = CALIBRATION_TWO_POINT;
    _isCalibrated = true;

    clearCalibrationPoints();
    addCalibrationPoint(knownValue1, rawValue1);
    addCalibrationPoint(knownValue2, rawValue2);

    return true;
}

void InteractiveSerialGeneralSensorCalibratorV2::setMaxCalibrationPoints(uint8_t maxPoints) {
    if (maxPoints > 0 && maxPoints != _maxCalibPoints) {
        _maxCalibPoints = maxPoints;
        initializeBuffers();
    }
}

void InteractiveSerialGeneralSensorCalibratorV2::startCalibration(uint8_t calibrationType) {
    if (_calibPoints == NULL || _polynomialCoeffs == NULL) {
        initializeBuffers();
    }

    _inCalibrationMode = true;
    _calibrationType = calibrationType;
    clearCalibrationPoints();
}

bool InteractiveSerialGeneralSensorCalibratorV2::addCalibrationPoint(float knownValue, float rawValue) {
    if (_calibPoints == NULL) {
        initializeBuffers();
    }

    if (_numCalibPoints >= _maxCalibPoints) {
        return false;
    }

    if (rawValue < 0) {
        rawValue = readRawValue();
    }

    _calibPoints[_numCalibPoints].rawValue = rawValue;
    _calibPoints[_numCalibPoints].knownValue = knownValue;
    _numCalibPoints++;

    return true;
}

bool InteractiveSerialGeneralSensorCalibratorV2::calculateCalibration() {
    if (_calibPoints == NULL || _polynomialCoeffs == NULL) {
        initializeBuffers();
    }

    if (_numCalibPoints < 1) {
        return false;
    }

    sortCalibrationPoints();

    if (_numCalibPoints == 1) {
        _scale = _calibPoints[0].knownValue / _calibPoints[0].rawValue;
        _calibrationType = CALIBRATION_ONE_POINT;
        _isCalibrated = true;
        _inCalibrationMode = false;
        return true;
    } else if (_numCalibPoints == 2) {
        float rawValue1 = _calibPoints[0].rawValue;
        float knownValue1 = _calibPoints[0].knownValue;
        float rawValue2 = _calibPoints[1].rawValue;
        float knownValue2 = _calibPoints[1].knownValue;

        _slope = (knownValue2 - knownValue1) / (rawValue2 - rawValue1);
        _offset = knownValue1 - _slope * rawValue1;
        _calibrationType = CALIBRATION_TWO_POINT;
        _isCalibrated = true;
        _inCalibrationMode = false;
        return true;
    } else {
        _calibrationType = CALIBRATION_MULTI_POINT;

        if (_interpolationMethod == 0) {
            _isCalibrated = true;
            _inCalibrationMode = false;
            return true;
        } else {
            bool success = calculatePolynomialRegression();
            if (success) {
                _isCalibrated = true;
                _inCalibrationMode = false;
            }
            return success;
        }
    }
}

void InteractiveSerialGeneralSensorCalibratorV2::cancelCalibration() {
    _inCalibrationMode = false;
}

bool InteractiveSerialGeneralSensorCalibratorV2::clearCalibrationPoints() {
    if (_calibPoints == NULL) {
        initializeBuffers();
        return false;
    }

    for (uint8_t i = 0; i < _maxCalibPoints; i++) {
        _calibPoints[i].rawValue = 0.0;
        _calibPoints[i].knownValue = 0.0;
    }
    _numCalibPoints = 0;
    return true;
}

bool InteractiveSerialGeneralSensorCalibratorV2::isCalibrated() {
    return _isCalibrated;
}

uint8_t InteractiveSerialGeneralSensorCalibratorV2::getCalibrationMethod() {
    return _calibrationType;
}

uint8_t InteractiveSerialGeneralSensorCalibratorV2::getCalibrationPointCount() {
    return _numCalibPoints;
}

void InteractiveSerialGeneralSensorCalibratorV2::setInterpolationMethod(uint8_t method) {
    _interpolationMethod = method;
}

void InteractiveSerialGeneralSensorCalibratorV2::setPolynomialDegree(uint8_t degree) {
    if (_polynomialCoeffs == NULL) {
        initializeBuffers();
    }

    if (degree < _maxCalibPoints - 1) {
        _polynomialDegree = degree;
    }
}

bool InteractiveSerialGeneralSensorCalibratorV2::processCommand(char cmd) {
    if (_state == STATE_WAITING_FOR_INPUT || _inputMode != INPUT_MODE_NONE) {
        return false;
    }

    switch (cmd) {
        case '1':
            startReading();
            return true;
        case '2':
            runOnePointCalibration();
            return true;
        case '3':
            runTwoPointCalibration();
            return true;
        case '4':
            runMultiPointCalibration();
            return true;
        case '5':
            showCalibrationDetails();
            return true;
        case '6':
            runClearCalibration();
            return true;
        case '7':
            showStatistics();
            return true;
        case '8':
            drawCalibrationCurve();
            return true;
        case '9': {
            uint8_t profileNum = readInt("Enter profile number (0-9): ", 0);
            saveCalibrationProfile(profileNum);
            return true;
        }
        case '0': {
            if (_completedCallback) {
                notifyCalibrationCompleted();
            } else {
                uint8_t profileNum = readInt("Enter profile number to load (0-9): ", 0);
                loadCalibrationProfile(profileNum);
            }
            return true;
        }
        case 'l':
        case 'L':
            listCalibrationProfiles();
            return true;
        case 's':
        case 'S':
            _state = STATE_SETTINGS;
            _serial->println("\n===== SETTINGS =====");
            _serial->println("1: Set Display Units");
            _serial->println("2: Set Display Precision");
            _serial->println("3: Save Preferences");
            _serial->println("4: Load Preferences");
            _serial->println("5: Reset to Defaults");
            _serial->println("0: Back to Main Menu");
            return true;
        case '?':
            printMenu();
            return true;
        default:
            if (_state == STATE_SETTINGS) {
                switch (cmd) {
                    case '1': {
                        String units = readString("Enter display units (max 7 chars): ", "");
                        setDisplayUnits(units.c_str());
                        return true;
                    }
                    case '2': {
                        uint8_t precision = readInt("Enter display precision (0-6): ", 3);
                        setDisplayPrecision(precision > 6 ? 6 : precision);
                        return true;
                    }
                    case '3':
                        savePreferences();
                        _serial->println("Preferences saved");
                        return true;
                    case '4':
                        loadPreferences();
                        _serial->println("Preferences loaded");
                        return true;
                    case '5':
                        resetToDefaults();
                        _serial->println("Reset to defaults");
                        return true;
                    case '0':
                        _state = STATE_MENU;
                        printMenu();
                        return true;
                }
            }
            if (_state == STATE_READING) {
                stopReading();
                printMenu();
                return true;
            }
            return false;
    }
}

void InteractiveSerialGeneralSensorCalibratorV2::printMenu() {
    _serial->println(_menuPrompt);

    if (_menuOptions && _numMenuOptions > 0) {
        for (uint8_t i = 0; i < _numMenuOptions; i++) {
            _serial->println(_menuOptions[i]);
        }
    } else {
        _serial->println("1: Read Sensor Value");
        _serial->println("2: One-Point Calibration");
        _serial->println("3: Two-Point Calibration");
        _serial->println("4: Multi-Point Calibration");
        _serial->println("5: Show Calibration Details");
        _serial->println("6: Clear Calibration");
        _serial->println("7: Show Statistics");
        _serial->println("8: Draw Calibration Curve");

        if (_completedCallback) {
            _serial->println("0: Back to Sensor Selection");
        } else {
            _serial->println("9: Save Calibration Profile");
            _serial->println("0: Load Calibration Profile");
            _serial->println("L: List Calibration Profiles");
            _serial->println("S: Settings");
        }

        _serial->println("?: Show This Menu");
    }

    _serial->println("================");

    if (_activeSensorName && _activeSensorValueLabel) {
        _serial->print("Sensor: ");
        _serial->print(_activeSensorName);
        _serial->print(" - ");
        _serial->println(_activeSensorValueLabel);
    }
}

void InteractiveSerialGeneralSensorCalibratorV2::setMenuPrompt(const char *prompt) {
    _menuPrompt = prompt;
}

void InteractiveSerialGeneralSensorCalibratorV2::setMenuOptions(const char *options[], uint8_t numOptions) {
    _menuOptions = options;
    _numMenuOptions = numOptions;
}

void InteractiveSerialGeneralSensorCalibratorV2::startReading() {
    _state = STATE_READING;
    _serial->println("\n===== CONTINUOUS SENSOR READINGS =====");
    _serial->println("Press any key to stop...");
    delay(1000);

    clearSerialBuffer();

    printReadingHeader();
    _lastReadTime = millis();
    _continuousReading = true;
}

void InteractiveSerialGeneralSensorCalibratorV2::stopReading() {
    _continuousReading = false;
    _serial->println("\nSensor reading stopped");
    _state = STATE_MENU;
    printMenu();
}

void InteractiveSerialGeneralSensorCalibratorV2::readSensor() {
    float raw = readRawValue();
    float calibrated = readCalibratedValue();

    _serial->print("Raw: ");
    _serial->print(raw, _displayPrecision);
    _serial->print(" | Calibrated: ");
    _serial->print(calibrated, _displayPrecision);

    if (strlen(_displayUnits) > 0) {
        _serial->print(" ");
        _serial->print(_displayUnits);
    }

    _serial->println();
}

void InteractiveSerialGeneralSensorCalibratorV2::runOnePointCalibration() {
    CalibrationStateV2 previousState = _state;
    _state = STATE_ONE_POINT_CAL;
    _serial->println("\n===== ONE-POINT CALIBRATION =====");
    _serial->println("This will calibrate the sensor using a single reference point");

    float rawValue = collectStableMeasurement();

    _serial->print("Current raw reading: ");
    _serial->print(rawValue, _displayPrecision);
    _serial->println();

    float knownValue = 0.0f;

    if (_serial) {
        clearSerialBuffer();
        _serial->print("Enter the known value for this reading: ");

        while (!_serial->available()) {
            delay(10);
        }

        String input = _serial->readStringUntil('\n');
        if (input.length() > 0) {
            knownValue = input.toFloat();
        }

        _serial->println(knownValue);
    }

    if (calibrateOnePoint(knownValue, rawValue)) {
        _serial->println("Calibration successful!");
        saveCalibration();
        showCalibrationDetails();
    } else {
        _serial->println("Calibration failed. Check your input values.");
    }

    _state = STATE_MENU;
    printMenu();
}

void InteractiveSerialGeneralSensorCalibratorV2::runTwoPointCalibration() {
    CalibrationStateV2 previousState = _state;
    _state = STATE_TWO_POINT_CAL;
    _serial->println("\n===== TWO-POINT CALIBRATION =====");
    _serial->println("This will calibrate using two reference points");

    _serial->println("\nSTEP 1: Set up your first reference point");
    _serial->println("Press Enter when ready...");

    clearSerialBuffer();
    waitForSerialInput();
    clearSerialBuffer();

    float rawValue1 = collectStableMeasurement();

    _serial->print("First point raw reading: ");
    _serial->print(rawValue1, _displayPrecision);
    _serial->println();

    float knownValue1 = 0.0f;

    if (_serial) {
        clearSerialBuffer();
        _serial->print("Enter the known value for this point: ");

        while (!_serial->available()) {
            delay(10);
        }

        String input = _serial->readStringUntil('\n');
        if (input.length() > 0) {
            knownValue1 = input.toFloat();
        }

        _serial->println(knownValue1);
    }

    _serial->println("\nSTEP 2: Set up your second reference point");
    _serial->println("Press Enter when ready...");

    clearSerialBuffer();
    waitForSerialInput();
    clearSerialBuffer();

    float rawValue2 = collectStableMeasurement();

    _serial->print("Second point raw reading: ");
    _serial->print(rawValue2, _displayPrecision);
    _serial->println();

    float knownValue2 = 0.0f;

    if (_serial) {
        clearSerialBuffer();
        _serial->print("Enter the known value for this point: ");

        while (!_serial->available()) {
            delay(10);
        }

        String input = _serial->readStringUntil('\n');
        if (input.length() > 0) {
            knownValue2 = input.toFloat();
        }

        _serial->println(knownValue2);
    }

    if (calibrateTwoPoint(knownValue1, rawValue1, knownValue2, rawValue2)) {
        _serial->println("Two-point calibration successful!");
        saveCalibration();
        showCalibrationDetails();
    } else {
        _serial->println("Calibration failed. The two points may be too close.");
    }

    _state = STATE_MENU;
    printMenu();
}

void InteractiveSerialGeneralSensorCalibratorV2::runMultiPointCalibration() {
    CalibrationStateV2 previousState = _state;
    _state = STATE_MULTI_POINT_CAL;
    _serial->println("\n===== MULTI-POINT CALIBRATION =====");

    int numPoints = 0;

    if (_serial) {
        clearSerialBuffer();
        _serial->print("How many calibration points (2-10)? ");

        while (!_serial->available()) {
            delay(10);
        }

        String input = _serial->readStringUntil('\n');
        if (input.length() > 0) {
            numPoints = input.toInt();
        } else {
            numPoints = 3;
        }

        _serial->println(numPoints);
    } else {
        numPoints = 3;
    }

    if (numPoints < 2 || numPoints > 10) {
        _serial->println("Invalid number of points. Use 2-10 points.");
        _state = STATE_MENU;
        printMenu();
        return;
    }

    setMaxCalibrationPoints(numPoints);
    startCalibration(CALIBRATION_MULTI_POINT);

    for (int i = 0; i < numPoints; i++) {
        _serial->print("\nPoint ");
        _serial->print(i + 1);
        _serial->print("/");
        _serial->print(numPoints);
        _serial->println(":");

        _serial->println("Set up your reference point");
        _serial->println("Press Enter when ready...");

        clearSerialBuffer();
        waitForSerialInput();
        clearSerialBuffer();

        float rawValue = collectStableMeasurement();

        _serial->print("Raw value: ");
        _serial->print(rawValue, _displayPrecision);
        _serial->println();

        float knownValue = 0.0f;

        if (_serial) {
            clearSerialBuffer();
            _serial->print("Enter the known value for this point: ");

            while (!_serial->available()) {
                delay(10);
            }

            String input = _serial->readStringUntil('\n');
            if (input.length() > 0) {
                knownValue = input.toFloat();
            }

            _serial->println(knownValue);
        }

        addCalibrationPoint(knownValue, rawValue);
    }

    _serial->println("\nSelect interpolation method:");
    _serial->println("1: Linear interpolation between points");
    _serial->println("2: Polynomial curve fitting");

    int method = 1;

    if (_serial) {
        clearSerialBuffer();
        _serial->print("");

        while (!_serial->available()) {
            delay(10);
        }

        String input = _serial->readStringUntil('\n');
        if (input.length() > 0) {
            method = input.toInt();
        }

        _serial->println(method);
    }

    if (method == 2) {
        setInterpolationMethod(1);

        int maxDegree = min(numPoints - 1, 3);
        _serial->print("Enter polynomial degree (1-");
        _serial->print(maxDegree);
        _serial->println("):");

        int degree = 2;

        if (_serial) {
            clearSerialBuffer();
            _serial->print("");

            while (!_serial->available()) {
                delay(10);
            }

            String input = _serial->readStringUntil('\n');
            if (input.length() > 0) {
                degree = input.toInt();
            }

            _serial->println(degree);
        }

        if (degree >= 1 && degree <= maxDegree) {
            setPolynomialDegree(degree);
        } else {
            _serial->println("Invalid degree, using degree 2");
            setPolynomialDegree(2);
        }
    } else {
        setInterpolationMethod(0);
    }

    if (calculateCalibration()) {
        _serial->println("Multi-point calibration successful!");
        saveCalibration();
        showCalibrationDetails();
    } else {
        _serial->println("Calibration failed");
    }

    _state = STATE_MENU;
    printMenu();
}

void InteractiveSerialGeneralSensorCalibratorV2::showCalibrationDetails() {
    CalibrationStateV2 previousState = _state;
    _state = STATE_CALIBRATION_DETAILS;
    printCalibrationDetails();
    _state = STATE_MENU;
    printMenu();
}

void InteractiveSerialGeneralSensorCalibratorV2::runClearCalibration() {
    CalibrationStateV2 previousState = _state;

    bool confirm = false;

    _serial->println("\nAre you sure you want to clear calibration data?");

    if (_serial) {
        clearSerialBuffer();
        _serial->print("Clear calibration (y/n)? ");

        while (!_serial->available()) {
            delay(10);
        }

        char c = _serial->read();
        clearSerialBuffer();

        confirm = (c == 'y' || c == 'Y');
        _serial->println(confirm ? "y" : "n");
    }

    if (confirm) {
        _serial->println("Clearing calibration data...");
        clearCalibrationPoints();
        _isCalibrated = false;
        _serial->println("Calibration data cleared");
    } else {
        _serial->println("Operation canceled");
    }

    _state = STATE_MENU;
    printMenu();
}

void InteractiveSerialGeneralSensorCalibratorV2::beginInputMode(InputModeType mode, const char *prompt) {
    _inputMode = mode;
    _inputBuffer = "";
    _inputComplete = false;
    _inputStartTime = millis();
    _state = STATE_WAITING_FOR_INPUT;

    if (_serial && prompt) {
        _serial->print(prompt);
    }
}

void InteractiveSerialGeneralSensorCalibratorV2::processSerialInput() {
    if (_inputMode == INPUT_MODE_NONE || !_serial) {
        return;
    }

    if (_inputComplete) {
        completeInputMode();
        return;
    }

    if (isSerialTimedOut(_inputStartTime)) {
        completeInputMode();
        return;
    }

    while (_serial && _serial->available() > 0) {
        char c = _serial->read();

        if (c == '\n' || c == '\r') {
            _inputComplete = true;
            break;
        } else if (c == 8 || c == 127) {
            if (_inputBuffer.length() > 0) {
                _inputBuffer.remove(_inputBuffer.length() - 1);
                _serial->write(8);
                _serial->write(' ');
                _serial->write(8);
            }
        } else {
            _inputBuffer += c;
            _serial->write(c);
        }
    }

    if (_inputComplete) {
        completeInputMode();
    }
}

void InteractiveSerialGeneralSensorCalibratorV2::completeInputMode() {
    if (!_serial) {
        _inputMode = INPUT_MODE_NONE;
        _state = STATE_MENU;
        return;
    }

    _serial->println();

    switch (_inputMode) {
        case INPUT_MODE_FLOAT: {
            float value = _inputDefaultFloat;
            if (_inputBuffer.length() > 0) {
                value = _inputBuffer.toFloat();
            }
            *((float *) _inputResultPtr) = value;
            break;
        }
        case INPUT_MODE_INT: {
            int value = _inputDefaultInt;
            if (_inputBuffer.length() > 0) {
                value = _inputBuffer.toInt();
            }
            *((int *) _inputResultPtr) = value;
            break;
        }
        case INPUT_MODE_CHAR: {
            char value = _inputDefaultChar;
            if (_inputBuffer.length() > 0) {
                value = _inputBuffer.charAt(0);
            }
            *((char *) _inputResultPtr) = value;
            break;
        }
        case INPUT_MODE_YES_NO: {
            bool value = _inputDefaultBool;
            if (_inputBuffer.length() > 0) {
                char c = _inputBuffer.charAt(0);
                value = (c == 'Y' || c == 'y');
            }
            *((bool *) _inputResultPtr) = value;
            break;
        }
        case INPUT_MODE_STRING: {
            String *strPtr = (String *) _inputResultPtr;
            if (_inputBuffer.length() > 0) {
                *strPtr = _inputBuffer;
            } else {
                *strPtr = _inputDefaultString;
            }
            break;
        }
        default:
            break;
    }

    _inputMode = INPUT_MODE_NONE;
    _state = STATE_MENU;
}

float InteractiveSerialGeneralSensorCalibratorV2::readFloat(const char *prompt, float defaultValue, unsigned long timeout) {
    if (!_serial) return defaultValue;

    float result = defaultValue;

    clearSerialBuffer();
    _serial->print(prompt);

    unsigned long startTime = millis();
    while (!_serial->available()) {
        if (timeout > 0 && (millis() - startTime) > timeout) {
            _serial->println(defaultValue);
            return defaultValue;
        }
        delay(10);
    }

    String input = _serial->readStringUntil('\n');
    if (input.length() > 0) {
        result = input.toFloat();
    }

    _serial->println(result);
    return result;
}

int InteractiveSerialGeneralSensorCalibratorV2::readInt(const char *prompt, int defaultValue, unsigned long timeout) {
    if (!_serial) return defaultValue;

    int result = defaultValue;

    clearSerialBuffer();
    _serial->print(prompt);

    unsigned long startTime = millis();
    while (!_serial->available()) {
        if (timeout > 0 && (millis() - startTime) > timeout) {
            _serial->println(defaultValue);
            return defaultValue;
        }
        delay(10);
    }

    String input = _serial->readStringUntil('\n');
    if (input.length() > 0) {
        result = input.toInt();
    }

    _serial->println(result);
    return result;
}

bool InteractiveSerialGeneralSensorCalibratorV2::readYesNo(const char *prompt, bool defaultValue, unsigned long timeout) {
    if (!_serial) return defaultValue;

    bool result = defaultValue;

    clearSerialBuffer();
    _serial->print(prompt);

    unsigned long startTime = millis();
    while (!_serial->available()) {
        if (timeout > 0 && (millis() - startTime) > timeout) {
            _serial->println(defaultValue ? "y" : "n");
            return defaultValue;
        }
        delay(10);
    }

    char c = _serial->read();
    clearSerialBuffer();

    result = (c == 'Y' || c == 'y');
    _serial->println(result ? "y" : "n");

    return result;
}

char InteractiveSerialGeneralSensorCalibratorV2::readChar(const char *prompt, char defaultValue, unsigned long timeout) {
    if (!_serial) return defaultValue;

    char result = defaultValue;

    clearSerialBuffer();
    _serial->print(prompt);

    unsigned long startTime = millis();
    while (!_serial->available()) {
        if (timeout > 0 && (millis() - startTime) > timeout) {
            _serial->println(defaultValue);
            return defaultValue;
        }
        delay(10);
    }

    result = _serial->read();
    clearSerialBuffer();

    _serial->println(result);
    return result;
}

String InteractiveSerialGeneralSensorCalibratorV2::readString(const char *prompt, const char *defaultValue, unsigned long timeout) {
    if (!_serial) return String(defaultValue);

    String result = defaultValue;

    clearSerialBuffer();
    _serial->print(prompt);

    unsigned long startTime = millis();
    while (!_serial->available()) {
        if (timeout > 0 && (millis() - startTime) > timeout) {
            _serial->println(defaultValue);
            return defaultValue;
        }
        delay(10);
    }

    result = _serial->readStringUntil('\n');
    _serial->println(result);

    return result;
}

bool InteractiveSerialGeneralSensorCalibratorV2::saveCalibration(int eepromAddress) {
    if (!_isCalibrated) {
        return false;
    }

    int addr = (eepromAddress >= 0) ? eepromAddress : _eepromAddress;

    EEPROM.write(addr++, _calibrationType);

    switch (_calibrationType) {
        case CALIBRATION_ONE_POINT:
            EEPROM.put(addr, _scale);
            addr += sizeof(float);
            break;

        case CALIBRATION_TWO_POINT:
            EEPROM.put(addr, _slope);
            addr += sizeof(float);
            EEPROM.put(addr, _offset);
            addr += sizeof(float);
            break;

        case CALIBRATION_MULTI_POINT:
            EEPROM.write(addr++, _numCalibPoints);
            EEPROM.write(addr++, _interpolationMethod);

            if (_interpolationMethod == 1) {
                EEPROM.write(addr++, _polynomialDegree);
                for (uint8_t i = 0; i <= _polynomialDegree; i++) {
                    EEPROM.put(addr, _polynomialCoeffs[i]);
                    addr += sizeof(float);
                }
            } else {
                for (uint8_t i = 0; i < _numCalibPoints; i++) {
                    EEPROM.put(addr, _calibPoints[i].rawValue);
                    addr += sizeof(float);
                    EEPROM.put(addr, _calibPoints[i].knownValue);
                    addr += sizeof(float);
                }
            }
            break;
    }

#if defined(ARDUINO_ARCH_ESP8266) || defined(ARDUINO_ARCH_ESP32)
    return EEPROM.commit();
#else
    return true;
#endif
}

bool InteractiveSerialGeneralSensorCalibratorV2::loadCalibration(int eepromAddress) {
    if (_calibPoints == NULL || _polynomialCoeffs == NULL) {
        initializeBuffers();
    }

    int addr = (eepromAddress >= 0) ? eepromAddress : _eepromAddress;
    uint8_t numPoints;
    float rawValue, knownValue;

    uint8_t calType = EEPROM.read(addr++);

    if (calType > CALIBRATION_MULTI_POINT) {
        return false;
    }

    _calibrationType = calType;

    switch (_calibrationType) {
        case CALIBRATION_ONE_POINT:
            EEPROM.get(addr, _scale);
            addr += sizeof(float);

            clearCalibrationPoints();

            if (_scale != 0) {
                addCalibrationPoint(1.0, 1.0 / _scale);
                _isCalibrated = true;
            } else {
                _isCalibrated = false;
            }
            break;

        case CALIBRATION_TWO_POINT:
            EEPROM.get(addr, _slope);
            addr += sizeof(float);
            EEPROM.get(addr, _offset);
            addr += sizeof(float);

            clearCalibrationPoints();
            addCalibrationPoint(_offset, 0);
            addCalibrationPoint(_slope + _offset, 1);

            _isCalibrated = true;
            break;

        case CALIBRATION_MULTI_POINT: {
            numPoints = EEPROM.read(addr++);
            _interpolationMethod = EEPROM.read(addr++);

            if (numPoints == 0 || numPoints > _maxCalibPoints) {
                return false;
            }

            if (_interpolationMethod == 1) {
                _polynomialDegree = EEPROM.read(addr++);

                if (_polynomialDegree >= _maxCalibPoints) {
                    return false;
                }

                for (uint8_t i = 0; i <= _polynomialDegree; i++) {
                    EEPROM.get(addr, _polynomialCoeffs[i]);
                    addr += sizeof(float);
                }
            } else {
                clearCalibrationPoints();

                for (uint8_t i = 0; i < numPoints; i++) {
                    EEPROM.get(addr, rawValue);
                    addr += sizeof(float);
                    EEPROM.get(addr, knownValue);
                    addr += sizeof(float);

                    addCalibrationPoint(knownValue, rawValue);
                }
            }

            _isCalibrated = true;
        }
            break;

        default:
            _isCalibrated = false;
            break;
    }

    return _isCalibrated;
}

bool InteractiveSerialGeneralSensorCalibratorV2::saveCalibrationProfile(uint8_t profileNumber) {
    if (profileNumber > 9) {
        _serial->println("Invalid profile number. Use 0-9.");
        return false;
    }

    int eepromAddr = getEepromProfileAddress(profileNumber);
    bool result = saveCalibration(eepromAddr);

    if (result) {
        _serial->print("Calibration saved to profile ");
        _serial->println(profileNumber);
    } else {
        _serial->println("Failed to save calibration profile");
    }

    return result;
}

bool InteractiveSerialGeneralSensorCalibratorV2::loadCalibrationProfile(uint8_t profileNumber) {
    if (profileNumber > 9) {
        _serial->println("Invalid profile number. Use 0-9.");
        return false;
    }

    int eepromAddr = getEepromProfileAddress(profileNumber);
    bool result = loadCalibration(eepromAddr);

    if (result) {
        _serial->print("Calibration loaded from profile ");
        _serial->println(profileNumber);
        showCalibrationDetails();
    } else {
        _serial->println("Failed to load calibration profile (may not exist)");
    }

    return result;
}

void InteractiveSerialGeneralSensorCalibratorV2::listCalibrationProfiles() {
    _serial->println("\n===== CALIBRATION PROFILES =====");

    for (uint8_t i = 0; i < 10; i++) {
        int eepromAddr = getEepromProfileAddress(i);

        _serial->print("Profile ");
        _serial->print(i);
        _serial->print(": ");

        uint8_t calType = EEPROM.read(eepromAddr);

        if (calType <= CALIBRATION_MULTI_POINT) {
            _serial->print("Valid - ");

            switch (calType) {
                case CALIBRATION_NONE:
                    _serial->println("No Calibration");
                    break;
                case CALIBRATION_ONE_POINT:
                    _serial->println("One-Point Calibration");
                    break;
                case CALIBRATION_TWO_POINT:
                    _serial->println("Two-Point Calibration");
                    break;
                case CALIBRATION_MULTI_POINT:
                    _serial->println("Multi-Point Calibration");
                    break;
            }
        } else {
            _serial->println("Empty");
        }
    }
}

void InteractiveSerialGeneralSensorCalibratorV2::showStatistics(uint16_t numSamples) {
    CalibrationStateV2 previousState = _state;
    _serial->println("\n===== SENSOR STATISTICS =====");
    _serial->print("Collecting data for ");
    _serial->print(numSamples);
    _serial->println(" samples...");

    printStatsHeader();

    float sum = 0;
    float sumSq = 0;
    float min = 9999999;
    float max = -9999999;
    float current = 0;

    for (uint16_t i = 0; i < numSamples; i++) {
        current = readCalibratedValue();

        sum += current;
        sumSq += current * current;

        if (current < min) min = current;
        if (current > max) max = current;

        if (i % 10 == 0) {
            _serial->print("Sample ");
            _serial->print(i);
            _serial->print(": Value = ");
            _serial->print(current, _displayPrecision);

            if (strlen(_displayUnits) > 0) {
                _serial->print(" ");
                _serial->print(_displayUnits);
            }

            _serial->println();
        }

        delay(10);
    }

    float average = sum / numSamples;
    float variance = (sumSq - (sum * sum / numSamples)) / (numSamples - 1);
    float stdDev = sqrt(variance);
    float noise = stdDev / average * 100.0;

    _serial->println("\n===== RESULTS =====");
    _serial->print("Min: ");
    _serial->print(min, _displayPrecision);
    _serial->print(" | Max: ");
    _serial->print(max, _displayPrecision);
    _serial->print(" | Range: ");
    _serial->println(max - min, _displayPrecision);

    _serial->print("Average: ");
    _serial->print(average, _displayPrecision);
    _serial->print(" | Std Dev: ");
    _serial->print(stdDev, _displayPrecision);
    _serial->print(" | Noise: ");
    _serial->print(noise, 2);
    _serial->println("%");

    _state = STATE_MENU;
    printMenu();
}

void InteractiveSerialGeneralSensorCalibratorV2::drawCalibrationCurve(uint8_t width, uint8_t height) {
    CalibrationStateV2 previousState = _state;

    if (!isCalibrated() || getCalibrationPointCount() < 2) {
        _serial->println("Not enough calibration points to draw curve");
        return;
    }

    _serial->println("\n===== CALIBRATION CURVE =====");

    float minRaw = _calibPoints[0].rawValue;
    float maxRaw = _calibPoints[getCalibrationPointCount() - 1].rawValue;

    float minValue = 999999;
    float maxValue = -999999;

    for (uint8_t i = 0; i < getCalibrationPointCount(); i++) {
        if (_calibPoints[i].knownValue < minValue) minValue = _calibPoints[i].knownValue;
        if (_calibPoints[i].knownValue > maxValue) maxValue = _calibPoints[i].knownValue;
    }

    _serial->print("Raw range: ");
    _serial->print(minRaw, _displayPrecision);
    _serial->print(" - ");
    _serial->println(maxRaw, _displayPrecision);

    _serial->print("Value range: ");
    _serial->print(minValue, _displayPrecision);

    if (strlen(_displayUnits) > 0) {
        _serial->print(" ");
        _serial->print(_displayUnits);
    }

    _serial->print(" - ");
    _serial->print(maxValue, _displayPrecision);

    if (strlen(_displayUnits) > 0) {
        _serial->print(" ");
        _serial->print(_displayUnits);
    }

    _serial->println();

    _serial->println("\n      Curve Visualization");
    _serial->print(maxValue, 1);

    if (strlen(_displayUnits) > 0) {
        _serial->print(_displayUnits);
    }

    _serial->print(" |");

    float rawStep = (maxRaw - minRaw) / width;
    float valueRange = maxValue - minValue;

    for (uint8_t x = 0; x < width; x++) {
        float rawValue = minRaw + x * rawStep;
        float value = 0;

        switch (_calibrationType) {
            case CALIBRATION_ONE_POINT:
                value = rawValue * _scale;
                break;
            case CALIBRATION_TWO_POINT:
                value = _slope * rawValue + _offset;
                break;
            case CALIBRATION_MULTI_POINT:
                if (_interpolationMethod == 0) {
                    value = linearInterpolate(rawValue);
                } else {
                    value = polynomialInterpolate(rawValue);
                }
                break;
        }

        int y = (int) ((value - minValue) / valueRange * height);

        for (int i = 0; i < height; i++) {
            if (i == height - y - 1) {
                _serial->print("*");
            } else {
                _serial->print(" ");
            }
        }
    }

    _serial->println("|");

    _serial->print(minValue, 1);

    if (strlen(_displayUnits) > 0) {
        _serial->print(_displayUnits);
    }

    _serial->print(" |");

    for (uint8_t i = 0; i < width; i++) {
        _serial->print("-");
    }

    _serial->println("|");

    _serial->print("      ");
    _serial->print(minRaw, 1);

    for (uint8_t i = 0; i < width - 8; i++) {
        _serial->print(" ");
    }

    _serial->print(maxRaw, 1);
    _serial->println();

    _state = STATE_MENU;
    printMenu();
}

void InteractiveSerialGeneralSensorCalibratorV2::setDisplayUnits(const char *units) {
    strncpy(_displayUnits, units, 7);
    _displayUnits[7] = '\0';
}

void InteractiveSerialGeneralSensorCalibratorV2::setDisplayPrecision(uint8_t precision) {
    _displayPrecision = precision > 6 ? 6 : precision;
}

const char *InteractiveSerialGeneralSensorCalibratorV2::getDisplayUnits() const {
    return _displayUnits;
}

uint8_t InteractiveSerialGeneralSensorCalibratorV2::getDisplayPrecision() const {
    return _displayPrecision;
}

bool InteractiveSerialGeneralSensorCalibratorV2::loadPreferences(int eepromAddress) {
    uint8_t validMarker = EEPROM.read(eepromAddress);

    if (validMarker != 0xAA) {
        return false;
    }

    _displayPrecision = EEPROM.read(eepromAddress + 1);

    for (uint8_t i = 0; i < 8; i++) {
        _displayUnits[i] = EEPROM.read(eepromAddress + 2 + i);
    }

    return true;
}

bool InteractiveSerialGeneralSensorCalibratorV2::savePreferences(int eepromAddress) {
    EEPROM.write(eepromAddress, 0xAA);
    EEPROM.write(eepromAddress + 1, _displayPrecision);

    for (uint8_t i = 0; i < 8; i++) {
        EEPROM.write(eepromAddress + 2 + i, _displayUnits[i]);
    }

#if defined(ARDUINO_ARCH_ESP8266) || defined(ARDUINO_ARCH_ESP32)
    return EEPROM.commit();
#else
    return true;
#endif
}

void InteractiveSerialGeneralSensorCalibratorV2::resetToDefaults() {
    strcpy(_displayUnits, "");
    _displayPrecision = 3;
    _readInterval = 1000;
}

void InteractiveSerialGeneralSensorCalibratorV2::clearSerialBuffer() {
    if (!_serial) return;

    while (_serial->available()) {
        _serial->read();
    }
}

void InteractiveSerialGeneralSensorCalibratorV2::waitForSerialInput() {
    if (!_serial) return;

    unsigned long startTime = millis();

    while (!_serial->available()) {
        if (isSerialTimedOut(startTime)) {
            break;
        }
        delay(10);
    }
}

bool InteractiveSerialGeneralSensorCalibratorV2::isWaitingForInput() const {
    return _state == STATE_WAITING_FOR_INPUT || _inputMode != INPUT_MODE_NONE;
}

bool InteractiveSerialGeneralSensorCalibratorV2::handleSerialInput() {
    if (_state == STATE_WAITING_FOR_INPUT) {
        processSerialInput();
        return true;
    }
    return false;
}

void InteractiveSerialGeneralSensorCalibratorV2::end() {
    _state = STATE_IDLE;
    _isFinished = true;

    if (_serial) {
        _serial->println("\n===== Exiting Sensor Calibration =====");
    }
}

float InteractiveSerialGeneralSensorCalibratorV2::collectStableMeasurement(uint8_t numSamples, unsigned long delayBetweenSamples) {
    _serial->println("Stabilizing readings...");
    delay(500);

    float sum = 0;

    for (uint8_t i = 0; i < numSamples; i++) {
        sum += readRawValue();
        delay(delayBetweenSamples);
    }

    return sum / numSamples;
}

bool InteractiveSerialGeneralSensorCalibratorV2::isSerialTimedOut(unsigned long startTime) const {
    return (millis() - startTime) > _timeout;
}

void InteractiveSerialGeneralSensorCalibratorV2::printReadingHeader() {
    _serial->println();
    _serial->print("Raw Value | Calibrated Value");

    if (strlen(_displayUnits) > 0) {
        _serial->print(" (");
        _serial->print(_displayUnits);
        _serial->print(")");
    }

    _serial->println();
    _serial->println("-------------------------------------------");
}

void InteractiveSerialGeneralSensorCalibratorV2::printStatsHeader() {
    _serial->println();
    _serial->print("Current Value: ");
    _serial->print(readCalibratedValue(), _displayPrecision);

    if (strlen(_displayUnits) > 0) {
        _serial->print(" ");
        _serial->print(_displayUnits);
    }

    _serial->println();

    _serial->print("Raw Value: ");
    _serial->println(readRawValue(), _displayPrecision);

    _serial->println();
}

void InteractiveSerialGeneralSensorCalibratorV2::printCalibrationDetails() {
    if (!_isCalibrated) {
        _serial->println("Sensor not calibrated");
        return;
    }

    _serial->println("\n===== CALIBRATION DETAILS =====");

    switch (_calibrationType) {
        case CALIBRATION_ONE_POINT:
            _serial->println("Method: One-point Calibration");
            _serial->print("Scale: ");
            _serial->println(_scale, 4);
            _serial->println("Formula: value = raw * scale");
            break;

        case CALIBRATION_TWO_POINT:
            _serial->println("Method: Two-point Calibration (Linear)");
            _serial->print("Slope: ");
            _serial->println(_slope, 4);
            _serial->print("Offset: ");
            _serial->println(_offset, 4);
            _serial->println("Formula: value = slope * raw + offset");
            break;

        case CALIBRATION_MULTI_POINT:
            _serial->println("Method: Multi-point Calibration");

            if (_interpolationMethod == 0) {
                _serial->println("Interpolation: Linear Segmented");
            } else {
                _serial->print("Interpolation: Polynomial Degree ");
                _serial->println(_polynomialDegree);

                _serial->println("Coefficients:");
                for (uint8_t i = 0; i <= _polynomialDegree; i++) {
                    _serial->print("  a");
                    _serial->print(i);
                    _serial->print(" = ");
                    _serial->println(_polynomialCoeffs[i], 6);
                }
            }
            break;
    }

    if (_calibPoints != NULL && _numCalibPoints > 0) {
        _serial->println("\nCalibration Points:");
        for (uint8_t i = 0; i < _numCalibPoints; i++) {
            _serial->print("  Point ");
            _serial->print(i + 1);
            _serial->print(": ");
            _serial->print(_calibPoints[i].rawValue, 3);
            _serial->print(" = ");
            _serial->print(_calibPoints[i].knownValue, 3);

            if (strlen(_displayUnits) > 0) {
                _serial->print(" ");
                _serial->print(_displayUnits);
            }

            _serial->println();
        }
    }

    _serial->println("\nCurrent Readings:");
    _serial->print("  Raw Value: ");
    _serial->println(readRawValue(), _displayPrecision);
    _serial->print("  Calibrated Value: ");
    _serial->print(readCalibratedValue(), _displayPrecision);

    if (strlen(_displayUnits) > 0) {
        _serial->print(" ");
        _serial->print(_displayUnits);
    }

    _serial->println();
}

void InteractiveSerialGeneralSensorCalibratorV2::initializeBuffers() {
    cleanupMemory();

    _calibPoints = new CalibrationPoint[_maxCalibPoints];
    for (uint8_t i = 0; i < _maxCalibPoints; i++) {
        _calibPoints[i].rawValue = 0.0;
        _calibPoints[i].knownValue = 0.0;
    }
    _numCalibPoints = 0;

    _polynomialCoeffs = new float[_maxCalibPoints];
    for (uint8_t i = 0; i < _maxCalibPoints; i++) {
        _polynomialCoeffs[i] = 0.0;
    }
}

void InteractiveSerialGeneralSensorCalibratorV2::cleanupMemory() {
    if (_calibPoints != NULL) {
        delete[] _calibPoints;
        _calibPoints = NULL;
    }

    if (_polynomialCoeffs != NULL) {
        delete[] _polynomialCoeffs;
        _polynomialCoeffs = NULL;
    }
}

float InteractiveSerialGeneralSensorCalibratorV2::linearInterpolate(float rawValue) {
    if (_calibPoints == NULL || _numCalibPoints < 2) {
        return rawValue;
    }

    if (rawValue <= _calibPoints[0].rawValue) {
        return _calibPoints[0].knownValue;
    }

    if (rawValue >= _calibPoints[_numCalibPoints - 1].rawValue) {
        return _calibPoints[_numCalibPoints - 1].knownValue;
    }

    for (uint8_t i = 0; i < _numCalibPoints - 1; i++) {
        if (rawValue >= _calibPoints[i].rawValue && rawValue <= _calibPoints[i + 1].rawValue) {
            float x0 = _calibPoints[i].rawValue;
            float y0 = _calibPoints[i].knownValue;
            float x1 = _calibPoints[i + 1].rawValue;
            float y1 = _calibPoints[i + 1].knownValue;

            return y0 + (y1 - y0) * (rawValue - x0) / (x1 - x0);
        }
    }

    return rawValue;
}

float InteractiveSerialGeneralSensorCalibratorV2::polynomialInterpolate(float rawValue) {
    if (_polynomialCoeffs == NULL || _polynomialDegree < 1) {
        return linearInterpolate(rawValue);
    }

    float result = 0;
    float x_power = 1;

    for (int i = 0; i <= _polynomialDegree; i++) {
        result += _polynomialCoeffs[i] * x_power;
        x_power *= rawValue;
    }

    return result;
}

bool InteractiveSerialGeneralSensorCalibratorV2::calculatePolynomialRegression() {
    if (_numCalibPoints <= 1 || _polynomialCoeffs == NULL) {
        return false;
    }

    if (_polynomialDegree == 2 && _numCalibPoints >= 3) {
        float x1 = _calibPoints[0].rawValue;
        float y1 = _calibPoints[0].knownValue;
        float x2 = _calibPoints[1].rawValue;
        float y2 = _calibPoints[1].knownValue;
        float x3 = _calibPoints[2].rawValue;
        float y3 = _calibPoints[2].knownValue;

        float x1_2 = x1 * x1;
        float x2_2 = x2 * x2;
        float x3_2 = x3 * x3;

        float det = x1_2 * (x2 - x3) - x1 * (x2_2 - x3_2) + (x2_2 * x3 - x2 * x3_2);

        if (abs(det) < 0.000001) {
            return false;
        }

        _polynomialCoeffs[2] = (y1 * (x2 - x3) - x1 * (y2 - y3) + (y2 * x3 - x2 * y3)) / det;
        _polynomialCoeffs[1] = (x1_2 * (y2 - y3) - y1 * (x2_2 - x3_2) + (x2_2 * y3 - y2 * x3_2)) / det;
        _polynomialCoeffs[0] = (x1_2 * (x2 * y3 - y2 * x3) - x1 * (x2_2 * y3 - y2 * x3_2) + y1 * (x2_2 * x3 - x2 * x3_2)) / det;

        return true;
    }

    return false;
}

bool InteractiveSerialGeneralSensorCalibratorV2::sortCalibrationPoints() {
    if (_numCalibPoints <= 1 || _calibPoints == NULL) {
        return false;
    }

    for (uint8_t i = 0; i < _numCalibPoints - 1; i++) {
        for (uint8_t j = 0; j < _numCalibPoints - i - 1; j++) {
            if (_calibPoints[j].rawValue > _calibPoints[j + 1].rawValue) {
                CalibrationPoint temp = _calibPoints[j];
                _calibPoints[j] = _calibPoints[j + 1];
                _calibPoints[j + 1] = temp;
            }
        }
    }

    return true;
}

uint16_t InteractiveSerialGeneralSensorCalibratorV2::getEepromProfileAddress(uint8_t profileNumber) const {
    return 50 + (profileNumber * 50);
}

float InteractiveSerialGeneralSensorCalibratorV2::sensorContextCallback(void *context) {
    SensorContext *ctx = static_cast<SensorContext *>(context);
    if (ctx && ctx->sensor) {
        return ctx->sensor->getFloatValue(ctx->valueKey);
    }
    return 0.0f;
}

void InteractiveSerialGeneralSensorCalibratorV2::notifyCalibrationCompleted() {
    if (_completedCallback) {
        _completedCallback(_completedCallbackContext);
    }

    end();
}
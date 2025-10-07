#include "InteractiveSerialGeneralSensorCalibrator.h"

InteractiveSerialGeneralSensorCalibrator::InteractiveSerialGeneralSensorCalibrator() {
    _serial = NULL;
    _timeout = 10000;
    _readInterval = 1000;
    _lastReadTime = 0;
    _continuousReading = false;
    _state = STATE_IDLE;

    _readCallback = NULL;
    _readCallbackWithContext = NULL;
    _callbackContext = NULL;
    _sensorValuePtr = NULL;

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
}

InteractiveSerialGeneralSensorCalibrator::~InteractiveSerialGeneralSensorCalibrator() {
    cleanupMemory();
}

void InteractiveSerialGeneralSensorCalibrator::begin(Stream *serialPtr, unsigned long timeout) {
    _serial = serialPtr;
    _timeout = timeout;

#if defined(ARDUINO_ARCH_ESP8266) || defined(ARDUINO_ARCH_ESP32)
    EEPROM.begin(512);
#endif

    initializeBuffers();

    _state = STATE_MENU;
    printMenu();
}

void InteractiveSerialGeneralSensorCalibrator::setSensorReadCallback(SensorReadCallback callback) {
    _readCallback = callback;
    _readCallbackWithContext = NULL;
    _callbackContext = NULL;
}

void InteractiveSerialGeneralSensorCalibrator::setSensorReadCallbackWithContext(SensorReadCallbackWithContext callback, void *context) {
    _readCallbackWithContext = callback;
    _callbackContext = context;
    _readCallback = NULL;
}

void InteractiveSerialGeneralSensorCalibrator::setSensorValuePointer(float *valuePtr) {
    _sensorValuePtr = valuePtr;
}

float InteractiveSerialGeneralSensorCalibrator::readRawValue() {
    if (_readCallbackWithContext != NULL) {
        return _readCallbackWithContext(_callbackContext);
    } else if (_readCallback != NULL) {
        return _readCallback();
    } else if (_sensorValuePtr != NULL) {
        return *_sensorValuePtr;
    }
    return 0.0;
}

float InteractiveSerialGeneralSensorCalibrator::readCalibratedValue() {
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

void InteractiveSerialGeneralSensorCalibrator::setReadInterval(unsigned long interval) {
    _readInterval = interval;
}

void InteractiveSerialGeneralSensorCalibrator::enableContinuousReading(bool enable) {
    _continuousReading = enable;
}

void InteractiveSerialGeneralSensorCalibrator::update() {
    unsigned long currentTime = millis();

    if (_continuousReading && (currentTime - _lastReadTime >= _readInterval)) {
        readSensor();
        _lastReadTime = currentTime;
    }

    if (_serial && _serial->available() > 0) {
        char cmd = _serial->read();
        processCommand(cmd);
    }
}

bool InteractiveSerialGeneralSensorCalibrator::calibrateOnePoint(float knownValue, float rawValue) {
    if (rawValue < 0) {
        rawValue = readRawValue();
    }

    if (rawValue == 0 || knownValue == 0) {
        return false;
    }

    _scale = knownValue / rawValue;
    _calibrationType = CALIBRATION_ONE_POINT;
    _isCalibrated = true;

    clearCalibrationPoints();
    addCalibrationPoint(knownValue, rawValue);

    return true;
}

bool InteractiveSerialGeneralSensorCalibrator::calibrateTwoPoint(float knownValue1, float rawValue1, float knownValue2, float rawValue2) {
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

void InteractiveSerialGeneralSensorCalibrator::setMaxCalibrationPoints(uint8_t maxPoints) {
    if (maxPoints > 0 && maxPoints != _maxCalibPoints) {
        _maxCalibPoints = maxPoints;
        initializeBuffers();
    }
}

void InteractiveSerialGeneralSensorCalibrator::startCalibration(uint8_t calibrationType) {
    _inCalibrationMode = true;
    _calibrationType = calibrationType;
    clearCalibrationPoints();
}

bool InteractiveSerialGeneralSensorCalibrator::addCalibrationPoint(float knownValue, float rawValue) {
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

bool InteractiveSerialGeneralSensorCalibrator::calculateCalibration() {
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

void InteractiveSerialGeneralSensorCalibrator::cancelCalibration() {
    _inCalibrationMode = false;
}

bool InteractiveSerialGeneralSensorCalibrator::clearCalibrationPoints() {
    if (_calibPoints != NULL) {
        for (uint8_t i = 0; i < _maxCalibPoints; i++) {
            _calibPoints[i].rawValue = 0.0;
            _calibPoints[i].knownValue = 0.0;
        }
        _numCalibPoints = 0;
        return true;
    }
    return false;
}

bool InteractiveSerialGeneralSensorCalibrator::isCalibrated() {
    return _isCalibrated;
}

uint8_t InteractiveSerialGeneralSensorCalibrator::getCalibrationMethod() {
    return _calibrationType;
}

uint8_t InteractiveSerialGeneralSensorCalibrator::getCalibrationPointCount() {
    return _numCalibPoints;
}

void InteractiveSerialGeneralSensorCalibrator::setInterpolationMethod(uint8_t method) {
    _interpolationMethod = method;
}

void InteractiveSerialGeneralSensorCalibrator::setPolynomialDegree(uint8_t degree) {
    if (degree < _maxCalibPoints - 1) {
        _polynomialDegree = degree;
    }
}

bool InteractiveSerialGeneralSensorCalibrator::processCommand(char cmd) {
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
            uint8_t profileNum = readInt("Enter profile number to load (0-9): ", 0);
            loadCalibrationProfile(profileNum);
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
                        char units[8];
                        _serial->print("Enter display units (max 7 chars): ");
                        waitForSerialInput();
                        int count = 0;
                        while (_serial->available() && count < 7) {
                            char c = _serial->read();
                            if (c == '\n' || c == '\r') break;
                            units[count++] = c;
                        }
                        units[count] = '\0';
                        setDisplayUnits(units);
                        _serial->println(units);
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

void InteractiveSerialGeneralSensorCalibrator::printMenu() {
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
        _serial->println("9: Save Calibration Profile");
        _serial->println("0: Load Calibration Profile");
        _serial->println("L: List Calibration Profiles");
        _serial->println("S: Settings");
        _serial->println("?: Show This Menu");
    }

    _serial->println("================");
}

void InteractiveSerialGeneralSensorCalibrator::setMenuPrompt(const char *prompt) {
    _menuPrompt = prompt;
}

void InteractiveSerialGeneralSensorCalibrator::setMenuOptions(const char *options[], uint8_t numOptions) {
    _menuOptions = options;
    _numMenuOptions = numOptions;
}

void InteractiveSerialGeneralSensorCalibrator::startReading() {
    _state = STATE_READING;
    _serial->println("\n===== CONTINUOUS SENSOR READINGS =====");
    _serial->println("Press any key to stop...");
    delay(1000);

    clearSerialBuffer();

    printReadingHeader();
    _lastReadTime = millis();
    _continuousReading = true;
}

void InteractiveSerialGeneralSensorCalibrator::stopReading() {
    _continuousReading = false;
    _state = STATE_MENU;
    _serial->println("\nSensor reading stopped");
}

void InteractiveSerialGeneralSensorCalibrator::readSensor() {
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

void InteractiveSerialGeneralSensorCalibrator::runOnePointCalibration() {
    _state = STATE_ONE_POINT_CAL;
    _serial->println("\n===== ONE-POINT CALIBRATION =====");
    _serial->println("This will calibrate the sensor using a single reference point");

    float rawValue = collectStableMeasurement();

    _serial->print("Current raw reading: ");
    _serial->print(rawValue, _displayPrecision);
    _serial->println();

    float knownValue = readFloat("Enter the known value for this reading: ");

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

void InteractiveSerialGeneralSensorCalibrator::runTwoPointCalibration() {
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

    float knownValue1 = readFloat("Enter the known value for this point: ");

    _serial->println("\nSTEP 2: Set up your second reference point");
    _serial->println("Press Enter when ready...");

    clearSerialBuffer();
    waitForSerialInput();
    clearSerialBuffer();

    float rawValue2 = collectStableMeasurement();

    _serial->print("Second point raw reading: ");
    _serial->print(rawValue2, _displayPrecision);
    _serial->println();

    float knownValue2 = readFloat("Enter the known value for this point: ");

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

void InteractiveSerialGeneralSensorCalibrator::runMultiPointCalibration() {
    _state = STATE_MULTI_POINT_CAL;
    _serial->println("\n===== MULTI-POINT CALIBRATION =====");

    int numPoints = readInt("How many calibration points (2-10)? ", 3);

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

        float knownValue = readFloat("Enter the known value for this point: ");

        addCalibrationPoint(knownValue, rawValue);
    }

    _serial->println("\nSelect interpolation method:");
    _serial->println("1: Linear interpolation between points");
    _serial->println("2: Polynomial curve fitting");

    int method = readInt("", 1);

    if (method == 2) {
        setInterpolationMethod(1);

        int maxDegree = min(numPoints - 1, 3);
        _serial->print("Enter polynomial degree (1-");
        _serial->print(maxDegree);
        _serial->println("):");

        int degree = readInt("", 2);

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

void InteractiveSerialGeneralSensorCalibrator::showCalibrationDetails() {
    _state = STATE_CALIBRATION_DETAILS;
    printCalibrationDetails();
    _state = STATE_MENU;
    printMenu();
}

void InteractiveSerialGeneralSensorCalibrator::runClearCalibration() {
    _serial->println("\nAre you sure you want to clear calibration data?");
    bool confirm = readYesNo("Clear calibration (y/n)? ", false);

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

float InteractiveSerialGeneralSensorCalibrator::readFloat(const char *prompt, float defaultValue, unsigned long timeout) {
    if (!_serial) return defaultValue;

    _serial->print(prompt);

    unsigned long startTime = millis();
    clearSerialBuffer();

    while (!_serial->available()) {
        if (timeout > 0 && isSerialTimedOut(startTime)) {
            _serial->println(defaultValue);
            return defaultValue;
        }
        delay(10);
    }

    float value = _serial->parseFloat();
    clearSerialBuffer();

    _serial->println(value);
    return value;
}

int InteractiveSerialGeneralSensorCalibrator::readInt(const char *prompt, int defaultValue, unsigned long timeout) {
    if (!_serial) return defaultValue;

    _serial->print(prompt);

    unsigned long startTime = millis();
    clearSerialBuffer();

    while (!_serial->available()) {
        if (timeout > 0 && isSerialTimedOut(startTime)) {
            _serial->println(defaultValue);
            return defaultValue;
        }
        delay(10);
    }

    int value = _serial->parseInt();
    clearSerialBuffer();

    _serial->println(value);
    return value;
}

bool InteractiveSerialGeneralSensorCalibrator::readYesNo(const char *prompt, bool defaultValue, unsigned long timeout) {
    if (!_serial) return defaultValue;

    _serial->print(prompt);

    unsigned long startTime = millis();
    clearSerialBuffer();

    while (!_serial->available()) {
        if (timeout > 0 && isSerialTimedOut(startTime)) {
            _serial->println(defaultValue ? "y" : "n");
            return defaultValue;
        }
        delay(10);
    }

    char c = _serial->read();
    clearSerialBuffer();

    bool result = (c == 'y' || c == 'Y');
    _serial->println(result ? "y" : "n");

    return result;
}

char InteractiveSerialGeneralSensorCalibrator::readChar(const char *prompt, char defaultValue, unsigned long timeout) {
    if (!_serial) return defaultValue;

    _serial->print(prompt);

    unsigned long startTime = millis();
    clearSerialBuffer();

    while (!_serial->available()) {
        if (timeout > 0 && isSerialTimedOut(startTime)) {
            _serial->println(defaultValue);
            return defaultValue;
        }
        delay(10);
    }

    char c = _serial->read();
    clearSerialBuffer();

    _serial->println(c);
    return c;
}

bool InteractiveSerialGeneralSensorCalibrator::saveCalibration(int eepromAddress) {
    if (!_isCalibrated) {
        return false;
    }

    int addr = eepromAddress;

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

bool InteractiveSerialGeneralSensorCalibrator::loadCalibration(int eepromAddress) {
    int addr = eepromAddress;
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
            addCalibrationPoint(1.0, 1.0 / _scale);

            _isCalibrated = true;
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

            if (numPoints > _maxCalibPoints) {
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

bool InteractiveSerialGeneralSensorCalibrator::saveCalibrationProfile(uint8_t profileNumber) {
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

bool InteractiveSerialGeneralSensorCalibrator::loadCalibrationProfile(uint8_t profileNumber) {
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

void InteractiveSerialGeneralSensorCalibrator::listCalibrationProfiles() {
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

void InteractiveSerialGeneralSensorCalibrator::showStatistics(uint16_t numSamples) {
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

void InteractiveSerialGeneralSensorCalibrator::drawCalibrationCurve(uint8_t width, uint8_t height) {
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

void InteractiveSerialGeneralSensorCalibrator::setDisplayUnits(const char *units) {
    strncpy(_displayUnits, units, 7);
    _displayUnits[7] = '\0';
}

void InteractiveSerialGeneralSensorCalibrator::setDisplayPrecision(uint8_t precision) {
    _displayPrecision = precision > 6 ? 6 : precision;
}

const char *InteractiveSerialGeneralSensorCalibrator::getDisplayUnits() const {
    return _displayUnits;
}

uint8_t InteractiveSerialGeneralSensorCalibrator::getDisplayPrecision() const {
    return _displayPrecision;
}

bool InteractiveSerialGeneralSensorCalibrator::loadPreferences(int eepromAddress) {
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

bool InteractiveSerialGeneralSensorCalibrator::savePreferences(int eepromAddress) {
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

void InteractiveSerialGeneralSensorCalibrator::resetToDefaults() {
    strcpy(_displayUnits, "");
    _displayPrecision = 3;
    _readInterval = 1000;
}

float InteractiveSerialGeneralSensorCalibrator::collectStableMeasurement(uint8_t numSamples, unsigned long delayBetweenSamples) {
    _serial->println("Stabilizing readings...");
    delay(500);

    float sum = 0;

    for (uint8_t i = 0; i < numSamples; i++) {
        sum += readRawValue();
        delay(delayBetweenSamples);
    }

    return sum / numSamples;
}

bool InteractiveSerialGeneralSensorCalibrator::isSerialTimedOut(unsigned long startTime) const {
    return (millis() - startTime) > _timeout;
}

void InteractiveSerialGeneralSensorCalibrator::clearSerialBuffer() {
    if (!_serial) return;

    while (_serial->available()) {
        _serial->read();
    }
}

void InteractiveSerialGeneralSensorCalibrator::waitForSerialInput() {
    if (!_serial) return;

    unsigned long startTime = millis();

    while (!_serial->available()) {
        if (isSerialTimedOut(startTime)) {
            break;
        }
        delay(10);
    }
}

void InteractiveSerialGeneralSensorCalibrator::printReadingHeader() {
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

void InteractiveSerialGeneralSensorCalibrator::printStatsHeader() {
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

void InteractiveSerialGeneralSensorCalibrator::printCalibrationDetails() {
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

void InteractiveSerialGeneralSensorCalibrator::initializeBuffers() {
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

void InteractiveSerialGeneralSensorCalibrator::cleanupMemory() {
    if (_calibPoints != NULL) {
        delete[] _calibPoints;
        _calibPoints = NULL;
    }

    if (_polynomialCoeffs != NULL) {
        delete[] _polynomialCoeffs;
        _polynomialCoeffs = NULL;
    }
}

float InteractiveSerialGeneralSensorCalibrator::linearInterpolate(float rawValue) {
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

float InteractiveSerialGeneralSensorCalibrator::polynomialInterpolate(float rawValue) {
    if (_polynomialCoeffs != NULL) {
        float result = 0;
        float x_power = 1;

        for (int i = 0; i <= _polynomialDegree; i++) {
            result += _polynomialCoeffs[i] * x_power;
            x_power *= rawValue;
        }

        return result;
    }

    return linearInterpolate(rawValue);
}

bool InteractiveSerialGeneralSensorCalibrator::calculatePolynomialRegression() {
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

bool InteractiveSerialGeneralSensorCalibrator::sortCalibrationPoints() {
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

uint16_t InteractiveSerialGeneralSensorCalibrator::getEepromProfileAddress(uint8_t profileNumber) const {
    return 50 + (profileNumber * 50);
}
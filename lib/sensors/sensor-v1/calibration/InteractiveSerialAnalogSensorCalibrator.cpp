#include "InteractiveSerialAnalogSensorCalibrator.h"

InteractiveSerialAnalogSensorCalibrator::InteractiveSerialAnalogSensorCalibrator(int pin, float referenceVoltage, float adcRange)
        : AnalogSensorCalibrator(pin, referenceVoltage, adcRange) {
    _serial = NULL;
    _timeout = 10000;
    _readInterval = 1000;
    _lastReadTime = 0;
    _continuousReading = false;
    _state = STATE_IDLE;

    _menuPrompt = "\n===== MENU =====";
    _menuOptions = NULL;
    _numMenuOptions = 0;

    strcpy(_displayUnits, "");
    _displayPrecision = 3;
}

InteractiveSerialAnalogSensorCalibrator::~InteractiveSerialAnalogSensorCalibrator() {
}

void InteractiveSerialAnalogSensorCalibrator::begin(Stream *serialPtr, unsigned long timeout) {
    _serial = serialPtr;
    _timeout = timeout;

    AnalogSensorCalibrator::begin();

    _state = STATE_MENU;
    printMenu();
}

void InteractiveSerialAnalogSensorCalibrator::setReadInterval(unsigned long interval) {
    _readInterval = interval;
}

void InteractiveSerialAnalogSensorCalibrator::enableContinuousReading(bool enable) {
    _continuousReading = enable;
}

void InteractiveSerialAnalogSensorCalibrator::update() {
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

bool InteractiveSerialAnalogSensorCalibrator::processCommand(char cmd) {
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

void InteractiveSerialAnalogSensorCalibrator::printMenu() {
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

void InteractiveSerialAnalogSensorCalibrator::setMenuPrompt(const char *prompt) {
    _menuPrompt = prompt;
}

void InteractiveSerialAnalogSensorCalibrator::setMenuOptions(const char *options[], uint8_t numOptions) {
    _menuOptions = options;
    _numMenuOptions = numOptions;
}

void InteractiveSerialAnalogSensorCalibrator::startReading() {
    _state = STATE_READING;
    _serial->println("\n===== CONTINUOUS SENSOR READINGS =====");
    _serial->println("Press any key to stop...");
    delay(1000);

    clearSerialBuffer();

    printReadingHeader();
    _lastReadTime = millis();
    _continuousReading = true;
}

void InteractiveSerialAnalogSensorCalibrator::stopReading() {
    _continuousReading = false;
    _state = STATE_MENU;
    _serial->println("\nSensor reading stopped");
}

void InteractiveSerialAnalogSensorCalibrator::readSensor() {
    float raw = readRaw();
    float voltage = readVoltage();
    float value = readValue();

    _serial->print("Raw ADC: ");
    _serial->print(raw);
    _serial->print(" | Voltage: ");
    _serial->print(voltage, _displayPrecision);
    _serial->print("V | Calibrated: ");
    _serial->print(value, _displayPrecision);

    if (strlen(_displayUnits) > 0) {
        _serial->print(" ");
        _serial->print(_displayUnits);
    }

    _serial->println();
}

void InteractiveSerialAnalogSensorCalibrator::runOnePointCalibration() {
    _state = STATE_ONE_POINT_CAL;
    _serial->println("\n===== ONE-POINT CALIBRATION =====");
    _serial->println("This will calibrate the sensor using a single reference point");

    float voltage = collectStableMeasurement();

    _serial->print("Current voltage reading: ");
    _serial->print(voltage, _displayPrecision);
    _serial->println("V");

    float knownValue = readFloat("Enter the known value for this voltage: ");

    if (calibrateOnePoint(knownValue, voltage)) {
        _serial->println("Calibration successful!");
        saveCalibration();
        showCalibrationDetails();
    } else {
        _serial->println("Calibration failed. Check your input values.");
    }

    _state = STATE_MENU;
    printMenu();
}

void InteractiveSerialAnalogSensorCalibrator::runTwoPointCalibration() {
    _state = STATE_TWO_POINT_CAL;
    _serial->println("\n===== TWO-POINT CALIBRATION =====");
    _serial->println("This will calibrate using two reference points");

    _serial->println("\nSTEP 1: Set up your first reference point");
    _serial->println("Press Enter when ready...");

    clearSerialBuffer();
    waitForSerialInput();
    clearSerialBuffer();

    float voltage1 = collectStableMeasurement();

    _serial->print("First point voltage: ");
    _serial->print(voltage1, _displayPrecision);
    _serial->println("V");

    float value1 = readFloat("Enter the known value for this point: ");

    _serial->println("\nSTEP 2: Set up your second reference point");
    _serial->println("Press Enter when ready...");

    clearSerialBuffer();
    waitForSerialInput();
    clearSerialBuffer();

    float voltage2 = collectStableMeasurement();

    _serial->print("Second point voltage: ");
    _serial->print(voltage2, _displayPrecision);
    _serial->println("V");

    float value2 = readFloat("Enter the known value for this point: ");

    if (calibrateTwoPoint(value1, voltage1, value2, voltage2)) {
        _serial->println("Two-point calibration successful!");
        saveCalibration();
        showCalibrationDetails();
    } else {
        _serial->println("Calibration failed. The two points may be too close.");
    }

    _state = STATE_MENU;
    printMenu();
}

void InteractiveSerialAnalogSensorCalibrator::runMultiPointCalibration() {
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

        float voltage = collectStableMeasurement();

        _serial->print("Voltage: ");
        _serial->print(voltage, _displayPrecision);
        _serial->println("V");

        float value = readFloat("Enter the known value for this point: ");

        addCalibrationPoint(value, voltage);
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

void InteractiveSerialAnalogSensorCalibrator::showCalibrationDetails() {
    _state = STATE_CALIBRATION_DETAILS;
    printCalibrationDetails();
    _state = STATE_MENU;
    printMenu();
}

void InteractiveSerialAnalogSensorCalibrator::runClearCalibration() {
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

float InteractiveSerialAnalogSensorCalibrator::readFloat(const char *prompt, float defaultValue, unsigned long timeout) {
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

int InteractiveSerialAnalogSensorCalibrator::readInt(const char *prompt, int defaultValue, unsigned long timeout) {
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

bool InteractiveSerialAnalogSensorCalibrator::readYesNo(const char *prompt, bool defaultValue, unsigned long timeout) {
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

char InteractiveSerialAnalogSensorCalibrator::readChar(const char *prompt, char defaultValue, unsigned long timeout) {
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

bool InteractiveSerialAnalogSensorCalibrator::saveCalibrationProfile(uint8_t profileNumber) {
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

bool InteractiveSerialAnalogSensorCalibrator::loadCalibrationProfile(uint8_t profileNumber) {
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

void InteractiveSerialAnalogSensorCalibrator::listCalibrationProfiles() {
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

void InteractiveSerialAnalogSensorCalibrator::showStatistics(uint16_t numSamples) {
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
        current = readValue();

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

void InteractiveSerialAnalogSensorCalibrator::drawCalibrationCurve(uint8_t width, uint8_t height) {
    if (!isCalibrated() || getCalibrationPointCount() < 2) {
        _serial->println("Not enough calibration points to draw curve");
        return;
    }

    _serial->println("\n===== CALIBRATION CURVE =====");

    float minVoltage = _calibPoints[0].rawValue;
    float maxVoltage = _calibPoints[getCalibrationPointCount() - 1].rawValue;

    float minValue = 999999;
    float maxValue = -999999;

    for (uint8_t i = 0; i < getCalibrationPointCount(); i++) {
        if (_calibPoints[i].knownValue < minValue) minValue = _calibPoints[i].knownValue;
        if (_calibPoints[i].knownValue > maxValue) maxValue = _calibPoints[i].knownValue;
    }

    _serial->print("Voltage range: ");
    _serial->print(minVoltage, _displayPrecision);
    _serial->print("V - ");
    _serial->print(maxVoltage, _displayPrecision);
    _serial->println("V");

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

    float voltageStep = (maxVoltage - minVoltage) / width;
    float valueRange = maxValue - minValue;

    for (uint8_t x = 0; x < width; x++) {
        float voltage = minVoltage + x * voltageStep;
        float value = 0;

        switch (_calibrationType) {
            case CALIBRATION_ONE_POINT:
                value = voltage * _scale;
                break;
            case CALIBRATION_TWO_POINT:
                value = _slope * voltage + _offset;
                break;
            case CALIBRATION_MULTI_POINT:
                if (_interpolationMethod == 0) {
                    value = linearInterpolate(voltage);
                } else {
                    value = polynomialInterpolate(voltage);
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
    _serial->print(minVoltage, 1);
    _serial->print("V");

    for (uint8_t i = 0; i < width - 8; i++) {
        _serial->print(" ");
    }

    _serial->print(maxVoltage, 1);
    _serial->println("V");

    _state = STATE_MENU;
    printMenu();
}

void InteractiveSerialAnalogSensorCalibrator::setDisplayUnits(const char *units) {
    strncpy(_displayUnits, units, 7);
    _displayUnits[7] = '\0';
}

void InteractiveSerialAnalogSensorCalibrator::setDisplayPrecision(uint8_t precision) {
    _displayPrecision = precision > 6 ? 6 : precision;
}

bool InteractiveSerialAnalogSensorCalibrator::loadPreferences(int eepromAddress) {
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

bool InteractiveSerialAnalogSensorCalibrator::savePreferences(int eepromAddress) {
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

void InteractiveSerialAnalogSensorCalibrator::resetToDefaults() {
    strcpy(_displayUnits, "");
    _displayPrecision = 3;
    _readInterval = 1000;
}

float InteractiveSerialAnalogSensorCalibrator::collectStableMeasurement(uint8_t numSamples, unsigned long delayBetweenSamples) {
    _serial->println("Stabilizing readings...");
    delay(500);

    float sum = 0;

    for (uint8_t i = 0; i < numSamples; i++) {
        sum += readVoltage();
        delay(delayBetweenSamples);
    }

    return sum / numSamples;
}

bool InteractiveSerialAnalogSensorCalibrator::isSerialTimedOut(unsigned long startTime) const {
    return (millis() - startTime) > _timeout;
}

void InteractiveSerialAnalogSensorCalibrator::clearSerialBuffer() {
    if (!_serial) return;

    while (_serial->available()) {
        _serial->read();
    }
}

void InteractiveSerialAnalogSensorCalibrator::waitForSerialInput() {
    if (!_serial) return;

    unsigned long startTime = millis();

    while (!_serial->available()) {
        if (isSerialTimedOut(startTime)) {
            break;
        }
        delay(10);
    }
}

void InteractiveSerialAnalogSensorCalibrator::printReadingHeader() {
    _serial->println();
    _serial->print("Raw ADC | Voltage (");
    _serial->print(_displayPrecision);
    _serial->print(" decimals) | Calibrated Value");

    if (strlen(_displayUnits) > 0) {
        _serial->print(" (");
        _serial->print(_displayUnits);
        _serial->print(")");
    }

    _serial->println();
    _serial->println("-------------------------------------------");
}

void InteractiveSerialAnalogSensorCalibrator::printStatsHeader() {
    _serial->println();
    _serial->print("Current Value: ");
    _serial->print(readValue(), _displayPrecision);

    if (strlen(_displayUnits) > 0) {
        _serial->print(" ");
        _serial->print(_displayUnits);
    }

    _serial->println();

    _serial->print("Raw ADC: ");
    _serial->print(readRaw());
    _serial->print(" | Voltage: ");
    _serial->print(readVoltage(), _displayPrecision);
    _serial->println("V");

    _serial->println();
}

uint16_t InteractiveSerialAnalogSensorCalibrator::getEepromProfileAddress(uint8_t profileNumber) const {
    return 50 + (profileNumber * 50);
}
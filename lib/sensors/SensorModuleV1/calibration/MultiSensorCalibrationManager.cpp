#include "MultiSensorCalibrationManager.h"

MultiSensorCalibrationManager::MultiSensorCalibrationManager() {
    _calibrators = new InteractiveSerialGeneralSensorCalibrator *[MAX_CALIBRATORS];
    _sensorNames = new char *[MAX_CALIBRATORS];
    _allReadings = new float[MAX_CALIBRATORS];
    _allCalibrated = new float[MAX_CALIBRATORS];

    for (uint8_t i = 0; i < MAX_CALIBRATORS; i++) {
        _calibrators[i] = NULL;
        _sensorNames[i] = new char[MAX_NAME_LENGTH];
        _sensorNames[i][0] = '\0';
        _allReadings[i] = 0.0;
        _allCalibrated[i] = 0.0;
    }

    _activeIndex = 0;
    _numCalibrators = 0;
    _serial = NULL;
    _timeout = 10000;
    _readInterval = 1000;
    _lastReadTime = 0;
    _continuousReading = false;
}

MultiSensorCalibrationManager::~MultiSensorCalibrationManager() {
    if (_calibrators) {
        delete[] _calibrators;
    }

    if (_sensorNames) {
        for (uint8_t i = 0; i < MAX_CALIBRATORS; i++) {
            if (_sensorNames[i]) {
                delete[] _sensorNames[i];
            }
        }
        delete[] _sensorNames;
    }

    if (_allReadings) {
        delete[] _allReadings;
    }

    if (_allCalibrated) {
        delete[] _allCalibrated;
    }
}

bool MultiSensorCalibrationManager::addCalibrator(const char *name, InteractiveSerialGeneralSensorCalibrator *calibrator) {
    if (_numCalibrators >= MAX_CALIBRATORS || calibrator == NULL) {
        return false;
    }

    _calibrators[_numCalibrators] = calibrator;
    strncpy(_sensorNames[_numCalibrators], name, MAX_NAME_LENGTH - 1);
    _sensorNames[_numCalibrators][MAX_NAME_LENGTH - 1] = '\0';

    _numCalibrators++;
    return true;
}

void MultiSensorCalibrationManager::setCalibrationInfo(uint8_t index, const char *name, const char *units, uint8_t precision) {
    if (index >= _numCalibrators || _calibrators[index] == NULL) {
        return;
    }

    strncpy(_sensorNames[index], name, MAX_NAME_LENGTH - 1);
    _sensorNames[index][MAX_NAME_LENGTH - 1] = '\0';

    _calibrators[index]->setDisplayUnits(units);
    _calibrators[index]->setDisplayPrecision(precision);
}

void MultiSensorCalibrationManager::setCalibrationInfo(const char *name, const char *units, uint8_t precision) {
    int index = findCalibratorByName(name);
    if (index >= 0) {
        _calibrators[index]->setDisplayUnits(units);
        _calibrators[index]->setDisplayPrecision(precision);
    }
}

bool MultiSensorCalibrationManager::selectCalibrator(uint8_t index) {
    if (index >= _numCalibrators || _calibrators[index] == NULL) {
        return false;
    }

    _activeIndex = index;
    if (_serial) {
        _serial->print("\nSwitched to ");
        _serial->print(_sensorNames[_activeIndex]);
        _serial->println(" sensor");
        _calibrators[_activeIndex]->printMenu();
    }

    return true;
}

bool MultiSensorCalibrationManager::selectCalibrator(const char *name) {
    int index = findCalibratorByName(name);
    if (index >= 0) {
        return selectCalibrator(index);
    }
    return false;
}

InteractiveSerialGeneralSensorCalibrator *MultiSensorCalibrationManager::getActiveCalibrator() {
    if (_activeIndex < _numCalibrators) {
        return _calibrators[_activeIndex];
    }
    return NULL;
}

uint8_t MultiSensorCalibrationManager::getActiveCalibratorIndex() {
    return _activeIndex;
}

const char *MultiSensorCalibrationManager::getActiveCalibratorName() {
    if (_activeIndex < _numCalibrators) {
        return _sensorNames[_activeIndex];
    }
    return "None";
}

void MultiSensorCalibrationManager::begin(Stream *serialPtr, unsigned long timeout) {
    _serial = serialPtr;
    _timeout = timeout;

    for (uint8_t i = 0; i < _numCalibrators; i++) {
        if (_calibrators[i] != NULL) {
            _calibrators[i]->begin(_serial, _timeout);
        }
    }

    if (_numCalibrators > 0 && _calibrators[0] != NULL) {
        _activeIndex = 0;
    }

    printMainMenu();
}

void MultiSensorCalibrationManager::update() {
    unsigned long currentTime = millis();

    if (_continuousReading && (currentTime - _lastReadTime >= _readInterval)) {
        readAllSensors();
        _lastReadTime = currentTime;
    }

    if (_serial && _serial->available() > 0) {
        char cmd = _serial->read();
        processCommand(cmd);
    }

    for (uint8_t i = 0; i < _numCalibrators; i++) {
        if (_calibrators[i] != NULL) {
            // Update only pass-through commands, don't check Serial
            if (_calibrators[i]->isCalibrated()) {
                _allCalibrated[i] = _calibrators[i]->readCalibratedValue();
            } else {
                _allCalibrated[i] = _calibrators[i]->readRawValue();
            }
            _allReadings[i] = _calibrators[i]->readRawValue();
        }
    }
}

void MultiSensorCalibrationManager::processCommand(char cmd) {
    if (cmd >= '1' && cmd <= '8') {
        int sensorIndex = cmd - '1';
        if (sensorIndex < _numCalibrators) {
            selectCalibrator(sensorIndex);
            return;
        }
    }

    switch (cmd) {
        case 'r':
        case 'R':
            readAllSensors();
            printMainMenu();
            break;
        case 's':
        case 'S':
            showAllStats();
            printMainMenu();
            break;
        case 'c':
        case 'C': {
            bool enable = _continuousReading ? false : true;
            enableContinuousReading(enable);
            _serial->print("Continuous reading ");
            _serial->println(enable ? "enabled" : "disabled");
            if (!enable) {
                printMainMenu();
            }
            break;
        }
        case 'a':
        case 'A': {
            bool result = saveAllCalibrations();
            _serial->println(result ? "All calibrations saved" : "Failed to save calibrations");
            printMainMenu();
            break;
        }
        case 'l':
        case 'L': {
            bool result = loadAllCalibrations();
            _serial->println(result ? "All calibrations loaded" : "Failed to load calibrations");
            printMainMenu();
            break;
        }
        case 'i':
        case 'I': {
            unsigned long interval = readInt("Enter read interval (ms): ", 1000);
            setReadInterval(interval);
            _serial->print("Read interval set to ");
            _serial->print(interval);
            _serial->println("ms");
            printMainMenu();
            break;
        }
        case 'm':
        case 'M':
        case '?':
            printMainMenu();
            break;
        case '0': {
            if (_numCalibrators > 0 && _activeIndex < _numCalibrators) {
                _serial->print("\nEntering ");
                _serial->print(_sensorNames[_activeIndex]);
                _serial->println(" Menu");
                _calibrators[_activeIndex]->printMenu();
                return;
            }
            break;
        }
        default:
            if (_numCalibrators > 0 && _activeIndex < _numCalibrators) {
                _calibrators[_activeIndex]->processCommand(cmd);
            }
            break;
    }
}

void MultiSensorCalibrationManager::printMainMenu() {
    _serial->println("\n===== MULTI-SENSOR CALIBRATION MANAGER =====");
    _serial->println("Available Sensors:");

    for (uint8_t i = 0; i < _numCalibrators; i++) {
        _serial->print(i + 1);
        _serial->print(": ");
        _serial->println(_sensorNames[i]);
    }

    _serial->println("\nCommands:");
    _serial->println("1-8: Select Sensor");
    _serial->println("0: Enter Selected Sensor Menu");
    _serial->println("R: Read All Sensors");
    _serial->println("S: Show Statistics for All Sensors");
    _serial->println("C: Toggle Continuous Reading");
    _serial->println("A: Save All Calibrations");
    _serial->println("L: Load All Calibrations");
    _serial->println("I: Set Reading Interval");
    _serial->println("M: Show This Menu");
    _serial->println("================");

    _serial->print("Active Sensor: ");
    _serial->print(_sensorNames[_activeIndex]);
    _serial->print(" (");
    _serial->print(_activeIndex + 1);
    _serial->println(")");

    _serial->print("Continuous Reading: ");
    _serial->println(_continuousReading ? "Enabled" : "Disabled");
}

void MultiSensorCalibrationManager::readAllSensors() {
    _serial->println("\n===== ALL SENSOR READINGS =====");

    for (uint8_t i = 0; i < _numCalibrators; i++) {
        if (_calibrators[i] != NULL) {
            _allReadings[i] = _calibrators[i]->readRawValue();
            _allCalibrated[i] = _calibrators[i]->readCalibratedValue();

            _serial->print(_sensorNames[i]);
            _serial->print(": Raw = ");
            _serial->print(_allReadings[i], _calibrators[i]->getDisplayPrecision());
            _serial->print(", Calibrated = ");
            _serial->print(_allCalibrated[i], _calibrators[i]->getDisplayPrecision());

            const char *units = _calibrators[i]->getDisplayUnits();
            if (units != NULL && strlen(units) > 0) {
                _serial->print(" ");
                _serial->print(units);
            }

            _serial->println();
        }
    }

    _serial->println("================");
}

void MultiSensorCalibrationManager::showAllStats() {
    _serial->println("\n===== ALL SENSOR STATISTICS =====");

    for (uint8_t i = 0; i < _numCalibrators; i++) {
        if (_calibrators[i] != NULL) {
            _serial->print("\n--- ");
            _serial->print(_sensorNames[i]);
            _serial->println(" ---");
            _calibrators[i]->showStatistics(50);
        }
    }
}

bool MultiSensorCalibrationManager::saveAllCalibrations(int baseEepromAddress) {
    bool allSuccess = true;

    for (uint8_t i = 0; i < _numCalibrators; i++) {
        if (_calibrators[i] != NULL) {
            int addr = baseEepromAddress + (i * 100);
            bool success = _calibrators[i]->saveCalibration(addr);

            _serial->print("Saving ");
            _serial->print(_sensorNames[i]);
            _serial->print(" calibration: ");
            _serial->println(success ? "Success" : "Failed");

            if (!success) allSuccess = false;
        }
    }

    return allSuccess;
}

bool MultiSensorCalibrationManager::loadAllCalibrations(int baseEepromAddress) {
    bool allSuccess = true;

    for (uint8_t i = 0; i < _numCalibrators; i++) {
        if (_calibrators[i] != NULL) {
            int addr = baseEepromAddress + (i * 100);
            bool success = _calibrators[i]->loadCalibration(addr);

            _serial->print("Loading ");
            _serial->print(_sensorNames[i]);
            _serial->print(" calibration: ");
            _serial->println(success ? "Success" : "Failed");

            if (!success) allSuccess = false;
        }
    }

    return allSuccess;
}

void MultiSensorCalibrationManager::enableContinuousReading(bool enable) {
    _continuousReading = enable;
    _lastReadTime = millis();
}

void MultiSensorCalibrationManager::setReadInterval(unsigned long interval) {
    _readInterval = interval;
}

void MultiSensorCalibrationManager::clearSerialBuffer() {
    if (!_serial) return;

    while (_serial->available()) {
        _serial->read();
    }
}

int MultiSensorCalibrationManager::findCalibratorByName(const char *name) {
    for (uint8_t i = 0; i < _numCalibrators; i++) {
        if (strcmp(_sensorNames[i], name) == 0) {
            return i;
        }
    }
    return -1;
}

bool MultiSensorCalibrationManager::isSerialTimedOut(unsigned long startTime) const {
    return (millis() - startTime) > _timeout;
}

int MultiSensorCalibrationManager::readInt(const char *prompt, int defaultValue, unsigned long timeout) {
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

char MultiSensorCalibrationManager::readChar(const char *prompt, char defaultValue, unsigned long timeout) {
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

bool MultiSensorCalibrationManager::readYesNo(const char *prompt, bool defaultValue, unsigned long timeout) {
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
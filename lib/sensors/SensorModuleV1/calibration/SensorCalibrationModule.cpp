#include "SensorCalibrationModule.h"

SensorCalibrationModule::SensorCalibrationModule()
        : SensorModule(),
          _calibrationMode(false),
          _activeIndex(0),
          _calibrationModeTimeout(300000),
          _lastCalibrationActivity(0),
          _autoSaveCalibration(true),
          _eepromStartAddress(SENSOR_CAL_EEPROM_START_ADDR),
          _inSensorMenu(false) {

    _calibrators = new InteractiveSerialGeneralSensorCalibrator *[MAX_CALIBRATORS];
    _hasCalibrator = new bool[MAX_CALIBRATORS];
    _sensorContexts = new SensorContext *[MAX_CALIBRATORS];

    for (uint8_t i = 0; i < MAX_CALIBRATORS; i++) {
        _calibrators[i] = nullptr;
        _hasCalibrator[i] = false;
        _sensorContexts[i] = nullptr;
    }

    _serial = nullptr;
}

SensorCalibrationModule::~SensorCalibrationModule() {
    if (_calibrators) {
        for (uint8_t i = 0; i < MAX_CALIBRATORS; i++) {
            if (_calibrators[i]) {
                delete _calibrators[i];
            }
        }
        delete[] _calibrators;
    }

    if (_hasCalibrator) {
        delete[] _hasCalibrator;
    }

    if (_sensorContexts) {
        for (uint8_t i = 0; i < MAX_CALIBRATORS; i++) {
            if (_sensorContexts[i]) {
                delete _sensorContexts[i];
            }
        }
        delete[] _sensorContexts;
    }
}

void SensorCalibrationModule::init(void (*initializeCallback)(void)) {
    SensorModule::init(initializeCallback);

    loadAllCalibrations(_eepromStartAddress);
}

void SensorCalibrationModule::update(void (*updateCallback)(void)) {
    SensorModule::update(updateCallback);

    if (_calibrationMode && _serial && _serial->available() > 0) {
        char cmd = _serial->read();
        processCalibrationCommand(cmd);
        _lastCalibrationActivity = millis();
    }

    if (_calibrationMode && isCalibrationModeTimedOut()) {
        stopCalibrationMode();
    }

    for (uint8_t i = 0; i < getModuleCount(); i++) {
        if (_hasCalibrator[i] && _calibrators[i]) {
            _calibrators[i]->update();
        }
    }
}

bool SensorCalibrationModule::addCalibrator(uint8_t index) {
    if (index >= getModuleCount() || _hasCalibrator[index]) {
        return false;
    }

    _calibrators[index] = new InteractiveSerialGeneralSensorCalibrator();
    _hasCalibrator[index] = true;

    _sensorContexts[index] = new SensorContext{this, index};

    _calibrators[index]->setSensorReadCallbackWithContext(sensorReadCallback, _sensorContexts[index]);
    _calibrators[index]->setDisplayUnits("");
    _calibrators[index]->setDisplayPrecision(2);

    return true;
}

bool SensorCalibrationModule::addCalibrator(const char *name) {
    int index = findSensorIndex(name);
    if (index < 0) {
        return false;
    }

    return addCalibrator(index);
}

bool SensorCalibrationModule::removeCalibrator(uint8_t index) {
    if (index >= getModuleCount() || !_hasCalibrator[index]) {
        return false;
    }

    if (_calibrators[index]) {
        delete _calibrators[index];
        _calibrators[index] = nullptr;
    }

    if (_sensorContexts[index]) {
        delete _sensorContexts[index];
        _sensorContexts[index] = nullptr;
    }

    _hasCalibrator[index] = false;
    return true;
}

bool SensorCalibrationModule::removeCalibrator(const char *name) {
    int index = findSensorIndex(name);
    if (index < 0) {
        return false;
    }

    return removeCalibrator(index);
}

bool SensorCalibrationModule::hasCalibrator(uint8_t index) const {
    if (index >= getModuleCount()) {
        return false;
    }

    return _hasCalibrator[index];
}

bool SensorCalibrationModule::hasCalibrator(const char *name) const {
    int index = findSensorIndex(name);
    if (index < 0) {
        return false;
    }

    return _hasCalibrator[index];
}

InteractiveSerialGeneralSensorCalibrator *SensorCalibrationModule::getCalibrator(uint8_t index) const {
    if (index >= getModuleCount() || !_hasCalibrator[index]) {
        return nullptr;
    }

    return _calibrators[index];
}

InteractiveSerialGeneralSensorCalibrator *SensorCalibrationModule::getCalibrator(const char *name) const {
    int index = findSensorIndex(name);
    if (index < 0 || !_hasCalibrator[index]) {
        return nullptr;
    }

    return _calibrators[index];
}

void SensorCalibrationModule::startCalibrationMode(Stream *serialPtr, uint32_t timeout) {
    _serial = serialPtr;
    _calibrationMode = true;
    _calibrationModeTimeout = timeout;
    _lastCalibrationActivity = millis();
    _inSensorMenu = false;

    _serial->println("\n===== SENSOR CALIBRATION MODE =====");
    _serial->println("Entering sensor calibration mode. Type 'h' for help.");

    printCalibrationMenu();
}

void SensorCalibrationModule::stopCalibrationMode() {
    if (_calibrationMode && _serial) {
        _serial->println("\n===== EXITING CALIBRATION MODE =====");
    }

    _calibrationMode = false;
    _activeIndex = 0;
    _inSensorMenu = false;
}

bool SensorCalibrationModule::isInCalibrationMode() const {
    return _calibrationMode;
}

float SensorCalibrationModule::getRawValue(uint8_t index) const {
    if (index >= getModuleCount()) {
        return 0.0f;
    }

    BaseSens *module = getModulePtr(index);
    if (!module) {
        return 0.0f;
    }

    return module->getValueF();
}

float SensorCalibrationModule::getRawValue(const char *name) const {
    int index = findSensorIndex(name);
    if (index < 0) {
        return 0.0f;
    }

    return getRawValue(index);
}

float SensorCalibrationModule::getCalibratedValue(uint8_t index) const {
    if (index >= getModuleCount()) {
        return 0.0f;
    }

    float rawValue = getRawValue(index);

    if (_hasCalibrator[index] && _calibrators[index] && _calibrators[index]->isCalibrated()) {
        float value = _calibrators[index]->readCalibratedValue();
        return value;
    }

    return rawValue;
}

float SensorCalibrationModule::getCalibratedValue(const char *name) const {
    int index = findSensorIndex(name);
    if (index < 0) {
        return 0.0f;
    }

    return getCalibratedValue(index);
}

bool SensorCalibrationModule::saveAllCalibrations(int baseEepromAddress) {
    bool allSaved = true;

    for (uint8_t i = 0; i < getModuleCount(); i++) {
        if (_hasCalibrator[i] && _calibrators[i] && _calibrators[i]->isCalibrated()) {
            int addr = baseEepromAddress + (i * 50);
            bool success = _calibrators[i]->saveCalibration(addr);

            if (!success) {
                allSaved = false;
            }
        }
    }

    return allSaved;
}

bool SensorCalibrationModule::loadAllCalibrations(int baseEepromAddress) {
    bool allLoaded = true;

    for (uint8_t i = 0; i < getModuleCount(); i++) {
        if (_hasCalibrator[i] && _calibrators[i]) {
            int addr = baseEepromAddress + (i * 50);
            bool success = _calibrators[i]->loadCalibration(addr);

            if (!success) {
                allLoaded = false;
            }
        }
    }

    return allLoaded;
}

void SensorCalibrationModule::setAutoSaveCalibration(bool autoSave) {
    _autoSaveCalibration = autoSave;
}

bool SensorCalibrationModule::getAutoSaveCalibration() const {
    return _autoSaveCalibration;
}

void SensorCalibrationModule::setEEPROMStartAddress(int address) {
    _eepromStartAddress = address;
}

int SensorCalibrationModule::getEEPROMStartAddress() const {
    return _eepromStartAddress;
}

void SensorCalibrationModule::setCalibrationParameter(uint8_t index, const char *units, uint8_t precision) {
    if (index >= getModuleCount() || !_hasCalibrator[index]) {
        return;
    }

    _calibrators[index]->setDisplayUnits(units);
    _calibrators[index]->setDisplayPrecision(precision);
}

void SensorCalibrationModule::setCalibrationParameter(const char *name, const char *units, uint8_t precision) {
    int index = findSensorIndex(name);
    if (index < 0) {
        return;
    }

    setCalibrationParameter(index, units, precision);
}

void SensorCalibrationModule::calibrateOnePoint(uint8_t index, float knownValue) {
    if (index >= getModuleCount() || !_hasCalibrator[index]) {
        return;
    }

    float rawValue = getRawValue(index);
    _calibrators[index]->calibrateOnePoint(knownValue, rawValue);

    if (_autoSaveCalibration) {
        int addr = _eepromStartAddress + (index * 50);
        _calibrators[index]->saveCalibration(addr);
    }
}

void SensorCalibrationModule::calibrateOnePoint(const char *name, float knownValue) {
    int index = findSensorIndex(name);
    if (index < 0) {
        return;
    }

    calibrateOnePoint(index, knownValue);
}

void SensorCalibrationModule::calibrateTwoPoint(uint8_t index, float knownValue1, float rawValue1, float knownValue2, float rawValue2) {
    if (index >= getModuleCount() || !_hasCalibrator[index]) {
        return;
    }

    _calibrators[index]->calibrateTwoPoint(knownValue1, rawValue1, knownValue2, rawValue2);

    if (_autoSaveCalibration) {
        int addr = _eepromStartAddress + (index * 50);
        _calibrators[index]->saveCalibration(addr);
    }
}

void SensorCalibrationModule::calibrateTwoPoint(const char *name, float knownValue1, float rawValue1, float knownValue2, float rawValue2) {
    int index = findSensorIndex(name);
    if (index < 0) {
        return;
    }

    calibrateTwoPoint(index, knownValue1, rawValue1, knownValue2, rawValue2);
}

void SensorCalibrationModule::debug(const char *searchName, bool showHeapMemory, bool endl) {
    int index = findSensorIndex(searchName);

    if (index >= 0 && _hasCalibrator[index] && _calibrators[index]->isCalibrated()) {
        float rawValue = getRawValue(index);
        float calibratedValue = getCalibratedValue(index);

        Serial.print("| ");
        Serial.print(searchName);
        Serial.print(": Raw=");
        Serial.print(rawValue, _calibrators[index]->getDisplayPrecision());
        Serial.print(" Cal=");
        Serial.print(calibratedValue, _calibrators[index]->getDisplayPrecision());

        const char *units = _calibrators[index]->getDisplayUnits();
        if (units && strlen(units) > 0) {
            Serial.print(" ");
            Serial.print(units);
        }

        if (showHeapMemory) {
#if defined(ESP32)
            Serial.print(" | mem: ");
            Serial.print(ESP.getFreeHeap());
#endif
        }

        if (endl) Serial.println();
    } else {
        SensorModule::debug(searchName, showHeapMemory, endl);
    }
}

void SensorCalibrationModule::debug(bool showHeapMemory) {
    for (uint8_t i = 0; i < getModuleCount(); i++) {
        const char *sensorName = getName(i);
        debug(sensorName, false, false);
    }

#if defined(ESP32)
    if (showHeapMemory) {
        Serial.print("| mem: ");
        Serial.print(ESP.getFreeHeap());
    }
#endif

    Serial.println();
}

void SensorCalibrationModule::clearSerialBuffer() const {
    if (!_serial) return;

    while (_serial->available()) {
        _serial->read();
    }
}

int SensorCalibrationModule::findSensorIndex(const char *name) const {
    for (uint8_t i = 0; i < getModuleCount(); i++) {
        if (strcmp(getName(i), name) == 0) {
            return i;
        }
    }

    return -1;
}

bool SensorCalibrationModule::isCalibrationModeTimedOut() const {
    return (millis() - _lastCalibrationActivity) > _calibrationModeTimeout;
}

void SensorCalibrationModule::processCalibrationCommand(char cmd) {
    if (cmd == '\r' || cmd == '\n') {
        return;
    }

    switch (cmd) {
        case 'h':
        case 'H':
        case '?':
            printCalibrationMenu();
            _inSensorMenu = false;
            break;

        case 'x':
        case 'X':
        case 'q':
        case 'Q':
            stopCalibrationMode();
            _inSensorMenu = false;
            break;

        case 'l':
        case 'L':
            _serial->println("\nAvailable Sensors:");
            for (uint8_t i = 0; i < getModuleCount(); i++) {
                _serial->print(i + 1);
                _serial->print(": ");
                _serial->print(getName(i));

                if (_hasCalibrator[i] && _calibrators[i]->isCalibrated()) {
                    _serial->print(" [Calibrated]");
                } else if (_hasCalibrator[i]) {
                    _serial->print(" [Uncalibrated]");
                } else {
                    _serial->print(" [No Calibrator]");
                }

                _serial->println();
            }
            _inSensorMenu = false;
            break;

        case 's':
        case 'S':
            if (saveAllCalibrations(_eepromStartAddress)) {
                _serial->println("All calibrations saved successfully");
            } else {
                _serial->println("Failed to save some calibrations");
            }
            _inSensorMenu = false;
            break;

        case 'r':
        case 'R':
            if (loadAllCalibrations(_eepromStartAddress)) {
                _serial->println("All calibrations loaded successfully");
            } else {
                _serial->println("Failed to load some calibrations");
            }
            _inSensorMenu = false;
            break;

        case 'a':
        case 'A': {
            _serial->print("Auto-save calibration is now ");
            _autoSaveCalibration = !_autoSaveCalibration;
            _serial->println(_autoSaveCalibration ? "ON" : "OFF");
            _inSensorMenu = false;
            break;
        }

        default:
            if (!_inSensorMenu && cmd >= '1' && cmd <= '9') {
                int sensorIndex = cmd - '1';
                if (sensorIndex < getModuleCount()) {
                    _activeIndex = sensorIndex;

                    _serial->print("\nSelected sensor: ");
                    _serial->println(getName(_activeIndex));

                    if (!_hasCalibrator[_activeIndex]) {
                        _serial->println("Adding calibrator for this sensor");
                        addCalibrator(_activeIndex);
                    }

                    if (_hasCalibrator[_activeIndex] && _calibrators[_activeIndex]) {
                        _serial->println("\nSensor Calibration Menu:");
                        _serial->println("1: Read Raw Value");
                        _serial->println("2: One-Point Calibration");
                        _serial->println("3: Two-Point Calibration");
                        _serial->println("4: Multi-Point Calibration");
                        _serial->println("5: Show Calibration Details");
                        _serial->println("6: Clear Calibration");
                        _serial->println("7: Set Display Units");
                        _serial->println("8: Set Display Precision");
                        _serial->println("0: Back to Main Menu");

                        _inSensorMenu = true;
                    }
                } else {
                    _serial->println("Invalid sensor index");
                }
            } else if (_inSensorMenu) {
                if (cmd == '0') {
                    _inSensorMenu = false;
                    printCalibrationMenu();
                } else if (_activeIndex < getModuleCount() && _hasCalibrator[_activeIndex]) {
                    processSensorCalibrationCommand(cmd);
                }
            }
            break;
    }
}

void SensorCalibrationModule::printCalibrationMenu() {
    _serial->println("\n===== SENSOR CALIBRATION MENU =====");
    _serial->println("1-9: Select Sensor (by number)");
    _serial->println("L: List Available Sensors");
    _serial->println("S: Save All Calibrations");
    _serial->println("R: Load All Calibrations");
    _serial->println("A: Toggle Auto-save Calibration");
    _serial->println("H: Show This Menu");
    _serial->println("X: Exit Calibration Mode");
    _serial->println("=================================");

    _serial->print("Auto-save calibration: ");
    _serial->println(_autoSaveCalibration ? "ON" : "OFF");
}

void SensorCalibrationModule::processSensorCalibrationCommand(char cmd) {
    if (_activeIndex >= getModuleCount() || !_hasCalibrator[_activeIndex] || !_calibrators[_activeIndex]) {
        return;
    }

    InteractiveSerialGeneralSensorCalibrator *calibrator = _calibrators[_activeIndex];
    const char *sensorName = getName(_activeIndex);

    switch (cmd) {
        case '1':
            _serial->print("\nRaw value for ");
            _serial->print(sensorName);
            _serial->print(": ");
            _serial->println(getRawValue(_activeIndex), calibrator->getDisplayPrecision());
            break;

        case '2': {
            _serial->println("\n===== ONE-POINT CALIBRATION =====");
            _serial->print("Current raw reading: ");
            _serial->println(getRawValue(_activeIndex), calibrator->getDisplayPrecision());

            _serial->print("Enter known value: ");
            clearSerialBuffer();
            while (!_serial->available()) {
                delay(10);
            }

            float knownValue = _serial->parseFloat();
            clearSerialBuffer();

            _serial->println(knownValue, calibrator->getDisplayPrecision());

            calibrateOnePoint(_activeIndex, knownValue);

            _serial->println("One-point calibration complete");
            break;
        }

        case '3': {
            _serial->println("\n===== TWO-POINT CALIBRATION =====");

            _serial->println("STEP 1: Setup first point");
            _serial->print("Current raw reading: ");
            float rawValue1 = getRawValue(_activeIndex);
            _serial->println(rawValue1, calibrator->getDisplayPrecision());

            _serial->print("Enter first known value: ");
            clearSerialBuffer();
            while (!_serial->available()) {
                delay(10);
            }

            float knownValue1 = _serial->parseFloat();
            clearSerialBuffer();

            _serial->println(knownValue1, calibrator->getDisplayPrecision());

            _serial->println("\nSTEP 2: Change sensor condition");
            _serial->println("Press any key when ready for second point...");
            clearSerialBuffer();
            while (!_serial->available()) {
                delay(10);
            }
            clearSerialBuffer();

            _serial->print("Current raw reading: ");
            float rawValue2 = getRawValue(_activeIndex);
            _serial->println(rawValue2, calibrator->getDisplayPrecision());

            _serial->print("Enter second known value: ");
            clearSerialBuffer();
            while (!_serial->available()) {
                delay(10);
            }

            float knownValue2 = _serial->parseFloat();
            clearSerialBuffer();

            _serial->println(knownValue2, calibrator->getDisplayPrecision());

            calibrateTwoPoint(_activeIndex, knownValue1, rawValue1, knownValue2, rawValue2);

            _serial->println("Two-point calibration complete");
            break;
        }

        case '4':
            _serial->println("\nEntering multi-point calibration mode...");
            calibrator->processCommand('4');
            break;

        case '5':
            _serial->println("\n===== CALIBRATION DETAILS =====");
            if (calibrator->isCalibrated()) {
                _serial->print("Calibration type: ");
                switch (calibrator->getCalibrationMethod()) {
                    case 1:
                        _serial->println("One-point calibration");
                        break;
                    case 2:
                        _serial->println("Two-point calibration");
                        break;
                    case 3:
                        _serial->println("Multi-point calibration");
                        break;
                    default:
                        _serial->println("Unknown");
                        break;
                }

                _serial->print("Raw value: ");
                _serial->println(getRawValue(_activeIndex), calibrator->getDisplayPrecision());

                _serial->print("Calibrated value: ");
                _serial->print(getCalibratedValue(_activeIndex), calibrator->getDisplayPrecision());

                const char *units = calibrator->getDisplayUnits();
                if (units && strlen(units) > 0) {
                    _serial->print(" ");
                    _serial->print(units);
                }
                _serial->println();
            } else {
                _serial->println("Sensor not calibrated");
            }
            break;

        case '6': {
            _serial->println("\nAre you sure you want to clear calibration data?");
            _serial->print("Enter Y to confirm: ");

            clearSerialBuffer();
            while (!_serial->available()) {
                delay(10);
            }

            char c = _serial->read();
            clearSerialBuffer();

            if (c == 'Y' || c == 'y') {
                calibrator->clearCalibrationPoints();
                _serial->println("Calibration data cleared");
            } else {
                _serial->println("Operation cancelled");
            }
            break;
        }

        case '7': {
            _serial->print("Enter display units (max 7 chars): ");

            clearSerialBuffer();
            char units[8] = {0};
            uint8_t idx = 0;

            while (idx < 7) {
                if (_serial->available()) {
                    char c = _serial->read();
                    if (c == '\r' || c == '\n') {
                        break;
                    }
                    units[idx++] = c;
                }
                delay(10);
            }

            units[idx] = '\0';
            calibrator->setDisplayUnits(units);

            _serial->print("Display units set to: ");
            _serial->println(units);
            break;
        }

        case '8': {
            _serial->print("Enter display precision (0-6): ");

            clearSerialBuffer();
            while (!_serial->available()) {
                delay(10);
            }

            int precision = _serial->parseInt();
            clearSerialBuffer();

            if (precision < 0) precision = 0;
            if (precision > 6) precision = 6;

            calibrator->setDisplayPrecision(precision);

            _serial->print("Display precision set to: ");
            _serial->println(precision);
            break;
        }

        default:
            break;
    }
}

float SensorCalibrationModule::getRawValueForSensor(uint8_t index) const {
    if (index >= getModuleCount()) {
        return 0.0f;
    }

    return getRawValue(index);
}

float SensorCalibrationModule::sensorReadCallback(void *context) {
    if (context == nullptr) return 0.0f;

    SensorContext *ctx = static_cast<SensorContext *>(context);
    if (ctx->module == nullptr) return 0.0f;

    return ctx->module->getRawValueForSensor(ctx->sensorIndex);
}
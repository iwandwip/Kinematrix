#include "SensorCalibrationModuleV2.h"

SensorCalibrationModuleV2::SensorCalibrationModuleV2()
        : SensorModuleV2(),
          _entries(nullptr),
          _entryCount(0),
          _entryCapacity(0),
          _serial(nullptr),
          _calibrationMode(false),
          _activeEntryIndex(0),
          _calibrationModeTimeout(300000),
          _lastCalibrationActivity(0),
          _autoSaveCalibration(true),
          _eepromStartAddress(0),
          _timeout(10000) {

    _entryCapacity = 16;
    _entries = (CalibrationEntry *) malloc(_entryCapacity * sizeof(CalibrationEntry));

    if (_entries) {
        for (uint16_t i = 0; i < _entryCapacity; i++) {
            _entries[i].sensor = nullptr;
            _entries[i].sensorName = nullptr;
            _entries[i].valueKey = nullptr;
            _entries[i].calibrator = nullptr;
            _entries[i].isActive = false;
        }
    }
}

SensorCalibrationModuleV2::~SensorCalibrationModuleV2() {
    if (_entries) {
        for (uint16_t i = 0; i < _entryCount; i++) {
            if (_entries[i].isActive) {
                if (_entries[i].sensorName) free(_entries[i].sensorName);
                if (_entries[i].valueKey) free(_entries[i].valueKey);
                if (_entries[i].calibrator) delete _entries[i].calibrator;
            }
        }
        free(_entries);
    }
}

void SensorCalibrationModuleV2::init() {
    SensorModuleV2::init();

#if defined(ARDUINO_ARCH_ESP8266) || defined(ARDUINO_ARCH_ESP32)
    EEPROM.begin(512);
#endif

    loadAllCalibrations(_eepromStartAddress);
}

void SensorCalibrationModuleV2::update() {
    SensorModuleV2::update();

    if (_calibrationMode) {
        _lastCalibrationActivity = millis();

        for (uint16_t i = 0; i < _entryCount; i++) {
            if (_entries[i].isActive && _entries[i].calibrator && _entries[i].calibrator->isActive()) {
                _entries[i].calibrator->update();

                if (_entries[i].calibrator->isFinished()) {
                    _serial->println("\nReturning to sensor selection...");
                    printCalibrationMenu();
                }

                return;
            }
        }

        if (_serial && _serial->available() > 0) {
            char cmd = _serial->read();
            processCalibrationCommand(cmd);
            _lastCalibrationActivity = millis();
        }
    }

    if (_calibrationMode && isCalibrationModeTimedOut()) {
        stopCalibrationMode();
    }
}

void SensorCalibrationModuleV2::setSerialOutput(Stream *serialPtr) {
    _serial = serialPtr;
}

void SensorCalibrationModuleV2::discoverSensorValues() {
    for (uint8_t i = 0; i < getSensorCount(); i++) {
        BaseSensV2 *sensor = getSensor(i);
        const char *sensorName = getSensorName(i);

        if (sensor) {
            for (uint8_t j = 0; j < sensor->getValueCount(); j++) {
                const SensorValueInfo *info = sensor->getValueInfo(j);
                if (info) {
                    addCalibrationEntry(sensor, sensorName, info->key);
                }
            }
        }
    }
}

bool SensorCalibrationModuleV2::addCalibrationEntry(BaseSensV2 *sensor, const char *sensorName, const char *valueKey) {
    if (!sensor || !sensorName || !valueKey) {
        return false;
    }

    if (findEntryIndex(sensorName, valueKey) >= 0) {
        return false;
    }

    if (_entries == nullptr) {
        return false;
    }

    if (_entryCount >= _entryCapacity) {
        uint16_t newCapacity = _entryCapacity + 16;
        CalibrationEntry *newEntries = (CalibrationEntry *) realloc(_entries,
                                                                    newCapacity * sizeof(CalibrationEntry));

        if (newEntries) {
            _entries = newEntries;
            _entryCapacity = newCapacity;

            for (uint16_t i = _entryCount; i < _entryCapacity; i++) {
                _entries[i].sensor = nullptr;
                _entries[i].sensorName = nullptr;
                _entries[i].valueKey = nullptr;
                _entries[i].calibrator = nullptr;
                _entries[i].isActive = false;
            }
        } else {
            return false;
        }
    }

    auto *calibrator = new InteractiveSerialGeneralSensorCalibratorV2();
    if (!calibrator) {
        return false;
    }

    const SensorValueInfo *info = sensor->getValueInfoByKey(valueKey);
    if (info) {
        calibrator->setDisplayUnits(info->unit);
        calibrator->setDisplayPrecision(info->precision);
    }

    int eepromAddr = _eepromStartAddress + (_entryCount * 50);
    calibrator->setEEPROMAddress(eepromAddr);
    calibrator->setSensorValueContext(sensor, valueKey);
    calibrator->setCompletedCallback(calibrationCompletedCallback, this);

    _entries[_entryCount].sensor = sensor;
    _entries[_entryCount].sensorName = strdup(sensorName);
    _entries[_entryCount].valueKey = strdup(valueKey);
    _entries[_entryCount].calibrator = calibrator;
    _entries[_entryCount].isActive = true;

    _entryCount++;
    return true;
}

bool SensorCalibrationModuleV2::addCalibrationEntry(const char *sensorName, const char *valueKey) {
    BaseSensV2 *sensor = getSensorByName(sensorName);
    if (!sensor) return false;

    return addCalibrationEntry(sensor, sensorName, valueKey);
}

void SensorCalibrationModuleV2::calibrateOnePoint(const char *sensorName, const char *valueKey, float knownValue) {
    int index = findEntryIndex(sensorName, valueKey);
    if (index < 0) return;

    float rawValue = getRawValue(sensorName, valueKey);

    if (rawValue == 0) {
        rawValue = 0.0001;
    }

    if (_entries[index].calibrator) {
        _entries[index].calibrator->calibrateOnePoint(knownValue, rawValue);

        if (_autoSaveCalibration) {
            int addr = _eepromStartAddress + (index * 50);
            _entries[index].calibrator->saveCalibration(addr);

#if defined(ARDUINO_ARCH_ESP8266) || defined(ARDUINO_ARCH_ESP32)
            EEPROM.commit();
#endif
        }
    }
}

void SensorCalibrationModuleV2::calibrateTwoPoint(const char *sensorName, const char *valueKey,
                                                  float knownValue1, float rawValue1,
                                                  float knownValue2, float rawValue2) {
    int index = findEntryIndex(sensorName, valueKey);
    if (index < 0) return;

    if (_entries[index].calibrator) {
        _entries[index].calibrator->calibrateTwoPoint(knownValue1, rawValue1, knownValue2, rawValue2);

        if (_autoSaveCalibration) {
            int addr = _eepromStartAddress + (index * 50);
            _entries[index].calibrator->saveCalibration(addr);

#if defined(ARDUINO_ARCH_ESP8266) || defined(ARDUINO_ARCH_ESP32)
            EEPROM.commit();
#endif
        }
    }
}

void SensorCalibrationModuleV2::startCalibrationMode(Stream *serialPtr, uint32_t timeout) {
    _serial = serialPtr;
    _calibrationMode = true;
    _calibrationModeTimeout = timeout;
    _lastCalibrationActivity = millis();

    if (_serial) {
        _serial->println("\n===== SENSOR CALIBRATION MODE V2 =====");
        _serial->println("Type 'h' for help, 'x' to exit");

        printCalibrationMenu();
    }
}

void SensorCalibrationModuleV2::startCalibrationMode(uint32_t timeout) {
    _calibrationMode = true;
    _calibrationModeTimeout = timeout;
    _lastCalibrationActivity = millis();

    if (_serial) {
        _serial->println("\n===== SENSOR CALIBRATION MODE V2 =====");
        _serial->println("Type 'h' for help, 'x' to exit");

        printCalibrationMenu();
    }
}

void SensorCalibrationModuleV2::stopCalibrationMode() {
    if (_calibrationMode && _serial) {
        _serial->println("\n===== EXITING CALIBRATION MODE =====");
    }

    _calibrationMode = false;
}

bool SensorCalibrationModuleV2::isInCalibrationMode() const {
    return _calibrationMode;
}

float SensorCalibrationModuleV2::getRawValue(const char *sensorName, const char *valueKey) const {
    BaseSensV2 *sensor = getSensorByName(sensorName);
    if (sensor) {
        return sensor->getFloatValue(valueKey);
    }
    return 0.0f;
}

float SensorCalibrationModuleV2::getCalibratedValue(const char *sensorName, const char *valueKey) const {
    int index = findEntryIndex(sensorName, valueKey);
    if (index < 0) {
        return getRawValue(sensorName, valueKey);
    }

    if (_entries[index].calibrator && _entries[index].calibrator->isCalibrated()) {
        return _entries[index].calibrator->readCalibratedValue();
    } else {
        return getRawValue(sensorName, valueKey);
    }
}

bool SensorCalibrationModuleV2::saveAllCalibrations(int baseEepromAddress) {
    bool allSuccess = true;
    int actualBaseAddress = (baseEepromAddress != 0) ? baseEepromAddress : _eepromStartAddress;

    for (uint16_t i = 0; i < _entryCount; i++) {
        if (_entries[i].isActive && _entries[i].calibrator && _entries[i].calibrator->isCalibrated()) {
            int addr = actualBaseAddress + (i * 50);
            bool success = _entries[i].calibrator->saveCalibration(addr);
            if (!success) allSuccess = false;
        }
    }

#if defined(ARDUINO_ARCH_ESP8266) || defined(ARDUINO_ARCH_ESP32)
    EEPROM.commit();
#endif

    return allSuccess;
}

bool SensorCalibrationModuleV2::loadAllCalibrations(int baseEepromAddress) {
    CalibrationLoadResult result = loadAllCalibrationsWithStatus(baseEepromAddress);
    return (result.successCount == result.totalEntries);
}

CalibrationLoadResult SensorCalibrationModuleV2::loadAllCalibrationsWithStatus(int baseEepromAddress) {
    CalibrationLoadResult result = {0, 0, 0, 0};
    result.totalEntries = _entryCount;

    int actualBaseAddress = (baseEepromAddress != 0) ? baseEepromAddress : _eepromStartAddress;

    for (uint16_t i = 0; i < _entryCount; i++) {
        if (_entries[i].isActive && _entries[i].calibrator) {
            int addr = actualBaseAddress + (i * 50);
            _entries[i].calibrator->setEEPROMAddress(addr);

            bool success = false;
            try {
                success = _entries[i].calibrator->loadCalibration(addr);
            } catch (...) {
                success = false;
            }

            if (success) {
                result.successCount++;
            } else {
                uint8_t calType = EEPROM.read(addr);
                if (calType > CALIBRATION_MULTI_POINT) {
                    result.notCalibratedCount++;
                } else {
                    result.errorCount++;
                }
            }
        }
    }

    return result;
}

void SensorCalibrationModuleV2::setAutoSaveCalibration(bool autoSave) {
    _autoSaveCalibration = autoSave;
}

bool SensorCalibrationModuleV2::getAutoSaveCalibration() const {
    return _autoSaveCalibration;
}

void SensorCalibrationModuleV2::setEEPROMStartAddress(int address) {
    _eepromStartAddress = address;

    for (uint16_t i = 0; i < _entryCount; i++) {
        if (_entries[i].isActive && _entries[i].calibrator) {
            int addr = _eepromStartAddress + (i * 50);
            _entries[i].calibrator->setEEPROMAddress(addr);
        }
    }
}

int SensorCalibrationModuleV2::getEEPROMStartAddress() const {
    return _eepromStartAddress;
}

void SensorCalibrationModuleV2::setCalibrationTimeout(unsigned long timeout) {
    _timeout = timeout;
}

unsigned long SensorCalibrationModuleV2::getCalibrationTimeout() const {
    return _timeout;
}

void SensorCalibrationModuleV2::listAllSensorValues() {
    if (!_serial) return;

    _serial->println("\n===== AVAILABLE SENSOR VALUES =====");

    for (uint8_t i = 0; i < getSensorCount(); i++) {
        BaseSensV2 *sensor = getSensor(i);
        const char *sensorName = getSensorName(i);

        if (!sensor || !sensorName) continue;

        _serial->print("Sensor: ");
        _serial->println(sensorName);

        for (uint8_t j = 0; j < sensor->getValueCount(); j++) {
            const SensorValueInfo *info = sensor->getValueInfo(j);
            if (!info) continue;

            _serial->print("  - ");
            _serial->print(info->label);
            _serial->print(" (");
            _serial->print(info->key);
            _serial->print("): ");

            switch (info->type) {
                case TYPE_FLOAT: {
                    float value = sensor->getFloatValue(info->key);
                    _serial->print(value, info->precision);
                    break;
                }
                case TYPE_INT: {
                    int value = sensor->getIntValue(info->key);
                    _serial->print(value);
                    break;
                }
                case TYPE_STRING: {
                    const char *value = sensor->getStringValue(info->key);
                    if (value) _serial->print(value);
                    break;
                }
            }

            if (info->unit && strlen(info->unit) > 0) {
                _serial->print(" ");
                _serial->print(info->unit);
            }

            int calIndex = findEntryIndex(sensorName, info->key);
            if (calIndex >= 0 && _entries[calIndex].calibrator && _entries[calIndex].calibrator->isCalibrated()) {
                _serial->print(" [Calibrated]");
            }

            _serial->println();
        }
    }
}

void SensorCalibrationModuleV2::printCalibrationStatus() {
    if (!_serial) return;

    _serial->println("\n===== DETAILED CALIBRATION STATUS =====");

    uint8_t sensorCount = getSensorCount();
    if (sensorCount == 0) {
        _serial->println("No sensors available.");
        return;
    }

    for (uint8_t i = 0; i < sensorCount; i++) {
        BaseSensV2 *sensor = getSensor(i);
        const char *sensorName = getSensorName(i);

        if (!sensor || !sensorName) continue;

        uint8_t valueCount = sensor->getValueCount();
        if (valueCount == 0) continue;

        for (uint8_t j = 0; j < valueCount; j++) {
            const SensorValueInfo *info = sensor->getValueInfo(j);
            if (!info || !info->key) continue;

            _serial->print(sensorName);
            _serial->print(" - ");
            _serial->print(info->label ? info->label : "");
            _serial->print(": ");

            float rawValue = sensor->getFloatValue(info->key);
            float calibratedValue = getCalibratedValue(sensorName, info->key);

            int idx = findEntryIndex(sensorName, info->key);
            bool hasCalibration = (idx >= 0 && _entries && _entries[idx].isActive &&
                                   _entries[idx].calibrator && _entries[idx].calibrator->isCalibrated());

            if (hasCalibration) {
                _serial->print("CALIBRATED (");

                if (_entries[idx].calibrator) {
                    uint8_t method = _entries[idx].calibrator->getCalibrationMethod();
                    switch (method) {
                        case CALIBRATION_ONE_POINT:
                            _serial->print("One-Point, ");
                            break;
                        case CALIBRATION_TWO_POINT:
                            _serial->print("Two-Point, ");
                            break;
                        case CALIBRATION_MULTI_POINT:
                            _serial->print("Multi-Point, ");
                            break;
                        default:
                            _serial->print("Unknown, ");
                            break;
                    }
                }

                _serial->print("Raw: ");
                _serial->print(rawValue, info->precision);
                _serial->print(" → Calibrated: ");
                _serial->print(calibratedValue, info->precision);

                if (info->unit && strlen(info->unit) > 0) {
                    _serial->print(" ");
                    _serial->print(info->unit);
                }

                _serial->println(")");
            } else {
                _serial->print("Not calibrated (Raw: ");
                _serial->print(rawValue, info->precision);

                if (info->unit && strlen(info->unit) > 0) {
                    _serial->print(" ");
                    _serial->print(info->unit);
                }

                _serial->println(")");
            }
        }
    }
}

void SensorCalibrationModuleV2::clearSerialBuffer() const {
    if (!_serial) return;

    while (_serial->available()) {
        _serial->read();
    }
}

bool SensorCalibrationModuleV2::isCalibrationModeTimedOut() const {
    return (millis() - _lastCalibrationActivity) > _calibrationModeTimeout;
}

void SensorCalibrationModuleV2::processCalibrationCommand(char cmd) {
    if (cmd == '\r' || cmd == '\n') return;

    switch (cmd) {
        case 'h':
        case 'H':
        case '?':
            printCalibrationMenu();
            break;

        case 'x':
        case 'X':
        case 'q':
        case 'Q':
            stopCalibrationMode();
            break;

        case 'l':
        case 'L':
            listAllSensorValues();
            break;

        case 's':
        case 'S':
            if (saveAllCalibrations(_eepromStartAddress)) {
                _serial->println("All calibrations saved successfully");
            } else {
                _serial->println("Some calibrations failed to save");
            }
            break;

        case 'r':
        case 'R': {
            CalibrationLoadResult result = loadAllCalibrationsWithStatus(_eepromStartAddress);
            _serial->println("\n===== LOADING CALIBRATIONS =====");
            _serial->print("Total sensors: ");
            _serial->println(result.totalEntries);

            if (result.successCount > 0) {
                _serial->print("Successfully loaded: ");
                _serial->println(result.successCount);
            }

            if (result.notCalibratedCount > 0) {
                _serial->print("Not yet calibrated: ");
                _serial->println(result.notCalibratedCount);
            }

            if (result.errorCount > 0) {
                _serial->print("Error loading: ");
                _serial->println(result.errorCount);
            }

            printCalibrationStatus();
            break;
        }

        case 'a':
        case 'A':
            _autoSaveCalibration = !_autoSaveCalibration;
            _serial->print("Auto-save calibration is now ");
            _serial->println(_autoSaveCalibration ? "ON" : "OFF");
            break;

        default:
            if (cmd >= '1' && cmd <= '9') {
                int index = cmd - '1';
                if (index < _entryCount && _entries[index].isActive) {
                    _activeEntryIndex = index;
                    startCalibratorForEntry(index);
                } else {
                    _serial->println("Invalid selection");
                }
            }
            break;
    }
}

void SensorCalibrationModuleV2::printCalibrationMenu() {
    if (!_serial) return;

    _serial->println("\n===== SENSOR VALUE CALIBRATION MENU =====");
    _serial->println("Available Values:");

    for (uint16_t i = 0; i < _entryCount; i++) {
        if (!_entries[i].isActive) continue;

        const SensorValueInfo *info = _entries[i].sensor->getValueInfoByKey(_entries[i].valueKey);

        _serial->print(i + 1);
        _serial->print(": ");
        _serial->print(_entries[i].sensorName);
        _serial->print(" - ");

        if (info) {
            _serial->print(info->label);
        } else {
            _serial->print(_entries[i].valueKey);
        }

        if (_entries[i].calibrator && _entries[i].calibrator->isCalibrated()) {
            _serial->print(" [Calibrated]");
        }

        _serial->println();
    }

    _serial->println("\nL: List All Sensor Values");
    _serial->println("S: Save All Calibrations");
    _serial->println("R: Load All Calibrations");
    _serial->println("A: Toggle Auto-save");
    _serial->println("H: Show This Menu");
    _serial->println("X: Exit Calibration Mode");
}

int SensorCalibrationModuleV2::findEntryIndex(const char *sensorName, const char *valueKey) const {
    if (!sensorName || !valueKey || !_entries) {
        return -1;
    }

    for (uint16_t i = 0; i < _entryCount; i++) {
        if (_entries[i].isActive &&
            _entries[i].sensorName &&
            _entries[i].valueKey &&
            strcmp(_entries[i].sensorName, sensorName) == 0 &&
            strcmp(_entries[i].valueKey, valueKey) == 0) {
            return i;
        }
    }
    return -1;
}

void SensorCalibrationModuleV2::calibrationCompletedCallback(void *context) {
    SensorCalibrationModuleV2 *instance = static_cast<SensorCalibrationModuleV2 *>(context);
    if (instance) {
        instance->_lastCalibrationActivity = millis();
    }
}

void SensorCalibrationModuleV2::startCalibratorForEntry(uint16_t entryIndex) {
    if (entryIndex >= _entryCount || !_entries[entryIndex].isActive) {
        return;
    }

    CalibrationEntry &entry = _entries[entryIndex];
    if (!entry.sensor || !entry.calibrator) {
        return;
    }

    const SensorValueInfo *info = entry.sensor->getValueInfoByKey(entry.valueKey);
    const char *valueLabel = (info) ? info->label : entry.valueKey;

    int eepromAddr = _eepromStartAddress + (entryIndex * 50);

    entry.calibrator->setSensorInfo(entry.sensorName, valueLabel);
    entry.calibrator->setEEPROMAddress(eepromAddr);
    entry.calibrator->begin(_serial, _timeout, eepromAddr);
}
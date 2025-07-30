#include "SensorCalibrationModuleV2.h"

SensorCalibrationModuleV2::SensorCalibrationModuleV2()
        : SensorModuleV2(),
          _entries(nullptr),
          _entryCount(0),
          _entryCapacity(0),
          _sensorControls(nullptr),
          _sensorControlCount(0),
          _sensorControlCapacity(0),
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
            _entries[i].isEnabled = true;
            _entries[i].valueTypeCode = TYPE_UNKNOWN;
        }
    }

    _sensorControlCapacity = 8;
    _sensorControls = (SensorCalibrationControl *) malloc(_sensorControlCapacity * sizeof(SensorCalibrationControl));
    if (_sensorControls) {
        for (uint8_t i = 0; i < _sensorControlCapacity; i++) {
            _sensorControls[i].sensorName = nullptr;
            _sensorControls[i].isEnabled = true;
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

    if (_sensorControls) {
        for (uint8_t i = 0; i < _sensorControlCount; i++) {
            if (_sensorControls[i].sensorName) {
                free(_sensorControls[i].sensorName);
            }
        }
        free(_sensorControls);
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

void SensorCalibrationModuleV2::discoverCalibrableValues() {
    for (uint8_t i = 0; i < getSensorCount(); i++) {
        BaseSensV2 *sensor = getSensor(i);
        const char *sensorName = getSensorName(i);

        if (!sensor || !sensor->isCalibrationEnabled() ||
            !isSensorCalibrationEnabled(sensorName))
            continue;

        for (uint8_t j = 0; j < sensor->getValueCount(); j++) {
            const SensorValueInfo *info = sensor->getValueInfo(j);
            if (info && sensor->isNumericValue(info->key) &&
                sensor->isValueCalibrable(info->key)) {
                addCalibrationEntry(sensor, sensorName, info->key);
            }
        }
    }
}

void SensorCalibrationModuleV2::discoverNestedCalibrableValues() {
    for (uint8_t i = 0; i < getSensorCount(); i++) {
        BaseSensV2 *sensor = getSensor(i);
        const char *sensorName = getSensorName(i);

        if (!sensor || !sensor->isCalibrationEnabled() ||
            !isSensorCalibrationEnabled(sensorName))
            continue;

        JsonDocument *doc = sensor->getDocument();
        if (doc && (*doc)[sensorName].is<JsonObject>()) {
            JsonVariant sensorObj = (*doc)[sensorName];
            extractNestedPaths(sensor, sensorName, sensorObj);
        }

        for (uint8_t j = 0; j < sensor->getValueCount(); j++) {
            const SensorValueInfo *info = sensor->getValueInfo(j);
            if (info && isPathCalibrable(sensor, info->key)) {
                addCalibrationEntry(sensor, sensorName, info->key);
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
                _entries[i].isEnabled = true;
                _entries[i].valueTypeCode = TYPE_UNKNOWN;
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

    char* sensorNameDup = strdup(sensorName);
    char* valueKeyDup = strdup(valueKey);
    
    if (!sensorNameDup || !valueKeyDup) {
        if (sensorNameDup) free(sensorNameDup);
        if (valueKeyDup) free(valueKeyDup);
        delete calibrator;
        return false;
    }
    
    _entries[_entryCount].sensor = sensor;
    _entries[_entryCount].sensorName = sensorNameDup;
    _entries[_entryCount].valueKey = valueKeyDup;
    _entries[_entryCount].calibrator = calibrator;
    _entries[_entryCount].isActive = true;
    _entries[_entryCount].isEnabled = sensor->isValueCalibrable(valueKey);

    if (strchr(valueKey, '.') || strchr(valueKey, '[')) {
        _entries[_entryCount].valueTypeCode = sensor->getTypeAtPath(valueKey);
    } else {
        _entries[_entryCount].valueTypeCode = sensor->getValueTypeCode(valueKey);
    }

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

    float rawValue = getRawValue<float>(sensorName, valueKey);

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
    return getRawValue<float>(sensorName, valueKey);
}

float SensorCalibrationModuleV2::getCalibratedValue(const char *sensorName, const char *valueKey) const {
    return getCalibratedValue<float>(sensorName, valueKey);
}

bool SensorCalibrationModuleV2::enableValueCalibration(const char *sensorName, const char *valueKey, bool enable) {
    int index = findEntryIndex(sensorName, valueKey);
    if (index < 0) return false;

    _entries[index].isEnabled = enable;

    BaseSensV2 *sensor = getSensorByName(sensorName);
    if (sensor) {
        sensor->setValueCalibrable(valueKey, enable);
    }

    return true;
}

bool SensorCalibrationModuleV2::isValueCalibrationEnabled(const char *sensorName, const char *valueKey) const {
    int index = findEntryIndex(sensorName, valueKey);
    if (index < 0) {
        BaseSensV2 *sensor = getSensorByName(sensorName);
        if (sensor) {
            return sensor->isValueCalibrable(valueKey);
        }
        return false;
    }

    return _entries[index].isEnabled;
}

void SensorCalibrationModuleV2::enableSensorCalibration(const char *sensorName, bool enable) {
    int index = findSensorControlIndex(sensorName);
    if (index >= 0) {
        _sensorControls[index].isEnabled = enable;
    } else {
        addSensorControl(sensorName, enable);
    }

    BaseSensV2 *sensor = getSensorByName(sensorName);
    if (sensor) {
        sensor->enableCalibration(enable);
    }
}

bool SensorCalibrationModuleV2::isSensorCalibrationEnabled(const char *sensorName) const {
    int index = findSensorControlIndex(sensorName);
    if (index >= 0) {
        return _sensorControls[index].isEnabled;
    }
    return true;
}

void SensorCalibrationModuleV2::enableAllCalibration(bool enable) {
    for (uint8_t i = 0; i < getSensorCount(); i++) {
        const char *sensorName = getSensorName(i);
        if (sensorName) {
            enableSensorCalibration(sensorName, enable);
        }
    }

    for (uint16_t i = 0; i < _entryCount; i++) {
        if (_entries[i].isActive) {
            _entries[i].isEnabled = enable;
        }
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

            bool success = _entries[i].calibrator->loadCalibration(addr);

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

            SensorTypeCode typeCode;
            if (strchr(info->key, '.') || strchr(info->key, '[')) {
                typeCode = sensor->getTypeAtPath(info->key);
            } else {
                typeCode = sensor->getValueTypeCode(info->key);
            }

            switch (typeCode) {
                case TYPE_FLOAT: {
                    float value = sensor->getValue<float>(info->key);
                    _serial->print(value, info->precision);
                    break;
                }
                case TYPE_INT: {
                    int value = sensor->getValue<int>(info->key);
                    _serial->print(value);
                    break;
                }
                case TYPE_STRING: {
                    String value = sensor->getValue<String>(info->key);
                    _serial->print(value);
                    break;
                }
                case TYPE_BOOL: {
                    bool value = sensor->getValue<bool>(info->key);
                    _serial->print(value ? "true" : "false");
                    break;
                }
                case TYPE_LONG: {
                    long value = sensor->getValue<long>(info->key);
                    _serial->print(value);
                    break;
                }
                case TYPE_DOUBLE: {
                    double value = sensor->getValue<double>(info->key);
                    _serial->print(value, info->precision);
                    break;
                }
                default: {
                    _serial->print("N/A");
                    break;
                }
            }

            if (info->unit && strlen(info->unit) > 0) {
                _serial->print(" ");
                _serial->print(info->unit);
            }

            _serial->print(" [");
            _serial->print(BaseSensV2::getTypeCodeName(typeCode));
            _serial->print("]");

            int calIndex = findEntryIndex(sensorName, info->key);
            if (calIndex >= 0 && _entries[calIndex].calibrator && _entries[calIndex].calibrator->isCalibrated()) {
                _serial->print(" [Calibrated]");
            }

            if (sensor->isNumericValue(info->key)) {
                if (sensor->isValueCalibrable(info->key)) {
                    _serial->print(" [Calibrable]");
                } else {
                    _serial->print(" [Cal-Disabled]");
                }
            }

            _serial->println();
        }
    }
}

void SensorCalibrationModuleV2::listCalibrableValues(bool showDisabled) {
    if (!_serial) return;

    _serial->println("\n===== CALIBRABLE VALUES =====");

    for (uint16_t i = 0; i < _entryCount; i++) {
        if (!_entries[i].isActive) continue;
        if (!showDisabled && !_entries[i].isEnabled) continue;

        const SensorValueInfo *info = _entries[i].sensor->getValueInfoByKey(_entries[i].valueKey);

        _serial->print(i + 1);
        _serial->print(": ");
        _serial->print(_entries[i].sensorName);
        _serial->print(" - ");

        if (info) {
            _serial->print(info->label);
        } else {
            String cleanPath = String(_entries[i].valueKey);
            cleanPath.replace(".", " → ");
            cleanPath.replace("[", " [");
            cleanPath.replace("]", "]");
            _serial->print(cleanPath);
        }

        _serial->print(" [");
        _serial->print(BaseSensV2::getTypeCodeName(_entries[i].valueTypeCode));
        _serial->print("]");

        if (_entries[i].calibrator && _entries[i].calibrator->isCalibrated()) {
            _serial->print(" [Calibrated]");
        }

        if (!_entries[i].isEnabled) {
            _serial->print(" [DISABLED]");
        }

        _serial->println();
    }
}

void SensorCalibrationModuleV2::listNumericValues() {
    if (!_serial) return;

    _serial->println("\n===== NUMERIC SENSOR VALUES =====");

    for (uint8_t i = 0; i < getSensorCount(); i++) {
        BaseSensV2 *sensor = getSensor(i);
        const char *sensorName = getSensorName(i);

        if (!sensor || !sensorName) continue;

        bool hasNumericValues = false;
        for (uint8_t j = 0; j < sensor->getValueCount(); j++) {
            const SensorValueInfo *info = sensor->getValueInfo(j);
            if (info && sensor->isNumericValue(info->key)) {
                if (!hasNumericValues) {
                    _serial->print("Sensor: ");
                    _serial->println(sensorName);
                    hasNumericValues = true;
                }

                _serial->print("  - ");
                _serial->print(info->label);
                _serial->print(" (");
                _serial->print(info->key);
                _serial->print("): ");

                SensorTypeCode typeCode = sensor->getValueTypeCode(info->key);
                switch (typeCode) {
                    case TYPE_FLOAT: {
                        float value = sensor->getValue<float>(info->key);
                        _serial->print(value, info->precision);
                        break;
                    }
                    case TYPE_INT: {
                        int value = sensor->getValue<int>(info->key);
                        _serial->print(value);
                        break;
                    }
                    case TYPE_LONG: {
                        long value = sensor->getValue<long>(info->key);
                        _serial->print(value);
                        break;
                    }
                    case TYPE_DOUBLE: {
                        double value = sensor->getValue<double>(info->key);
                        _serial->print(value, info->precision);
                        break;
                    }
                    default:
                        _serial->print("N/A");
                        break;
                }

                if (info->unit && strlen(info->unit) > 0) {
                    _serial->print(" ");
                    _serial->print(info->unit);
                }

                _serial->print(" [");
                _serial->print(BaseSensV2::getTypeCodeName(typeCode));
                _serial->print("]");

                _serial->println();
            }
        }
    }
}

void SensorCalibrationModuleV2::listSensorCalibrationStatus() {
    if (!_serial) return;

    _serial->println("\n===== SENSOR CALIBRATION STATUS =====");

    for (uint8_t i = 0; i < getSensorCount(); i++) {
        BaseSensV2 *sensor = getSensor(i);
        const char *sensorName = getSensorName(i);

        if (!sensor || !sensorName) continue;

        _serial->print("Sensor: ");
        _serial->print(sensorName);
        _serial->print(" - Master Calibration: ");
        _serial->print(sensor->isCalibrationEnabled() ? "ENABLED" : "DISABLED");
        _serial->print(" - Module Control: ");
        _serial->println(isSensorCalibrationEnabled(sensorName) ? "ENABLED" : "DISABLED");

        for (uint8_t j = 0; j < sensor->getValueCount(); j++) {
            const SensorValueInfo *info = sensor->getValueInfo(j);
            if (info && sensor->isNumericValue(info->key)) {
                _serial->print("  - ");
                _serial->print(info->label);
                _serial->print(": ");

                if (sensor->isValueCalibrable(info->key)) {
                    _serial->print("Calibrable");

                    int calIndex = findEntryIndex(sensorName, info->key);
                    if (calIndex >= 0) {
                        if (_entries[calIndex].calibrator && _entries[calIndex].calibrator->isCalibrated()) {
                            _serial->print(" & Calibrated");
                        }
                        if (!_entries[calIndex].isEnabled) {
                            _serial->print(" but DISABLED");
                        }
                    }
                } else {
                    _serial->print("Not Calibrable");
                }

                _serial->println();
            }
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

        case 'c':
        case 'C':
            listCalibrableValues(true);
            break;

        case 'n':
        case 'N':
            listNumericValues();
            break;

        case 't':
        case 'T':
            listSensorCalibrationStatus();
            break;

        case 'd':
        case 'D':
            discoverNestedCalibrableValues();
            _serial->println("Nested calibrable values discovered");
            printCalibrationMenu();
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

        case 'e':
        case 'E': {
            _serial->print("Enter entry number to toggle: ");
            clearSerialBuffer();

            while (!_serial->available()) {
                delay(10);
            }

            int entryNum = _serial->parseInt();
            clearSerialBuffer();
            _serial->println(entryNum);

            if (entryNum > 0 && entryNum <= _entryCount) {
                int index = entryNum - 1;
                if (_entries[index].isActive) {
                    _entries[index].isEnabled = !_entries[index].isEnabled;
                    _serial->print("Entry ");
                    _serial->print(entryNum);
                    _serial->print(" calibration is now ");
                    _serial->println(_entries[index].isEnabled ? "ENABLED" : "DISABLED");
                }
            } else {
                _serial->println("Invalid entry number");
            }
            break;
        }

        default:
            if (cmd >= '1' && cmd <= '9') {
                int index = cmd - '1';
                if (index < _entryCount && _entries[index].isActive) {
                    if (_entries[index].isEnabled) {
                        _activeEntryIndex = index;
                        startCalibratorForEntry(index);
                    } else {
                        _serial->println("Calibration is disabled for this value");
                    }
                } else {
                    _serial->println("Invalid selection");
                }
            }
            break;
    }
}

void SensorCalibrationModuleV2::printCalibrationMenu() {
    if (!_serial) return;

    _serial->println("\n===== SENSOR CALIBRATION MENU V2 =====");
    _serial->println("Calibrable Values:");

    uint8_t displayCount = 0;
    for (uint16_t i = 0; i < _entryCount; i++) {
        if (!_entries[i].isActive) continue;

        displayCount++;
        _serial->print(displayCount);
        _serial->print(": ");
        _serial->print(_entries[i].sensorName);
        _serial->print(" - ");

        const SensorValueInfo *info = _entries[i].sensor->getValueInfoByKey(_entries[i].valueKey);
        if (info) {
            _serial->print(info->label);
        } else {
            String cleanPath = String(_entries[i].valueKey);
            cleanPath.replace(".", " → ");
            cleanPath.replace("[", " [");
            cleanPath.replace("]", "]");
            _serial->print(cleanPath);
        }

        _serial->print(" [");
        _serial->print(BaseSensV2::getTypeCodeName(_entries[i].valueTypeCode));
        _serial->print("]");

        if (_entries[i].calibrator && _entries[i].calibrator->isCalibrated()) {
            _serial->print(" [Calibrated]");
        }

        if (!_entries[i].isEnabled) {
            _serial->print(" [DISABLED]");
        }

        _serial->println();
    }

    _serial->println("\nE: Enable/Disable Entry (E<num>)");
    _serial->println("L: List All Sensor Values");
    _serial->println("C: List Calibrable Values");
    _serial->println("N: List Numeric Values Only");
    _serial->println("T: List Sensor Calibration Status");
    _serial->println("D: Discover Nested Calibrable Values");
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

int SensorCalibrationModuleV2::findSensorControlIndex(const char *sensorName) const {
    if (!sensorName || !_sensorControls) {
        return -1;
    }

    for (uint8_t i = 0; i < _sensorControlCount; i++) {
        if (_sensorControls[i].sensorName &&
            strcmp(_sensorControls[i].sensorName, sensorName) == 0) {
            return i;
        }
    }
    return -1;
}

void SensorCalibrationModuleV2::addSensorControl(const char *sensorName, bool enabled) {
    if (!sensorName) return;

    if (_sensorControlCount >= _sensorControlCapacity) {
        uint8_t newCapacity = _sensorControlCapacity + 4;
        SensorCalibrationControl *newControls = (SensorCalibrationControl *) realloc(
                _sensorControls, newCapacity * sizeof(SensorCalibrationControl));

        if (newControls) {
            _sensorControls = newControls;
            _sensorControlCapacity = newCapacity;

            for (uint8_t i = _sensorControlCount; i < _sensorControlCapacity; i++) {
                _sensorControls[i].sensorName = nullptr;
                _sensorControls[i].isEnabled = true;
            }
        } else {
            return;
        }
    }

    _sensorControls[_sensorControlCount].sensorName = strdup(sensorName);
    _sensorControls[_sensorControlCount].isEnabled = enabled;
    _sensorControlCount++;
}

void SensorCalibrationModuleV2::extractNestedPaths(BaseSensV2 *sensor, const char *sensorName, JsonVariant obj, String currentPath) {
    if (obj.is<JsonObject>()) {
        JsonObject jsonObj = obj.as<JsonObject>();
        for (JsonPair kv: jsonObj) {
            String newPath = currentPath.length() > 0 ? currentPath + "." + kv.key().c_str() : String(kv.key().c_str());

            if (kv.value().is<JsonObject>() || kv.value().is<JsonArray>()) {
                extractNestedPaths(sensor, sensorName, kv.value(), newPath);
            } else if (isPathCalibrable(sensor, newPath.c_str())) {
                addCalibrationEntry(sensor, sensorName, newPath.c_str());
            }
        }
    } else if (obj.is<JsonArray>()) {
        JsonArray jsonArray = obj.as<JsonArray>();
        for (size_t i = 0; i < jsonArray.size(); i++) {
            String newPath = currentPath + "[" + String(i) + "]";

            if (jsonArray[i].is<JsonObject>() || jsonArray[i].is<JsonArray>()) {
                extractNestedPaths(sensor, sensorName, jsonArray[i], newPath);
            } else if (isPathCalibrable(sensor, newPath.c_str())) {
                addCalibrationEntry(sensor, sensorName, newPath.c_str());
            }
        }
    }
}

bool SensorCalibrationModuleV2::isPathCalibrable(BaseSensV2 *sensor, const char *path) const {
    if (!sensor || !path) return false;

    if (!sensor->hasPath(path)) return false;

    SensorTypeCode typeCode = sensor->getTypeAtPath(path);
    if (!BaseSensV2::isNumericTypeCode(typeCode)) return false;

    return sensor->isValueCalibrable(path);
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
    const char *valueLabel;

    if (info) {
        valueLabel = info->label;
    } else {
        valueLabel = entry.valueKey;
    }

    int eepromAddr = _eepromStartAddress + (entryIndex * 50);

    entry.calibrator->setSensorInfo(entry.sensorName, valueLabel);
    entry.calibrator->setEEPROMAddress(eepromAddr);
    entry.calibrator->begin(_serial, _timeout, eepromAddr);
}
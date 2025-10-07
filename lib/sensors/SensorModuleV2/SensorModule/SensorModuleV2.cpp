#include "SensorModuleV2.h"
#include "Utils/SensorUtilityV2.h"

BaseSensV2::BaseSensV2() : _valueInfos(nullptr), _valueCount(0), _valueCapacity(0),
                           _doc(nullptr), _name(nullptr),
                           _calibrationConfigs(nullptr), _configCount(0), _configCapacity(0),
                           _calibrationEnabled(true) {
    _valueCapacity = 4;
    _valueInfos = (SensorValueInfo **) malloc(_valueCapacity * sizeof(SensorValueInfo *));

    _configCapacity = 8;
    _calibrationConfigs = (CalibrationConfig *) malloc(_configCapacity * sizeof(CalibrationConfig));
    for (uint8_t i = 0; i < _configCapacity; i++) {
        _calibrationConfigs[i].valueKey = nullptr;
        _calibrationConfigs[i].isCalibrable = true;
    }
}

BaseSensV2::~BaseSensV2() {
    if (_valueInfos) {
        for (uint8_t i = 0; i < _valueCount; i++) {
            if (_valueInfos[i]) {
                delete _valueInfos[i];
            }
        }
        free(_valueInfos);
    }

    if (_name) free(_name);

    if (_calibrationConfigs) {
        for (uint8_t i = 0; i < _configCount; i++) {
            if (_calibrationConfigs[i].valueKey) {
                free(_calibrationConfigs[i].valueKey);
            }
        }
        free(_calibrationConfigs);
    }
}

bool BaseSensV2::hasValue(const char *key) const {
    if (!_doc || !_name) return false;
    return (*_doc)[_name][key].is<JsonVariant>();
}

bool BaseSensV2::isNumericValue(const char *key) const {
    SensorTypeCode typeCode = getValueTypeCode(key);
    return isNumericTypeCode(typeCode);
}

SensorTypeCode BaseSensV2::getValueTypeCode(const char *key) const {
    if (!_doc || !_name) return TYPE_UNKNOWN;

    JsonVariant v = (*_doc)[_name][key];
    if (v.is<bool>()) return TYPE_BOOL;
    if (v.is<int>()) return TYPE_INT;
    if (v.is<float>()) return TYPE_FLOAT;
    if (v.is<double>()) return TYPE_DOUBLE;
    if (v.is<long>()) return TYPE_LONG;
    if (v.is<unsigned int>()) return TYPE_UINT;
    if (v.is<unsigned long>()) return TYPE_ULONG;
    if (v.is<const char *>()) return TYPE_STRING;
    if (v.is<JsonObject>()) return TYPE_OBJECT;
    if (v.is<JsonArray>()) return TYPE_ARRAY;
    return TYPE_UNKNOWN;
}

const char *BaseSensV2::getTypeCodeName(SensorTypeCode typeCode) {
    switch (typeCode) {
        case TYPE_BOOL:
            return "bool";
        case TYPE_INT:
            return "int";
        case TYPE_FLOAT:
            return "float";
        case TYPE_DOUBLE:
            return "double";
        case TYPE_LONG:
            return "long";
        case TYPE_UINT:
            return "uint";
        case TYPE_ULONG:
            return "ulong";
        case TYPE_STRING:
            return "string";
        case TYPE_OBJECT:
            return "object";
        case TYPE_ARRAY:
            return "array";
        case TYPE_UNKNOWN:
        default:
            return "unknown";
    }
}

bool BaseSensV2::isNumericTypeCode(SensorTypeCode typeCode) {
    return typeCode == TYPE_INT || typeCode == TYPE_FLOAT || typeCode == TYPE_DOUBLE ||
           typeCode == TYPE_LONG || typeCode == TYPE_UINT || typeCode == TYPE_ULONG;
}

bool BaseSensV2::hasPath(const char *path) const {
    JsonVariant variant = getValueAtPath(path);
    return !variant.isNull();
}

SensorTypeCode BaseSensV2::getTypeAtPath(const char *path) const {
    JsonVariant v = getValueAtPath(path);
    if (v.is<bool>()) return TYPE_BOOL;
    if (v.is<int>()) return TYPE_INT;
    if (v.is<float>()) return TYPE_FLOAT;
    if (v.is<double>()) return TYPE_DOUBLE;
    if (v.is<long>()) return TYPE_LONG;
    if (v.is<unsigned int>()) return TYPE_UINT;
    if (v.is<unsigned long>()) return TYPE_ULONG;
    if (v.is<const char *>()) return TYPE_STRING;
    if (v.is<JsonObject>()) return TYPE_OBJECT;
    if (v.is<JsonArray>()) return TYPE_ARRAY;
    return TYPE_UNKNOWN;
}

void BaseSensV2::setValueCalibrable(const char *key, bool calibrable) {
    addCalibrationConfig(key, calibrable);
}

bool BaseSensV2::isValueCalibrable(const char *key) const {
    if (!_calibrationEnabled) return false;

    int index = findConfigIndex(key);
    if (index >= 0) {
        return _calibrationConfigs[index].isCalibrable;
    }

    return isNumericValue(key);
}

void BaseSensV2::enableCalibration(bool enable) {
    _calibrationEnabled = enable;
}

bool BaseSensV2::isCalibrationEnabled() const {
    return _calibrationEnabled;
}

void BaseSensV2::addValueInfo(const char *key, const char *label, const char *unit, uint8_t precision, bool calibrable) {
    if (_valueCount >= _valueCapacity) {
        uint8_t newCapacity = _valueCapacity + 4;
        SensorValueInfo **newInfos = (SensorValueInfo **) realloc(_valueInfos,
                                                                  newCapacity * sizeof(SensorValueInfo *));
        if (newInfos) {
            _valueInfos = newInfos;
            _valueCapacity = newCapacity;
        } else {
            return;
        }
    }

    SensorValueInfo* newInfo = new SensorValueInfo(key, label, unit, precision);
    if (newInfo && newInfo->key) {
        _valueInfos[_valueCount] = newInfo;
        _valueCount++;
        setValueCalibrable(key, calibrable);
    } else {
        if (newInfo) delete newInfo;
    }
}

void BaseSensV2::addPathValueInfo(const char *path, const char *label, const char *unit, uint8_t precision, bool calibrable) {
    addValueInfo(path, label, unit, precision, calibrable);
}

void BaseSensV2::updateValue(const char *key, float value) {
    if (_doc && _name) {
        (*_doc)[_name][key] = value;
    }
}

void BaseSensV2::updateValue(const char *key, int value) {
    if (_doc && _name) {
        (*_doc)[_name][key] = value;
    }
}

void BaseSensV2::updateValue(const char *key, const char *value) {
    if (_doc && _name) {
        (*_doc)[_name][key] = value;
    }
}

uint8_t BaseSensV2::getValueCount() const {
    return _valueCount;
}

const SensorValueInfo *BaseSensV2::getValueInfo(uint8_t index) const {
    if (index < _valueCount) {
        return _valueInfos[index];
    }
    return nullptr;
}

const SensorValueInfo *BaseSensV2::getValueInfoByKey(const char *key) const {
    for (uint8_t i = 0; i < _valueCount; i++) {
        if (strcmp(_valueInfos[i]->key, key) == 0) {
            return _valueInfos[i];
        }
    }
    return nullptr;
}

void BaseSensV2::setValueVisibility(const char *key, bool visible) {
    for (uint8_t i = 0; i < _valueCount; i++) {
        if (strcmp(_valueInfos[i]->key, key) == 0) {
            _valueInfos[i]->visible = visible;
            return;
        }
    }
}

bool BaseSensV2::isValueVisible(const char *key) const {
    const SensorValueInfo *info = getValueInfoByKey(key);
    return info ? info->visible : false;
}

float BaseSensV2::getFloatValue(const char *key) const {
    return getValue<float>(key);
}

int BaseSensV2::getIntValue(const char *key) const {
    return getValue<int>(key);
}

const char *BaseSensV2::getStringValue(const char *key) const {
    if (_doc && _name && (*_doc)[_name][key].is<JsonVariant>()) {
        JsonVariant variant = (*_doc)[_name][key];
        if (variant.is<const char*>()) {
            return variant.as<const char*>();
        }
    }
    return "";
}

JsonObject BaseSensV2::getObject(const char *key) const {
    if (_doc && _name) {
        return (*_doc)[_name][key].as<JsonObject>();
    }
    return JsonObject();
}

JsonArray BaseSensV2::getArray(const char *key) const {
    if (_doc && _name) {
        return (*_doc)[_name][key].as<JsonArray>();
    }
    return JsonArray();
}

void BaseSensV2::setDocument(const char *objName) {
    if (_name) free(_name);
    _name = strdup(objName);
    if (!_name && objName) {
        _name = nullptr;
    }
}

void BaseSensV2::setDocumentValue(JsonDocument *docBase) {
    _doc = docBase;
}

JsonDocument *BaseSensV2::getDocument() const {
    return _doc;
}

JsonVariant BaseSensV2::getVariant(const char *key) const {
    if (_doc && _name) {
        return (*_doc)[_name][key];
    }
    JsonVariant empty;
    return empty;
}

int BaseSensV2::findConfigIndex(const char *key) const {
    for (uint8_t i = 0; i < _configCount; i++) {
        if (_calibrationConfigs[i].valueKey &&
            strcmp(_calibrationConfigs[i].valueKey, key) == 0) {
            return i;
        }
    }
    return -1;
}

void BaseSensV2::addCalibrationConfig(const char *key, bool calibrable) {
    int index = findConfigIndex(key);
    if (index >= 0) {
        _calibrationConfigs[index].isCalibrable = calibrable;
        return;
    }

    if (_configCount >= _configCapacity) {
        uint8_t newCapacity = _configCapacity + 4;
        CalibrationConfig *newConfigs = (CalibrationConfig *) realloc(
                _calibrationConfigs, newCapacity * sizeof(CalibrationConfig));

        if (newConfigs) {
            _calibrationConfigs = newConfigs;
            _configCapacity = newCapacity;

            for (uint8_t i = _configCount; i < _configCapacity; i++) {
                _calibrationConfigs[i].valueKey = nullptr;
                _calibrationConfigs[i].isCalibrable = true;
            }
        } else {
            return;
        }
    }

    char* keyDup = strdup(key);
    if (keyDup) {
        _calibrationConfigs[_configCount].valueKey = keyDup;
        _calibrationConfigs[_configCount].isCalibrable = calibrable;
        _configCount++;
    }
}

JsonVariant BaseSensV2::getValueAtPath(const char *path) const {
    if (!_doc || !_name) return JsonVariant();

    JsonVariant current = (*_doc)[_name];
    String pathStr = String(path);

    int start = 0;
    int end = pathStr.indexOf('.', start);

    while (end != -1) {
        String segment = pathStr.substring(start, end);

        if (segment.indexOf('[') != -1) {
            int bracketStart = segment.indexOf('[');
            int bracketEnd = segment.indexOf(']');
            String arrayName = segment.substring(0, bracketStart);
            int index = segment.substring(bracketStart + 1, bracketEnd).toInt();
            current = current[arrayName][index];
        } else {
            current = current[segment];
        }

        start = end + 1;
        end = pathStr.indexOf('.', start);
    }

    String lastSegment = pathStr.substring(start);
    if (lastSegment.indexOf('[') != -1) {
        int bracketStart = lastSegment.indexOf('[');
        int bracketEnd = lastSegment.indexOf(']');
        String arrayName = lastSegment.substring(0, bracketStart);
        int index = lastSegment.substring(bracketStart + 1, bracketEnd).toInt();
        current = current[arrayName][index];
    } else {
        current = current[lastSegment];
    }

    return current;
}

void BaseSensV2::setValueAtPath(const char *path, JsonVariant value) {
    if (!_doc || !_name) return;

    JsonVariant current = (*_doc)[_name];
    String pathStr = String(path);

    int start = 0;
    int end = pathStr.indexOf('.', start);

    while (end != -1) {
        String segment = pathStr.substring(start, end);

        if (segment.indexOf('[') != -1) {
            int bracketStart = segment.indexOf('[');
            int bracketEnd = segment.indexOf(']');
            String arrayName = segment.substring(0, bracketStart);
            int index = segment.substring(bracketStart + 1, bracketEnd).toInt();

            if (!current[arrayName].is<JsonArray>()) {
                current[arrayName].to<JsonArray>();
            }
            JsonArray arr = current[arrayName].as<JsonArray>();
            while (arr.size() <= index) {
                arr.add(JsonVariant());
            }
            current = arr[index];
        } else {
            if (!current[segment].is<JsonObject>()) {
                current[segment].to<JsonObject>();
            }
            current = current[segment];
        }

        start = end + 1;
        end = pathStr.indexOf('.', start);
    }

    String lastSegment = pathStr.substring(start);
    if (lastSegment.indexOf('[') != -1) {
        int bracketStart = lastSegment.indexOf('[');
        int bracketEnd = lastSegment.indexOf(']');
        String arrayName = lastSegment.substring(0, bracketStart);
        int index = lastSegment.substring(bracketStart + 1, bracketEnd).toInt();

        if (!current[arrayName].is<JsonArray>()) {
            current[arrayName].to<JsonArray>();
        }
        JsonArray arr = current[arrayName].as<JsonArray>();
        while (arr.size() <= index) {
            arr.add(JsonVariant());
        }
        arr[index] = value;
    } else {
        current[lastSegment] = value;
    }
}

SensorModuleV2::SensorModuleV2() : _sensors(nullptr), _names(nullptr), _sensorCount(0),
                                   _sensorCapacity(0), _doc(nullptr), _sensorInitStatus(nullptr),
                                   _alertSystem(nullptr), _filterSystem(nullptr) {
    _sensorCapacity = 8;
    _sensors = (BaseSensV2 **) malloc(_sensorCapacity * sizeof(BaseSensV2 *));
    _names = (char **) malloc(_sensorCapacity * sizeof(char *));

    for (uint8_t i = 0; i < _sensorCapacity; i++) {
        _sensors[i] = nullptr;
        _names[i] = nullptr;
    }
}

SensorModuleV2::~SensorModuleV2() {
    if (_doc) delete _doc;

    if (_sensors) {
        for (uint8_t i = 0; i < _sensorCount; i++) {
            if (_sensors[i]) {
                delete _sensors[i];
            }
        }
        free(_sensors);
    }

    if (_names) {
        for (uint8_t i = 0; i < _sensorCount; i++) {
            if (_names[i]) {
                free(_names[i]);
            }
        }
        free(_names);
    }

    if (_sensorInitStatus) free(_sensorInitStatus);

    if (_alertSystem) {
        delete _alertSystem;
        _alertSystem = nullptr;
    }

    if (_filterSystem) {
        delete _filterSystem;
        _filterSystem = nullptr;
    }
}

void SensorModuleV2::init() {
    if (_sensorCount == 0) return;

    _doc = new JsonDocument;
    _sensorInitStatus = (bool *) malloc(_sensorCount * sizeof(bool));

    for (uint8_t i = 0; i < _sensorCount; i++) {
        _sensors[i]->setDocument(_names[i]);
        _sensors[i]->setDocumentValue(_doc);
        _sensorInitStatus[i] = _sensors[i]->init();

        if (_sensorInitStatus[i]) {
            Serial.print("| [NAME]: ");
            Serial.print(_names[i]);
            Serial.print(" [INFO]: Init Success");
            Serial.println();
        } else {
            Serial.print("| [NAME]: ");
            Serial.print(_names[i]);
            Serial.print(" [ERROR]: Init Failed");
            Serial.println();
        }
    }
}

void SensorModuleV2::update() {
    if (_sensorCount == 0) return;

    for (uint8_t i = 0; i < _sensorCount; i++) {
        if (_sensorInitStatus[i]) {
            _sensors[i]->update();
        }
    }

    if (_filterSystem) {
        _filterSystem->updateFilters(this);
    }

    if (_alertSystem) {
        _alertSystem->checkAlerts(this);
    }
}

void SensorModuleV2::addSensor(const char *name, BaseSensV2 *sensor) {
    if (_sensorCount >= _sensorCapacity) {
        uint8_t newCapacity = _sensorCapacity + 8;
        BaseSensV2 **newSensors = (BaseSensV2 **) realloc(_sensors,
                                                          newCapacity * sizeof(BaseSensV2 *));
        char **newNames = (char **) realloc(_names,
                                            newCapacity * sizeof(char *));

        if (newSensors && newNames) {
            _sensors = newSensors;
            _names = newNames;
            _sensorCapacity = newCapacity;

            for (uint8_t i = _sensorCount; i < _sensorCapacity; i++) {
                _sensors[i] = nullptr;
                _names[i] = nullptr;
            }
        } else {
            return;
        }
    }

    char* nameDup = strdup(name);
    if (nameDup) {
        _sensors[_sensorCount] = sensor;
        _names[_sensorCount] = nameDup;
        _sensorCount++;
    } else {
        delete sensor;
    }
}

void SensorModuleV2::addSensor(const char *name, SensorCreateCallback callbackSensModule) {
    BaseSensV2 *sensor = callbackSensModule();

    if (sensor) {
        addSensor(name, sensor);
    }
}

BaseSensV2 *SensorModuleV2::getSensor(uint8_t index) const {
    if (index < _sensorCount) {
        return _sensors[index];
    }
    return nullptr;
}

BaseSensV2 *SensorModuleV2::getSensorByName(const char *name) const {
    for (uint8_t i = 0; i < _sensorCount; i++) {
        if (strcmp(_names[i], name) == 0) {
            return _sensors[i];
        }
    }
    return nullptr;
}

const char *SensorModuleV2::getSensorName(uint8_t index) const {
    if (index < _sensorCount) {
        return _names[index];
    }
    return nullptr;
}

uint8_t SensorModuleV2::getSensorCount() const {
    return _sensorCount;
}

JsonDocument *SensorModuleV2::getDocument() const {
    return _doc;
}

bool *SensorModuleV2::getSensorInitStatus() const {
    return _sensorInitStatus;
}

SensorProxy SensorModuleV2::operator[](const char *sensorName) {
    return SensorUtilityV2::createSensorProxy(this, sensorName);
}


bool SensorModuleV2::isUpdated(const char *sensorName) const {
    BaseSensV2* sensor = getSensorByName(sensorName);
    if (sensor) {
        return sensor->isUpdated();
    }
    return false;
}

void SensorModuleV2::printValues(const char *sensorName) const {
    BaseSensV2* sensor = getSensorByName(sensorName);
    if (sensor) {
        SensorUtilityV2::debugAll(const_cast<SensorModuleV2*>(this), false);
    }
}

void SensorModuleV2::debug(const char *sensorName, const char *valueKey, bool showHeapMemory) {
    SensorUtilityV2::debug(this, sensorName, valueKey, showHeapMemory);
}

void SensorModuleV2::debugAll(bool showHeapMemory) {
    SensorUtilityV2::debugAll(this, showHeapMemory);
}

void SensorModuleV2::debugHorizontal(bool showHeapMemory) {
    SensorUtilityV2::debugHorizontal(this, showHeapMemory);
}

void SensorModuleV2::debugHorizontalWithTime(uint32_t time, bool showHeapMemory) {
    SensorUtilityV2::debugHorizontalWithTime(this, time, showHeapMemory);
}

void SensorModuleV2::debugPretty(uint32_t time) {
    SensorUtilityV2::debugPretty(this, time);
}

bool SensorModuleV2::setThreshold(const char *sensorName, const char *valueKey,
                                  float lowThreshold, float highThreshold,
                                  AlertType type) {
    if (_alertSystem == nullptr) {
        _alertSystem = new SensorAlertSystemV2();
    }
    return _alertSystem->setThreshold(sensorName, valueKey, lowThreshold, highThreshold, type);
}

bool SensorModuleV2::setThresholdParams(const char *sensorName, const char *valueKey,
                                        uint32_t hysteresis, uint32_t debounceTime) {
    if (_alertSystem == nullptr) {
        return false;
    }
    return _alertSystem->setThresholdParams(sensorName, valueKey, hysteresis, debounceTime);
}

bool SensorModuleV2::removeThreshold(const char *sensorName, const char *valueKey) {
    if (_alertSystem == nullptr) {
        return false;
    }
    return _alertSystem->removeThreshold(sensorName, valueKey);
}

void SensorModuleV2::removeAllThresholds() {
    if (_alertSystem) {
        _alertSystem->removeAllThresholds();
    }
}

AlertState SensorModuleV2::getAlertState(const char *sensorName, const char *valueKey) {
    if (_alertSystem == nullptr) {
        return ALERT_INACTIVE;
    }
    return _alertSystem->getAlertState(sensorName, valueKey);
}

bool SensorModuleV2::isAlertActive(const char *sensorName, const char *valueKey) {
    if (_alertSystem == nullptr) {
        return false;
    }
    return _alertSystem->isAlertActive(sensorName, valueKey);
}

void SensorModuleV2::acknowledgeAlert(const char *sensorName, const char *valueKey) {
    if (_alertSystem) {
        _alertSystem->acknowledgeAlert(sensorName, valueKey);
    }
}

void SensorModuleV2::acknowledgeAllAlerts() {
    if (_alertSystem) {
        _alertSystem->acknowledgeAllAlerts();
    }
}

void SensorModuleV2::resetAlert(const char *sensorName, const char *valueKey) {
    if (_alertSystem) {
        _alertSystem->resetAlert(sensorName, valueKey);
    }
}

void SensorModuleV2::resetAllAlerts() {
    if (_alertSystem) {
        _alertSystem->resetAllAlerts();
    }
}

void SensorModuleV2::setGlobalAlertCallback(AlertCallback callback) {
    if (_alertSystem == nullptr) {
        _alertSystem = new SensorAlertSystemV2();
    }
    _alertSystem->setGlobalAlertCallback(callback);
}

bool SensorModuleV2::setSensorAlertCallback(const char *sensorName, AlertCallback callback) {
    if (_alertSystem == nullptr) {
        _alertSystem = new SensorAlertSystemV2();
    }
    return _alertSystem->setSensorAlertCallback(sensorName, callback);
}

void SensorModuleV2::clearAlertCallbacks() {
    if (_alertSystem) {
        _alertSystem->clearCallbacks();
    }
}

void SensorModuleV2::setDefaultHysteresis(uint32_t hysteresis) {
    if (_alertSystem == nullptr) {
        _alertSystem = new SensorAlertSystemV2();
    }
    _alertSystem->setDefaultHysteresis(hysteresis);
}

void SensorModuleV2::setDefaultDebounceTime(uint32_t debounceTime) {
    if (_alertSystem == nullptr) {
        _alertSystem = new SensorAlertSystemV2();
    }
    _alertSystem->setDefaultDebounceTime(debounceTime);
}

void SensorModuleV2::enableAlertSystem(bool enable) {
    if (enable) {
        if (_alertSystem == nullptr) {
            _alertSystem = new SensorAlertSystemV2();
        }
    } else {
        if (_alertSystem) {
            delete _alertSystem;
            _alertSystem = nullptr;
        }
    }
}

bool SensorModuleV2::hasAlertSystem() const {
    return _alertSystem != nullptr;
}

bool SensorModuleV2::attachFilter(const char *sensorName, const char *valueKey,
                                  FilterType type, FilterParams params) {
    if (_filterSystem == nullptr) {
        _filterSystem = new SensorFilterV2();
    }
    return _filterSystem->attachFilter(sensorName, valueKey, type, params);
}

bool SensorModuleV2::updateFilter(const char *sensorName, const char *valueKey,
                                  FilterType type, FilterParams params) {
    if (_filterSystem == nullptr) {
        return false;
    }
    return _filterSystem->updateFilter(sensorName, valueKey, type, params);
}

bool SensorModuleV2::detachFilter(const char *sensorName, const char *valueKey) {
    if (_filterSystem == nullptr) {
        return false;
    }
    return _filterSystem->detachFilter(sensorName, valueKey);
}

void SensorModuleV2::detachAllFilters() {
    if (_filterSystem) {
        _filterSystem->detachAllFilters();
    }
}

float SensorModuleV2::getFilteredValue(const char *sensorName, const char *valueKey) {
    if (_filterSystem == nullptr) {
        BaseSensV2 *sensor = getSensorByName(sensorName);
        if (sensor) {
            return sensor->getFloatValue(valueKey);
        }
        return 0.0f;
    }
    return _filterSystem->getFilteredValue(this, sensorName, valueKey);
}

float SensorModuleV2::getLastFilteredValue(const char *sensorName, const char *valueKey) {
    if (_filterSystem == nullptr) {
        return 0.0f;
    }
    return _filterSystem->getLastFilteredValue(sensorName, valueKey);
}

void SensorModuleV2::resetFilter(const char *sensorName, const char *valueKey) {
    if (_filterSystem) {
        _filterSystem->resetFilter(sensorName, valueKey);
    }
}

void SensorModuleV2::resetAllFilters() {
    if (_filterSystem) {
        _filterSystem->resetAllFilters();
    }
}

bool SensorModuleV2::hasFilter(const char *sensorName, const char *valueKey) {
    if (_filterSystem == nullptr) {
        return false;
    }
    return _filterSystem->hasFilter(sensorName, valueKey);
}

FilterType SensorModuleV2::getFilterType(const char *sensorName, const char *valueKey) {
    if (_filterSystem == nullptr) {
        return FILTER_NONE;
    }
    return _filterSystem->getFilterType(sensorName, valueKey);
}

void SensorModuleV2::enableFilterSystem(bool enable) {
    if (enable) {
        if (_filterSystem == nullptr) {
            _filterSystem = new SensorFilterV2();
        }
    } else {
        if (_filterSystem) {
            delete _filterSystem;
            _filterSystem = nullptr;
        }
    }
}

bool SensorModuleV2::hasFilterSystem() const {
    return _filterSystem != nullptr;
}
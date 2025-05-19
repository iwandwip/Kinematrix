#include "SensorModuleV2.h"
#include "SensorUtilityV2.h"

BaseSensV2::BaseSensV2() : _valueInfos(nullptr), _valueCount(0), _valueCapacity(0),
                           _doc(nullptr), _name(nullptr) {
    _valueCapacity = 4;
    _valueInfos = (SensorValueInfo **) malloc(_valueCapacity * sizeof(SensorValueInfo *));
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
}

void BaseSensV2::addValueInfo(const char *key, const char *label, const char *unit,
                              uint8_t precision, SensorValueType type) {
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

    _valueInfos[_valueCount] = new SensorValueInfo(key, label, unit, precision, type);
    _valueCount++;
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

SensorValueData BaseSensV2::getValueByKey(const char *key) const {
    SensorValueData emptyData;
    emptyData.floatValue = 0.0f;
    emptyData.type = TYPE_FLOAT;

    const SensorValueInfo *info = getValueInfoByKey(key);
    if (!info) return emptyData;

    SensorValueData data;
    data.type = info->type;

    if (_doc && _name) {
        switch (info->type) {
            case TYPE_FLOAT:
                data.floatValue = (*_doc)[_name][key].as<float>();
                break;
            case TYPE_INT:
                data.intValue = (*_doc)[_name][key].as<int>();
                break;
            case TYPE_STRING:
                data.stringValue = strdup((*_doc)[_name][key].as<const char *>());
                break;
        }
    }

    return data;
}

float BaseSensV2::getFloatValue(const char *key) const {
    SensorValueData data = getValueByKey(key);
    if (data.type == TYPE_FLOAT) {
        return data.floatValue;
    } else if (data.type == TYPE_INT) {
        return (float) data.intValue;
    }
    return 0.0f;
}

int BaseSensV2::getIntValue(const char *key) const {
    SensorValueData data = getValueByKey(key);
    if (data.type == TYPE_INT) {
        return data.intValue;
    } else if (data.type == TYPE_FLOAT) {
        return (int) data.floatValue;
    }
    return 0;
}

const char *BaseSensV2::getStringValue(const char *key) const {
    SensorValueData data = getValueByKey(key);
    if (data.type == TYPE_STRING) {
        return data.stringValue;
    }
    return "";
}

void BaseSensV2::setDocument(const char *objName) {
    if (_name) free(_name);
    _name = strdup(objName);
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

    _sensors[_sensorCount] = sensor;
    _names[_sensorCount] = strdup(name);
    _sensorCount++;
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
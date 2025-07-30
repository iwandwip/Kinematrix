#ifndef SENSOR_MODULE_V2_H
#define SENSOR_MODULE_V2_H

#include "Arduino.h"
#include "ArduinoJson.h"
#include "Systems/SensorAlertSystemV2.h"
#include "Systems/SensorFilterV2.h"

enum SensorTypeCode {
    TYPE_UNKNOWN = 0,
    TYPE_BOOL = 1,
    TYPE_INT = 2,
    TYPE_FLOAT = 3,
    TYPE_DOUBLE = 4,
    TYPE_LONG = 5,
    TYPE_UINT = 6,
    TYPE_ULONG = 7,
    TYPE_STRING = 8,
    TYPE_OBJECT = 9,
    TYPE_ARRAY = 10
};

struct SensorValueInfo {
    char *key;
    char *label;
    char *unit;
    uint8_t precision;
    bool visible;

    SensorValueInfo(const char *_key, const char *_label, const char *_unit, uint8_t _precision) 
        : key(nullptr), label(nullptr), unit(nullptr), precision(_precision), visible(true) {
        
        if (_key) {
            key = strdup(_key);
            if (!key) return;
        }
        
        if (_label) {
            label = strdup(_label);
            if (!label) {
                if (key) { free(key); key = nullptr; }
                return;
            }
        }
        
        if (_unit) {
            unit = strdup(_unit);
            if (!unit) {
                if (key) { free(key); key = nullptr; }
                if (label) { free(label); label = nullptr; }
                return;
            }
        }
    }

    ~SensorValueInfo() {
        if (key) free(key);
        if (label) free(label);
        if (unit) free(unit);
    }
};

struct CalibrationConfig {
    char *valueKey;
    bool isCalibrable;
};

class BaseSensV2 {
private:
    SensorValueInfo **_valueInfos;
    uint8_t _valueCount;
    uint8_t _valueCapacity;

    JsonDocument *_doc;
    char *_name;

    CalibrationConfig *_calibrationConfigs;
    uint8_t _configCount;
    uint8_t _configCapacity;
    bool _calibrationEnabled;

    int findConfigIndex(const char *key) const;
    void addCalibrationConfig(const char *key, bool calibrable);
    JsonVariant getValueAtPath(const char *path) const;
    void setValueAtPath(const char *path, JsonVariant value);

public:
    BaseSensV2();
    virtual ~BaseSensV2();

    virtual bool init() = 0;
    virtual bool update() = 0;
    virtual bool isUpdated() const { return false; }

    template<typename T>
    T getValue(const char *key) const {
        if (_doc && _name) {
            return (*_doc)[_name][key].as<T>();
        }
        return T{};
    }

    template<typename T>
    void updateValue(const char *key, T value) {
        if (_doc && _name) {
            (*_doc)[_name][key] = value;
        }
    }

    template<typename T>
    T getValueAtPath(const char *path) const {
        JsonVariant variant = getValueAtPath(path);
        return variant.as<T>();
    }

    template<typename T>
    void updateValueAtPath(const char *path, T value) {
        setValueAtPath(path, JsonVariant(value));
    }

    bool hasValue(const char *key) const;

    bool isNumericValue(const char *key) const;
    SensorTypeCode getValueTypeCode(const char *key) const;
    static const char *getTypeCodeName(SensorTypeCode typeCode);
    static bool isNumericTypeCode(SensorTypeCode typeCode);

    bool hasPath(const char *path) const;
    SensorTypeCode getTypeAtPath(const char *path) const;

    void setValueCalibrable(const char *key, bool calibrable = true);
    bool isValueCalibrable(const char *key) const;
    void enableCalibration(bool enable = true);
    bool isCalibrationEnabled() const;

    void addValueInfo(const char *key, const char *label, const char *unit, uint8_t precision = 3, bool calibrable = true);
    void addPathValueInfo(const char *path, const char *label, const char *unit, uint8_t precision = 3, bool calibrable = true);
    void updateValue(const char *key, float value);
    void updateValue(const char *key, int value);
    void updateValue(const char *key, const char *value);

    uint8_t getValueCount() const;
    const SensorValueInfo *getValueInfo(uint8_t index) const;
    const SensorValueInfo *getValueInfoByKey(const char *key) const;
    void setValueVisibility(const char *key, bool visible);
    bool isValueVisible(const char *key) const;

    float getFloatValue(const char *key) const;
    int getIntValue(const char *key) const;
    const char *getStringValue(const char *key) const;

    JsonObject getObject(const char *key) const;
    JsonArray getArray(const char *key) const;

    void setDocument(const char *objName);
    void setDocumentValue(JsonDocument *docBase);
    JsonDocument *getDocument() const;
    JsonVariant getVariant(const char *key) const;
};

typedef BaseSensV2 *(*SensorCreateCallback)();

class SensorUtilityV2;

class SensorProxy;

class SensorModuleV2 {
private:
    BaseSensV2 **_sensors;
    char **_names;
    uint8_t _sensorCount;
    uint8_t _sensorCapacity;
    JsonDocument *_doc;
    bool *_sensorInitStatus;

    SensorAlertSystemV2 *_alertSystem;
    SensorFilterV2 *_filterSystem;

    friend class SensorUtilityV2;

public:
    SensorModuleV2();
    ~SensorModuleV2();

    void init();
    void update();

    void addSensor(const char *name, BaseSensV2 *sensor);
    void addSensor(const char *name, SensorCreateCallback callbackSensModule);

    BaseSensV2 *getSensor(uint8_t index) const;
    BaseSensV2 *getSensorByName(const char *name) const;
    const char *getSensorName(uint8_t index) const;
    uint8_t getSensorCount() const;
    JsonDocument *getDocument() const;
    bool *getSensorInitStatus() const;

    SensorProxy operator[](const char *sensorName);
    
    template<typename T>
    T getValue(const char *sensorName, const char *valueKey) const;
    bool isUpdated(const char *sensorName) const;
    void printValues(const char *sensorName) const;

    void debug(const char *sensorName, const char *valueKey, bool showHeapMemory = false);
    void debugAll(bool showHeapMemory = false);
    void debugHorizontal(bool showHeapMemory = false);
    void debugHorizontalWithTime(uint32_t time, bool showHeapMemory = false);
    void debugPretty(uint32_t time = 1000);

    bool setThreshold(const char *sensorName, const char *valueKey,
                      float lowThreshold, float highThreshold,
                      AlertType type = ALERT_OUTSIDE);
    bool setThresholdParams(const char *sensorName, const char *valueKey,
                            uint32_t hysteresis, uint32_t debounceTime);
    bool removeThreshold(const char *sensorName, const char *valueKey);
    void removeAllThresholds();

    AlertState getAlertState(const char *sensorName, const char *valueKey);
    bool isAlertActive(const char *sensorName, const char *valueKey);

    void acknowledgeAlert(const char *sensorName, const char *valueKey);
    void acknowledgeAllAlerts();

    void resetAlert(const char *sensorName, const char *valueKey);
    void resetAllAlerts();

    void setGlobalAlertCallback(AlertCallback callback);
    bool setSensorAlertCallback(const char *sensorName, AlertCallback callback);
    void clearAlertCallbacks();

    void setDefaultHysteresis(uint32_t hysteresis);
    void setDefaultDebounceTime(uint32_t debounceTime);

    void enableAlertSystem(bool enable = true);
    bool hasAlertSystem() const;

    bool attachFilter(const char *sensorName, const char *valueKey,
                      FilterType type, FilterParams params);
    bool updateFilter(const char *sensorName, const char *valueKey,
                      FilterType type, FilterParams params);
    bool detachFilter(const char *sensorName, const char *valueKey);
    void detachAllFilters();

    float getFilteredValue(const char *sensorName, const char *valueKey);
    float getLastFilteredValue(const char *sensorName, const char *valueKey);

    void resetFilter(const char *sensorName, const char *valueKey);
    void resetAllFilters();

    bool hasFilter(const char *sensorName, const char *valueKey);
    FilterType getFilterType(const char *sensorName, const char *valueKey);

    void enableFilterSystem(bool enable = true);
    bool hasFilterSystem() const;
};

template<typename T>
T SensorModuleV2::getValue(const char *sensorName, const char *valueKey) const {
    BaseSensV2* sensor = getSensorByName(sensorName);
    if (sensor) {
        return sensor->getValue<T>(valueKey);
    }
    return T();
}

#endif
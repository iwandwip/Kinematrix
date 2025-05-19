#ifndef SENSOR_MODULE_V2_H
#define SENSOR_MODULE_V2_H

#include "Arduino.h"
#include "ArduinoJson.h"
#include "SensorAlertSystemV2.h"
#include "SensorFilterV2.h"

enum SensorValueType {
    TYPE_FLOAT,
    TYPE_INT,
    TYPE_STRING
};

struct SensorValueData {
    union {
        float floatValue;
        int intValue;
        char *stringValue;
    };
    SensorValueType type;
};

struct SensorValueInfo {
    char *key;
    char *label;
    char *unit;
    uint8_t precision;
    SensorValueType type;

    SensorValueInfo(const char *_key, const char *_label, const char *_unit, uint8_t _precision, SensorValueType _type) {
        key = strdup(_key);
        label = strdup(_label);
        unit = strdup(_unit);
        precision = _precision;
        type = _type;
    }

    ~SensorValueInfo() {
        if (key) free(key);
        if (label) free(label);
        if (unit) free(unit);
    }
};

class BaseSensV2 {
private:
    SensorValueInfo **_valueInfos;
    uint8_t _valueCount;
    uint8_t _valueCapacity;

    JsonDocument *_doc;
    char *_name;

public:
    BaseSensV2();
    virtual ~BaseSensV2();

    virtual bool init() = 0;
    virtual bool update() = 0;

    void addValueInfo(const char *key, const char *label, const char *unit, uint8_t precision, SensorValueType type);
    void updateValue(const char *key, float value);
    void updateValue(const char *key, int value);
    void updateValue(const char *key, const char *value);

    uint8_t getValueCount() const;
    const SensorValueInfo *getValueInfo(uint8_t index) const;
    const SensorValueInfo *getValueInfoByKey(const char *key) const;

    virtual SensorValueData getValueByKey(const char *key) const;

    float getFloatValue(const char *key) const;
    int getIntValue(const char *key) const;
    const char *getStringValue(const char *key) const;

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

#endif
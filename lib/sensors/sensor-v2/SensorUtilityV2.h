#ifndef SENSOR_UTILITY_V2_H
#define SENSOR_UTILITY_V2_H

#include "SensorModuleV2.h"

class SensorValueProxy {
private:
    JsonVariant _value;
    SensorValueType _type;

public:
    SensorValueProxy(JsonVariant value, SensorValueType type = TYPE_FLOAT);
    SensorValueProxy operator[](const char *key) const;

    operator float() const;
    operator int() const;
    operator const char *() const;

    JsonVariant getVariant() const;
};

class SensorProxy {
private:
    JsonVariant _sensorData;

public:
    SensorProxy(JsonVariant sensorData);
    SensorValueProxy operator[](const char *key) const;
};

class SensorUtilityV2 {
private:
    static void printSensorValue(BaseSensV2 *sensor, const SensorValueInfo *info, const char *valueKey);

public:
    static SensorProxy createSensorProxy(SensorModuleV2 *module, const char *sensorName);

    static void debug(SensorModuleV2 *module, const char *sensorName, const char *valueKey, bool showHeapMemory = false);
    static void debugAll(SensorModuleV2 *module, bool showHeapMemory = false);
    static void debugHorizontal(SensorModuleV2 *module, bool showHeapMemory = false);
    static void debugHorizontalWithTime(SensorModuleV2 *module, uint32_t time, bool showHeapMemory = false);
    static void debugPretty(SensorModuleV2 *module, uint32_t time = 1000);
};

#endif
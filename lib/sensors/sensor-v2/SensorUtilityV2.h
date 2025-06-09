#ifndef SENSOR_UTILITY_V2_H
#define SENSOR_UTILITY_V2_H

#include "SensorModuleV2.h"

class SensorValueProxy {
private:
    JsonVariant _value;
    SensorTypeCode _type;

public:
    SensorValueProxy(JsonVariant value, SensorTypeCode type = TYPE_FLOAT);
    SensorValueProxy operator[](const char *key) const;

    operator float() const;
    operator int() const;
    operator long() const;
    operator double() const;
    operator bool() const;
    operator unsigned int() const;
    operator unsigned long() const;
    operator const char *() const;

    JsonVariant getVariant() const;
    SensorTypeCode getTypeCode() const;
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
    static void printTypedValue(BaseSensV2 *sensor, const char *valueKey, SensorTypeCode typeCode, uint8_t precision);
    static void printPathValue(BaseSensV2 *sensor, const char *path, uint8_t precision);
    static void extractAllPaths(BaseSensV2 *sensor, const char *sensorName, JsonVariant obj, String currentPath = "");
    static void printNestedStructure(JsonVariant obj, String indent = "");
    static String formatPathDisplay(const char *path);
    static bool isValidPath(const char *path);

public:
    static SensorProxy createSensorProxy(SensorModuleV2 *module, const char *sensorName);

    static void debug(SensorModuleV2 *module, const char *sensorName, const char *valueKey, bool showHeapMemory = false);
    static void debugAll(SensorModuleV2 *module, bool showHeapMemory = false);
    static void debugHorizontal(SensorModuleV2 *module, bool showHeapMemory = false);
    static void debugHorizontalWithTime(SensorModuleV2 *module, uint32_t time, bool showHeapMemory = false);
    static void debugPretty(SensorModuleV2 *module, uint32_t time = 1000);
    static void debugNested(SensorModuleV2 *module, const char *sensorName, bool showHeapMemory = false);
    static void debugAllNested(SensorModuleV2 *module, bool showHeapMemory = false);
    static void debugPath(SensorModuleV2 *module, const char *sensorName, const char *path, bool showHeapMemory = false);

    static void printValueWithType(BaseSensV2 *sensor, const char *valueKey, uint8_t precision = 3);
    static void printPathWithType(BaseSensV2 *sensor, const char *path, uint8_t precision = 3);
    static void printAllPaths(SensorModuleV2 *module, const char *sensorName);
    static void printSensorStructure(SensorModuleV2 *module, const char *sensorName);
    static void printMemoryInfo();

    static String getValueAsString(BaseSensV2 *sensor, const char *valueKey, uint8_t precision = 3);
    static String getPathAsString(BaseSensV2 *sensor, const char *path, uint8_t precision = 3);
    static String getTypeAsString(SensorTypeCode typeCode);
};

#endif
#include "SensorUtilityV2.h"

SensorValueProxy::SensorValueProxy(JsonVariant value, SensorValueType type)
        : _value(value), _type(type) {
}

SensorValueProxy SensorValueProxy::operator[](const char *key) const {
    JsonVariant variant = _value[key];
    return SensorValueProxy(variant);
}

SensorValueProxy::operator float() const {
    return _value.as<float>();
}

SensorValueProxy::operator int() const {
    return _value.as<int>();
}

SensorValueProxy::operator const char *() const {
    return _value.as<const char *>();
}

JsonVariant SensorValueProxy::getVariant() const {
    return _value;
}

SensorProxy::SensorProxy(JsonVariant sensorData)
        : _sensorData(sensorData) {
}

SensorValueProxy SensorProxy::operator[](const char *key) const {
    JsonVariant variant = _sensorData[key];
    return SensorValueProxy(variant);
}

SensorProxy SensorUtilityV2::createSensorProxy(SensorModuleV2 *module, const char *sensorName) {
    if (module->_doc) {
        JsonVariant variant = (*(module->_doc))[sensorName];
        return SensorProxy(variant);
    }
    JsonVariant empty;
    return SensorProxy(empty);
}

void SensorUtilityV2::printSensorValue(BaseSensV2 *sensor, const SensorValueInfo *info, const char *valueKey) {
    switch (info->type) {
        case TYPE_FLOAT: {
            float value = sensor->getFloatValue(valueKey);
            Serial.print(value, info->precision);
            break;
        }
        case TYPE_INT: {
            int value = sensor->getIntValue(valueKey);
            Serial.print(value);
            break;
        }
        case TYPE_STRING: {
            const char *value = sensor->getStringValue(valueKey);
            Serial.print(value);
            break;
        }
    }

    if (strlen(info->unit) > 0) {
        Serial.print(info->unit);
    }
}

void SensorUtilityV2::debug(SensorModuleV2 *module, const char *sensorName, const char *valueKey, bool showHeapMemory) {
    BaseSensV2 *sensor = module->getSensorByName(sensorName);
    if (!sensor) return;

    const SensorValueInfo *info = sensor->getValueInfoByKey(valueKey);
    if (!info) return;

    Serial.print("| ");
    Serial.print(sensorName);
    Serial.print(" - ");
    Serial.print(info->label);
    Serial.print(": ");

    printSensorValue(sensor, info, valueKey);

    if (showHeapMemory) {
        Serial.print(" | mem: ");
#if defined(ESP32)
        Serial.print(ESP.getFreeHeap());
#endif
    }

    Serial.println();
}

void SensorUtilityV2::debugAll(SensorModuleV2 *module, bool showHeapMemory) {
    for (uint8_t i = 0; i < module->getSensorCount(); i++) {
        BaseSensV2 *sensor = module->getSensor(i);
        const char *sensorName = module->getSensorName(i);

        for (uint8_t j = 0; j < sensor->getValueCount(); j++) {
            const SensorValueInfo *info = sensor->getValueInfo(j);
            debug(module, sensorName, info->key, false);
        }
    }

    if (showHeapMemory) {
        Serial.print("| mem: ");
#if defined(ESP32)
        Serial.print(ESP.getFreeHeap());
#endif
    }

    Serial.println();
}

void SensorUtilityV2::debugHorizontal(SensorModuleV2 *module, bool showHeapMemory) {
    if (module->getSensorCount() == 0) return;

    for (uint8_t i = 0; i < module->getSensorCount(); i++) {
        BaseSensV2 *sensor = module->getSensor(i);
        const char *sensorName = module->getSensorName(i);

        Serial.print("| ");
        Serial.print(sensorName);
        Serial.print(": ");

        bool isFirst = true;
        for (uint8_t j = 0; j < sensor->getValueCount(); j++) {
            const SensorValueInfo *info = sensor->getValueInfo(j);
            if (!isFirst) {
                Serial.print(" | ");
            }
            isFirst = false;

            Serial.print(info->key);
            Serial.print("=");

            printSensorValue(sensor, info, info->key);
        }

        if (i < module->getSensorCount() - 1) {
            Serial.print(" ");
        }
    }

    if (showHeapMemory) {
        Serial.print(" | mem: ");
#if defined(ESP32)
        Serial.print(ESP.getFreeHeap());
#endif
    }

    Serial.println();
}

void SensorUtilityV2::debugHorizontalWithTime(SensorModuleV2 *module, uint32_t time, bool showHeapMemory) {
    static uint32_t lastDebugTime = 0;
    if (millis() - lastDebugTime >= time) {
        debugHorizontal(module, showHeapMemory);
        lastDebugTime = millis();
    }
}

void SensorUtilityV2::debugPretty(SensorModuleV2 *module, uint32_t time) {
    JsonDocument *doc = module->getDocument();
    static uint32_t lastPrettyTime = 0;
    if (millis() - lastPrettyTime >= time && doc) {
        serializeJsonPretty(*doc, Serial);
        Serial.println();
        lastPrettyTime = millis();
    }
}
#include "SensorUtilityV2.h"

SensorValueProxy::SensorValueProxy(JsonVariant value, SensorTypeCode type)
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

SensorValueProxy::operator long() const {
    return _value.as<long>();
}

SensorValueProxy::operator double() const {
    return _value.as<double>();
}

SensorValueProxy::operator bool() const {
    return _value.as<bool>();
}

SensorValueProxy::operator unsigned int() const {
    return _value.as<unsigned int>();
}

SensorValueProxy::operator unsigned long() const {
    return _value.as<unsigned long>();
}

SensorValueProxy::operator const char *() const {
    return _value.as<const char *>();
}

JsonVariant SensorValueProxy::getVariant() const {
    return _value;
}

SensorTypeCode SensorValueProxy::getTypeCode() const {
    return _type;
}

SensorProxy::SensorProxy(JsonVariant sensorData)
        : _sensorData(sensorData) {
}

SensorValueProxy SensorProxy::operator[](const char *key) const {
    JsonVariant variant = _sensorData[key];
    return SensorValueProxy(variant);
}

void SensorUtilityV2::printSensorValue(BaseSensV2 *sensor, const SensorValueInfo *info, const char *valueKey) {
    SensorTypeCode typeCode = sensor->getValueTypeCode(valueKey);
    printTypedValue(sensor, valueKey, typeCode, info->precision);

    if (strlen(info->unit) > 0) {
        Serial.print(info->unit);
    }
}

void SensorUtilityV2::printTypedValue(BaseSensV2 *sensor, const char *valueKey, SensorTypeCode typeCode, uint8_t precision) {
    switch (typeCode) {
        case TYPE_FLOAT: {
            float value = sensor->getValue<float>(valueKey);
            Serial.print(value, precision);
            break;
        }
        case TYPE_INT: {
            int value = sensor->getValue<int>(valueKey);
            Serial.print(value);
            break;
        }
        case TYPE_LONG: {
            long value = sensor->getValue<long>(valueKey);
            Serial.print(value);
            break;
        }
        case TYPE_DOUBLE: {
            double value = sensor->getValue<double>(valueKey);
            Serial.print(value, precision);
            break;
        }
        case TYPE_BOOL: {
            bool value = sensor->getValue<bool>(valueKey);
            Serial.print(value ? "true" : "false");
            break;
        }
        case TYPE_UINT: {
            unsigned int value = sensor->getValue<unsigned int>(valueKey);
            Serial.print(value);
            break;
        }
        case TYPE_ULONG: {
            unsigned long value = sensor->getValue<unsigned long>(valueKey);
            Serial.print(value);
            break;
        }
        case TYPE_STRING: {
            String value = sensor->getValue<String>(valueKey);
            Serial.print(value);
            break;
        }
        case TYPE_UNKNOWN:
        default: {
            Serial.print("N/A");
            break;
        }
    }
}

void SensorUtilityV2::printPathValue(BaseSensV2 *sensor, const char *path, uint8_t precision) {
    if (path[0] == '\0') return;

    SensorTypeCode typeCode = sensor->getTypeAtPath(path);

    switch (typeCode) {
        case TYPE_FLOAT: {
            float value = sensor->getValueAtPath<float>(path);
            Serial.print(value, precision);
            break;
        }
        case TYPE_INT: {
            int value = sensor->getValueAtPath<int>(path);
            Serial.print(value);
            break;
        }
        case TYPE_LONG: {
            long value = sensor->getValueAtPath<long>(path);
            Serial.print(value);
            break;
        }
        case TYPE_DOUBLE: {
            double value = sensor->getValueAtPath<double>(path);
            Serial.print(value, precision);
            break;
        }
        case TYPE_BOOL: {
            bool value = sensor->getValueAtPath<bool>(path);
            Serial.print(value ? "true" : "false");
            break;
        }
        case TYPE_UINT: {
            unsigned int value = sensor->getValueAtPath<unsigned int>(path);
            Serial.print(value);
            break;
        }
        case TYPE_ULONG: {
            unsigned long value = sensor->getValueAtPath<unsigned long>(path);
            Serial.print(value);
            break;
        }
        case TYPE_STRING: {
            String value = sensor->getValueAtPath<String>(path);
            Serial.print(value);
            break;
        }
        case TYPE_OBJECT:
            Serial.print("{object}");
            break;
        case TYPE_ARRAY:
            Serial.print("[array]");
            break;
        case TYPE_UNKNOWN:
        default:
            Serial.print("N/A");
            break;
    }
}

void SensorUtilityV2::extractAllPaths(BaseSensV2 *sensor, const char *sensorName, JsonVariant obj, String currentPath) {
    if (obj.is<JsonObject>()) {
        JsonObject jsonObj = obj.as<JsonObject>();
        for (JsonPair kv: jsonObj) {
            String newPath = currentPath.length() > 0 ? currentPath + "." + kv.key().c_str() : String(kv.key().c_str());

            if (kv.value().is<JsonObject>() || kv.value().is<JsonArray>()) {
                extractAllPaths(sensor, sensorName, kv.value(), newPath);
            } else {
                Serial.print("  ");
                Serial.print(formatPathDisplay(newPath.c_str()));
                Serial.print(": ");
                printPathValue(sensor, newPath.c_str(), 3);
                Serial.print(" [");
                Serial.print(getTypeAsString(sensor->getTypeAtPath(newPath.c_str())));
                Serial.println("]");
            }
        }
    } else if (obj.is<JsonArray>()) {
        JsonArray jsonArray = obj.as<JsonArray>();
        for (size_t i = 0; i < jsonArray.size(); i++) {
            String newPath = currentPath + "[" + String(i) + "]";

            if (jsonArray[i].is<JsonObject>() || jsonArray[i].is<JsonArray>()) {
                extractAllPaths(sensor, sensorName, jsonArray[i], newPath);
            } else {
                Serial.print("  ");
                Serial.print(formatPathDisplay(newPath.c_str()));
                Serial.print(": ");
                printPathValue(sensor, newPath.c_str(), 3);
                Serial.print(" [");
                Serial.print(getTypeAsString(sensor->getTypeAtPath(newPath.c_str())));
                Serial.println("]");
            }
        }
    }
}

void SensorUtilityV2::printNestedStructure(JsonVariant obj, String indent) {
    if (obj.is<JsonObject>()) {
        JsonObject jsonObj = obj.as<JsonObject>();
        for (JsonPair kv: jsonObj) {
            Serial.print(indent);
            Serial.print(kv.key().c_str());

            if (kv.value().is<JsonObject>()) {
                Serial.println(" {");
                printNestedStructure(kv.value(), indent + "  ");
                Serial.print(indent);
                Serial.println("}");
            } else if (kv.value().is<JsonArray>()) {
                Serial.println(" [");
                printNestedStructure(kv.value(), indent + "  ");
                Serial.print(indent);
                Serial.println("]");
            } else {
                Serial.print(": ");
                Serial.println(kv.value().as<String>());
            }
        }
    } else if (obj.is<JsonArray>()) {
        JsonArray jsonArray = obj.as<JsonArray>();
        for (size_t i = 0; i < jsonArray.size(); i++) {
            Serial.print(indent);
            Serial.print("[");
            Serial.print(i);
            Serial.print("]");

            if (jsonArray[i].is<JsonObject>()) {
                Serial.println(" {");
                printNestedStructure(jsonArray[i], indent + "  ");
                Serial.print(indent);
                Serial.println("}");
            } else if (jsonArray[i].is<JsonArray>()) {
                Serial.println(" [");
                printNestedStructure(jsonArray[i], indent + "  ");
                Serial.print(indent);
                Serial.println("]");
            } else {
                Serial.print(": ");
                Serial.println(jsonArray[i].as<String>());
            }
        }
    }
}

String SensorUtilityV2::formatPathDisplay(const char *path) {
    String result = String(path);
    result.replace(".", " → ");
    result.replace("[", " [");
    return result;
}

bool SensorUtilityV2::isValidPath(const char *path) {
    return path != nullptr && strlen(path) > 0;
}

SensorProxy SensorUtilityV2::createSensorProxy(SensorModuleV2 *module, const char *sensorName) {
    if (module->_doc) {
        JsonVariant variant = (*(module->_doc))[sensorName];
        return SensorProxy(variant);
    }
    JsonVariant empty;
    return SensorProxy(empty);
}

void SensorUtilityV2::debug(SensorModuleV2 *module, const char *sensorName, const char *valueKey, bool showHeapMemory) {
    BaseSensV2 *sensor = module->getSensorByName(sensorName);
    if (!sensor) return;

    const SensorValueInfo *info = sensor->getValueInfoByKey(valueKey);
    if (!info || !info->visible) return;

    Serial.print("| ");
    Serial.print(sensorName);
    Serial.print(" - ");
    Serial.print(info->label);
    Serial.print(": ");

    printSensorValue(sensor, info, valueKey);

    if (showHeapMemory) {
        Serial.print(" | mem: ");
        printMemoryInfo();
    }

    Serial.println();
}

void SensorUtilityV2::debugAll(SensorModuleV2 *module, bool showHeapMemory) {
    for (uint8_t i = 0; i < module->getSensorCount(); i++) {
        BaseSensV2 *sensor = module->getSensor(i);
        const char *sensorName = module->getSensorName(i);

        for (uint8_t j = 0; j < sensor->getValueCount(); j++) {
            const SensorValueInfo *info = sensor->getValueInfo(j);
            if (!info->visible) continue;
            debug(module, sensorName, info->key, false);
        }
    }

    if (showHeapMemory) {
        Serial.print("| mem: ");
        printMemoryInfo();
        Serial.println();
    }
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
            
            if (!info->visible) continue;
            
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
        printMemoryInfo();
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

void SensorUtilityV2::debugNested(SensorModuleV2 *module, const char *sensorName, bool showHeapMemory) {
    BaseSensV2 *sensor = module->getSensorByName(sensorName);
    if (!sensor) return;

    Serial.print("| ");
    Serial.print(sensorName);
    Serial.println(" - Nested Structure:");

    JsonDocument *doc = sensor->getDocument();
    if (doc && (*doc)[sensorName].is<JsonObject>()) {
        JsonVariant sensorObj = (*doc)[sensorName];
        extractAllPaths(sensor, sensorName, sensorObj);
    }

    if (showHeapMemory) {
        Serial.print("| mem: ");
        printMemoryInfo();
        Serial.println();
    }
}

void SensorUtilityV2::debugAllNested(SensorModuleV2 *module, bool showHeapMemory) {
    for (uint8_t i = 0; i < module->getSensorCount(); i++) {
        const char *sensorName = module->getSensorName(i);
        debugNested(module, sensorName, false);
        Serial.println();
    }

    if (showHeapMemory) {
        Serial.print("| mem: ");
        printMemoryInfo();
        Serial.println();
    }
}

void SensorUtilityV2::debugPath(SensorModuleV2 *module, const char *sensorName, const char *path, bool showHeapMemory) {
    BaseSensV2 *sensor = module->getSensorByName(sensorName);
    if (!sensor || !isValidPath(path)) return;

    Serial.print("| ");
    Serial.print(sensorName);
    Serial.print(" - ");
    Serial.print(formatPathDisplay(path));
    Serial.print(": ");

    printPathValue(sensor, path, 3);

    Serial.print(" [");
    Serial.print(getTypeAsString(sensor->getTypeAtPath(path)));
    Serial.print("]");

    if (showHeapMemory) {
        Serial.print(" | mem: ");
        printMemoryInfo();
    }

    Serial.println();
}

void SensorUtilityV2::printValueWithType(BaseSensV2 *sensor, const char *valueKey, uint8_t precision) {
    SensorTypeCode typeCode = sensor->getValueTypeCode(valueKey);

    printTypedValue(sensor, valueKey, typeCode, precision);

    Serial.print(" [");
    Serial.print(BaseSensV2::getTypeCodeName(typeCode));
    Serial.print("]");
}

void SensorUtilityV2::printPathWithType(BaseSensV2 *sensor, const char *path, uint8_t precision) {
    if (!sensor || !isValidPath(path)) return;

    SensorTypeCode typeCode = sensor->getTypeAtPath(path);
    printPathValue(sensor, path, precision);

    Serial.print(" [");
    Serial.print(getTypeAsString(typeCode));
    Serial.print("]");
}

void SensorUtilityV2::printAllPaths(SensorModuleV2 *module, const char *sensorName) {
    BaseSensV2 *sensor = module->getSensorByName(sensorName);
    if (!sensor) return;

    Serial.print("All Paths for ");
    Serial.print(sensorName);
    Serial.println(":");

    JsonDocument *doc = sensor->getDocument();
    if (doc && (*doc)[sensorName].is<JsonObject>()) {
        JsonVariant sensorObj = (*doc)[sensorName];
        extractAllPaths(sensor, sensorName, sensorObj);
    }
}

void SensorUtilityV2::printSensorStructure(SensorModuleV2 *module, const char *sensorName) {
    BaseSensV2 *sensor = module->getSensorByName(sensorName);
    if (!sensor) return;

    Serial.print("Structure of ");
    Serial.print(sensorName);
    Serial.println(":");

    JsonDocument *doc = sensor->getDocument();
    if (doc && (*doc)[sensorName].is<JsonObject>()) {
        JsonVariant sensorObj = (*doc)[sensorName];
        printNestedStructure(sensorObj, "  ");
    }
}

void SensorUtilityV2::printMemoryInfo() {
#if defined(ESP32)
    Serial.print(ESP.getFreeHeap());
#elif defined(ESP8266)
    Serial.print(ESP.getFreeHeap());
#elif defined(ARDUINO_ARCH_AVR)
    extern int __heap_start, *__brkval;
    int v;
    Serial.print((int) &v - (__brkval == 0 ? (int) &__heap_start : (int) __brkval));
#else
    Serial.print("N/A");
#endif
}

String SensorUtilityV2::getValueAsString(BaseSensV2 *sensor, const char *valueKey, uint8_t precision) {
    SensorTypeCode typeCode = sensor->getValueTypeCode(valueKey);
    String result = "";

    switch (typeCode) {
        case TYPE_FLOAT: {
            float value = sensor->getValue<float>(valueKey);
            result = String(value, (unsigned int) precision);
            break;
        }
        case TYPE_INT: {
            int value = sensor->getValue<int>(valueKey);
            result = String(value);
            break;
        }
        case TYPE_LONG: {
            long value = sensor->getValue<long>(valueKey);
            result = String(value);
            break;
        }
        case TYPE_DOUBLE: {
            double value = sensor->getValue<double>(valueKey);
            result = String(value, (unsigned int) precision);
            break;
        }
        case TYPE_BOOL: {
            bool value = sensor->getValue<bool>(valueKey);
            result = value ? "true" : "false";
            break;
        }
        case TYPE_UINT: {
            unsigned int value = sensor->getValue<unsigned int>(valueKey);
            result = String(value);
            break;
        }
        case TYPE_ULONG: {
            unsigned long value = sensor->getValue<unsigned long>(valueKey);
            result = String(value);
            break;
        }
        case TYPE_STRING: {
            result = sensor->getValue<String>(valueKey);
            break;
        }
        case TYPE_UNKNOWN:
        default: {
            result = "N/A";
            break;
        }
    }

    return result;
}

String SensorUtilityV2::getPathAsString(BaseSensV2 *sensor, const char *path, uint8_t precision) {
    if (!sensor || !isValidPath(path)) return "N/A";

    SensorTypeCode typeCode = sensor->getTypeAtPath(path);
    String result = "";

    switch (typeCode) {
        case TYPE_FLOAT: {
            float value = sensor->getValueAtPath<float>(path);
            result = String(value, (unsigned int) precision);
            break;
        }
        case TYPE_INT: {
            int value = sensor->getValueAtPath<int>(path);
            result = String(value);
            break;
        }
        case TYPE_LONG: {
            long value = sensor->getValueAtPath<long>(path);
            result = String(value);
            break;
        }
        case TYPE_DOUBLE: {
            double value = sensor->getValueAtPath<double>(path);
            result = String(value, (unsigned int) precision);
            break;
        }
        case TYPE_BOOL: {
            bool value = sensor->getValueAtPath<bool>(path);
            result = value ? "true" : "false";
            break;
        }
        case TYPE_UINT: {
            unsigned int value = sensor->getValueAtPath<unsigned int>(path);
            result = String(value);
            break;
        }
        case TYPE_ULONG: {
            unsigned long value = sensor->getValueAtPath<unsigned long>(path);
            result = String(value);
            break;
        }
        case TYPE_STRING: {
            result = sensor->getValueAtPath<String>(path);
            break;
        }
        case TYPE_OBJECT:
            result = "{object}";
            break;
        case TYPE_ARRAY:
            result = "[array]";
            break;
        case TYPE_UNKNOWN:
        default:
            result = "N/A";
            break;
    }

    return result;
}

String SensorUtilityV2::getTypeAsString(SensorTypeCode typeCode) {
    return String(BaseSensV2::getTypeCodeName(typeCode));
}
/*
 *  sensor-module.cpp
 *
 *  sensor module c
 *  Created on: 2023. 4. 3
 */

#include "HardwareSerial.h"
#include "sensor-module.h"

SensorModule::SensorModule()
        : base(nullptr),
          name(nullptr),
          len(0),
          lenName(0),
          sensorInit(nullptr),
          sensorEnable(false),
          sensorReady(false) {
}

SensorModule::~SensorModule() {
    if (base != nullptr) {
        free(base);
        base = nullptr;
        len = 0;
    }
    if (doc != nullptr) {
        delete doc;
        doc = nullptr;
    }
}

void SensorModule::init(void (*initializeCallback)(void)) {
    if (base == nullptr) return;
    doc = new JsonDocument;
    this->sensorInit = new bool[len];
    for (uint8_t i = 0; i < len; i++) {
        base[i]->setDocument(name[i]);
        base[i]->setDocumentValue(doc);
        this->sensorInit[i] = base[i]->init();
        if (this->sensorInit[i]) {
            Serial.print("| [NAME]: ");
            Serial.print(name[i]);
            Serial.print(" [INFO]: Init Success");
            Serial.println();
        } else {
            Serial.print("| [NAME]: ");
            Serial.print(name[i]);
            Serial.print(" [ERROR]: Init Failed");
            Serial.println();
        }
    }
    this->enable();
    if (!this->sensorInit) return;
    if (initializeCallback != nullptr) initializeCallback();
}

void SensorModule::update(void (*updateCallback)(void)) {
    if (base == nullptr || !sensorEnable) return;
    if (updateCallback != nullptr && sensorReady) updateCallback();
    for (uint8_t i = 0; i < len; i++) {
        if (this->sensorInit[i]) base[i]->update();
        else {
            Serial.print("| [NAME]: ");
            Serial.print(name[i]);
            Serial.print(" [ERROR]: Not Initialized Correctly");
            Serial.println();
        }
    }
    if (!sensorReady) sensorReady = true;
}

void SensorModule::update(const char *searchName) {
    if (base == nullptr || !sensorEnable) return;
    BaseSens *module = getModuleByNamePtr(searchName);
    module->update();
    if (!sensorReady) sensorReady = true;
}

bool SensorModule::isReady(void (*readyCallback)()) {
    if (readyCallback != nullptr) readyCallback();
    return sensorReady && sensorInit;
}

void SensorModule::enable() {
    sensorEnable = true;
}

void SensorModule::disable() {
    sensorEnable = false;
}

bool SensorModule::isEnable() const {
    return sensorEnable;
}

void SensorModule::addModule(BaseSens *sensModule) {
    auto **newBase = (BaseSens **) realloc(base, (len + 1) * sizeof(BaseSens *));  // increase length by 1
    if (newBase == nullptr) {
        Serial.println("Memory Allocation Failed !");
        return;
    }
    base = newBase;
    base[len] = sensModule;  // assign to correct index
    len++;
}

void SensorModule::addName(const char *newName) {
    char *dynamicName = (char *) malloc(strlen(newName) + 1);
    if (dynamicName != nullptr) {
        strcpy(dynamicName, newName);
        char **newTextArray = (char **) realloc(name, (lenName + 1) * sizeof(char *));
        if (newTextArray != nullptr) {
            newTextArray[lenName] = dynamicName;
            name = newTextArray;
            lenName++;
        } else {
            free(dynamicName);
        }
    }
}

void SensorModule::addModule(const char *newName, BaseSens *sensModule) {
    addModule(sensModule);
    addName(newName);
}

void SensorModule::addModule(const char *newName, BaseSens *(*callbackSensModule)()) {
    BaseSens *sensModule = callbackSensModule();
    if (sensModule != nullptr) addModule(newName, sensModule);
    else Serial.println("Error Add Module");
}

void SensorModule::removeModule(uint8_t index) {
    if (base == nullptr || index >= len) return;
    delete base[index];
    for (uint8_t i = index; i < len - 1; i++) {
        base[i] = base[i + 1];
    }
    len--;
    auto **newBase = (BaseSens **) realloc(base, len * sizeof(BaseSens *));
    if (newBase != nullptr) {
        base = newBase;
    }
}

JsonVariant SensorModule::operator[](const char *searchName) {
    auto module = getModuleByNamePtr(searchName);
    return module->getVariant(searchName);
}

JsonDocument SensorModule::operator()(const char *searchName) {
    auto module = getModuleByNamePtr(searchName);
    return module->getDocument();
}

BaseSens &SensorModule::getModule(uint8_t index) {
    return *(base[index]);
}

BaseSens * SensorModule::getModulePtr(uint8_t index) const {
    if (base == nullptr || index >= len) return nullptr;
    return base[index];
}

BaseSens &SensorModule::getModuleByName(const char *searchName) {
    int count = 0;
    for (int i = 0; i < lenName; ++i) {
        if (strcmp(name[i], searchName) == 0) return *(base[i]);
        count++;
    }
    return *(base[count]);
}

BaseSens *SensorModule::getModuleByNamePtr(const char *searchName) {
    int count = 0;
    for (int i = 0; i < lenName; ++i) {
        if (strcmp(name[i], searchName) == 0) return base[i];
        count++;
    }
    return base[count];
}

char * SensorModule::getName(uint8_t index) const {
    return name[index];
}

char **SensorModule::getNames() {
    return name;
}

void SensorModule::clearModules() {
    if (base == nullptr) return;
    for (uint8_t i = 0; i < len; i++) {
        delete base[i];
    }
    free(base);
    base = nullptr;
    len = 0;

    if (lenName > 0) {
        for (int i = 0; i < lenName; ++i) {
            free(name[i]);
        }
        free(name);
        name = nullptr;
        lenName = 0;
    }

    if (doc != nullptr) {
        delete doc;
        doc = nullptr;
    }
}

uint8_t SensorModule::getModuleCount() const {
    return len;
}

void SensorModule::setModule(uint8_t index, BaseSens *sensModule) {
    if (base == nullptr || index >= len) return;
    delete base[index];
    base[index] = sensModule;
}

void SensorModule::swapModules(uint8_t index1, uint8_t index2) {
    if (base == nullptr || index1 >= len || index2 >= len) return;
    BaseSens *temp = base[index1];
    base[index1] = base[index2];
    base[index2] = temp;
}

bool SensorModule::isModulePresent(BaseSens *sensModule) {
    if (base == nullptr) return false;
    for (uint8_t i = 0; i < len; i++) {
        if (base[i] == sensModule) {
            return true;
        }
    }
    return false;
}

bool SensorModule::isModulePresent(uint8_t index) {
    if (base == nullptr || index >= len) return false;
    return true;
}

void SensorModule::debug(const char *searchName, bool showHeapMemory, bool endl) {
    if (!isReady() || searchName == nullptr) return;

    Serial.print(F("| "));
    Serial.print(searchName);
    Serial.print(F(": "));

    auto variant = this->operator[](searchName);
    if (variant.is<JsonArray>()) {
        for (int i = 0; i < variant.size(); ++i) {
            Serial.print(F("["));
            Serial.print(i);
            Serial.print(F("] "));
            if (variant[i].is<int>()) {
                Serial.print(variant[i].as<int>());
            } else if (variant[i].is<float>()) {
                Serial.print(variant[i].as<float>());
            } else {
                Serial.print(variant[i].as<const char *>());
            }
            Serial.print(F(" "));
        }
    } else if (variant.is<JsonObject>()) {
        for (JsonPair kv: variant.as<JsonObject>()) {
            Serial.print(F("["));
            Serial.print(kv.key().c_str());
            Serial.print(F("] "));
            if (kv.value().is<int>()) {
                Serial.print(kv.value().as<int>());
            } else if (kv.value().is<float>()) {
                Serial.print(kv.value().as<float>());
            } else {
                Serial.print(kv.value().as<const char *>());
            }
            Serial.print(F(" "));
        }
    } else {
        if (variant.is<int>()) {
            Serial.print(variant.as<int>());
        } else if (variant.is<float>()) {
            Serial.print(variant.as<float>());
        } else {
            Serial.print(variant.as<const char *>());
        }
    }

#if defined(ESP32)
    if (showHeapMemory) {
        Serial.print(F("| mem: "));
        Serial.print(ESP.getFreeHeap());
    }
#else
    if (showHeapMemory) {
        Serial.print(F("| mem: "));
        Serial.print(freeMemory());
    }
#endif

    if (endl) Serial.println();

//    if (!isReady() || searchName == nullptr) return;
//    String output = "| " + String(searchName) + ": ";
//    auto variant = this->operator[](searchName);
//    if (variant.is<JsonArray>()) {
//        for (int i = 0; i < variant.size(); ++i) {
//            if (variant[i].is<int>()) {
//                output += "[" + String(i) + "] " + String(variant[i].as<int>()) + " ";
//            } else if (variant[i].is<float>() || variant[i].is<double>()) {
//                output += "[" + String(i) + "] " + String(variant[i].as<float>()) + " ";
//            } else {
//                output += "[" + String(i) + "] " + String(variant[i].as<const char *>()) + " ";
//            }
//        }
//    } else if (variant.is<JsonObject>()) {
//        for (JsonPair kv: variant.as<JsonObject>()) {
//            if (kv.value().is<int>()) {
//                output += "[" + String(kv.key().c_str()) + "] " + String(kv.value().as<int>()) + " ";
//            } else if (kv.value().is<float>() || kv.value().is<double>()) {
//                output += "[" + String(kv.key().c_str()) + "] " + String(kv.value().as<float>()) + " ";
//            } else {
//                output += "[" + String(kv.key().c_str()) + "] " + String(kv.value().as<const char *>()) + " ";
//            }
//        }
//    } else {
//        if (variant.is<int>()) {
//            output += String(variant.as<int>());
//        } else if (variant.is<float>() || variant.is<double>()) {
//            output += String(variant.as<float>());
//        } else {
//            output += String(variant.as<const char *>());
//        }
//    }
//    Serial.print(output);
//#if defined(ESP32)
//    if (showHeapMemory) {
//        Serial.print("| mem: ");
//        Serial.print(ESP.getFreeHeap());
//    }
//#else
//    if (showHeapMemory) {
//        Serial.print("| mem: ");
//        Serial.print(freeMemory());
//    }
//#endif
//    if (endl) Serial.println();
}

void SensorModule::debug(bool showHeapMemory) {
    if (!isReady()) return;
    for (uint8_t i = 0; i < len; i++) {
        debug(name[i], false, false);
    }
#if defined(ESP32)
    if (showHeapMemory) {
        Serial.print("| mem: ");
        Serial.print(ESP.getFreeHeap());
    }
#else
    if (showHeapMemory) {
        Serial.print("| mem: ");
        Serial.print(freeMemory());
    }
#endif
    Serial.println();
}

void SensorModule::debug(uint32_t time, bool showHeapMemory, void (*debugCallback)()) {
    if (!isReady()) return;
    static uint32_t debugTime = 0;
    if (millis() - debugTime >= time) {
        if (debugCallback != nullptr) {
            debugCallback();
        }
        debug(showHeapMemory);
        debugTime = millis();
    }
}

void SensorModule::debugPretty(uint32_t time) {
    if (!isReady()) return;
    static uint32_t debugPrettyTime = 0;
    if (millis() - debugPrettyTime >= time) {
        serializeJsonPretty((*doc), Serial);
        debugPrettyTime = millis();
    }
}

void SensorModule::print(const char *format, ...) {
    va_list args;
    va_start(args, format);
    char buffer[256];
    vsnprintf(buffer, sizeof(buffer), format, args);
    Serial.println(buffer);
    va_end(args);
}

void SensorModule::wait(uint32_t time) {
    delay(time);
}
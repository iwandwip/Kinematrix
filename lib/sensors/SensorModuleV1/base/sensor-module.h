/*  
 *  sensor-module.h
 *
 *  sensor module header
 *  Created on: 2023. 4. 3
 */

#pragma once

#ifndef SENSOR_MODULE_H
#define SENSOR_MODULE_H

#pragma message("[COMPILED]: sensor-module.h")

#include "Arduino.h"
#include "ArduinoJson.h"

#include "sensor-debug.h"
#include "sensor-header.h"
//#include "sensor-calibration.h"
#include "../addons/sensor-filter.h"

class BaseSens {
public:
    /*pure virtual function*/
    virtual bool init() = 0;
    virtual bool update() = 0;

    /*virtual function*/
    virtual void getValue(float *output);
    virtual void getValue(int *output);

    virtual float getValueF() const;

    virtual void setDocument(const char *objName);
    virtual void setDocumentValue(JsonDocument *docBase);
    virtual JsonDocument getDocument();
    virtual JsonVariant getVariant(const char *searchName);

    /*additional function*/
    virtual void process();

    BaseSens &operator=(const BaseSens &) = default;
    BaseSens &operator=(BaseSens &&) = default;
};

class SensorModule {
private:
    JsonDocument *doc;
    BaseSens **base;
    char **name;
    uint8_t len;
    uint8_t lenName;
    bool *sensorInit;
    bool sensorEnable;
    bool sensorReady;
public:
    SensorModule();
    ~SensorModule();

    virtual void init(void (*initializeCallback)() = nullptr);
    virtual void update(void (*updateCallback)() = nullptr);
    virtual void update(const char *searchName);
    virtual bool isReady(void (*readyCallback)() = nullptr);

    void enable();
    void disable();
    bool isEnable() const;

    void addModule(BaseSens *sensModule);
    void addName(const char *newName);

    void addModule(const char *newName, BaseSens *sensModule);
    void addModule(const char *newName, BaseSens *(*callbackSensModule)());

    void removeModule(uint8_t index);

    JsonVariant operator[](const char *searchName);
    JsonDocument operator()(const char *searchName);

    BaseSens &getModule(uint8_t index);
    BaseSens * getModulePtr(uint8_t index) const;
    BaseSens &getModuleByName(const char *searchName);
    BaseSens *getModuleByNamePtr(const char *searchName);

    char * getName(uint8_t index) const;
    char **getNames();
    void clearModules();

    uint8_t getModuleCount() const;
    void setModule(uint8_t index, BaseSens *sensModule);
    void swapModules(uint8_t index1, uint8_t index2);
    bool isModulePresent(BaseSens *sensModule);
    bool isModulePresent(uint8_t index);

    template<typename T>
    T *getModule(const char *searchName, void (*getModuleCb)(T *) = nullptr) {
        BaseSens *module = getModuleByNamePtr(searchName);
        T *modulePtr = static_cast<T *>(module);
        if (getModuleCb != nullptr && module != nullptr) getModuleCb(modulePtr);
        return modulePtr;
    }

    virtual void debug(const char *searchName, bool showHeapMemory = false, bool endl = true);
    virtual void debug(bool showHeapMemory = false);
    virtual void debug(uint32_t time, bool showHeapMemory = false, void (*debugCallback)() = nullptr);
    virtual void debugPretty(uint32_t time = 1000);
    static void print(const char *format, ...);
    static void wait(uint32_t time);
};

#endif  // SENSOR_MODULE_H
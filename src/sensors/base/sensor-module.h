/*  
 *  sensor-module.h
 *
 *  sensor module header
 *  Created on: 2023. 4. 3
 */

#pragma once

#ifndef SENSOR_MODULE_H
#define SENSOR_MODULE_H

#include "Arduino.h"
#include "HardwareSerial.h"
#include "SPI.h"

typedef enum {
    SENS_RET_RAW_DATA,
    SENS_RET_FILTERED_DATA,
    SENS_RET_OFFSET_DATA,
    SENS_RET_ACT_DATA,
    SENS_RET_AVG_DATA,
    SENS_RET_TOTAL_DATA,
} sens_ret_index_t;

class BaseSens {
public:
    /*pure virtual function*/
    virtual void init() = 0;
    virtual void update() = 0;

    /*virtual function*/
    virtual void getValue(float *output);
    virtual void getValue(int *output);
    virtual void getValue(char *output);
    virtual void getValue(String *output);

    /*additional function*/
    virtual void process();

    BaseSens &operator=(const BaseSens &) = default;
    BaseSens &operator=(BaseSens &&) = default;
};

class SensorModule {
private:
    BaseSens **base;
    char **name;
    uint8_t len;
    uint8_t lenName;
public:
    SensorModule();
    ~SensorModule();
    void init(void (*initializeCallback)() = nullptr);
    void update(void (*updateCallback)() = nullptr);

    void addModule(BaseSens *sensModule);
    void addModule(BaseSens *sensModule, const char *newName);
    void removeModule(uint8_t index);
    BaseSens &getModule(uint8_t index);
    BaseSens *getModulePtr(uint8_t index);
    BaseSens &getModuleByName(const char *searchName);
    void clearModules();

    uint8_t getModuleCount();
    void setModule(uint8_t index, BaseSens *sensModule);
    void swapModules(uint8_t index1, uint8_t index2);
    bool isModulePresent(BaseSens *sensModule);
    bool isModulePresent(uint8_t index);
};

#endif  // SENSOR_MODULE_H
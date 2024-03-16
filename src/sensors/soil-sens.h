/*
 *  soil-sens.h
 *
 *  soil sensor lib
 *  Created on: 2023. 4. 3
 */

#pragma once

#ifndef SOIL_SENS_H
#define SOIL_SENS_H

#include "Arduino.h"
#include "base/sensor-module.h"

class SoilMoistureSens : public BaseSens {
private:
    /*variables*/
    void *sensorClass;
    float sensorValue;
    uint8_t sensorPin;
    uint32_t sensorTimer;

    /*optional object*/
    void (*sensorCallbackFunc)();

public:
    SoilMoistureSens();
    explicit SoilMoistureSens(uint8_t _pin);
    ~SoilMoistureSens();
    void init() override;
    void update() override;
    void getValue(float *output) override;
    float getValueSoilMoistureSens() const;
    void setPins(uint8_t _pin);
};

#endif  // SOIL_SENS_H
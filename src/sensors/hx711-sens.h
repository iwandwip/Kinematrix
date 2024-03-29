/*
 *  hx711-sens.h
 *
 *  hx711 sensor lib
 *  Created on: 2023. 4. 3
 */

#pragma once

#ifndef HX711_SENS_H
#define HX711_SENS_H

#include "Arduino.h"
#include "base/sensor-module.h"
#include "HX711.h"

class HX711Sens : public BaseSens, public HX711 {
private:
    float sensorValue;
    uint32_t sensorTimer[2];
    uint8_t sensorDOUTPin;
    uint8_t sensorSCKPin;
    using HX711::HX711;

public:
    HX711Sens();
    explicit HX711Sens(uint8_t _sensorDOUTPin, uint8_t _sensorSCKPin);
    void init() override;
    void update() override;
    void getValue(float *output) override;
    void getValue(int *output) override;
    void getValue(char *output) override;
    float getCalibrateFactorInit(float weight);
    bool isReady();
    void setScaleDelay(long time, float scale = 1.f);
    void setScale(float scale = 1.f);
    void tareDelay(long time);
    float getUnits(byte times = 1);
    uint32_t getADC(byte times = 1);
    float getCalibrateFactor(float units, float weight);
    float getValueWeight(bool isCanZero = true) const;
    void setPins(uint8_t _sensorDOUTPin, uint8_t _sensorSCKPin);
};

#endif  // HX711_SENS_H
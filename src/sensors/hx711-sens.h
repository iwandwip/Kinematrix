/*
 *  hx711-sens.h
 *
 *  hx711 sensor lib
 *  Created on: 2023. 4. 3
 */

#ifdef USE_HX711_SENS
#pragma once

#ifndef HX711_SENS_H
#define HX711_SENS_H

#include "Arduino.h"
#include "base/sensor-module.h"
#include "HX711.h"

class HX711Sens : public BaseSens, public HX711 {
private:
    JsonDocument *doc;
    const char *name;

    uint32_t sensorTimer[2];
    uint8_t sensorDOUTPin;
    uint8_t sensorSCKPin;
    float format;
    float (*sensorFilterCb)(float value);

    using HX711::HX711;

public:
    explicit HX711Sens(uint8_t _sensorDOUTPin, uint8_t _sensorSCKPin, float _format = HX711Sens::G);
    bool init() override;
    bool update() override;

    void setDocument(const char *objName) override;
    void setDocumentValue(JsonDocument *docBase) override;
    JsonDocument getDocument() override;
    JsonVariant getVariant(const char *searchName) override;

    void filter(float (*sensorFilterCallback)(float value) = nullptr);

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

    constexpr static const float G = 1.0;
    constexpr static const float KG = 1000.0;
    constexpr static const float POUND = 453.6;
};

#endif  // HX711_SENS_H
#endif
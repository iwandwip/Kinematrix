#pragma once

#ifndef HX711_SENS_H
#define HX711_SENS_H

#pragma message("[COMPILED]: hx711-sens.h")

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

    float stabilityTolerance;
    uint8_t sampleCount;
    uint32_t stabilityTime;
    float resetThreshold;

    float *lastWeights;
    uint8_t weightIndex;
    bool isLocked;
    float lockedWeight;
    uint32_t stabilityTimer;

    using HX711::HX711;

public:
    explicit HX711Sens(uint8_t _sensorDOUTPin, uint8_t _sensorSCKPin, float _format = HX711Sens::G,
                       float _stabilityTolerance = 0.2, uint8_t _sampleCount = 5,
                       uint32_t _stabilityTime = 3000, float _resetThreshold = 5.0);
    ~HX711Sens();
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
    [[nodiscard]] float getValueWeight(bool isCanZero = true) const;
    void setPins(uint8_t _sensorDOUTPin, uint8_t _sensorSCKPin);

    void setStabilityTolerance(float tolerance);
    void setSampleCount(uint8_t count);
    void setStabilityTime(uint32_t time);
    void setResetThreshold(float threshold);
    void resetLock();

    constexpr static const float G = 1.0;
    constexpr static const float KG = 1000.0;
    constexpr static const float POUND = 453.6;
};

#endif
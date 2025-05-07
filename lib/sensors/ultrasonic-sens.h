#pragma once

#ifndef ULTRASONIC_SENS_H
#define ULTRASONIC_SENS_H

#pragma message("[COMPILED]: ultrasonic-sens.h")

#include "Arduino.h"
#include "base/sensor-module.h"
#include "NewPing.h"

class UltrasonicSens : public BaseSens, public NewPing {
private:
    JsonDocument *doc;
    const char *name;

    uint8_t sensorPin;
    uint32_t sensorTimer;

    float stabilityTolerance;
    uint8_t sampleCount;
    uint32_t stabilityTime;
    float resetThreshold;

    float *lastDistances;
    uint8_t distanceIndex;
    bool isLocked;
    float lockedDistance;
    uint32_t stabilityTimer;

    using NewPing::NewPing;

public:
    explicit UltrasonicSens(uint8_t trigger_pin, uint8_t echo_pin,
                            unsigned int max_distance = 400,
                            float _stabilityTolerance = 1.0,
                            uint8_t _sampleCount = 5,
                            uint32_t _stabilityTime = 1000,
                            float _resetThreshold = 10.0);
    ~UltrasonicSens();
    bool init() override;
    bool update() override;

    void setDocument(const char *objName) override;
    void setDocumentValue(JsonDocument *docBase) override;
    JsonDocument getDocument() override;
    JsonVariant getVariant(const char *searchName) override;

    float getValueCm() const;
    float getValueIn();
    void setPins(uint8_t _trigPin, uint8_t _echoPin);

    void setStabilityTolerance(float tolerance);
    void setSampleCount(uint8_t count);
    void setStabilityTime(uint32_t time);
    void setResetThreshold(float threshold);
    void resetLock();
    bool isValueLocked() const;
    float getLockedValue() const;
};

#endif
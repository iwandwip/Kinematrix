/*
 *  ultrasonic-sens.h
 *
 *  ultrasonic sensor lib
 *  Created on: 2023. 4. 3
 */
#ifdef USE_ULTRASONIC_SENS
#pragma once

#ifndef ULTRASONIC_SENS_H
#define ULTRASONIC_SENS_H

#include "Arduino.h"
#include "base/sensor-module.h"
#include "NewPing.h"

class UltrasonicSens : public BaseSens, public NewPing {
private:
    JsonDocument *doc;
    const char *name;

    uint8_t sensorPin;
    uint32_t sensorTimer;

    using NewPing::NewPing;

public:
    ~UltrasonicSens();
    bool init() override;
    bool update() override;

    void setDocument(const char *objName) override;
    void setDocumentValue(JsonDocument *docBase) override;
    JsonDocument getDocument() override;
    JsonVariant getVariant(const char *searchName) override;

    float getValueCm() const;
    float getValueIn();
    void setPins(uint8_t _pin);
};

#endif  // ULTRASONIC_SENS_H
#endif
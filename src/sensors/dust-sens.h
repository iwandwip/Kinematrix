/*
 *  dust-sens.h
 *
 *  dust sensor lib
 *  Created on: 2023. 4. 3
 */

#pragma once

#ifndef DUST_SENS_H
#define DUST_SENS_H

#include "Arduino.h"
#include "base/sensor-module.h"

class DustSens : public BaseSens {
private:
    JsonDocument *doc;
    const char *name;

    const uint32_t samplingTime = 280;
    const uint32_t deltaTime = 40;
    const uint32_t sleepTime = 9680;

    float voMeasured = 0;
    float calcVoltage = 0;
    float dustDensity = 0;

    enum State {
        IDLE,
        SAMPLING,
        WAITING,
        SLEEPING
    };

    State currentState;

    uint8_t ledPin;
    uint8_t sensorPin;
    uint32_t sensorTimer;

public:
    DustSens(uint8_t _ledPin, uint8_t _measurePin);
    virtual ~DustSens();
    bool init() override;
    bool update() override;

    void setDocument(const char *objName) override;
    void setDocumentValue(JsonDocument *docBase) override;
    JsonDocument getDocument() override;
    JsonVariant getVariant(const char *searchName) override;

    float getValueDustSens() const;
    void setPins(uint8_t _pin);
};

#endif  // DUST_SENS_H
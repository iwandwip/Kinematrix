/*
 *  analog-sens.h
 *
 *  analog sensor lib
 *  Created on: 2023. 4. 3
 */

#pragma once

#ifndef ANALOG_SENS_H
#define ANALOG_SENS_H

#pragma message("[COMPILED]: analog-sens.h")

#include "Arduino.h"
#include "base/sensor-module.h"

class AnalogSens : public BaseSens {
private:
    JsonDocument *doc;
    const char *name;

    float voltageReference;
    int adcRange;

    uint8_t sensorPin;
    uint32_t sensorTimer;

    void (*onCustomData)(JsonVariant sensorRef, int analogValue, float voltage);

public:
    AnalogSens(uint8_t _pin, float _vref, int _adcRange,
               void (*_onCustomData)(JsonVariant value, int analogValue, float voltage) = nullptr);
    virtual ~AnalogSens();
    bool init() override;
    bool update() override;

    void setDocument(const char *objName) override;
    void setDocumentValue(JsonDocument *docBase) override;
    JsonDocument getDocument() override;
    JsonVariant getVariant(const char *searchName) override;

    float getValueAnalogSens() const;
    void setPins(uint8_t _pin);
};

#endif  // ANALOG_SENS_H
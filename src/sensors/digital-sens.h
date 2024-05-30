/*
 *  digital-sens.h
 *
 *  digital sensor lib
 *  Created on: 2023. 4. 3
 */

#pragma once

#ifndef DIGITAL_SENS_H
#define DIGITAL_SENS_H

#include "Arduino.h"
#include "base/sensor-module.h"

class DigitalSens : public BaseSens {
private:
    JsonDocument *doc;
    const char *name;

    uint8_t mode;

    uint8_t sensorPin;
    uint32_t sensorTimer;

public:
    DigitalSens(uint8_t _inputPin, uint8_t _mode = INPUT);
    virtual ~DigitalSens();
    bool init() override;
    bool update() override;

    void setDocument(const char *objName) override;
    void setDocumentValue(JsonDocument *docBase) override;
    JsonDocument getDocument() override;
    JsonVariant getVariant(const char *searchName) override;

    float getValueDigitalSens() const;
    void setPins(uint8_t _pin);
};

#endif  // DIGITAL_SENS_H
/*
 *  soilph-sens.h
 *
 *  soilph sensor lib
 *  Created on: 2023. 4. 3
 */

#pragma once

#ifndef SOILPH_SENS_H
#define SOILPH_SENS_H

#pragma message("[COMPILED]: soilph-sens.h")

#include "Arduino.h"
#include "base/sensor-module.h"

class SoilPHSens : public BaseSens {
private:
    JsonDocument *doc;
    const char *name;

    uint32_t pHTimer;
    int pHState;
    uint8_t relayPin;

    uint8_t sensorPin;
    uint32_t sensorTimer;

public:
    explicit SoilPHSens(uint8_t _pin, uint8_t _relayPin);
    ~SoilPHSens();

    bool init() override;
    bool update() override;

    void setDocument(const char *objName) override;
    void setDocumentValue(JsonDocument *docBase) override;
    JsonDocument getDocument() override;
    JsonVariant getVariant(const char *searchName) override;

    float getValueSoilPHSens() const;
    void setPins(uint8_t _pin);
};

#endif  // SOILPH_SENS_H
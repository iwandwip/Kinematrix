/*
 *  turbidity-sens.h
 *
 *  turbidity sensor lib
 *  Created on: 2023. 4. 3
 */

#pragma once

#ifndef TURBIDITY_SENS_H
#define TURBIDITY_SENS_H

#pragma message("[COMPILED]: turbidity-sens.h")

#include "Arduino.h"
#include "base/sensor-module.h"

class TurbiditySens : public BaseSens {
private:
    JsonDocument *doc;
    const char *name;

    uint8_t sensorPin;
    uint32_t sensorTimer;

public:
    TurbiditySens(int _sensorPin);
    virtual ~TurbiditySens();
    bool init() override;
    bool update() override;

    void setDocument(const char *objName) override;
    void setDocumentValue(JsonDocument *docBase) override;
    JsonDocument getDocument() override;
    JsonVariant getVariant(const char *searchName) override;

    float roundToDp(float input, int decimalPlace);
    float getValueTurbiditySens() const;
    void setPins(uint8_t _pin);
};

#endif  // TURBIDITY_SENS_H
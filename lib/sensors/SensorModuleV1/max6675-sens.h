/*
 *  max6675-sens.h
 *
 *  max6675 sensor lib
 *  Created on: 2023. 4. 3
 */

#pragma once

#ifndef MAX6675_SENS_H
#define MAX6675_SENS_H

#pragma message("[COMPILED]: max6675-sens.h")

#include "Arduino.h"
#include "base/sensor-module.h"
#include "max6675.h"

class MAX6675Sens : public BaseSens, public MAX6675{
private:
    JsonDocument *doc;
    const char *name;

    int enumRandomValue;
    float dummyValue1;
    float dummyValue2;

    uint8_t sensorPin;
    uint32_t sensorTimer;

    using MAX6675::MAX6675;
public:
    virtual ~MAX6675Sens();
    bool init() override;
    bool update() override;

    void setDocument(const char *objName) override;
    void setDocumentValue(JsonDocument *docBase) override;
    JsonDocument getDocument() override;
    JsonVariant getVariant(const char *searchName) override;

    float getValueMAX6675Sens() const;
    void setPins(uint8_t _pin);
};

#endif  // MAX6675_SENS_H
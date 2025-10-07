/*
 *  ina219-sens.h
 *
 *  ina219 sensor lib
 *  Created on: 2023. 4. 3
 */

#pragma once

#ifndef INA219_SENS_H
#define INA219_SENS_H

#pragma message("[COMPILED]: ina219-sens.h")

#include "Arduino.h"
#include "base/sensor-module.h"
#include "Adafruit_INA219.h"

class INA219Sens : public BaseSens, public Adafruit_INA219 {
private:
    JsonDocument *doc;
    const char *name;

    uint8_t sensorPin;
    uint32_t sensorTimer;

    using Adafruit_INA219::Adafruit_INA219;
public:
    virtual ~INA219Sens();
    bool init() override;
    bool update() override;

    void setDocument(const char *objName) override;
    void setDocumentValue(JsonDocument *docBase) override;
    JsonDocument getDocument() override;
    JsonVariant getVariant(const char *searchName) override;

    float getValueINA219Sens() const;
    void setPins(uint8_t _pin);
};

#endif  // INA219_SENS_H
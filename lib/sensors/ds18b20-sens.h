/*
 *  ds18b20-sens.h
 *
 *  ds18b20 sensor lib
 *  Created on: 2023. 4. 3
 */

#pragma once

#ifndef DS18B20_SENS_H
#define DS18B20_SENS_H

#pragma message("[COMPILED]: ds18b20-sens.h")

#include "Arduino.h"
#include "base/sensor-module.h"
#include "DallasTemperature.h"

class DS18B20Sens : public BaseSens, public DallasTemperature {
private:
    JsonDocument *doc;
    const char *name;

    uint8_t sensorPin;
    uint32_t sensorTimer;

    /*optional object*/
    using DallasTemperature::DallasTemperature;

public:
    explicit DS18B20Sens(uint8_t _pin);
    explicit DS18B20Sens(OneWire *_wire);
    ~DS18B20Sens();

    bool init() override;
    bool update() override;

    void setDocument(const char *objName) override;
    void setDocumentValue(JsonDocument *docBase) override;
    JsonDocument getDocument() override;
    JsonVariant getVariant(const char *searchName) override;

    float getValueTemperature() const;
    void setPins(uint8_t _pin);
};

#endif  // DS18B20_SENS_H
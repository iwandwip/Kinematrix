/*
 *  aht-sens.h
 *
 *  aht sensor lib
 *  Created on: 2023. 4. 3
 */

#pragma once

#ifndef AHT_SENS_H
#define AHT_SENS_H

#pragma message("[COMPILED]: aht-sens.h")

#include "Arduino.h"
#include "base/sensor-module.h"
#include "Adafruit_Sensor.h"
#include "Adafruit_AHTX0.h"

class AHTSens : public BaseSens, public Adafruit_AHTX0 {
private:
    JsonDocument *doc;
    const char *name;

    uint8_t sensorPin;
    uint32_t sensorTimer;

    using Adafruit_AHTX0::Adafruit_AHTX0;
public:
    virtual ~AHTSens();
    bool init() override;
    bool update() override;

    void setDocument(const char *objName) override;
    void setDocumentValue(JsonDocument *docBase) override;
    JsonDocument getDocument() override;
    JsonVariant getVariant(const char *searchName) override;

    float getValueAHTSens() const;
    void setPins(uint8_t _pin);
};

#endif  // AHT_SENS_H
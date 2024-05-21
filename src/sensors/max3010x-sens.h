/*
 *  max3010x-sens.h
 *
 *  max3010x sensor lib
 *  Created on: 2023. 4. 3
 */

#pragma once

#ifndef MAX_3010X_SENS_H
#define MAX_3010X_SENS_H

#include "Arduino.h"
#include "base/sensor-module.h"
#include "MAX30105.h"
#include "heartRate.h"

const byte RATE_SIZE = 4; // Increase this for more averaging. 4 is good.

class MAX3010xSens : public BaseSens, public MAX30105 {
private:
    JsonDocument *doc;
    const char *name;

    byte rates[RATE_SIZE]; // Array of heart rates
    byte rateSpot = 0;
    long lastBeat = 0; // Time at which the last
    float beatsPerMinute = 0;
    int beatAvg = 0;

    uint8_t sensorPin;
    uint32_t sensorTimer;

public:
    virtual ~MAX3010xSens();
    bool init() override;
    bool update() override;

    void setDocument(const char *objName) override;
    void setDocumentValue(JsonDocument *docBase) override;
    JsonDocument getDocument() override;
    JsonVariant getVariant(const char *searchName) override;

    float getValueMAX3010xSens() const;
    void setPins(uint8_t _pin);
};

#endif  // MAX_3010X_SENS_H
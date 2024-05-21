/*
 *  soil-sens.h
 *
 *  soil sensor lib
 *  Created on: 2023. 4. 3
 */

#pragma once

#ifndef SOIL_SENS_H
#define SOIL_SENS_H

#include "Arduino.h"
#include "base/sensor-module.h"

class SoilMoistureSens : public BaseSens {
private:
    JsonDocument *doc;
    const char *name;

    uint8_t sensorPin;
    uint32_t sensorTimer;

public:
    explicit SoilMoistureSens(uint8_t _pin);
    ~SoilMoistureSens();

    bool init() override;
    bool update() override;

    void setDocument(const char *objName) override;
    void setDocumentValue(JsonDocument *docBase) override;
    JsonDocument getDocument() override;
    JsonVariant getVariant(const char *searchName) override;

    float getValueSoilMoistureSens() const;
    void setPins(uint8_t _pin);
};

#endif  // SOIL_SENS_H
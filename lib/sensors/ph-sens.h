/*
 *  ph-sens.h
 *
 *  ph sensor lib
 *  Created on: 2023. 4. 3
 */

#pragma once

#ifndef PH_SENS_H
#define PH_SENS_H

#pragma message("[COMPILED]: abstract-sens.h")

#include "Arduino.h"
#include "base/sensor-module.h"

#define PH_SENS_BUFFER_SIZE 10

class PhSens : public BaseSens {
private:
    JsonDocument *doc;
    const char *name;

    float voltage;
    float resolution;
    float *calibrationValue;
    uint32_t bufferAnalog[PH_SENS_BUFFER_SIZE];

    uint8_t sensorPin;
    uint32_t sensorTimer;

public:
    PhSens(uint8_t sensorPin, float voltage, float resolution, float* calibrationValue);
    virtual ~PhSens();
    bool init() override;
    bool update() override;

    void setDocument(const char *objName) override;
    void setDocumentValue(JsonDocument *docBase) override;
    JsonDocument getDocument() override;
    JsonVariant getVariant(const char *searchName) override;

    float getValuePhSens() const;
    void setPins(uint8_t _pin);
};

#endif  // PH_SENS_H
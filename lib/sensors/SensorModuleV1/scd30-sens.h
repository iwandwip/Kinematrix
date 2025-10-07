/*
 *  scd30-sens.h
 *
 *  scd30 sensor lib
 *  Created on: 2023. 4. 3
 */

#pragma once

#ifndef SCD30_SENS_H
#define SCD30_SENS_H

#pragma message("[COMPILED]: scd30-sens.h")

#include "Arduino.h"
#include "SensirionI2cScd30.h"
#include "Wire.h"
#include "base/sensor-module.h"

class SCD30Sens : public BaseSens, public SensirionI2cScd30 {
private:
    JsonDocument *doc;
    const char *name;

    char errorMessage[128];
    int16_t error;

    uint32_t sensorTimer;

    using SensirionI2cScd30::SensirionI2cScd30;
public:
    virtual ~SCD30Sens();
    bool init() override;
    bool update() override;

    void setDocument(const char *objName) override;
    void setDocumentValue(JsonDocument *docBase) override;
    JsonDocument getDocument() override;
    JsonVariant getVariant(const char *searchName) override;
};

#endif  // SCD30_SENS_H
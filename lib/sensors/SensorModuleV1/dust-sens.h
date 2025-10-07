/*
 *  dust-sens.h
 *
 *  dust sensor lib
 *  Created on: 2023. 4. 3
 */

#pragma once

#ifndef DUST_SENS_H
#define DUST_SENS_H

#pragma message("[COMPILED]: dust-sens.h")

#include "Arduino.h"
#include "base/sensor-module.h"
#include "lib/Dust/Dust.h"

class DustSens : public BaseSens, public GP2YDustSensor {
private:
    JsonDocument *doc;
    const char *name;

    uint32_t sensorTimer;

    using GP2YDustSensor::GP2YDustSensor;
public:
    virtual ~DustSens();
    bool init() override;
    bool update() override;

    void setDocument(const char *objName) override;
    void setDocumentValue(JsonDocument *docBase) override;
    JsonDocument getDocument() override;
    JsonVariant getVariant(const char *searchName) override;

    float getValueDustSens() const;
    void setPins(uint8_t _pin);
};

#endif  // DUST_SENS_H
/*
 *  max30100lib-sens.h
 *
 *  max30100lib sensor lib
 *  Created on: 2023. 4. 3
 */

#pragma once

#ifndef MAX30100LIB_SENS_H
#define MAX30100LIB_SENS_H

#pragma message("[COMPILED]: max30100lib-sens.h")

#include "Arduino.h"
#include "base/sensor-module.h"
#include "Wire.h"
#include "MAX30100_PulseOximeter.h"

class MAX30100lib : public BaseSens, public PulseOximeter {
private:
    JsonDocument *doc;
    const char *name;

    uint8_t sensorPin;
    uint32_t sensorTimer;

    using PulseOximeter::PulseOximeter;

public:
    MAX30100lib(void (*_onBeatCallback)() = nullptr);
    virtual ~MAX30100lib();
    bool init() override;
    bool update() override;

    void setDocument(const char *objName) override;
    void setDocumentValue(JsonDocument *docBase) override;
    JsonDocument getDocument() override;
    JsonVariant getVariant(const char *searchName) override;

    float getValueMAX30100lib() const;
    void setPins(uint8_t _pin);
};

#endif  // MAX30100LIB_SENS_H
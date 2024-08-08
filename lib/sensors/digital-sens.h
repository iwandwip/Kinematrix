/*  https://esp32.com/viewtopic.php?t=4061
 *  digital-sens.h
 *
 *  digital sensor lib
 *  Created on: 2023. 4. 3
 */

#pragma once

#ifndef DIGITAL_SENS_H
#define DIGITAL_SENS_H

#pragma message("[COMPILED]: digital-sens.h")

#include "Arduino.h"
#include "base/sensor-module.h"
#include "../modules/io/input-module.h"

class DigitalSens : public BaseSens, public DigitalIn {
private:
    JsonDocument *doc;
    const char *name;

    uint8_t mode;

    uint8_t sensorPin;
    uint32_t sensorTimer;

    using DigitalIn::DigitalIn;
public:
    virtual ~DigitalSens();
    bool init() override;
    bool update() override;

    void setDocument(const char *objName) override;
    void setDocumentValue(JsonDocument *docBase) override;
    JsonDocument getDocument() override;
    JsonVariant getVariant(const char *searchName) override;

    float getValueDigitalSens() const;
    void setPins(uint8_t _pin);
};

#endif  // DIGITAL_SENS_H
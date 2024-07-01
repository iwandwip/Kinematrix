/*
 *  rpm-sens.h
 *
 *  rpm sensor lib
 *  Created on: 2023. 4. 3
 */

#pragma once

#ifndef RPM_SENS_H
#define RPM_SENS_H

#include "Arduino.h"
#include "base/sensor-module.h"

class RPMSens : public BaseSens {
private:
    JsonDocument *doc;
    const char *name;

    volatile int encoderCount;
    uint32_t interval;

    int encoderPin;
    void (*interruptCallback)();
    int mode;

    uint8_t sensorPin;
    uint32_t sensorTimer;

public:
    RPMSens(int _encoderPin, void (*_interruptCallback)() = nullptr, int _mode = RISING);
    virtual ~RPMSens();
    bool init() override;
    bool update() override;

    void count();

    void setDocument(const char *objName) override;
    void setDocumentValue(JsonDocument *docBase) override;
    JsonDocument getDocument() override;
    JsonVariant getVariant(const char *searchName) override;

    float getValueRPMSens() const;
    void setPins(uint8_t _pin);
};

#endif  // RPM_SENS_H
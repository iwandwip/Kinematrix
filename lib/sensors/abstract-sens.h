/*
 *  abstract-sens.h
 *
 *  abstract sensor lib
 *  Created on: 2023. 4. 3
 */

#pragma once

#ifndef ABSTRACT_SENS_H
#define ABSTRACT_SENS_H

#pragma message("[COMPILED]: abstract-sens.h")

#include "Arduino.h"
#include "base/sensor-module.h"

class Abstract : public BaseSens {
private:
    JsonDocument *doc;
    const char *name;

    int enumRandomValue;
    float dummyValue1;
    float dummyValue2;

    uint8_t sensorPin;
    uint32_t sensorTimer;

public:
    Abstract(int _enumRandomValue = 0);
    Abstract(int _enumRandomValue, float _dummyValue1, float _dummyValue2 = 0);
    virtual ~Abstract();
    bool init() override;
    bool update() override;

    void setDocument(const char *objName) override;
    void setDocumentValue(JsonDocument *docBase) override;
    JsonDocument getDocument() override;
    JsonVariant getVariant(const char *searchName) override;

    float getValueAbstract() const;
    void setPins(uint8_t _pin);
};

#endif  // ABSTRACT_SENS_H
/*
 *  abstract-sens.h
 *
 *  abstract sensor lib
 *  Created on: 2023. 4. 3
 */

#pragma once

#ifndef ABSTRACT_SENS_H
#define ABSTRACT_SENS_H

#include "Arduino.h"
#include "base/sensor-module.h"

class Abstract : public BaseSens {
private:
    JsonDocument *doc;
    const char *name;

    bool randomValue;

    uint8_t sensorPin;
    uint32_t sensorTimer;

public:
    explicit Abstract(bool random_value = false);
    virtual ~Abstract();
    void init() override;
    void update() override;

    void setDocument(const char *objName) override;
    void setDocumentValue(JsonDocument *docBase) override;
    JsonDocument getDocument() override;
    JsonVariant getVariant(const char *searchName) override;

    float getValueAbstract() const;
    void setPins(uint8_t _pin);
};

#endif  // ABSTRACT_SENS_H
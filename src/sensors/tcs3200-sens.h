/*
 *  tcs3200-sens.h
 *
 *  tcs3200 sensor lib
 *  Created on: 2023. 4. 3
 */

#ifdef USE_TCS3200_SENS
#pragma once

#ifndef TCS_3200_SENS_H
#define TCS_3200_SENS_H

#include "Arduino.h"
#include "base/sensor-module.h"
#include "tcs3200.h"

class TCS3200Sens : public BaseSens, public tcs3200 {
private:
    JsonDocument *doc;
    const char *name;

    uint8_t sensorPin;
    uint32_t sensorTimer;

    using tcs3200::tcs3200;

public:
    virtual ~TCS3200Sens();
    bool init() override;
    bool update() override;

    void setDocument(const char *objName) override;
    void setDocumentValue(JsonDocument *docBase) override;
    JsonDocument getDocument() override;
    JsonVariant getVariant(const char *searchName) override;

    float getValueTCS3200Sens() const;
    void setPins(uint8_t _pin);
};


TCS3200Sens::~TCS3200Sens() = default;

bool TCS3200Sens::init() {
    if (strcmp(name, "") == 0 && doc == nullptr) {
        name = "TCS3200Sens";
        doc = new JsonDocument;
    }
    (*doc)[name]["r"] = 0;
    (*doc)[name]["g"] = 0;
    (*doc)[name]["b"] = 0;
    return true;
}

bool TCS3200Sens::update() {
    if (millis() - sensorTimer >= 200) {
        (*doc)[name]["r"] = tcs3200::colorRead('r');
        (*doc)[name]["g"] = tcs3200::colorRead('g');
        (*doc)[name]["b"] = tcs3200::colorRead('b');
        sensorTimer = millis();
        return true;
    }
    return false;
}

void TCS3200Sens::setDocument(const char *objName) {
    name = objName;
}

void TCS3200Sens::setDocumentValue(JsonDocument *docBase) {
    doc = docBase;
}

JsonDocument TCS3200Sens::getDocument() {
    return (*doc);
}

JsonVariant TCS3200Sens::getVariant(const char *searchName) {
    return (*doc)[searchName];
}

float TCS3200Sens::getValueTCS3200Sens() const {
    return (*doc)[name].as<float>();
}

void TCS3200Sens::setPins(uint8_t _pin) {
    sensorPin = _pin;
}

#endif  // TCS_3200_SENS_H
#endif
/*
 *  soilph-sens.h
 *
 *  soilph sensor lib
 *  Created on: 2023. 4. 3
 */
#ifdef USE_SOILPH_SENS
#pragma once

#ifndef SOILPH_SENS_H
#define SOILPH_SENS_H

#include "Arduino.h"
#include "base/sensor-module.h"

class SoilPHSens : public BaseSens {
private:
    JsonDocument *doc;
    const char *name;

    uint8_t sensorPin;
    uint32_t sensorTimer;

public:
    explicit SoilPHSens(uint8_t _pin);
    ~SoilPHSens();

    bool init() override;
    bool update() override;

    void setDocument(const char *objName) override;
    void setDocumentValue(JsonDocument *docBase) override;
    JsonDocument getDocument() override;
    JsonVariant getVariant(const char *searchName) override;

    float getValueSoilPHSens() const;
    void setPins(uint8_t _pin);
};


SoilPHSens::SoilPHSens(uint8_t _pin)
        : sensorPin(_pin),
          sensorTimer(0) {
}

SoilPHSens::~SoilPHSens() = default;

bool SoilPHSens::init() {
    pinMode(sensorPin, INPUT);
    if (strcmp(name, "") == 0 && doc == nullptr) {
        name = "SoilPHSens";
        doc = new JsonDocument;
    }
    (*doc)[name]["raw"] = 0;
    (*doc)[name]["val"] = 0;
    (*doc)[name]["volt"] = 0.0;
    return true;
}

bool SoilPHSens::update() {
    if (millis() - sensorTimer >= 500) {
        float value = analogRead(sensorPin);
        double soilPh = (-0.0693 * value) + 7.3855; // y = -0.0693x + 7.3855
        soilPh = (soilPh < 0) ? 0 : (soilPh > 14) ? 14 : soilPh;
        (*doc)[name]["raw"] = value;
        (*doc)[name]["val"] = soilPh;
        (*doc)[name]["volt"] = (value * 5.0) / 1023.0;
        sensorTimer = millis();
        return true;
    }
    return false;
}

void SoilPHSens::setDocument(const char *objName) {
    name = objName;
}

void SoilPHSens::setDocumentValue(JsonDocument *docBase) {
    doc = docBase;
}

JsonDocument SoilPHSens::getDocument() {
    return (*doc);
}

JsonVariant SoilPHSens::getVariant(const char *searchName) {
    return (*doc)[searchName];
}

float SoilPHSens::getValueSoilPHSens() const {
    return (*doc)[name].as<float>();
}

void SoilPHSens::setPins(uint8_t _pin) {
    sensorPin = _pin;
}

#endif  // SOILPH_SENS_H
#endif
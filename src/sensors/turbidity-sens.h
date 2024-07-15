/*
 *  turbidity-sens.h
 *
 *  turbidity sensor lib
 *  Created on: 2023. 4. 3
 */
#ifdef USE_TURBIDITY_SENS
#pragma once

#ifndef TURBIDITY_SENS_H
#define TURBIDITY_SENS_H

#include "Arduino.h"
#include "base/sensor-module.h"

class TurbiditySens : public BaseSens {
private:
    JsonDocument *doc;
    const char *name;

    uint8_t sensorPin;
    uint32_t sensorTimer;

public:
    TurbiditySens(int _sensorPin);
    virtual ~TurbiditySens();
    bool init() override;
    bool update() override;

    void setDocument(const char *objName) override;
    void setDocumentValue(JsonDocument *docBase) override;
    JsonDocument getDocument() override;
    JsonVariant getVariant(const char *searchName) override;

    float roundToDp(float input, int decimalPlace);
    float getValueTurbiditySens() const;
    void setPins(uint8_t _pin);
};


#define SAMPLE 800

TurbiditySens::TurbiditySens(int _sensorPin)
        : doc(nullptr),
          name(""),
          sensorPin(_sensorPin) {}

TurbiditySens::~TurbiditySens() = default;

bool TurbiditySens::init() {
    pinMode(sensorPin, INPUT);
    if (strcmp(name, "") == 0 && doc == nullptr) {
        name = "TurbiditySens";
        doc = new JsonDocument;
    }
    (*doc)[name]["volt"] = 0;
    (*doc)[name]["ntu"] = 0;
}

bool TurbiditySens::update() {
    if (millis() - sensorTimer >= 500) {
        float voltage = 0.0;
        double ntu = 0.0;
        for (int i = 0; i < SAMPLE; i++) {
            voltage += ((float) analogRead(sensorPin) / 1023) * 5;
        }
        voltage = voltage / SAMPLE;
        voltage = roundToDp(voltage, 2);
        if (voltage < 2.5) {
            ntu = 3000;
        } else {
            ntu = -1120.4 * sq(voltage) + 5742.3 * voltage - 4353.8;
        }
        (*doc)[name]["volt"] = voltage;
        (*doc)[name]["ntu"] = ntu;
        sensorTimer = millis();
    }
}

void TurbiditySens::setDocument(const char *objName) {
    name = objName;
}

void TurbiditySens::setDocumentValue(JsonDocument *docBase) {
    doc = docBase;
}

JsonDocument TurbiditySens::getDocument() {
    return (*doc);
}

JsonVariant TurbiditySens::getVariant(const char *searchName) {
    return (*doc)[searchName];
}

float TurbiditySens::roundToDp(float input, int decimalPlace) {
    float multiplier = powf(10.0f, decimalPlace);
    input = roundf(input * multiplier) / multiplier;
    return input;
}

float TurbiditySens::getValueTurbiditySens() const {
    return (*doc)[name].as<float>();
}

void TurbiditySens::setPins(uint8_t _pin) {
    sensorPin = _pin;
}


#endif  // TURBIDITY_SENS_H
#endif
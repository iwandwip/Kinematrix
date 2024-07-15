/*
 *  ultrasonic-sens.h
 *
 *  ultrasonic sensor lib
 *  Created on: 2023. 4. 3
 */
#ifdef USE_ULTRASONIC_SENS
#pragma once

#ifndef ULTRASONIC_SENS_H
#define ULTRASONIC_SENS_H

#include "Arduino.h"
#include "base/sensor-module.h"
#include "NewPing.h"

class UltrasonicSens : public BaseSens, public NewPing {
private:
    JsonDocument *doc;
    const char *name;

    uint8_t sensorPin;
    uint32_t sensorTimer;

    using NewPing::NewPing;

public:
    ~UltrasonicSens();
    bool init() override;
    bool update() override;

    void setDocument(const char *objName) override;
    void setDocumentValue(JsonDocument *docBase) override;
    JsonDocument getDocument() override;
    JsonVariant getVariant(const char *searchName) override;

    float getValueCm() const;
    float getValueIn();
    void setPins(uint8_t _pin);
};


UltrasonicSens::~UltrasonicSens() = default;

bool UltrasonicSens::init() {

    if (strcmp(name, "") == 0 && doc == nullptr) {
        name = "UltrasonicSens";
        doc = new JsonDocument;
    }
    (*doc)[name] = 0;
    return true;
}

bool UltrasonicSens::update() {
    if (millis() - sensorTimer >= 50) {
        (*doc)[name] = (int) this->ping_cm();
        sensorTimer = millis();
        return true;
    }
    return false;
}

void UltrasonicSens::setDocument(const char *objName) {
    name = objName;
}

JsonDocument UltrasonicSens::getDocument() {
    return (*doc);
}

void UltrasonicSens::setDocumentValue(JsonDocument *docBase) {
    doc = docBase;
}

JsonVariant UltrasonicSens::getVariant(const char *searchName) {
    return (*doc)[searchName];
}

float UltrasonicSens::getValueCm() const {
    return (*doc)[name].as<float>();
}

float UltrasonicSens::getValueIn() {
    return (float) this->ping_in();
}

void UltrasonicSens::setPins(uint8_t _pin) {
    sensorPin = _pin;
}

#endif  // ULTRASONIC_SENS_H
#endif
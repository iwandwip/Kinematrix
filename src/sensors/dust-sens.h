/*
 *  dust-sens.h
 *
 *  dust sensor lib
 *  Created on: 2023. 4. 3
 */
#ifdef USE_DUST_SENS
#pragma once

#ifndef DUST_SENS_H
#define DUST_SENS_H

#include "Arduino.h"
#include "base/sensor-module.h"

class DustSens : public BaseSens {
private:
    JsonDocument *doc;
    const char *name;

    const uint32_t samplingTime = 280;
    const uint32_t deltaTime = 40;
    const uint32_t sleepTime = 9680;

    float voMeasured = 0;
    float calcVoltage = 0;
    float dustDensity = 0;

    enum State {
        IDLE,
        SAMPLING,
        WAITING,
        SLEEPING
    };

    State currentState;

    uint8_t ledPin;
    uint8_t sensorPin;
    uint32_t sensorTimer;

public:
    DustSens(uint8_t _ledPin, uint8_t _measurePin);
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


DustSens::DustSens(uint8_t _ledPin, uint8_t _measurePin)
        : doc(nullptr),
          name(""),
          ledPin(_ledPin),
          sensorPin(_measurePin) {}

DustSens::~DustSens() = default;

bool DustSens::init() {
    if (strcmp(name, "") == 0 && doc == nullptr) {
        name = "DustSens";
        doc = new JsonDocument;
    }
    pinMode(sensorPin, INPUT);
    pinMode(ledPin, OUTPUT);
    digitalWrite(ledPin, HIGH);
    sensorTimer = micros();
    currentState = IDLE;
    (*doc)[name] = 0;
    return true;
}

bool DustSens::update() {
    switch (currentState) {
        case IDLE:
            digitalWrite(ledPin, LOW); // power on the LED
            sensorTimer = micros();
            currentState = SAMPLING;
            break;
        case SAMPLING:
            if (micros() - sensorTimer >= samplingTime) {
                voMeasured = analogRead(sensorPin); // read the dust value
                sensorTimer = micros();
                currentState = WAITING;
            }
            break;
        case WAITING:
            if (micros() - sensorTimer >= deltaTime) {
                digitalWrite(ledPin, HIGH); // turn the LED off
                sensorTimer = micros();
                currentState = SLEEPING;
            }
            break;
        case SLEEPING:
            if (micros() - sensorTimer >= sleepTime) {
                currentState = IDLE; // go back to IDLE state
            }
            // linear equation taken from http://www.howmuchsnow.com/arduino/airquality/
            calcVoltage = voMeasured * (3.3 / 4095.0);
            dustDensity = 0.17 * calcVoltage - 0.1;
            dustDensity *= 1000;
            dustDensity = dustDensity < 0 ? 0 : dustDensity;
            (*doc)[name] = dustDensity;
            break;
    }
    return true;
}

void DustSens::setDocument(const char *objName) {
    name = objName;
}

void DustSens::setDocumentValue(JsonDocument *docBase) {
    doc = docBase;
}

JsonDocument DustSens::getDocument() {
    return (*doc);
}

JsonVariant DustSens::getVariant(const char *searchName) {
    return (*doc)[searchName];
}

float DustSens::getValueDustSens() const {
    return (*doc)[name].as<float>();
}

void DustSens::setPins(uint8_t _pin) {
    sensorPin = _pin;
}
#endif  // DUST_SENS_H
#endif
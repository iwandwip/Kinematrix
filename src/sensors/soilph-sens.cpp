/*
 *  soilph-sens.cpp
 *
 *  soilph sensor c
 *  Created on: 2023. 4. 3
 */

#include "soilph-sens.h"
#include "Arduino.h"

SoilPHSens::SoilPHSens(uint8_t _pin)
        : sensorPin(_pin),
          sensorTimer(0) {
}

SoilPHSens::~SoilPHSens() = default;

void SoilPHSens::init() {
    pinMode(sensorPin, INPUT);
    doc[name] = 0.0;
}

void SoilPHSens::update() {
    if (millis() - sensorTimer >= 500) {
        float soilPh = (-0.0693 * (float) analogRead(sensorPin)) + 7.3855;
        soilPh = (soilPh < 0) ? 0 : soilPh;
        doc[name] = soilPh;
        sensorTimer = millis();
    }
}

void SoilPHSens::setDocument(const char *objName) {
    name = objName;
}

JsonDocument SoilPHSens::getDocument() {
    return doc;
}

JsonVariant SoilPHSens::getVariant(const char *searchName) {
    return doc[searchName];
}

float SoilPHSens::getValueSoilPHSens() const {
    return doc[name].as<float>();
}

void SoilPHSens::setPins(uint8_t _pin) {
    sensorPin = _pin;
}
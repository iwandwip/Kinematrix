/*
 *  soil-sens.cpp
 *
 *  soil sensor c
 *  Created on: 2023. 4. 3
 */

#include "soil-sens.h"
#include "Arduino.h"

SoilMoistureSens::SoilMoistureSens()
        : sensorClass(nullptr),
          sensorValue(0.0),
          sensorPin(A0),
          sensorTimer(0),
          sensorCallbackFunc(nullptr) {
}

SoilMoistureSens::SoilMoistureSens(uint8_t _pin)
        : sensorClass(nullptr),
          sensorValue(0.0),
          sensorPin(_pin),
          sensorTimer(0),
          sensorCallbackFunc(nullptr) {
}

SoilMoistureSens::~SoilMoistureSens() = default;

void SoilMoistureSens::init() {
    pinMode(sensorPin, INPUT);
}

void SoilMoistureSens::update() {
    if (millis() - sensorTimer >= 500) {
        sensorValue = analogRead(sensorPin);
        sensorValue *= (5.0 / 1023.0);
        sensorTimer = millis();
    }
}

void SoilMoistureSens::getValue(float *output) {
    *output = sensorValue;
}

float SoilMoistureSens::getValueSoilMoistureSens() const {
    return sensorValue;
}

void SoilMoistureSens::setPins(uint8_t _pin) {
    sensorPin = _pin;
}
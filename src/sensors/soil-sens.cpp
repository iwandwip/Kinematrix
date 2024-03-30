/*
 *  soil-sens.cpp
 *
 *  soil sensor c
 *  Created on: 2023. 4. 3
 */

#include "soil-sens.h"
#include "Arduino.h"

SoilMoistureSens::SoilMoistureSens(uint8_t _pin)
        : sensorPin(_pin),
          sensorTimer(0) {
}

SoilMoistureSens::~SoilMoistureSens() = default;

void SoilMoistureSens::init() {
    if (strcmp(name, "") == 0 && doc == nullptr) {
        name = "SoilMoistureSens";
        doc = new JsonDocument;
    }
    pinMode(sensorPin, INPUT);
    (*doc)[name] = 0.0;
}

void SoilMoistureSens::update() {
    if (millis() - sensorTimer >= 500) {
        int value = analogRead(sensorPin);
        (*doc)[name] = (int) map(value, 0, 1023, 0, 100);
        sensorTimer = millis();
    }
}

void SoilMoistureSens::setDocument(const char *objName) {
    name = objName;
}

void SoilMoistureSens::setDocumentValue(JsonDocument *docBase) {
    doc = docBase;
}

JsonDocument SoilMoistureSens::getDocument() {
    return (*doc);
}

JsonVariant SoilMoistureSens::getVariant(const char *searchName) {
    return (*doc)[searchName];
}

float SoilMoistureSens::getValueSoilMoistureSens() const {
    return (*doc)[name].as<float>();
}

void SoilMoistureSens::setPins(uint8_t _pin) {
    sensorPin = _pin;
}
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

bool SoilPHSens::init() {
    pinMode(sensorPin, INPUT);
    if (strcmp(name, "") == 0 && doc == nullptr) {
        name = "SoilPHSens";
        doc = new JsonDocument;
    }
    (*doc)[name] = 0;
}

bool SoilPHSens::update() {
    if (millis() - sensorTimer >= 500) {
        float x = analogRead(sensorPin);
        double soilPh = -0.0693 * x + 7.3855;
        soilPh = (soilPh < 0) ? 0 : soilPh;
        (*doc)[name] = soilPh;
        sensorTimer = millis();
    }
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
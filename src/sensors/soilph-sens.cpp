/*
 *  soilph-sens.cpp
 *
 *  soilph sensor c
 *  Created on: 2023. 4. 3
 */
#ifdef USE_SOILPH_SENS
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
#endif
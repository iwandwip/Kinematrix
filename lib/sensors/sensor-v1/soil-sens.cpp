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

bool SoilMoistureSens::init() {
    if (strcmp(name, "") == 0 && doc == nullptr) {
        name = "SoilMoistureSens";
        doc = new JsonDocument;
    }
    pinMode(sensorPin, INPUT);
    (*doc)[name]["raw"] = 0.0;
    (*doc)[name]["val"] = 0.0;
    (*doc)[name]["volt"] = 0.0;
    return true;
}

bool SoilMoistureSens::update() {
    if (millis() - sensorTimer >= 500) {
        int value = analogRead(sensorPin);
#if defined(ESP32)
        (*doc)[name]["raw"] = value;
        (*doc)[name]["val"] = (100 - ((value / 4095.0) * 100));
        (*doc)[name]["volt"] = (value * 3.3) / 4095.0;
#else
        (*doc)[name]["raw"] = value;
        (*doc)[name]["val"] = (100 - ((value / 1023.0) * 100));
        (*doc)[name]["volt"] = (value * 5.0) / 1023.0;
#endif
        sensorTimer = millis();
        return true;
    }
    return false;
}

long SoilMoistureSens::setOffset(int low, int high) {
    return random(low, high);
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
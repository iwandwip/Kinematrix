/*
 *  max6675-sens.cpp
 *
 *  max6675 sensor c
 *  Created on: 2023. 4. 3
 */

#include "max6675-sens.h"
#include "Arduino.h"

MAX6675Sens::~MAX6675Sens() = default;

bool MAX6675Sens::init() {
    if (strcmp(name, "") == 0 && doc == nullptr) {
        name = "MAX6675Sens";
        doc = new JsonDocument;
    }
    (*doc)[name] = 0;
    return true;
}

bool MAX6675Sens::update() {
    if (millis() - sensorTimer >= 500) {
        (*doc)[name] = MAX6675::readCelsius();
        sensorTimer = millis();
    }
    return true;
}

void MAX6675Sens::setDocument(const char *objName) {
    name = objName;
}

void MAX6675Sens::setDocumentValue(JsonDocument *docBase) {
    doc = docBase;
}

JsonDocument MAX6675Sens::getDocument() {
    return (*doc);
}

JsonVariant MAX6675Sens::getVariant(const char *searchName) {
    return (*doc)[searchName];
}

float MAX6675Sens::getValueMAX6675Sens() const {
    return (*doc)[name].as<float>();
}

void MAX6675Sens::setPins(uint8_t _pin) {
    sensorPin = _pin;
}
/*
 *  tcs3200-sens.cpp
 *
 *  tcs3200 sensor c
 *  Created on: 2023. 4. 3
 */

#include "tcs3200-sens.h"
#include "Arduino.h"

TCS3200Sens::~TCS3200Sens() = default;

bool TCS3200Sens::init() {
    if (strcmp(name, "") == 0 && doc == nullptr) {
        name = "TCS3200Sens";
        doc = new JsonDocument;
    }
    (*doc)[name]["r"] = 0;
    (*doc)[name]["g"] = 0;
    (*doc)[name]["b"] = 0;
    return true;
}

bool TCS3200Sens::update() {
    if (millis() - sensorTimer >= 200) {
        (*doc)[name]["r"] = tcs3200::colorRead('r');
        (*doc)[name]["g"] = tcs3200::colorRead('g');
        (*doc)[name]["b"] = tcs3200::colorRead('b');
        sensorTimer = millis();
        return true;
    }
    return false;
}

void TCS3200Sens::setDocument(const char *objName) {
    name = objName;
}

void TCS3200Sens::setDocumentValue(JsonDocument *docBase) {
    doc = docBase;
}

JsonDocument TCS3200Sens::getDocument() {
    return (*doc);
}

JsonVariant TCS3200Sens::getVariant(const char *searchName) {
    return (*doc)[searchName];
}

float TCS3200Sens::getValueTCS3200Sens() const {
    return (*doc)[name].as<float>();
}

void TCS3200Sens::setPins(uint8_t _pin) {
    sensorPin = _pin;
}
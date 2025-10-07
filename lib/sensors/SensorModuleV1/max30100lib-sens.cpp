/*
 *  max30100lib-sens.cpp
 *
 *  max30100lib sensor c
 *  Created on: 2023. 4. 3
 */

#include "max30100lib-sens.h"
#include "Arduino.h"

MAX30100lib::MAX30100lib(void (*_onBeatCallback)()) {
    PulseOximeter::setOnBeatDetectedCallback(_onBeatCallback);
}

MAX30100lib::~MAX30100lib() = default;

bool MAX30100lib::init() {
    if (strcmp(name, "") == 0 && doc == nullptr) {
        name = "MAX30100lib";
        doc = new JsonDocument;
    }
    if (!PulseOximeter::begin()) return false;
    (*doc)[name]["rate"] = 0;
    (*doc)[name]["spo2"] = 0;
    return true;
}

bool MAX30100lib::update() {
    PulseOximeter::update();
    if (millis() - sensorTimer >= 1000) {
        (*doc)[name]["rate"] = PulseOximeter::getHeartRate();
        (*doc)[name]["spo2"] = PulseOximeter::getSpO2();
        sensorTimer = millis();
        return true;
    }
    return false;
}

void MAX30100lib::setDocument(const char *objName) {
    name = objName;
}

void MAX30100lib::setDocumentValue(JsonDocument *docBase) {
    doc = docBase;
}

JsonDocument MAX30100lib::getDocument() {
    return (*doc);
}

JsonVariant MAX30100lib::getVariant(const char *searchName) {
    return (*doc)[searchName];
}

float MAX30100lib::getValueMAX30100lib() const {
    return (*doc)[name].as<float>();
}

void MAX30100lib::setPins(uint8_t _pin) {
    sensorPin = _pin;
}
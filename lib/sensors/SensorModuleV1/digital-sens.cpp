/*
 *  digital-sens.cpp
 *
 *  digital sensor c
 *  Created on: 2023. 4. 3
 */

#include "digital-sens.h"
#include "Arduino.h"

DigitalSens::~DigitalSens() = default;

bool DigitalSens::init() {
    if (strcmp(name, "") == 0 && doc == nullptr) {
        name = "DigitalSens";
        doc = new JsonDocument;
    }
    (*doc)[name]["raw"] = 0;
    (*doc)[name]["press"] = 0;
    return true;
}

bool DigitalSens::update() {
    (*doc)[name]["raw"] = DigitalIn::getStateRaw();
    (*doc)[name]["press"] = (int) DigitalIn::isPressed();
    DigitalIn::update();
    return true;
}

void DigitalSens::setDocument(const char *objName) {
    name = objName;
}

void DigitalSens::setDocumentValue(JsonDocument *docBase) {
    doc = docBase;
}

JsonDocument DigitalSens::getDocument() {
    return (*doc);
}

JsonVariant DigitalSens::getVariant(const char *searchName) {
    return (*doc)[searchName];
}

float DigitalSens::getValueDigitalSens() const {
    return (*doc)[name].as<float>();
}

void DigitalSens::setPins(uint8_t _pin) {
    sensorPin = _pin;
}
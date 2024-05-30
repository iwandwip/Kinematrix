/*
 *  digital-sens.cpp
 *
 *  digital sensor c
 *  Created on: 2023. 4. 3
 */

#include "digital-sens.h"
#include "Arduino.h"

DigitalSens::DigitalSens(uint8_t _inputPin, uint8_t _mode)
        : doc(nullptr),
          name(""),
          mode(_mode),
          sensorPin(_inputPin) {}

DigitalSens::~DigitalSens() = default;

bool DigitalSens::init() {
    if (strcmp(name, "") == 0 && doc == nullptr) {
        name = "DigitalSens";
        doc = new JsonDocument;
    }
    pinMode(sensorPin, mode);
    (*doc)[name] = 0;
    return true;
}

bool DigitalSens::update() {
    if (millis() - sensorTimer >= 50) {
        int value = digitalRead(sensorPin);
        (*doc)[name] = value;
        sensorTimer = millis();
    }
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
/*
 *  abstract-sens.cpp
 *
 *  abstract sensor c
 *  Created on: 2023. 4. 3
 */

#include "abstract-sens.h"
#include "Arduino.h"

Abstract::Abstract(bool random_value)
        : doc(nullptr),
          name(""),
          randomValue(random_value),
          sensorPin(0),
          sensorTimer(0) {
}

Abstract::~Abstract() = default;

void Abstract::init() {
    if (strcmp(name, "") == 0 && doc == nullptr) {
        name = "Abstract";
        doc = new JsonDocument;
    }
    (*doc)[name] = 0;
}

void Abstract::update() {
    if (millis() - sensorTimer >= 500) {
        if (randomValue) {
            (*doc)[name] = float(random(1 * 10, 1000 * 10)) * 0.1;
        }
        sensorTimer = millis();
    }
}

void Abstract::setDocument(const char *objName) {
    name = objName;
}

void Abstract::setDocumentValue(JsonDocument *docBase) {
    doc = docBase;
}

JsonDocument Abstract::getDocument() {
    return (*doc);
}

JsonVariant Abstract::getVariant(const char *searchName) {
    return (*doc)[searchName];
}

float Abstract::getValueAbstract() const {
    return (*doc)[name].as<float>();
}

void Abstract::setPins(uint8_t _pin) {
    sensorPin = _pin;
}
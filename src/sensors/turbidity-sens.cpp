/*
 *  turbidity-sens.cpp
 *
 *  turbidity sensor c
 *  Created on: 2023. 4. 3
 */

#include "turbidity-sens.h"
#include "Arduino.h"

TurbiditySens::TurbiditySens(int _sensorPin)
        : doc(nullptr),
          name(""),
          sensorPin(_sensorPin) {}

TurbiditySens::~TurbiditySens() = default;

void TurbiditySens::init() {
    pinMode(sensorPin, INPUT);
    if (strcmp(name, "") == 0 && doc == nullptr) {
        name = "TurbiditySens";
        doc = new JsonDocument;
    }
    (*doc)[name] = 0;
}

void TurbiditySens::update() {
    if (millis() - sensorTimer >= 500) {
        (*doc)[name] = analogRead(sensorPin);
        sensorTimer = millis();
    }
}

void TurbiditySens::setDocument(const char *objName) {
    name = objName;
}

void TurbiditySens::setDocumentValue(JsonDocument *docBase) {
    doc = docBase;
}

JsonDocument TurbiditySens::getDocument() {
    return (*doc);
}

JsonVariant TurbiditySens::getVariant(const char *searchName) {
    return (*doc)[searchName];
}

float TurbiditySens::getValueTurbiditySens() const {
    return (*doc)[name].as<float>();
}

void TurbiditySens::setPins(uint8_t _pin) {
    sensorPin = _pin;
}
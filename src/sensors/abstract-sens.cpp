/*
 *  abstract-sens.cpp
 *
 *  abstract sensor c
 *  Created on: 2023. 4. 3
 */

#include "abstract-sens.h"
#include "Arduino.h"

Abstract::Abstract(int _enumRandomValue)
        : doc(nullptr),
          name(""),
          enumRandomValue(_enumRandomValue) {}

Abstract::Abstract(int _enumRandomValue, float _dummyValue1, float _dummyValue2)
        : doc(nullptr),
          name(""),
          enumRandomValue(_enumRandomValue),
          dummyValue1(_dummyValue1),
          dummyValue2(_dummyValue2) {}

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
        switch (enumRandomValue) {
            case 0:
                (*doc)[name] = float(random(1 * 10, 1000 * 10)) * 0.1;
                break;
            case 1:
                (*doc)[name] = dummyValue1;
                break;
            case 2:
                (*doc)[name] = float(random(0 * 10, (long) dummyValue1 * 10)) * 0.1;
                break;
            case 3:
                (*doc)[name] = float(random((long) dummyValue1 * 10, (long) dummyValue2 * 10)) * 0.1;
                break;
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
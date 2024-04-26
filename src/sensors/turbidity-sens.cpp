/*
 *  turbidity-sens.cpp
 *
 *  turbidity sensor c
 *  Created on: 2023. 4. 3
 */

#include "turbidity-sens.h"
#include "Arduino.h"

#define SAMPLE 800

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
    (*doc)[name]["volt"] = 0;
    (*doc)[name]["ntu"] = 0;
}

void TurbiditySens::update() {
    if (millis() - sensorTimer >= 500) {
        float voltage = 0.0;
        double ntu = 0.0;
        for (int i = 0; i < SAMPLE; i++) {
            voltage += ((float) analogRead(sensorPin) / 1023) * 5;
        }
        voltage = voltage / SAMPLE;
        voltage = roundToDp(voltage, 2);
        if (voltage < 2.5) {
            ntu = 3000;
        } else {
            ntu = -1120.4 * square(voltage) + 5742.3 * voltage - 4353.8;
        }
        (*doc)[name]["volt"] = voltage;
        (*doc)[name]["ntu"] = ntu;
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

float TurbiditySens::roundToDp(float input, int decimalPlace) {
    float multiplier = powf(10.0f, decimalPlace);
    input = roundf(input * multiplier) / multiplier;
    return input;
}

float TurbiditySens::getValueTurbiditySens() const {
    return (*doc)[name].as<float>();
}

void TurbiditySens::setPins(uint8_t _pin) {
    sensorPin = _pin;
}
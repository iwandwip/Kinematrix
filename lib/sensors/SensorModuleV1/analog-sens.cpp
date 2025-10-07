/*
 *  analog-sens.cpp
 *
 *  analog sensor c
 *  Created on: 2023. 4. 3
 */

#include "analog-sens.h"
#include "Arduino.h"

AnalogSens::AnalogSens(uint8_t _pin, float _vref, int _adcRange,
                       void (*_onCustomData)(JsonVariant value, int analogValue, float voltage))
        : doc(nullptr),
          name(""),
          voltageReference(_vref),
          adcRange(_adcRange),
          sensorPin(_pin),
          onCustomData(_onCustomData) {
}

AnalogSens::~AnalogSens() = default;

bool AnalogSens::init() {
    if (strcmp(name, "") == 0 && doc == nullptr) {
        name = "AnalogSens";
        doc = new JsonDocument;
    }
    pinMode(sensorPin, INPUT);
    (*doc)[name]["raw"] = 0;
    (*doc)[name]["volt"] = 0;
    return true;
}

bool AnalogSens::update() {
    int analogValue = analogRead(sensorPin);
    float voltageValue = analogValue * (voltageReference / adcRange);
    (*doc)[name]["raw"] = analogValue;
    (*doc)[name]["volt"] = voltageValue;
    if (onCustomData != nullptr) {
        onCustomData((*doc)[name], analogValue, voltageValue);
    }
    return true;
}

void AnalogSens::setDocument(const char *objName) {
    name = objName;
}

void AnalogSens::setDocumentValue(JsonDocument *docBase) {
    doc = docBase;
}

JsonDocument AnalogSens::getDocument() {
    return (*doc);
}

JsonVariant AnalogSens::getVariant(const char *searchName) {
    return (*doc)[searchName];
}

float AnalogSens::getValueAnalogSens() const {
    return (*doc)[name].as<float>();
}

void AnalogSens::setPins(uint8_t _pin) {
    sensorPin = _pin;
}
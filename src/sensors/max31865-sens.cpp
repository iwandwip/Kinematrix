/*
 *  abstract-sens.cpp
 *
 *  abstract sensor c
 *  Created on: 2023. 4. 3
 */

#include "max31865-sens.h"
#include "Arduino.h"

MAX31865Sens::MAX31865Sens(int8_t spiCs, int8_t spiMosi, int8_t spiMiso, int8_t spiClk, max31865_numwires wire,
                           float _rtdNominal, float _refResistor)
        : Adafruit_MAX31865(spiCs, spiMosi, spiMiso, spiClk),
          sensorRtdNominal(_rtdNominal),
          sensorRefResistor(_refResistor),
          sensorTimer(0) {
    Adafruit_MAX31865::begin(wire);
}

MAX31865Sens::~MAX31865Sens() = default;

bool MAX31865Sens::init() {
    if (strcmp(name, "") == 0 && doc == nullptr) {
        name = "MAX31865Sens";
        doc = new JsonDocument;
    }
    (*doc)[name] = 0;
    return true;
}

bool MAX31865Sens::update() {
    if (millis() - sensorTimer >= 500) {
        (*doc)[name] = Adafruit_MAX31865::temperature(sensorRtdNominal, sensorRefResistor);;
        sensorTimer = millis();
        return true;
    }
    return false;
}

void MAX31865Sens::setDocument(const char *objName) {
    name = objName;
}

void MAX31865Sens::setDocumentValue(JsonDocument *docBase) {
    doc = docBase;
}

JsonDocument MAX31865Sens::getDocument() {
    return (*doc);
}

JsonVariant MAX31865Sens::getVariant(const char *searchName) {
    return (*doc)[searchName];
}

float MAX31865Sens::getValueMAX31865Sens() const {
    return (*doc)[name].as<float>();
}

void MAX31865Sens::setPins(uint8_t _pin) {
    sensorPin = _pin;
}
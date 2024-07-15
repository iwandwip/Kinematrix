/*
 *  max31865-sens.h
 *
 *  max31865 sensor lib
 *  Created on: 2023. 4. 3
 */

#ifdef USE_MAX31865_SENS
#pragma once

#ifndef MAX31865_SENS_H
#define MAX31865_SENS_H

#include "Arduino.h"
#include "base/sensor-module.h"
#include "Adafruit_MAX31865.h"

#define MAX_31865_PT100_RREF        430.0
#define MAX_31865_PT1000_RREF       4300.0
#define MAX_31865_PT100_RNOMINAL    100.0
#define MAX_31865_PT1000_RNOMINAL   1000.0

class MAX31865Sens : public BaseSens, public Adafruit_MAX31865 {
private:
    JsonDocument *doc;
    const char *name;

    float sensorRtdNominal;
    float sensorRefResistor;

    uint8_t sensorPin;
    uint32_t sensorTimer;
    using Adafruit_MAX31865::Adafruit_MAX31865;

public:
    explicit MAX31865Sens(int8_t spiCs = 10, int8_t spiMosi = 11, int8_t spiMiso = 12, int8_t spiClk = 13,
                          max31865_numwires wire = MAX31865_2WIRE, float _rtdNominal = MAX_31865_PT100_RNOMINAL,
                          float _refResistor = MAX_31865_PT100_RREF);
    virtual ~MAX31865Sens();
    bool init() override;
    bool update() override;

    void setDocument(const char *objName) override;
    void setDocumentValue(JsonDocument *docBase) override;
    JsonDocument getDocument() override;
    JsonVariant getVariant(const char *searchName) override;

    float getValueMAX31865Sens() const;
    void setPins(uint8_t _pin);
};


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

#endif  // MAX31865_SENS_H
#endif
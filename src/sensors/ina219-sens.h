/*
 *  ina219-sens.h
 *
 *  ina219 sensor lib
 *  Created on: 2023. 4. 3
 */
#ifdef USE_INA219_SENS
#pragma once

#ifndef INA219_SENS_H
#define INA219_SENS_H

#include "Arduino.h"
#include "base/sensor-module.h"
#include "Adafruit_INA219.h"

class INA219Sens : public BaseSens, public Adafruit_INA219 {
private:
    JsonDocument *doc;
    const char *name;

    uint8_t sensorPin;
    uint32_t sensorTimer;

    using Adafruit_INA219::Adafruit_INA219;
public:
    virtual ~INA219Sens();
    bool init() override;
    bool update() override;

    void setDocument(const char *objName) override;
    void setDocumentValue(JsonDocument *docBase) override;
    JsonDocument getDocument() override;
    JsonVariant getVariant(const char *searchName) override;

    float getValueINA219Sens() const;
    void setPins(uint8_t _pin);
};



INA219Sens::~INA219Sens() = default;

bool INA219Sens::init() {
    if (strcmp(name, "") == 0 && doc == nullptr) {
        name = "INA219Sens";
        doc = new JsonDocument;
    }
    if (!Adafruit_INA219::begin()) return false;
//    (*doc)[name]["shuntmV"] = 0;
    (*doc)[name]["V"] = 0;
    (*doc)[name]["I"] = 0;
    (*doc)[name]["P"] = 0;
    return true;
}

bool INA219Sens::update() {
    if (millis() - sensorTimer >= 500) {
//        (*doc)[name]["shuntmV"] = Adafruit_INA219::getShuntVoltage_mV();
        (*doc)[name]["V"] = Adafruit_INA219::getBusVoltage_V();
        (*doc)[name]["I"] = Adafruit_INA219::getCurrent_mA() / 1000;
        (*doc)[name]["P"] = Adafruit_INA219::getPower_mW() / 1000;
        sensorTimer = millis();
        return true;
    }
    return false;
}

void INA219Sens::setDocument(const char *objName) {
    name = objName;
}

void INA219Sens::setDocumentValue(JsonDocument *docBase) {
    doc = docBase;
}

JsonDocument INA219Sens::getDocument() {
    return (*doc);
}

JsonVariant INA219Sens::getVariant(const char *searchName) {
    return (*doc)[searchName];
}

float INA219Sens::getValueINA219Sens() const {
    return (*doc)[name].as<float>();
}

void INA219Sens::setPins(uint8_t _pin) {
    sensorPin = _pin;
}

#endif  // INA219_SENS_H
#endif
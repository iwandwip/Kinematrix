/*
 *  ina219-sens.cpp
 *
 *  ina219 sensor c
 *  Created on: 2023. 4. 3
 */

#include "ina219-sens.h"
#include "Arduino.h"

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
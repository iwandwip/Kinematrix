/*
 *  ds18b20-sens.cpp
 *
 *  ds18b20 sensor c
 *  Created on: 2023. 4. 3
 */

#include "ds18b20-sens.h"
#include "Arduino.h"

DS18B20Sens::DS18B20Sens(uint8_t _pin)
        : DallasTemperature(new OneWire(_pin)),
          sensorPin(_pin),
          sensorTimer(0) {
}

DS18B20Sens::DS18B20Sens(OneWire *_wire)
        : DallasTemperature(_wire),
          sensorPin(2),
          sensorTimer(0) {
}

DS18B20Sens::~DS18B20Sens() = default;

void DS18B20Sens::init() {
    DallasTemperature::begin();
    doc[name] = 0;
}

void DS18B20Sens::update() {
    if (millis() - sensorTimer >= 3000) {
        DallasTemperature::requestTemperatures();
        doc[name] = DallasTemperature::getTempCByIndex(0);
        sensorTimer = millis();
    }
}

void DS18B20Sens::setDocument(const char *objName) {
    name = objName;
}

JsonDocument DS18B20Sens::getDocument() {
    return doc;
}

JsonVariant DS18B20Sens::getVariant(const char *searchName) {
    return doc[searchName];
}

float DS18B20Sens::getValueTemperature() const {
    return doc[name].as<float>();
}

void DS18B20Sens::setPins(uint8_t _pin) {
    sensorPin = _pin;
}
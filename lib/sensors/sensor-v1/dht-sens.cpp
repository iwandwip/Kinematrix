/*
 *  dht-sens.cpp
 *
 *  dht sensor c
 *  Created on: 2023. 4. 3
 */

#include "dht-sens.h"
#include "Arduino.h"

DHTSens::~DHTSens() = default;

bool DHTSens::init() {
    if (strcmp(name, "") == 0 && doc == nullptr) {
        name = "DHTSens";
        doc = new JsonDocument;
    }
    DHT::begin();
    (*doc)[name]["temp"] = 0;
    (*doc)[name]["hum"] = 0;
    return true;
}

bool DHTSens::update() {
    if (millis() - sensorTimer >= 500) {
        float temp = DHT::readTemperature();
        float hum = DHT::readHumidity();
        (*doc)[name]["temp"] = isnan(temp) ? 0.0f : DHT::readTemperature();
        (*doc)[name]["hum"] = isnan(hum) ? 0.0f : DHT::readHumidity();
        return true;
    }
    return false;
}

void DHTSens::setDocument(const char *objName) {
    name = objName;
}

void DHTSens::setDocumentValue(JsonDocument *docBase) {
    doc = docBase;
}

JsonDocument DHTSens::getDocument() {
    return (*doc);
}

JsonVariant DHTSens::getVariant(const char *searchName) {
    return (*doc)[searchName];
}

float DHTSens::getValueDHTSens() const {
    return (*doc)[name].as<float>();
}

void DHTSens::setPins(uint8_t _pin) {
    sensorPin = _pin;
}
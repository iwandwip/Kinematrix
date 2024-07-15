/*
 *  dht-sens.h
 *
 *  dht sensor lib
 *  Created on: 2023. 4. 3
 */
#ifdef USE_DHT_SENS
#pragma once

#ifndef DHT_SENS_H
#define DHT_SENS_H

#include "Arduino.h"
#include "base/sensor-module.h"
#include "Adafruit_Sensor.h"
#include "DHT.h"

class DHTSens : public BaseSens, public DHT {
private:
    JsonDocument *doc;
    const char *name;

    uint8_t sensorPin;
    uint32_t sensorTimer;

    using DHT::DHT;

public:
    virtual ~DHTSens();
    bool init() override;
    bool update() override;

    void setDocument(const char *objName) override;
    void setDocumentValue(JsonDocument *docBase) override;
    JsonDocument getDocument() override;
    JsonVariant getVariant(const char *searchName) override;

    float getValueDHTSens() const;
    void setPins(uint8_t _pin);
};


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
        (*doc)[name]["temp"] = DHT::readTemperature();
        (*doc)[name]["hum"] = DHT::readHumidity();
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
#endif  // DHT_SENS_H
#endif
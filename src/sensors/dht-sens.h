/*
 *  dht-sens.h
 *
 *  dht sensor lib
 *  Created on: 2023. 4. 3
 */

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

#endif  // DHT_SENS_H
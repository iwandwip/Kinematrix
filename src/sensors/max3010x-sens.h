/*
 *  max3010x-sens.h
 *
 *  max3010x sensor lib
 *  Created on: 2023. 4. 3
 */
#ifdef USE_MAX3010X_SENS
#pragma once

#ifndef MAX_3010X_SENS_H
#define MAX_3010X_SENS_H

#include "Arduino.h"
#include "base/sensor-module.h"
#include "MAX30105.h"
#include "heartRate.h"

const byte RATE_SIZE = 4; // Increase this for more averaging. 4 is good.

class MAX3010xSens : public BaseSens, public MAX30105 {
private:
    JsonDocument *doc;
    const char *name;

    byte rates[RATE_SIZE]; // Array of heart rates
    byte rateSpot = 0;
    long lastBeat = 0; // Time at which the last
    float beatsPerMinute = 0;
    int beatAvg = 0;

    uint8_t sensorPin;
    uint32_t sensorTimer;

public:
    virtual ~MAX3010xSens();
    bool init() override;
    bool update() override;

    void setDocument(const char *objName) override;
    void setDocumentValue(JsonDocument *docBase) override;
    JsonDocument getDocument() override;
    JsonVariant getVariant(const char *searchName) override;

    float getValueMAX3010xSens() const;
    void setPins(uint8_t _pin);
};


MAX3010xSens::~MAX3010xSens() = default;

bool MAX3010xSens::init() {
    if (strcmp(name, "") == 0 && doc == nullptr) {
        name = "MAX3010xSens";
        doc = new JsonDocument;
    }
    if (!MAX30105::begin(Wire, I2C_SPEED_FAST)) return false;
    MAX30105::setup(); // Configure sensor with default settings
    MAX30105::setPulseAmplitudeRed(0x0A); // Turn Red LED to low to indicate sensor is running
    MAX30105::setPulseAmplitudeGreen(0); // Turn off Green LED
    (*doc)[name]["ir"] = 0;
    (*doc)[name]["bpm"] = 0;
    (*doc)[name]["avgBpm"] = 0;
    return true;
}

bool MAX3010xSens::update() {
    long irValue = MAX30105::getIR();
    if (checkForBeat(irValue)) {
        // We sensed a beat!
        long delta = millis() - lastBeat;
        lastBeat = millis();
        beatsPerMinute = 60 / (delta / 1000.0);
        if (beatsPerMinute < 255 && beatsPerMinute > 20) {
            rates[rateSpot++] = (byte) beatsPerMinute; // Store this reading in the array
            rateSpot %= RATE_SIZE; // Wrap variable
            // Take average of readings
            beatAvg = 0;
            for (byte x = 0; x < RATE_SIZE; x++)
                beatAvg += rates[x];
            beatAvg /= RATE_SIZE;
        }
    }

    if (irValue < 50000) {
        (*doc)[name]["bpm"] = 0;
//        (*doc)[name]["avgBpm"] = 0;
        return false;
    } // No finger
    else {
        (*doc)[name]["ir"] = irValue;
        (*doc)[name]["bpm"] = beatsPerMinute;
        (*doc)[name]["avgBpm"] = beatAvg;
    }

    return true;
}

void MAX3010xSens::setDocument(const char *objName) {
    name = objName;
}

void MAX3010xSens::setDocumentValue(JsonDocument *docBase) {
    doc = docBase;
}

JsonDocument MAX3010xSens::getDocument() {
    return (*doc);
}

JsonVariant MAX3010xSens::getVariant(const char *searchName) {
    return (*doc)[searchName];
}

float MAX3010xSens::getValueMAX3010xSens() const {
    return (*doc)[name].as<float>();
}

void MAX3010xSens::setPins(uint8_t _pin) {
    sensorPin = _pin;
}

#endif  // MAX_3010X_SENS_H
#endif
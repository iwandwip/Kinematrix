/*
 *  voltage-sens.h
 *
 *  voltage sensor lib
 *  Created on: 2023. 4. 3
 */
#ifdef USE_VOLTAGE_SENS
#pragma once

#ifndef VOLTAGE_SENS_H
#define VOLTAGE_SENS_H

#include "Arduino.h"
#include "base/sensor-module.h"

class VoltageSens : public BaseSens {
private:
    /*variables*/
    float sensorValue;
    uint8_t sensorPin;
    uint32_t sensorTimer;

    float sensorRes1;
    float sensorRes2;
    float sensorRefVoltage;
    float sensorResolution;

public:
    VoltageSens();
    explicit VoltageSens(uint8_t _pin, float _res1 = 30000.0, float _res2 = 7500.0, float _ref_voltage = 5.0, float _resolution = 1024.0);
    ~VoltageSens();
    bool init() override;
    bool update() override;
    void getValue(float *output) override;
    float getValueVoltage() const;
    void setPins(uint8_t _pin);
};


VoltageSens::VoltageSens()
        : sensorValue(0.0),
          sensorPin(A0),
          sensorTimer(0),
          sensorRes1(30000.0),
          sensorRes2(7500.0),
          sensorRefVoltage(5.0),
          sensorResolution(1024.0) {
}

VoltageSens::VoltageSens(uint8_t _pin, float _res1, float _res2, float _ref_voltage, float _resolution)
        : sensorValue(0.0),
          sensorPin(_pin),
          sensorTimer(0),
          sensorRes1(_res1),
          sensorRes2(_res2),
          sensorRefVoltage(_ref_voltage),
          sensorResolution(_resolution) {
}

VoltageSens::~VoltageSens() = default;

bool VoltageSens::init() {
    pinMode(sensorPin, INPUT);
}

bool VoltageSens::update() {
    if (millis() - sensorTimer >= 500) {
        sensorValue = analogRead(sensorPin);
        sensorValue = (sensorValue * sensorRefVoltage) / sensorResolution;
        sensorValue = sensorValue / (sensorRes2 / (sensorRes1 + sensorRes2));
        sensorTimer = millis();
    }
}

void VoltageSens::getValue(float *output) {
    *output = sensorValue;
}

float VoltageSens::getValueVoltage() const {
    return sensorValue;
}

void VoltageSens::setPins(uint8_t _pin) {
    sensorPin = _pin;
}

#endif  // VOLTAGE_SENS_H
#endif
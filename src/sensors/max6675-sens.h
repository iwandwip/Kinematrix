/*
 *  max6675-sens.h
 *
 *  max6675 sensor lib
 *  Created on: 2023. 4. 3
 */

#ifdef USE_MAX6675_SENS
#pragma once

#ifndef MAX6675_SENS_H
#define MAX6675_SENS_H

#include "Arduino.h"
#include "base/sensor-module.h"
#include "max6675.h"

class MAX6675Sens : public BaseSens, public MAX6675 {
private:
    /*variables*/
    float sensorValue;
    uint8_t sensorPin;
    uint32_t sensorTimer;

    using MAX6675::MAX6675;

public:
    ~MAX6675Sens();
    bool init() override;
    bool update() override;
    void getValue(float *output) override;
    float getValueTemperature() const;
    void setPins(uint8_t _pin);
};


MAX6675Sens::~MAX6675Sens() = default;

bool MAX6675Sens::init() {

}

bool MAX6675Sens::update() {
    if (millis() - sensorTimer >= 500) {
        sensorValue = (float) this->readCelsius();
    }
}

void MAX6675Sens::getValue(float *output) {
    *output = sensorValue;
}

float MAX6675Sens::getValueTemperature() const {
    return sensorValue;
}

void MAX6675Sens::setPins(uint8_t _pin) {
    sensorPin = _pin;
}


#endif  // MAX6675_SENS_H
#endif
/*
 *  max31865-sens.h
 *
 *  max31865 sensor lib
 *  Created on: 2023. 4. 3
 */

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
    float sensorValue;
    float sensorRtdNominal;
    float sensorRefResistor;
    uint32_t sensorTimer;
    using Adafruit_MAX31865::Adafruit_MAX31865;

public:
    explicit MAX31865Sens(int8_t spiCs, int8_t spiMosi, int8_t spiMiso, int8_t spiClk, max31865_numwires wire = MAX31865_2WIRE, float _rtdNominal = MAX_31865_PT100_RNOMINAL, float _refResistor = MAX_31865_PT100_RREF);
    ~MAX31865Sens();
    void init() override;
    void update() override;
    void getValue(float *output) override;
    float getValueTemperature() const;
    void setPins(uint8_t _pin);
};

#endif  // MAX31865_SENS_H
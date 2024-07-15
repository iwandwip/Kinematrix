/*
 *  bme280-sens.h
 *
 *  bme280 sensor lib
 *  Created on: 2023. 4. 3
 */
#ifdef USE_BME280_SENS
#pragma once

#ifndef BME280_SENS_H
#define BME280_SENS_H

#include "Arduino.h"
#include "base/sensor-module.h"
#include "Adafruit_BME280.h"

#define BME_SENS_TEMPERATURE 0
#define BME_SENS_PRESSURE 1
#define BME_SENS_ALTITUDE 2
#define BME_SENS_HUMIDITY 3
#define BME_SENS_VALUE_NUM 4

#define SEALEVELPRESSURE_HPA (1013.25)

class BME280Sens : public BaseSens, public Adafruit_BME280 {
private:
    /*variables*/
    float sensorValue[BME_SENS_VALUE_NUM];
    uint32_t sensorTimer;
    using Adafruit_BME280::Adafruit_BME280;

public:
    explicit BME280Sens();
    explicit BME280Sens(int8_t _cs);
    explicit BME280Sens(int8_t _cs, int8_t _mosi, int8_t _miso, int8_t _sck);
    ~BME280Sens();
    bool init() override;
    bool update() override;
    void getValue(float *output) override;
    float getValueTemperature() const;
    float getValuePressure() const;
    float getValueAltitude() const;
    float getValueHumidity() const;
    void setPins(uint8_t _pin);
};


BME280Sens::BME280Sens()
        : Adafruit_BME280(),
          sensorTimer(0) {
}

BME280Sens::BME280Sens(int8_t _cs)
        : Adafruit_BME280(_cs),
          sensorTimer(0) {
}

BME280Sens::BME280Sens(int8_t _cs, int8_t _mosi, int8_t _miso, int8_t _sck)
        : Adafruit_BME280(_cs, _mosi, _miso, _sck),
          sensorTimer(0) {
}

BME280Sens::~BME280Sens() = default;

bool BME280Sens::init() {
    if (Adafruit_BME280::begin()) {
        Serial.println("Could not find a valid BME280 sensor, check wiring, address, sensor ID!");
    }
}

bool BME280Sens::update() {
    if (millis() - sensorTimer >= 500) {
        sensorValue[BME_SENS_TEMPERATURE] = Adafruit_BME280::readTemperature();
        sensorValue[BME_SENS_PRESSURE] = Adafruit_BME280::readPressure() / 100.0F;
        sensorValue[BME_SENS_ALTITUDE] = Adafruit_BME280::readAltitude(SEALEVELPRESSURE_HPA);
        sensorValue[BME_SENS_HUMIDITY] = Adafruit_BME280::readHumidity();
        sensorTimer = millis();
    }
}

void BME280Sens::getValue(float *output) {
    output[BME_SENS_TEMPERATURE] = sensorValue[BME_SENS_TEMPERATURE];
    output[BME_SENS_PRESSURE] = sensorValue[BME_SENS_PRESSURE];
    output[BME_SENS_ALTITUDE] = sensorValue[BME_SENS_ALTITUDE];
    output[BME_SENS_HUMIDITY] = sensorValue[BME_SENS_HUMIDITY];
}

float BME280Sens::getValueTemperature() const {
    return sensorValue[BME_SENS_TEMPERATURE];
}

float BME280Sens::getValuePressure() const {
    return sensorValue[BME_SENS_PRESSURE];
}

float BME280Sens::getValueAltitude() const {
    return sensorValue[BME_SENS_ALTITUDE];
}

float BME280Sens::getValueHumidity() const {
    return sensorValue[BME_SENS_HUMIDITY];
}

void BME280Sens::setPins(uint8_t _pin) {

}

#endif  // BME280_SENS_H
#endif
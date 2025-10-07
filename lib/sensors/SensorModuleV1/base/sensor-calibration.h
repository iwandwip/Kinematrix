/*
 *  sensor-calibration.h
 *
 *  sensor calibration
 *  Created on: 2023. 4. 3
 */

#pragma once

#ifndef SENSOR_CALIBRATION_H
#define SENSOR_CALIBRATION_H

#include "Arduino.h"

#if defined(ESP32)
#include "Preferences.h"
#elif defined(ESP8266)
#else

#ifndef ENABLE_MODULE_HELPER_EEPROM_LIB
#define ENABLE_MODULE_HELPER_EEPROM_LIB
#endif

#pragma message("[COMPILED]: sensor-calibration.h")

#include "KinematrixModulesHelper.h"
#include "sensor-header.h"

#endif

typedef enum {
    LAGRANGE_INTERPOLATION,
    LINEAR_INTERPOLATION,
    POLYNOMIAL_INTERPOLATION,
    CUBIC_SPLINE_INTERPOLATION,
} interpolation_cfg;

class AnalogCalibration {
private:
    struct CalibrationData {
        float voltage;
        float calibrationValue;
    };

    CalibrationData *calibrationDataArray;
    int calibrationDataCount;
    const char *calibrationName;

#ifdef ESP32
    Preferences *preferences;
#elif defined(ESP8266)
#else
    EEPROMLib *eepromLib;
    int eepromAddress;
#endif

    int sensorPin;
    float voltageReference;
    int adcRange;

    [[nodiscard]] CalibrationData getCalibrationValueFromUser(int i) const;
    static CalibrationData getCalibrationValueFromUserCustom(int i);

public:
#ifdef ESP32
    AnalogCalibration(const char *calibrationName, Preferences *preferences);
#elif defined(ESP8266)
#else
    AnalogCalibration(const char *calibrationName, EEPROMLib *eepromLib, int initialAddress);
#endif
    ~AnalogCalibration();

    void setAnalogConfig(int _sensorPin, float _voltageReference, int _adcRange);
    void calibrateSensor();
    void calibrateSensorCustom();
    void loadCalibration(bool debug = true);
    float voltageToValue(float voltage, interpolation_cfg cfg = LAGRANGE_INTERPOLATION);

    float lagrangeInterpolation(float voltage);
    float linearInterpolation(float voltage, float tolerance = 0.0);
    float polynomialInterpolation(float voltage);
    void computeCubicSplineCoefficients(float *a, float *b, float *c, float *d);
    float cubicSplineInterpolation(float voltage);
};

#endif  // SENSOR_CALIBRATION_H
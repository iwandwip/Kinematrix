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

#ifdef ESP32
#include "Preferences.h"
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
#else
    EEPROMLib *eepromLib;
    int eepromAddress;
#endif

    int sensorPin;
    float voltageReference;
    int adcRange;

    CalibrationData getCalibrationValueFromUser(int i);
    CalibrationData getCalibrationValueFromUserCustom(int i);
    float lagrangeInterpolation(float voltage);
    float linearInterpolation(float voltage);

public:
#ifdef ESP32
    AnalogCalibration(const char *calibrationName, Preferences *preferences);
#else
    AnalogCalibration(const char *calibrationName, EEPROMLib *eepromLib, int initialAddress);
#endif
    ~AnalogCalibration();

    void setAnalogConfig(int _sensorPin, float _voltageReference, int _adcRange);
    void calibrateSensor();
    void calibrateSensorCustom();
    void loadCalibration();
    float voltageToValue(float voltage, interpolation_cfg cfg = LAGRANGE_INTERPOLATION);
};

#endif  // SENSOR_CALIBRATION_H
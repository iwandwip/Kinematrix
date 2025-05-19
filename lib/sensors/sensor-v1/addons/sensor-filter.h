/*
 *  sensor-filter.h
 *
 *  sensor filter lib
 *  Created on: 2023. 4. 3
 */

#pragma once

#ifndef SENSOR_FILTER_H
#define SENSOR_FILTER_H

#include "Arduino.h"

class KalmanFilter {
private:
    float _err_measure;
    float _err_estimate;
    float _q;
    float _current_estimate = 0;
    float _last_estimate = 0;
    float _kalman_gain = 0;
public:
    explicit KalmanFilter(float mea_e = 2.0, float est_e = 2.0, float q = 0.01);
    float updateEstimate(float mea);
    void setMeasurementError(float mea_e);
    void setEstimateError(float est_e);
    void setProcessNoise(float q);
    float getKalmanGain() const;
    float getEstimateError() const;
};

class MovingAverageFilter {
private:
    int _windowSize;
    int _currentIndex;
    float *_values;
    float _runningSum;
public:
    explicit MovingAverageFilter(int windowSize);
    ~MovingAverageFilter();
    void addMeasurement(float value);
    float getFilteredValue() const;
    void clear();
};

#endif  // SENSOR_FILTER_H

/*
 *  sensor-filter.cpp
 *
 *  sensor filter c
 *  Created on: 2023. 4. 3
 */

#include "Arduino.h"
#include "sensor-filter.h"
#include "math.h"

// SensorKalmanFilter::SensorKalmanFilter

SensorKalmanFilter::SensorKalmanFilter(float mea_e, float est_e, float q) {
    _err_measure = mea_e;
    _err_estimate = est_e;
    _q = q;
}

float SensorKalmanFilter::updateEstimate(float mea) {
    _kalman_gain = _err_estimate / (_err_estimate + _err_measure);
    _current_estimate = _last_estimate + _kalman_gain * (mea - _last_estimate);
    _err_estimate = (1.0 - _kalman_gain) * _err_estimate + fabs(_last_estimate - _current_estimate) * _q;
    _last_estimate = _current_estimate;

    return _current_estimate;
}

void SensorKalmanFilter::setMeasurementError(float mea_e) {
    _err_measure = mea_e;
}

void SensorKalmanFilter::setEstimateError(float est_e) {
    _err_estimate = est_e;
}

void SensorKalmanFilter::setProcessNoise(float q) {
    _q = q;
}

float SensorKalmanFilter::getKalmanGain() const {
    return _kalman_gain;
}

float SensorKalmanFilter::getEstimateError() const {
    return _err_estimate;
}

// SensorMovingAverageFilter::SensorMovingAverageFilter

SensorMovingAverageFilter::SensorMovingAverageFilter(int windowSize) {
    _windowSize = windowSize;
    _currentIndex = 0;
    _values = new float[windowSize];
    _runningSum = 0.0;
    for (int i = 0; i < windowSize; ++i) {
        _values[i] = 0.0;
    }
}

SensorMovingAverageFilter::~SensorMovingAverageFilter() {
    delete[] _values;
}

void SensorMovingAverageFilter::addMeasurement(float value) {
    _runningSum -= _values[_currentIndex];
    _runningSum += value;
    _values[_currentIndex] = value;
    _currentIndex = (_currentIndex + 1) % _windowSize;
}

float SensorMovingAverageFilter::getFilteredValue() const {
    return _runningSum / _windowSize;
}

void SensorMovingAverageFilter::clear() {
    _runningSum = 0.0;
    for (int i = 0; i < _windowSize; ++i) {
        _values[i] = 0.0;
    }
}
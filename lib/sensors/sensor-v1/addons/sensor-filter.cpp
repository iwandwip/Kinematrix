/*
 *  sensor-filter.cpp
 *
 *  sensor filter c
 *  Created on: 2023. 4. 3
 */

#include "Arduino.h"
#include "sensor-filter.h"
#include "math.h"

// KalmanFilter::KalmanFilter

KalmanFilter::KalmanFilter(float mea_e, float est_e, float q) {
    _err_measure = mea_e;
    _err_estimate = est_e;
    _q = q;
}

float KalmanFilter::updateEstimate(float mea) {
    _kalman_gain = _err_estimate / (_err_estimate + _err_measure);
    _current_estimate = _last_estimate + _kalman_gain * (mea - _last_estimate);
    _err_estimate = (1.0 - _kalman_gain) * _err_estimate + fabs(_last_estimate - _current_estimate) * _q;
    _last_estimate = _current_estimate;

    return _current_estimate;
}

void KalmanFilter::setMeasurementError(float mea_e) {
    _err_measure = mea_e;
}

void KalmanFilter::setEstimateError(float est_e) {
    _err_estimate = est_e;
}

void KalmanFilter::setProcessNoise(float q) {
    _q = q;
}

float KalmanFilter::getKalmanGain() const {
    return _kalman_gain;
}

float KalmanFilter::getEstimateError() const {
    return _err_estimate;
}

// MovingAverageFilter::MovingAverageFilter

MovingAverageFilter::MovingAverageFilter(int windowSize) {
    _windowSize = windowSize;
    _currentIndex = 0;
    _values = new float[windowSize];
    _runningSum = 0.0;
    for (int i = 0; i < windowSize; ++i) {
        _values[i] = 0.0;
    }
}

MovingAverageFilter::~MovingAverageFilter() {
    delete[] _values;
}

void MovingAverageFilter::addMeasurement(float value) {
    _runningSum -= _values[_currentIndex];
    _runningSum += value;
    _values[_currentIndex] = value;
    _currentIndex = (_currentIndex + 1) % _windowSize;
}

float MovingAverageFilter::getFilteredValue() const {
    return _runningSum / _windowSize;
}

void MovingAverageFilter::clear() {
    _runningSum = 0.0;
    for (int i = 0; i < _windowSize; ++i) {
        _values[i] = 0.0;
    }
}
#include "KalmanFilter.h"

KalmanFilter::KalmanFilter(float process_noise, float measurement_noise, float initial_error)
    : q(process_noise), r(measurement_noise), p(initial_error), x(0), initialized(false) {}

void KalmanFilter::reset() {
    p = 1.0;
    x = 0;
    initialized = false;
}

float KalmanFilter::filter(float measurement) {
    if (!initialized) {
        x = measurement;
        initialized = true;
        return x;
    }

    p = p + q;
    float k = p / (p + r);
    x = x + k * (measurement - x);
    p = (1 - k) * p;

    return x;
}

void KalmanFilter::setState(float state) {
    x = state;
}

float KalmanFilter::getState() const {
    return x;
}

void KalmanFilter::setNoise(float process_noise, float measurement_noise) {
    q = process_noise;
    r = measurement_noise;
}
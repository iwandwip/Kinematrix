#include "ExponentialMovingAverageFilter.h"

ExponentialMovingAverageFilter::ExponentialMovingAverageFilter(float smoothingFactor) 
    : alpha(smoothingFactor), average(0), initialized(false) {
}

void ExponentialMovingAverageFilter::reset() {
    average = 0;
    initialized = false;
}

void ExponentialMovingAverageFilter::setAlpha(float smoothingFactor) {
    alpha = smoothingFactor;
    if (alpha < 0) alpha = 0;
    if (alpha > 1) alpha = 1;
}

float ExponentialMovingAverageFilter::getAlpha() const {
    return alpha;
}

float ExponentialMovingAverageFilter::filter(float value) {
    if (!initialized) {
        average = value;
        initialized = true;
        return average;
    }
    
    average = alpha * value + (1 - alpha) * average;
    return average;
}

float ExponentialMovingAverageFilter::getAverage() const {
    return average;
}

bool ExponentialMovingAverageFilter::isInitialized() const {
    return initialized;
}
#include "LowPassFilter.h"

LowPassFilter::LowPassFilter(float smoothingFactor) : lastOutput(0), initialized(false) {
    setAlpha(smoothingFactor);
}

void LowPassFilter::reset() {
    lastOutput = 0;
    initialized = false;
}

void LowPassFilter::setAlpha(float smoothingFactor) {
    alpha = smoothingFactor;
    if (alpha < 0.0f) alpha = 0.0f;
    if (alpha > 1.0f) alpha = 1.0f;
}

float LowPassFilter::getAlpha() const {
    return alpha;
}

float LowPassFilter::filter(float value) {
    if (!initialized) {
        lastOutput = value;
        initialized = true;
        return lastOutput;
    }

    float output = alpha * value + (1.0f - alpha) * lastOutput;
    lastOutput = output;
    return output;
}
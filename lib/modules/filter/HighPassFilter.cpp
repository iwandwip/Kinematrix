#include "HighPassFilter.h"

HighPassFilter::HighPassFilter(float smoothingFactor) : lastInput(0), lastOutput(0), initialized(false) {
    setAlpha(smoothingFactor);
}

void HighPassFilter::reset() {
    lastInput = 0;
    lastOutput = 0;
    initialized = false;
}

void HighPassFilter::setAlpha(float smoothingFactor) {
    alpha = smoothingFactor;
    if (alpha < 0.0f) alpha = 0.0f;
    if (alpha > 1.0f) alpha = 1.0f;
}

float HighPassFilter::getAlpha() const {
    return alpha;
}

float HighPassFilter::filter(float value) {
    if (!initialized) {
        lastInput = value;
        lastOutput = 0;
        initialized = true;
        return lastOutput;
    }

    float output = alpha * (lastOutput + value - lastInput);
    lastOutput = output;
    lastInput = value;

    return output;
}
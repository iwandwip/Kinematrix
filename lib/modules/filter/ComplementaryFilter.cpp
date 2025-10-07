#include "ComplementaryFilter.h"

ComplementaryFilter::ComplementaryFilter(float smoothingFactor) : lastAngle(0), initialized(false) {
    setAlpha(smoothingFactor);
}

void ComplementaryFilter::reset() {
    lastAngle = 0;
    initialized = false;
}

void ComplementaryFilter::setAlpha(float smoothingFactor) {
    alpha = smoothingFactor;
    if (alpha < 0.0f) alpha = 0.0f;
    if (alpha > 1.0f) alpha = 1.0f;
}

float ComplementaryFilter::getAlpha() const {
    return alpha;
}

float ComplementaryFilter::filter(float newAngle, float gyroRate, float dt) {
    if (!initialized) {
        lastAngle = newAngle;
        initialized = true;
    }

    // Integrate gyro rate to get angle change
    float gyroAngle = gyroRate * dt;

    // Complementary filter formula
    lastAngle = alpha * (lastAngle + gyroAngle) + (1.0f - alpha) * newAngle;

    return lastAngle;
}

#include "BandStopFilter.h"
#include <math.h>

BandStopFilter::BandStopFilter(float centerFreq, float bandwidth, float sampleRate)
    : x_n1(0), x_n2(0), y_n1(0), y_n2(0), initialized(false) {

    float omega0 = 2.0 * M_PI * centerFreq / sampleRate;
    float alpha = sin(omega0) * sinh(M_PI * bandwidth * centerFreq / sampleRate / sin(omega0));

    float cos_omega0 = cos(omega0);

    float common_denom = 1.0 + alpha;

    b0 = (1.0 + cos_omega0) / common_denom;
    b1 = -2.0 * cos_omega0 / common_denom;
    b2 = (1.0 + cos_omega0) / common_denom;

    a1 = (-2.0 * cos_omega0) / common_denom;
    a2 = (1.0 - alpha) / common_denom;
}

void BandStopFilter::reset() {
    x_n1 = 0;
    x_n2 = 0;
    y_n1 = 0;
    y_n2 = 0;
    initialized = false;
}

float BandStopFilter::filter(float input) {
    if (!initialized) {
        x_n1 = input;
        x_n2 = input;
        y_n1 = input;
        y_n2 = input;
        initialized = true;
        return input;
    }

    float output = b0 * input + b1 * x_n1 + b2 * x_n2 - a1 * y_n1 - a2 * y_n2;

    x_n2 = x_n1;
    x_n1 = input;
    y_n2 = y_n1;
    y_n1 = output;

    return output;
}

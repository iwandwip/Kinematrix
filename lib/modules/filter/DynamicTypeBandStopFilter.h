#ifndef DYNAMIC_TYPE_BAND_STOP_FILTER_H
#define DYNAMIC_TYPE_BAND_STOP_FILTER_H

#include <math.h>

template <typename T>
class DynamicTypeBandStopFilter {
private:
    T a1, a2, b0, b1, b2; // Filter coefficients
    T x_n1, x_n2;         // Previous inputs
    T y_n1, y_n2;         // Previous outputs
    bool initialized;

public:
    DynamicTypeBandStopFilter(T centerFreq, T bandwidth, T sampleRate);
    void reset();
    T filter(T input);
};

template <typename T>
DynamicTypeBandStopFilter<T>::DynamicTypeBandStopFilter(T centerFreq, T bandwidth, T sampleRate)
    : x_n1(0), x_n2(0), y_n1(0), y_n2(0), initialized(false) {

    T omega0 = 2.0 * M_PI * centerFreq / sampleRate;
    T alpha = sin(omega0) * sinh(M_PI * bandwidth * centerFreq / sampleRate / sin(omega0));

    T cos_omega0 = cos(omega0);

    T common_denom = 1.0 + alpha;

    b0 = (1.0 + cos_omega0) / common_denom;
    b1 = -2.0 * cos_omega0 / common_denom;
    b2 = (1.0 + cos_omega0) / common_denom;

    a1 = (-2.0 * cos_omega0) / common_denom;
    a2 = (1.0 - alpha) / common_denom;
}

template <typename T>
void DynamicTypeBandStopFilter<T>::reset() {
    x_n1 = 0;
    x_n2 = 0;
    y_n1 = 0;
    y_n2 = 0;
    initialized = false;
}

template <typename T>
T DynamicTypeBandStopFilter<T>::filter(T input) {
    if (!initialized) {
        x_n1 = input;
        x_n2 = input;
        y_n1 = input;
        y_n2 = input;
        initialized = true;
        return input;
    }

    T output = b0 * input + b1 * x_n1 + b2 * x_n2 - a1 * y_n1 - a2 * y_n2;

    x_n2 = x_n1;
    x_n1 = input;
    y_n2 = y_n1;
    y_n1 = output;

    return output;
}

#endif

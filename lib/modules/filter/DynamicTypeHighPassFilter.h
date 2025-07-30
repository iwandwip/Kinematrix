#ifndef DYNAMIC_TYPE_HIGH_PASS_FILTER_H
#define DYNAMIC_TYPE_HIGH_PASS_FILTER_H

template <typename T>
class DynamicTypeHighPassFilter {
private:
    T alpha;
    T lastInput;
    T lastOutput;
    bool initialized;

public:
    DynamicTypeHighPassFilter(T smoothingFactor = 0.9);
    void reset();
    T filter(T value);
    void setAlpha(T smoothingFactor);
    T getAlpha() const;
};

template <typename T>
DynamicTypeHighPassFilter<T>::DynamicTypeHighPassFilter(T smoothingFactor) 
    : lastInput(0), lastOutput(0), initialized(false) {
    setAlpha(smoothingFactor);
}

template <typename T>
void DynamicTypeHighPassFilter<T>::reset() {
    lastInput = 0;
    lastOutput = 0;
    initialized = false;
}

template <typename T>
void DynamicTypeHighPassFilter<T>::setAlpha(T smoothingFactor) {
    alpha = smoothingFactor;
    if (alpha < 0) alpha = 0;
    if (alpha > 1) alpha = 1;
}

template <typename T>
T DynamicTypeHighPassFilter<T>::getAlpha() const {
    return alpha;
}

template <typename T>
T DynamicTypeHighPassFilter<T>::filter(T value) {
    if (!initialized) {
        lastInput = value;
        lastOutput = 0;
        initialized = true;
        return lastOutput;
    }

    lastOutput = alpha * (lastOutput + value - lastInput);
    lastInput = value;

    return lastOutput;
}

#endif
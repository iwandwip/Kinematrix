#ifndef DYNAMIC_TYPE_EXPONENTIAL_MOVING_AVERAGE_FILTER_H
#define DYNAMIC_TYPE_EXPONENTIAL_MOVING_AVERAGE_FILTER_H

template <typename T>
class DynamicTypeExponentialMovingAverageFilter {
private:
    T average;
    T alpha;
    bool initialized;

public:
    DynamicTypeExponentialMovingAverageFilter(T smoothingFactor = 0.1);
    void reset();
    void setAlpha(T smoothingFactor);
    T getAlpha() const;
    T filter(T value);
    T getAverage() const;
    bool isInitialized() const;
};

template <typename T>
DynamicTypeExponentialMovingAverageFilter<T>::DynamicTypeExponentialMovingAverageFilter(T smoothingFactor) 
    : alpha(smoothingFactor), average(0), initialized(false) {
}

template <typename T>
void DynamicTypeExponentialMovingAverageFilter<T>::reset() {
    average = 0;
    initialized = false;
}

template <typename T>
void DynamicTypeExponentialMovingAverageFilter<T>::setAlpha(T smoothingFactor) {
    alpha = smoothingFactor;
    if (alpha < 0) alpha = 0;
    if (alpha > 1) alpha = 1;
}

template <typename T>
T DynamicTypeExponentialMovingAverageFilter<T>::getAlpha() const {
    return alpha;
}

template <typename T>
T DynamicTypeExponentialMovingAverageFilter<T>::filter(T value) {
    if (!initialized) {
        average = value;
        initialized = true;
        return average;
    }
    
    average = alpha * value + (1 - alpha) * average;
    return average;
}

template <typename T>
T DynamicTypeExponentialMovingAverageFilter<T>::getAverage() const {
    return average;
}

template <typename T>
bool DynamicTypeExponentialMovingAverageFilter<T>::isInitialized() const {
    return initialized;
}

#endif
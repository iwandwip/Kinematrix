#ifndef DYNAMIC_TYPE_COMPLEMENTARY_FILTER_H
#define DYNAMIC_TYPE_COMPLEMENTARY_FILTER_H

template <typename T>
class DynamicTypeComplementaryFilter {
private:
    T alpha;
    T lastAngle;
    bool initialized;

public:
    DynamicTypeComplementaryFilter(T smoothingFactor = 0.98);
    void reset();
    T filter(T newAngle, T gyroRate, T dt);
    void setAlpha(T smoothingFactor);
    T getAlpha() const;
};

template <typename T>
DynamicTypeComplementaryFilter<T>::DynamicTypeComplementaryFilter(T smoothingFactor) : lastAngle(0), initialized(false) {
    setAlpha(smoothingFactor);
}

template <typename T>
void DynamicTypeComplementaryFilter<T>::reset() {
    lastAngle = 0;
    initialized = false;
}

template <typename T>
void DynamicTypeComplementaryFilter<T>::setAlpha(T smoothingFactor) {
    alpha = smoothingFactor;
    if (alpha < 0) alpha = 0;
    if (alpha > 1) alpha = 1;
}

template <typename T>
T DynamicTypeComplementaryFilter<T>::getAlpha() const {
    return alpha;
}

template <typename T>
T DynamicTypeComplementaryFilter<T>::filter(T newAngle, T gyroRate, T dt) {
    if (!initialized) {
        lastAngle = newAngle;
        initialized = true;
    }

    T gyroAngle = gyroRate * dt;
    lastAngle = alpha * (lastAngle + gyroAngle) + (1 - alpha) * newAngle;

    return lastAngle;
}

#endif

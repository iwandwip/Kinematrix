#ifndef DYNAMIC_TYPE_MOVING_AVERAGE_FILTER_H
#define DYNAMIC_TYPE_MOVING_AVERAGE_FILTER_H

template <typename T>
class DynamicTypeMovingAverageFilter {
private:
    T* samples;
    int windowSize;
    int index;
    int count;
    T sum;
    bool initialized;

public:
    DynamicTypeMovingAverageFilter(int size);
    ~DynamicTypeMovingAverageFilter();
    void reset();
    T filter(T value);
    T getAverage() const;
    bool isReady() const;
    int getCount() const;
    int getWindowSize() const;
};

template <typename T>
DynamicTypeMovingAverageFilter<T>::DynamicTypeMovingAverageFilter(int size) 
    : windowSize(size), index(0), count(0), sum(0), initialized(false) {
    samples = new T[windowSize];
    for (int i = 0; i < windowSize; i++) {
        samples[i] = 0;
    }
}

template <typename T>
DynamicTypeMovingAverageFilter<T>::~DynamicTypeMovingAverageFilter() {
    delete[] samples;
}

template <typename T>
void DynamicTypeMovingAverageFilter<T>::reset() {
    index = 0;
    count = 0;
    sum = 0;
    initialized = false;
    for (int i = 0; i < windowSize; i++) {
        samples[i] = 0;
    }
}

template <typename T>
T DynamicTypeMovingAverageFilter<T>::filter(T value) {
    if (!initialized) {
        for (int i = 0; i < windowSize; i++) {
            samples[i] = value;
        }
        sum = value * windowSize;
        count = windowSize;
        initialized = true;
        return value;
    }

    sum -= samples[index];
    samples[index] = value;
    sum += value;
    
    index = (index + 1) % windowSize;
    if (count < windowSize) {
        count++;
    }
    
    return sum / count;
}

template <typename T>
T DynamicTypeMovingAverageFilter<T>::getAverage() const {
    if (count == 0) return 0;
    return sum / count;
}

template <typename T>
bool DynamicTypeMovingAverageFilter<T>::isReady() const {
    return count == windowSize;
}

template <typename T>
int DynamicTypeMovingAverageFilter<T>::getCount() const {
    return count;
}

template <typename T>
int DynamicTypeMovingAverageFilter<T>::getWindowSize() const {
    return windowSize;
}

#endif
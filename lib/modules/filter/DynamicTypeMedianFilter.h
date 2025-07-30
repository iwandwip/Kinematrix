#ifndef DYNAMIC_TYPE_MEDIAN_FILTER_H
#define DYNAMIC_TYPE_MEDIAN_FILTER_H

template <typename T>
class DynamicTypeMedianFilter {
private:
    T* buffer;
    T* sortedBuffer;
    int windowSize;
    int currentIndex;
    bool initialized;

    void sort(T* arr, int size) {
        for (int i = 1; i < size; i++) {
            T key = arr[i];
            int j = i - 1;
            while (j >= 0 && arr[j] > key) {
                arr[j + 1] = arr[j];
                j = j - 1;
            }
            arr[j + 1] = key;
        }
    }

public:
    DynamicTypeMedianFilter(int size = 5);
    ~DynamicTypeMedianFilter();
    void reset();
    T filter(T value);
};

template <typename T>
DynamicTypeMedianFilter<T>::DynamicTypeMedianFilter(int size) 
    : currentIndex(0), initialized(false) {
    windowSize = (size % 2 == 0) ? size + 1 : size;
    buffer = new T[windowSize];
    sortedBuffer = new T[windowSize];
    reset();
}

template <typename T>
DynamicTypeMedianFilter<T>::~DynamicTypeMedianFilter() {
    delete[] buffer;
    delete[] sortedBuffer;
}

template <typename T>
void DynamicTypeMedianFilter<T>::reset() {
    currentIndex = 0;
    initialized = false;
    for (int i = 0; i < windowSize; ++i) {
        buffer[i] = 0;
    }
}

template <typename T>
T DynamicTypeMedianFilter<T>::filter(T value) {
    if (!initialized) {
        for (int i = 0; i < windowSize; ++i) {
            buffer[i] = value;
        }
        initialized = true;
    }

    buffer[currentIndex] = value;
    currentIndex = (currentIndex + 1) % windowSize;

    for (int i = 0; i < windowSize; ++i) {
        sortedBuffer[i] = buffer[i];
    }
    sort(sortedBuffer, windowSize);

    return sortedBuffer[windowSize / 2];
}

#endif
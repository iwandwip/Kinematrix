#include "MedianFilter.h"

void MedianFilter::sort(float* arr, int size) {
    for (int i = 1; i < size; i++) {
        float key = arr[i];
        int j = i - 1;
        while (j >= 0 && arr[j] > key) {
            arr[j + 1] = arr[j];
            j = j - 1;
        }
        arr[j + 1] = key;
    }
}

MedianFilter::MedianFilter(int size) : currentIndex(0), initialized(false) {
    windowSize = (size % 2 == 0) ? size + 1 : size;
    buffer = new float[windowSize];
    sortedBuffer = new float[windowSize];
    reset();
}

MedianFilter::~MedianFilter() {
    delete[] buffer;
    delete[] sortedBuffer;
}

void MedianFilter::reset() {
    currentIndex = 0;
    initialized = false;
    for (int i = 0; i < windowSize; ++i) {
        buffer[i] = 0;
    }
}

float MedianFilter::filter(float value) {
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
#include "MovingAverageFilter.h"

MovingAverageFilter::MovingAverageFilter(int size) : windowSize(size), index(0), count(0), sum(0), initialized(false) {
    samples = new float[windowSize];
    for (int i = 0; i < windowSize; i++) {
        samples[i] = 0;
    }
}

MovingAverageFilter::~MovingAverageFilter() {
    delete[] samples;
}

void MovingAverageFilter::reset() {
    index = 0;
    count = 0;
    sum = 0;
    initialized = false;
    for (int i = 0; i < windowSize; i++) {
        samples[i] = 0;
    }
}

float MovingAverageFilter::filter(float value) {
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

float MovingAverageFilter::getAverage() const {
    if (count == 0) return 0;
    return sum / count;
}

bool MovingAverageFilter::isReady() const {
    return count == windowSize;
}

int MovingAverageFilter::getCount() const {
    return count;
}

int MovingAverageFilter::getWindowSize() const {
    return windowSize;
}
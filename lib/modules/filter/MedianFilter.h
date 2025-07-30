#ifndef MEDIAN_FILTER_H
#define MEDIAN_FILTER_H

class MedianFilter {
private:
    float* buffer;
    float* sortedBuffer;
    int windowSize;
    int currentIndex;
    bool initialized;

    void sort(float* arr, int size);

public:
    MedianFilter(int size = 5);
    ~MedianFilter();
    void reset();
    float filter(float value);
};

#endif
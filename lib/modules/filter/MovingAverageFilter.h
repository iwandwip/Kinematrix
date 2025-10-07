#ifndef MOVING_AVERAGE_FILTER_H
#define MOVING_AVERAGE_FILTER_H

class MovingAverageFilter {
private:
    float* samples;
    int windowSize;
    int index;
    int count;
    float sum;
    bool initialized;

public:
    MovingAverageFilter(int size);
    ~MovingAverageFilter();
    void reset();
    float filter(float value);
    float getAverage() const;
    bool isReady() const;
    int getCount() const;
    int getWindowSize() const;
};

#endif
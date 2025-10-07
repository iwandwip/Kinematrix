#ifndef EXPONENTIAL_MOVING_AVERAGE_FILTER_H
#define EXPONENTIAL_MOVING_AVERAGE_FILTER_H

class ExponentialMovingAverageFilter {
private:
    float average;
    float alpha;
    bool initialized;

public:
    ExponentialMovingAverageFilter(float smoothingFactor = 0.1);
    void reset();
    void setAlpha(float smoothingFactor);
    float getAlpha() const;
    float filter(float value);
    float getAverage() const;
    bool isInitialized() const;
};

#endif
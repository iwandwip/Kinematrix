#ifndef LOW_PASS_FILTER_H
#define LOW_PASS_FILTER_H

class LowPassFilter {
private:
    float alpha;
    float lastOutput;
    bool initialized;

public:
    LowPassFilter(float smoothingFactor = 0.1);
    void reset();
    float filter(float value);
    void setAlpha(float smoothingFactor);
    float getAlpha() const;
};

#endif
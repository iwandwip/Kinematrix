#ifndef HIGH_PASS_FILTER_H
#define HIGH_PASS_FILTER_H

class HighPassFilter {
private:
    float alpha;
    float lastInput;
    float lastOutput;
    bool initialized;

public:
    HighPassFilter(float smoothingFactor = 0.9);
    void reset();
    float filter(float value);
    void setAlpha(float smoothingFactor);
    float getAlpha() const;
};

#endif
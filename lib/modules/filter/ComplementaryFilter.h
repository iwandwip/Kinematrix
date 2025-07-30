#ifndef COMPLEMENTARY_FILTER_H
#define COMPLEMENTARY_FILTER_H

class ComplementaryFilter {
private:
    float alpha;
    float lastAngle;
    bool initialized;

public:
    ComplementaryFilter(float smoothingFactor = 0.98);
    void reset();
    float filter(float newAngle, float gyroRate, float dt);
    void setAlpha(float smoothingFactor);
    float getAlpha() const;
};

#endif

#ifndef KALMAN_FILTER_H
#define KALMAN_FILTER_H

class KalmanFilter {
private:
    float q;
    float r;
    float p;
    float x;
    bool initialized;

public:
    KalmanFilter(float process_noise, float measurement_noise, float initial_error = 1.0);
    void reset();
    float filter(float measurement);
    void setState(float state);
    float getState() const;
    void setNoise(float process_noise, float measurement_noise);
};

#endif
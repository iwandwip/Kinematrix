#ifndef STANDARD_SCALER_H
#define STANDARD_SCALER_H

#include <Arduino.h>

class StandardScaler {
private:
    float *means;
    float *stdDevs;
    int numFeatures;
    bool isFitted;
    
    float calculateMean(float **data, int numSamples, int featureIndex);
    float calculateStdDev(float **data, int numSamples, int featureIndex, float mean);

public:
    StandardScaler(int features);
    ~StandardScaler();
    
    void fit(float **data, int numSamples);
    void transform(float **data, int numSamples);
    void fitTransform(float **data, int numSamples);
    void inverseTransform(float **data, int numSamples);
    
    float getMean(int featureIndex);
    float getStdDev(int featureIndex);
    bool isTrained();
    void reset();
    
    void saveParameters();
    void loadParameters();
};

#endif
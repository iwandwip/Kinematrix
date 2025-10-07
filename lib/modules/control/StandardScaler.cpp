#include "StandardScaler.h"
#include <math.h>

#if defined(ESP32) || defined(ESP8266)
#include <EEPROM.h>
#define EEPROM_SCALER_ADDR 500
#endif

StandardScaler::StandardScaler(int features) {
    numFeatures = features;
    means = new float[numFeatures];
    stdDevs = new float[numFeatures];
    isFitted = false;
    
    for (int i = 0; i < numFeatures; i++) {
        means[i] = 0.0;
        stdDevs[i] = 1.0;
    }
}

StandardScaler::~StandardScaler() {
    delete[] means;
    delete[] stdDevs;
}

float StandardScaler::calculateMean(float **data, int numSamples, int featureIndex) {
    float sum = 0.0;
    for (int i = 0; i < numSamples; i++) {
        sum += data[i][featureIndex];
    }
    return sum / numSamples;
}

float StandardScaler::calculateStdDev(float **data, int numSamples, int featureIndex, float mean) {
    float sumSquaredDiff = 0.0;
    for (int i = 0; i < numSamples; i++) {
        float diff = data[i][featureIndex] - mean;
        sumSquaredDiff += diff * diff;
    }
    float variance = sumSquaredDiff / numSamples;
    return sqrt(variance);
}

void StandardScaler::fit(float **data, int numSamples) {
    for (int f = 0; f < numFeatures; f++) {
        means[f] = calculateMean(data, numSamples, f);
        stdDevs[f] = calculateStdDev(data, numSamples, f, means[f]);
        
        if (stdDevs[f] == 0.0) {
            stdDevs[f] = 1.0;
        }
    }
    isFitted = true;
}

void StandardScaler::transform(float **data, int numSamples) {
    if (!isFitted) return;
    
    for (int i = 0; i < numSamples; i++) {
        for (int f = 0; f < numFeatures; f++) {
            data[i][f] = (data[i][f] - means[f]) / stdDevs[f];
        }
    }
}

void StandardScaler::fitTransform(float **data, int numSamples) {
    fit(data, numSamples);
    transform(data, numSamples);
}

void StandardScaler::inverseTransform(float **data, int numSamples) {
    if (!isFitted) return;
    
    for (int i = 0; i < numSamples; i++) {
        for (int f = 0; f < numFeatures; f++) {
            data[i][f] = (data[i][f] * stdDevs[f]) + means[f];
        }
    }
}

float StandardScaler::getMean(int featureIndex) {
    if (featureIndex >= 0 && featureIndex < numFeatures) {
        return means[featureIndex];
    }
    return 0.0;
}

float StandardScaler::getStdDev(int featureIndex) {
    if (featureIndex >= 0 && featureIndex < numFeatures) {
        return stdDevs[featureIndex];
    }
    return 1.0;
}

bool StandardScaler::isTrained() {
    return isFitted;
}

void StandardScaler::reset() {
    isFitted = false;
    for (int i = 0; i < numFeatures; i++) {
        means[i] = 0.0;
        stdDevs[i] = 1.0;
    }
}

void StandardScaler::saveParameters() {
#if defined(ESP32) || defined(ESP8266)
    if (!isFitted) return;
    
    EEPROM.begin(512);
    int addr = EEPROM_SCALER_ADDR;
    
    EEPROM.put(addr, numFeatures);
    addr += sizeof(int);
    
    for (int i = 0; i < numFeatures; i++) {
        EEPROM.put(addr, means[i]);
        addr += sizeof(float);
        EEPROM.put(addr, stdDevs[i]);
        addr += sizeof(float);
    }
    
    EEPROM.commit();
#endif
}

void StandardScaler::loadParameters() {
#if defined(ESP32) || defined(ESP8266)
    EEPROM.begin(512);
    int addr = EEPROM_SCALER_ADDR;
    
    int savedFeatures;
    EEPROM.get(addr, savedFeatures);
    addr += sizeof(int);
    
    if (savedFeatures == numFeatures) {
        for (int i = 0; i < numFeatures; i++) {
            EEPROM.get(addr, means[i]);
            addr += sizeof(float);
            EEPROM.get(addr, stdDevs[i]);
            addr += sizeof(float);
        }
        isFitted = true;
    }
#endif
}
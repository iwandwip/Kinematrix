/*
 *  KNN.h
 *
 *  KNN lib
 *  Created on: 2023. 4. 3
 */

#pragma once

#ifndef KNN_LIB_H
#define KNN_LIB_H

#pragma message("[COMPILED]: KNN.h")

#include "Arduino.h"

class KNN {
private:
    int k;
    int maxFeatures;
    int maxData;
    int currentDataSize;
    float **trainingData;
    char **trainingLabels;

    float calculateDistance(float dataPoint[], float trainDataPoint[]) const;
    void sortDistances(float distances[], int labels[]) const;

public:
    KNN(int k, int maxFeatures, int maxData);
    ~KNN();
    void addTrainingData(const char *label, float features[]);
    const char *predict(float dataPoint[]);
};

#endif
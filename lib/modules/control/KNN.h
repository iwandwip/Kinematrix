/*
 *  KNN.h
 *
 *  Enhanced KNN library
 *  Created on: 2023. 4. 3
 */

#pragma once

#ifndef KNN_LIB_H
#define KNN_LIB_H

#pragma message("[COMPILED]: KNN.h")

#include "Arduino.h"
#ifdef ESP32
#include "SPIFFS.h"
#endif

enum DistanceMetric {
    EUCLIDEAN,
    MANHATTAN,
    COSINE
};

class KNN {
private:
    int k;
    int maxFeatures;
    int maxData;
    int currentDataSize;
    float **trainingData;
    char **trainingLabels;

    DistanceMetric metric;
    bool useWeightedVoting;
    bool normalizationEnabled;
    bool lowMemoryMode;
    bool debugMode;

    float *featureMin;
    float *featureMax;

    struct DistanceIndex {
        float distance;
        int index;
    };
    int *voteBuffer;
    DistanceIndex *distanceBuffer;

    bool errorState;
    char errorMessage[50];

    float calculateEuclideanDistance(const float dataPoint[], const float trainDataPoint[]) const;
    float calculateManhattanDistance(const float dataPoint[], const float trainDataPoint[]) const;
    float calculateCosineDistance(const float dataPoint[], const float trainDataPoint[]) const;
    float calculateDistance(const float dataPoint[], const float trainDataPoint[]) const;

    static int compareDistances(const void *a, const void *b);
    float normalizeFeature(float value, int featureIndex) const;

public:
    KNN(int k, int maxFeatures, int maxData);
    ~KNN();

    bool addTrainingData(const char *label, const float features[]);
    const char *predict(const float dataPoint[]);

    void setDistanceMetric(DistanceMetric newMetric);
    void setWeightedVoting(bool weighted);
    void enableNormalization(bool enable);
    void setLowMemoryMode(bool enable);
    void setDebugMode(bool enable);
    void calculateFeatureRanges();

    void clearTrainingData();
    bool removeTrainingData(int index);
    int getDataCount() const;
    int getDataCountByLabel(const char *label) const;

    bool getNearestNeighbors(const float dataPoint[], int indices[], float distances[], int neighborCount);
    float getPredictionConfidence(const float dataPoint[]);

    float evaluateAccuracy(const float **testFeatures, const char **testLabels, int testCount);
    float crossValidate(int folds);

    bool hasError() const;
    const char *getErrorMessage() const;
    void clearError();

#ifdef ESP32
    bool saveModel(const char *filename);
    bool loadModel(const char *filename);
#endif
};

#endif
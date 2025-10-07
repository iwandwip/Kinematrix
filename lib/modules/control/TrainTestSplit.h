#ifndef TRAIN_TEST_SPLIT_H
#define TRAIN_TEST_SPLIT_H

#include <Arduino.h>

struct DataSplit {
    float **X_train;
    float **X_test;
    float *y_train;
    float *y_test;
    int trainSize;
    int testSize;
};

class TrainTestSplit {
private:
    int *indices;
    int numSamples;
    int numFeatures;
    bool shuffled;
    
    void shuffleIndices();
    void allocateMemory(DataSplit &split, int trainSize, int testSize);

public:
    TrainTestSplit();
    ~TrainTestSplit();
    
    DataSplit split(float **X, float *y, int samples, int features, float testSize = 0.2f, bool shuffle = true, int randomState = -1);
    DataSplit splitByCount(float **X, float *y, int samples, int features, int testSamples, bool shuffle = true, int randomState = -1);
    
    void freeDataSplit(DataSplit &split);
    
    void setRandomSeed(int seed);
    void stratifiedSplit(float **X, float *y, int samples, int features, float testSize, DataSplit &split);
    
    void printSplitInfo(const DataSplit &split);
    bool validateSplit(const DataSplit &split, int originalSamples);
};

#endif
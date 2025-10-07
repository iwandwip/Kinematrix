#include "TrainTestSplit.h"
#include <stdlib.h>

TrainTestSplit::TrainTestSplit() {
    indices = nullptr;
    numSamples = 0;
    numFeatures = 0;
    shuffled = false;
}

TrainTestSplit::~TrainTestSplit() {
    if (indices != nullptr) {
        delete[] indices;
    }
}

void TrainTestSplit::shuffleIndices() {
    for (int i = numSamples - 1; i > 0; i--) {
        int j = rand() % (i + 1);
        int temp = indices[i];
        indices[i] = indices[j];
        indices[j] = temp;
    }
    shuffled = true;
}

void TrainTestSplit::allocateMemory(DataSplit &split, int trainSize, int testSize) {
    split.trainSize = trainSize;
    split.testSize = testSize;
    
    split.X_train = new float*[trainSize];
    split.X_test = new float*[testSize];
    split.y_train = new float[trainSize];
    split.y_test = new float[testSize];
    
    for (int i = 0; i < trainSize; i++) {
        split.X_train[i] = new float[numFeatures];
    }
    
    for (int i = 0; i < testSize; i++) {
        split.X_test[i] = new float[numFeatures];
    }
}

DataSplit TrainTestSplit::split(float **X, float *y, int samples, int features, float testSize, bool shuffle, int randomState) {
    int testSamples = (int)(samples * testSize);
    return splitByCount(X, y, samples, features, testSamples, shuffle, randomState);
}

DataSplit TrainTestSplit::splitByCount(float **X, float *y, int samples, int features, int testSamples, bool shuffle, int randomState) {
    numSamples = samples;
    numFeatures = features;
    
    if (randomState >= 0) {
        setRandomSeed(randomState);
    }
    
    if (indices != nullptr) {
        delete[] indices;
    }
    indices = new int[numSamples];
    
    for (int i = 0; i < numSamples; i++) {
        indices[i] = i;
    }
    
    if (shuffle) {
        shuffleIndices();
    }
    
    int trainSamples = numSamples - testSamples;
    DataSplit result;
    allocateMemory(result, trainSamples, testSamples);
    
    for (int i = 0; i < trainSamples; i++) {
        int idx = indices[i];
        result.y_train[i] = y[idx];
        for (int f = 0; f < numFeatures; f++) {
            result.X_train[i][f] = X[idx][f];
        }
    }
    
    for (int i = 0; i < testSamples; i++) {
        int idx = indices[trainSamples + i];
        result.y_test[i] = y[idx];
        for (int f = 0; f < numFeatures; f++) {
            result.X_test[i][f] = X[idx][f];
        }
    }
    
    return result;
}

void TrainTestSplit::freeDataSplit(DataSplit &split) {
    if (split.X_train != nullptr) {
        for (int i = 0; i < split.trainSize; i++) {
            delete[] split.X_train[i];
        }
        delete[] split.X_train;
        split.X_train = nullptr;
    }
    
    if (split.X_test != nullptr) {
        for (int i = 0; i < split.testSize; i++) {
            delete[] split.X_test[i];
        }
        delete[] split.X_test;
        split.X_test = nullptr;
    }
    
    if (split.y_train != nullptr) {
        delete[] split.y_train;
        split.y_train = nullptr;
    }
    
    if (split.y_test != nullptr) {
        delete[] split.y_test;
        split.y_test = nullptr;
    }
    
    split.trainSize = 0;
    split.testSize = 0;
}

void TrainTestSplit::setRandomSeed(int seed) {
    srand(seed);
}

void TrainTestSplit::stratifiedSplit(float **X, float *y, int samples, int features, float testSize, DataSplit &split) {
    numSamples = samples;
    numFeatures = features;
    
    float *uniqueLabels = new float[samples];
    int *labelCounts = new int[samples];
    int numUniqueLabels = 0;
    
    for (int i = 0; i < samples; i++) {
        bool found = false;
        for (int j = 0; j < numUniqueLabels; j++) {
            if (y[i] == uniqueLabels[j]) {
                labelCounts[j]++;
                found = true;
                break;
            }
        }
        if (!found) {
            uniqueLabels[numUniqueLabels] = y[i];
            labelCounts[numUniqueLabels] = 1;
            numUniqueLabels++;
        }
    }
    
    int testSamples = (int)(samples * testSize);
    int trainSamples = samples - testSamples;
    
    allocateMemory(split, trainSamples, testSamples);
    
    int trainIdx = 0, testIdx = 0;
    
    for (int label = 0; label < numUniqueLabels; label++) {
        int labelTestSize = (int)(labelCounts[label] * testSize);
        int labelTrainSize = labelCounts[label] - labelTestSize;
        
        int labelSamplesFound = 0;
        for (int i = 0; i < samples && labelSamplesFound < labelCounts[label]; i++) {
            if (y[i] == uniqueLabels[label]) {
                if (labelSamplesFound < labelTrainSize) {
                    split.y_train[trainIdx] = y[i];
                    for (int f = 0; f < features; f++) {
                        split.X_train[trainIdx][f] = X[i][f];
                    }
                    trainIdx++;
                } else {
                    split.y_test[testIdx] = y[i];
                    for (int f = 0; f < features; f++) {
                        split.X_test[testIdx][f] = X[i][f];
                    }
                    testIdx++;
                }
                labelSamplesFound++;
            }
        }
    }
    
    delete[] uniqueLabels;
    delete[] labelCounts;
}

void TrainTestSplit::printSplitInfo(const DataSplit &split) {
    Serial.println("=== Train/Test Split Info ===");
    Serial.println("Train samples: " + String(split.trainSize));
    Serial.println("Test samples: " + String(split.testSize));
    Serial.println("Total samples: " + String(split.trainSize + split.testSize));
    Serial.println("Test ratio: " + String((float)split.testSize / (split.trainSize + split.testSize), 3));
}

bool TrainTestSplit::validateSplit(const DataSplit &split, int originalSamples) {
    if (split.trainSize + split.testSize != originalSamples) {
        return false;
    }
    
    if (split.trainSize <= 0 || split.testSize <= 0) {
        return false;
    }
    
    if (split.X_train == nullptr || split.X_test == nullptr || 
        split.y_train == nullptr || split.y_test == nullptr) {
        return false;
    }
    
    return true;
}
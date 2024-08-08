/*
 *  PID.cpp
 *
 *  PID lib
 *  Created on: 2023. 4. 3
 */

#include "KNN.h"

KNN::KNN(int k, int maxFeatures, int maxData) : k(k), maxFeatures(maxFeatures), maxData(maxData) {
    trainingData = new float *[maxData];
    for (int i = 0; i < maxData; ++i) {
        trainingData[i] = new float[maxFeatures + 1]; // +1 untuk label
    }
    trainingLabels = new char *[maxData];
    for (int i = 0; i < maxData; ++i) {
        trainingLabels[i] = new char[20]; // Ukuran maksimum label 20 karakter
    }
    currentDataSize = 0;
}

KNN::~KNN() {
    for (int i = 0; i < maxData; ++i) {
        delete[] trainingData[i];
        delete[] trainingLabels[i];
    }
    delete[] trainingData;
    delete[] trainingLabels;
}

void KNN::addTrainingData(const char *label, float features[]) {
    if (currentDataSize < maxData) {
        for (int i = 0; i < maxFeatures; ++i) {
            trainingData[currentDataSize][i] = features[i];
        }
        trainingData[currentDataSize][maxFeatures] = currentDataSize; // Menyimpan indeks label
        strncpy(trainingLabels[currentDataSize], label, 20);
        currentDataSize++;
    }
}

const char *KNN::predict(float dataPoint[]) {
    float *distances = new float[currentDataSize];
    int *labels = new int[currentDataSize];
    // Menghitung jarak antara data baru dengan data training
    for (int i = 0; i < currentDataSize; i++) {
        distances[i] = calculateDistance(dataPoint, trainingData[i]);
        labels[i] = (int) trainingData[i][maxFeatures];
    }
    // Mengurutkan jarak dan mengambil k terdekat
    sortDistances(distances, labels);
    // Menghitung frekuensi dari k tetangga terdekat
    int *freq = new int[currentDataSize]();
    int maxLabelIndex = 0;
    for (int i = 0; i < k; i++) {
        freq[labels[i]]++;
        if (freq[labels[i]] > freq[maxLabelIndex]) {
            maxLabelIndex = labels[i];
        }
    }
    const char *predictedLabel = trainingLabels[maxLabelIndex];
    delete[] distances;
    delete[] labels;
    delete[] freq;
    return predictedLabel;
}

float KNN::calculateDistance(float dataPoint[], float trainDataPoint[]) {
    float sum = 0;
    for (int i = 0; i < maxFeatures; i++) {
        sum += pow(dataPoint[i] - trainDataPoint[i], 2);
    }
    return sqrt(sum);
}

void KNN::sortDistances(float distances[], int labels[]) {
    for (int i = 0; i < currentDataSize - 1; i++) {
        for (int j = i + 1; j < currentDataSize; j++) {
            if (distances[i] > distances[j]) {
                float tempDistance = distances[i];
                distances[i] = distances[j];
                distances[j] = tempDistance;

                int tempLabel = labels[i];
                labels[i] = labels[j];
                labels[j] = tempLabel;
            }
        }
    }
}
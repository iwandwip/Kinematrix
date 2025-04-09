/*
 *  KNN.cpp
 *
 *  Enhanced KNN library
 *  Created on: 2023. 4. 3
 */

#include "KNN.h"

int KNN::compareDistances(const void *a, const void *b) {
    const auto *da = (const DistanceIndex *) a;
    const auto *db = (const DistanceIndex *) b;

    if (da->distance < db->distance) return -1;
    if (da->distance > db->distance) return 1;
    return 0;
}

KNN::KNN(int k, int maxFeatures, int maxData) :
        k(k), maxFeatures(maxFeatures), maxData(maxData), currentDataSize(0),
        metric(EUCLIDEAN), useWeightedVoting(false), normalizationEnabled(false),
        lowMemoryMode(false), debugMode(false), featureMin(nullptr), featureMax(nullptr),
        voteBuffer(nullptr), distanceBuffer(nullptr), errorState(false) {

    if (k <= 0 || k > maxData || maxFeatures <= 0 || maxData <= 0) {
        errorState = true;
        strncpy(errorMessage, "Invalid parameters", 49);
        errorMessage[49] = '\0';
        return;
    }

    trainingData = new float *[maxData];
    if (trainingData == nullptr) {
        errorState = true;
        strncpy(errorMessage, "Memory allocation failed", 49);
        errorMessage[49] = '\0';
        return;
    }

    for (int i = 0; i < maxData; ++i) {
        trainingData[i] = new float[maxFeatures];
        if (trainingData[i] == nullptr) {
            for (int j = 0; j < i; ++j) {
                delete[] trainingData[j];
            }
            delete[] trainingData;
            trainingData = nullptr;

            errorState = true;
            strncpy(errorMessage, "Memory allocation failed", 49);
            errorMessage[49] = '\0';
            return;
        }
    }

    trainingLabels = new char *[maxData];
    if (trainingLabels == nullptr) {
        for (int i = 0; i < maxData; ++i) {
            delete[] trainingData[i];
        }
        delete[] trainingData;
        trainingData = nullptr;

        errorState = true;
        strncpy(errorMessage, "Memory allocation failed", 49);
        errorMessage[49] = '\0';
        return;
    }

    for (int i = 0; i < maxData; ++i) {
        trainingLabels[i] = new char[20];
        if (trainingLabels[i] == nullptr) {
            for (int j = 0; j < i; ++j) {
                delete[] trainingLabels[j];
            }
            delete[] trainingLabels;
            trainingLabels = nullptr;

            for (int j = 0; j < maxData; ++j) {
                delete[] trainingData[j];
            }
            delete[] trainingData;
            trainingData = nullptr;

            errorState = true;
            strncpy(errorMessage, "Memory allocation failed", 49);
            errorMessage[49] = '\0';
            return;
        }
        trainingLabels[i][0] = '\0';
    }

    voteBuffer = new int[maxData];
    distanceBuffer = new DistanceIndex[maxData];

    if (voteBuffer == nullptr || distanceBuffer == nullptr) {
        delete[] voteBuffer;
        delete[] distanceBuffer;
        voteBuffer = nullptr;
        distanceBuffer = nullptr;

        for (int i = 0; i < maxData; ++i) {
            delete[] trainingLabels[i];
            delete[] trainingData[i];
        }
        delete[] trainingLabels;
        delete[] trainingData;
        trainingLabels = nullptr;
        trainingData = nullptr;

        errorState = true;
        strncpy(errorMessage, "Memory allocation failed", 49);
        errorMessage[49] = '\0';
    }

    featureMin = new float[maxFeatures];
    featureMax = new float[maxFeatures];

    if (featureMin == nullptr || featureMax == nullptr) {
        delete[] featureMin;
        delete[] featureMax;
        featureMin = nullptr;
        featureMax = nullptr;

        delete[] voteBuffer;
        delete[] distanceBuffer;

        for (int i = 0; i < maxData; ++i) {
            delete[] trainingLabels[i];
            delete[] trainingData[i];
        }
        delete[] trainingLabels;
        delete[] trainingData;

        errorState = true;
        strncpy(errorMessage, "Memory allocation failed", 49);
        errorMessage[49] = '\0';
    }
}

KNN::~KNN() {
    if (trainingData != nullptr) {
        for (int i = 0; i < maxData; ++i) {
            if (trainingData[i] != nullptr) delete[] trainingData[i];
        }
        delete[] trainingData;
    }

    if (trainingLabels != nullptr) {
        for (int i = 0; i < maxData; ++i) {
            if (trainingLabels[i] != nullptr) delete[] trainingLabels[i];
        }
        delete[] trainingLabels;
    }

    delete[] voteBuffer;
    delete[] distanceBuffer;
    delete[] featureMin;
    delete[] featureMax;
}

bool KNN::addTrainingData(const char *label, const float features[]) {
    if (errorState) return false;

    if (currentDataSize >= maxData) {
        errorState = true;
        strncpy(errorMessage, "Training data full", 49);
        errorMessage[49] = '\0';
        return false;
    }

    if (label == nullptr || features == nullptr) {
        errorState = true;
        strncpy(errorMessage, "Null pointer provided", 49);
        errorMessage[49] = '\0';
        return false;
    }

    for (int i = 0; i < maxFeatures; ++i) {
        trainingData[currentDataSize][i] = features[i];
    }

    strncpy(trainingLabels[currentDataSize], label, 19);
    trainingLabels[currentDataSize][19] = '\0';

    currentDataSize++;

    if (normalizationEnabled && currentDataSize == 1) {
        calculateFeatureRanges();
    } else if (normalizationEnabled) {
        for (int i = 0; i < maxFeatures; ++i) {
            if (features[i] < featureMin[i]) featureMin[i] = features[i];
            if (features[i] > featureMax[i]) featureMax[i] = features[i];
        }
    }

    return true;
}

const char *KNN::predict(const float dataPoint[]) {
    if (errorState) return "ERROR";

    if (currentDataSize == 0) {
        errorState = true;
        strncpy(errorMessage, "No training data available", 49);
        errorMessage[49] = '\0';
        return "ERROR";
    }

    if (dataPoint == nullptr) {
        errorState = true;
        strncpy(errorMessage, "Null data point provided", 49);
        errorMessage[49] = '\0';
        return "ERROR";
    }

    if (voteBuffer == nullptr || distanceBuffer == nullptr) {
        errorState = true;
        strncpy(errorMessage, "Buffer not allocated", 49);
        errorMessage[49] = '\0';
        return "ERROR";
    }

    for (int i = 0; i < currentDataSize; i++) {
        voteBuffer[i] = 0;
    }

    for (int i = 0; i < currentDataSize; i++) {
        distanceBuffer[i].distance = calculateDistance(dataPoint, trainingData[i]);
        distanceBuffer[i].index = i;
    }

    qsort(distanceBuffer, currentDataSize, sizeof(DistanceIndex), compareDistances);

    int effectiveK = (k > currentDataSize) ? currentDataSize : k;

    int maxVotes = 0;
    int maxVotedIndex = distanceBuffer[0].index;

    if (useWeightedVoting) {
        auto *weights = new float[currentDataSize];
        if (weights == nullptr) {
            errorState = true;
            strncpy(errorMessage, "Memory allocation failed", 49);
            errorMessage[49] = '\0';
            return "ERROR";
        }

        for (int i = 0; i < effectiveK; i++) {
            int idx = distanceBuffer[i].index;
            float distance = distanceBuffer[i].distance;

            if (distance <= 0.0001f) {
                for (int j = 0; j < currentDataSize; j++) {
                    weights[j] = 0;
                }
                weights[idx] = 1.0f;
                maxVotedIndex = idx;
                break;
            }

            float weight = 1.0f / (distance + 0.0001f);
            weights[idx] += weight;

            if (weights[idx] > weights[maxVotedIndex]) {
                maxVotedIndex = idx;
            }
        }

        delete[] weights;
    } else {
        for (int i = 0; i < effectiveK; i++) {
            int idx = distanceBuffer[i].index;
            voteBuffer[idx]++;

            if (voteBuffer[idx] > maxVotes) {
                maxVotes = voteBuffer[idx];
                maxVotedIndex = idx;
            }
        }
    }

    if (debugMode) {
        Serial.print("Predicted label: ");
        Serial.println(trainingLabels[maxVotedIndex]);
    }

    return trainingLabels[maxVotedIndex];
}

void KNN::setDistanceMetric(DistanceMetric newMetric) {
    metric = newMetric;
}

void KNN::setWeightedVoting(bool weighted) {
    useWeightedVoting = weighted;
}

void KNN::enableNormalization(bool enable) {
    normalizationEnabled = enable;
    if (enable && currentDataSize > 0) {
        calculateFeatureRanges();
    }
}

void KNN::setLowMemoryMode(bool enable) {
    lowMemoryMode = enable;
}

void KNN::setDebugMode(bool enable) {
    debugMode = enable;
}

void KNN::calculateFeatureRanges() {
    if (currentDataSize == 0 || featureMin == nullptr || featureMax == nullptr) return;

    for (int i = 0; i < maxFeatures; i++) {
        featureMin[i] = trainingData[0][i];
        featureMax[i] = trainingData[0][i];
    }

    for (int i = 1; i < currentDataSize; i++) {
        for (int j = 0; j < maxFeatures; j++) {
            if (trainingData[i][j] < featureMin[j]) featureMin[j] = trainingData[i][j];
            if (trainingData[i][j] > featureMax[j]) featureMax[j] = trainingData[i][j];
        }
    }
}

void KNN::clearTrainingData() {
    currentDataSize = 0;
    clearError();
}

bool KNN::removeTrainingData(int index) {
    if (index < 0 || index >= currentDataSize) {
        errorState = true;
        strncpy(errorMessage, "Invalid index", 49);
        errorMessage[49] = '\0';
        return false;
    }

    for (int i = index; i < currentDataSize - 1; i++) {
        for (int j = 0; j < maxFeatures; j++) {
            trainingData[i][j] = trainingData[i + 1][j];
        }
        strncpy(trainingLabels[i], trainingLabels[i + 1], 19);
        trainingLabels[i][19] = '\0';
    }

    currentDataSize--;

    if (normalizationEnabled) {
        calculateFeatureRanges();
    }

    return true;
}

int KNN::getDataCount() const {
    return currentDataSize;
}

int KNN::getDataCountByLabel(const char *label) const {
    if (label == nullptr) return 0;

    int count = 0;
    for (int i = 0; i < currentDataSize; i++) {
        if (strcmp(trainingLabels[i], label) == 0) {
            count++;
        }
    }

    return count;
}

bool KNN::getNearestNeighbors(const float dataPoint[], int indices[], float distances[], int neighborCount) {
    if (errorState || dataPoint == nullptr || indices == nullptr || distances == nullptr) return false;
    if (neighborCount <= 0 || neighborCount > currentDataSize) return false;

    for (int i = 0; i < currentDataSize; i++) {
        distanceBuffer[i].distance = calculateDistance(dataPoint, trainingData[i]);
        distanceBuffer[i].index = i;
    }

    qsort(distanceBuffer, currentDataSize, sizeof(DistanceIndex), compareDistances);

    for (int i = 0; i < neighborCount; i++) {
        indices[i] = distanceBuffer[i].index;
        distances[i] = distanceBuffer[i].distance;
    }

    return true;
}

float KNN::getPredictionConfidence(const float dataPoint[]) {
    if (errorState || currentDataSize == 0 || dataPoint == nullptr) return 0.0f;

    for (int i = 0; i < currentDataSize; i++) {
        voteBuffer[i] = 0;
        distanceBuffer[i].distance = calculateDistance(dataPoint, trainingData[i]);
        distanceBuffer[i].index = i;
    }

    qsort(distanceBuffer, currentDataSize, sizeof(DistanceIndex), compareDistances);

    int effectiveK = (k > currentDataSize) ? currentDataSize : k;

    if (useWeightedVoting) {
        float totalWeight = 0.0f;
        float maxWeight = 0.0f;
        int maxIndex = -1;

        auto *labelWeights = new float[currentDataSize];
        if (labelWeights == nullptr) return 0.0f;

        for (int i = 0; i < currentDataSize; i++) {
            labelWeights[i] = 0.0f;
        }

        for (int i = 0; i < effectiveK; i++) {
            int idx = distanceBuffer[i].index;
            float distance = distanceBuffer[i].distance;
            float weight = 1.0f / (distance + 0.0001f);

            labelWeights[idx] += weight;
            totalWeight += weight;

            if (labelWeights[idx] > maxWeight) {
                maxWeight = labelWeights[idx];
                maxIndex = idx;
            }
        }

        float confidence = (totalWeight > 0.0f) ? (maxWeight / totalWeight) : 0.0f;
        delete[] labelWeights;
        return confidence;
    } else {
        int totalVotes = 0;
        int maxVotes = 0;

        for (int i = 0; i < effectiveK; i++) {
            int idx = distanceBuffer[i].index;
            voteBuffer[idx]++;
            totalVotes++;

            if (voteBuffer[idx] > maxVotes) {
                maxVotes = voteBuffer[idx];
            }
        }

        return (float) maxVotes / totalVotes;
    }
}

float KNN::evaluateAccuracy(const float **testFeatures, const char **testLabels, int testCount) {
    if (errorState || testFeatures == nullptr || testLabels == nullptr || testCount <= 0) return 0.0f;

    int correctPredictions = 0;

    for (int i = 0; i < testCount; i++) {
        const char *predictedLabel = predict(testFeatures[i]);
        if (strcmp(predictedLabel, testLabels[i]) == 0) {
            correctPredictions++;
        }
    }

    return (float) correctPredictions / testCount;
}

float KNN::crossValidate(int folds) {
    if (errorState || currentDataSize < folds || folds < 2) return 0.0f;

    int *indices = new int[currentDataSize];
    if (indices == nullptr) return 0.0f;

    for (int i = 0; i < currentDataSize; i++) {
        indices[i] = i;
    }

    for (int i = 0; i < currentDataSize; i++) {
        int j = random(currentDataSize);
        int temp = indices[i];
        indices[i] = indices[j];
        indices[j] = temp;
    }

    int foldSize = currentDataSize / folds;
    float totalAccuracy = 0.0f;

    for (int fold = 0; fold < folds; fold++) {
        KNN tempModel(k, maxFeatures, currentDataSize);
        tempModel.setDistanceMetric(metric);
        tempModel.setWeightedVoting(useWeightedVoting);
        tempModel.enableNormalization(normalizationEnabled);

        int testStart = fold * foldSize;
        int testEnd = (fold == folds - 1) ? currentDataSize : (fold + 1) * foldSize;

        for (int i = 0; i < currentDataSize; i++) {
            int idx = indices[i];
            if (i >= testStart && i < testEnd) continue;

            tempModel.addTrainingData(trainingLabels[idx], trainingData[idx]);
        }

        int correctPredictions = 0;
        int testCount = testEnd - testStart;

        for (int i = testStart; i < testEnd; i++) {
            int idx = indices[i];
            const char *predictedLabel = tempModel.predict(trainingData[idx]);
            if (strcmp(predictedLabel, trainingLabels[idx]) == 0) {
                correctPredictions++;
            }
        }

        totalAccuracy += (float) correctPredictions / testCount;
    }

    delete[] indices;
    return totalAccuracy / folds;
}

bool KNN::hasError() const {
    return errorState;
}

const char *KNN::getErrorMessage() const {
    return errorState ? errorMessage : "No error";
}

void KNN::clearError() {
    errorState = false;
    errorMessage[0] = '\0';
}

#ifdef ESP32

bool KNN::saveModel(const char *filename) {
    if (errorState || filename == nullptr) return false;

    File file = SPIFFS.open(filename, "w");
    if (!file) {
        errorState = true;
        strncpy(errorMessage, "Failed to open file for writing", 49);
        errorMessage[49] = '\0';
        return false;
    }

    file.write((uint8_t *) &k, sizeof(int));
    file.write((uint8_t *) &maxFeatures, sizeof(int));
    file.write((uint8_t *) &maxData, sizeof(int));
    file.write((uint8_t *) &currentDataSize, sizeof(int));
    file.write((uint8_t *) &metric, sizeof(DistanceMetric));
    file.write((uint8_t *) &useWeightedVoting, sizeof(bool));
    file.write((uint8_t *) &normalizationEnabled, sizeof(bool));

    if (normalizationEnabled) {
        file.write((uint8_t *) featureMin, maxFeatures * sizeof(float));
        file.write((uint8_t *) featureMax, maxFeatures * sizeof(float));
    }

    for (int i = 0; i < currentDataSize; i++) {
        file.write((uint8_t *) trainingData[i], maxFeatures * sizeof(float));
        size_t labelLen = strlen(trainingLabels[i]) + 1;
        file.write((uint8_t *) &labelLen, sizeof(size_t));
        file.write((uint8_t *) trainingLabels[i], labelLen);
    }

    file.close();
    return true;
}

bool KNN::loadModel(const char *filename) {
    if (filename == nullptr) return false;

    File file = SPIFFS.open(filename, "r");
    if (!file) {
        errorState = true;
        strncpy(errorMessage, "Failed to open file for reading", 49);
        errorMessage[49] = '\0';
        return false;
    }

    clearTrainingData();

    int newK, newMaxFeatures, newMaxData, newCurrentDataSize;
    file.read((uint8_t *) &newK, sizeof(int));
    file.read((uint8_t *) &newMaxFeatures, sizeof(int));
    file.read((uint8_t *) &newMaxData, sizeof(int));
    file.read((uint8_t *) &newCurrentDataSize, sizeof(int));

    if (newMaxFeatures != maxFeatures || newMaxData > maxData) {
        errorState = true;
        strncpy(errorMessage, "Incompatible model dimensions", 49);
        errorMessage[49] = '\0';
        file.close();
        return false;
    }

    k = newK;
    file.read((uint8_t *) &metric, sizeof(DistanceMetric));
    file.read((uint8_t *) &useWeightedVoting, sizeof(bool));
    file.read((uint8_t *) &normalizationEnabled, sizeof(bool));

    if (normalizationEnabled) {
        file.read((uint8_t *) featureMin, maxFeatures * sizeof(float));
        file.read((uint8_t *) featureMax, maxFeatures * sizeof(float));
    }

    for (int i = 0; i < newCurrentDataSize; i++) {
        float features[maxFeatures];
        file.read((uint8_t *) features, maxFeatures * sizeof(float));

        size_t labelLen;
        file.read((uint8_t *) &labelLen, sizeof(size_t));

        char label[20];
        if (labelLen > 20) labelLen = 20;
        file.read((uint8_t *) label, labelLen);

        addTrainingData(label, features);
    }

    file.close();
    return true;
}

#endif

float KNN::calculateDistance(const float dataPoint[], const float trainDataPoint[]) const {
    switch (metric) {
        case MANHATTAN:
            return calculateManhattanDistance(dataPoint, trainDataPoint);
        case COSINE:
            return calculateCosineDistance(dataPoint, trainDataPoint);
        case EUCLIDEAN:
        default:
            return calculateEuclideanDistance(dataPoint, trainDataPoint);
    }
}

float KNN::calculateEuclideanDistance(const float dataPoint[], const float trainDataPoint[]) const {
    float sum = 0.0f;

    for (int i = 0; i < maxFeatures; i++) {
        float a = dataPoint[i];
        float b = trainDataPoint[i];

        if (normalizationEnabled) {
            a = normalizeFeature(a, i);
            b = normalizeFeature(b, i);
        }

        float diff = a - b;
        sum += diff * diff;
    }

    return sqrt(sum);
}

float KNN::calculateManhattanDistance(const float dataPoint[], const float trainDataPoint[]) const {
    float sum = 0.0f;

    for (int i = 0; i < maxFeatures; i++) {
        float a = dataPoint[i];
        float b = trainDataPoint[i];

        if (normalizationEnabled) {
            a = normalizeFeature(a, i);
            b = normalizeFeature(b, i);
        }

        sum += abs(a - b);
    }

    return sum;
}

float KNN::calculateCosineDistance(const float dataPoint[], const float trainDataPoint[]) const {
    float dotProduct = 0.0f;
    float normA = 0.0f;
    float normB = 0.0f;

    for (int i = 0; i < maxFeatures; i++) {
        float a = dataPoint[i];
        float b = trainDataPoint[i];

        if (normalizationEnabled) {
            a = normalizeFeature(a, i);
            b = normalizeFeature(b, i);
        }

        dotProduct += a * b;
        normA += a * a;
        normB += b * b;
    }

    normA = sqrt(normA);
    normB = sqrt(normB);

    if (normA < 0.0001f || normB < 0.0001f) return 1.0f;

    float similarity = dotProduct / (normA * normB);

    if (similarity > 1.0f) similarity = 1.0f;
    if (similarity < -1.0f) similarity = -1.0f;

    return 1.0f - similarity;
}

float KNN::normalizeFeature(float value, int featureIndex) const {
    if (!normalizationEnabled || featureMin == nullptr || featureMax == nullptr) return value;
    if (featureIndex < 0 || featureIndex >= maxFeatures) return value;

    float min = featureMin[featureIndex];
    float max = featureMax[featureIndex];

    if (max - min < 0.0001f) return 0.5f;

    return (value - min) / (max - min);
}
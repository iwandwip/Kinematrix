/*
 *  DecisionTree.cpp
 *
 *  Decision Tree library for embedded systems
 *  Created on: 2025. 3. 29
 */

#include "DecisionTree.h"

DecisionTree::DecisionTree(int maxFeatures, int maxData, int maxDepth, int minSamplesSplit) :
        maxFeatures(maxFeatures), maxData(maxData), maxDepth(maxDepth),
        minSamplesSplit(minSamplesSplit), currentDataSize(0),
        criterion(GINI), debugMode(false), rootNode(nullptr),
        uniqueLabels(nullptr), uniqueLabelCount(0),
        featureMin(nullptr), featureMax(nullptr), errorState(false) {

    if (maxFeatures <= 0 || maxData <= 0 || maxDepth <= 0 || minSamplesSplit <= 1) {
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

    featureTypes = new FeatureType[maxFeatures];
    if (featureTypes == nullptr) {
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
        return;
    }

    for (int i = 0; i < maxFeatures; ++i) {
        featureTypes[i] = NUMERIC;
    }

    featureMin = new float[maxFeatures];
    featureMax = new float[maxFeatures];

    if (featureMin == nullptr || featureMax == nullptr) {
        delete[] featureMin;
        delete[] featureMax;
        delete[] featureTypes;
        featureMin = nullptr;
        featureMax = nullptr;
        featureTypes = nullptr;

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
}

DecisionTree::~DecisionTree() {
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

    if (uniqueLabels != nullptr) {
        for (int i = 0; i < uniqueLabelCount; ++i) {
            if (uniqueLabels[i] != nullptr) delete[] uniqueLabels[i];
        }
        delete[] uniqueLabels;
    }

    delete[] featureTypes;
    delete[] featureMin;
    delete[] featureMax;
    delete rootNode;
}

bool DecisionTree::addTrainingData(const char *label, const float features[]) {
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

    return true;
}

void DecisionTree::setFeatureType(int featureIndex, FeatureType type) {
    if (featureIndex >= 0 && featureIndex < maxFeatures) {
        featureTypes[featureIndex] = type;
    }
}

void DecisionTree::clearTrainingData() {
    currentDataSize = 0;
    delete rootNode;
    rootNode = nullptr;
    clearError();
}

bool DecisionTree::removeTrainingData(int index) {
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
    delete rootNode;
    rootNode = nullptr;

    return true;
}

int DecisionTree::getDataCount() const {
    return currentDataSize;
}

int DecisionTree::getDataCountByLabel(const char *label) const {
    if (label == nullptr) return 0;

    int count = 0;
    for (int i = 0; i < currentDataSize; i++) {
        if (strcmp(trainingLabels[i], label) == 0) {
            count++;
        }
    }

    return count;
}

bool DecisionTree::train(SplitCriterion splitCriterion) {
    if (errorState) return false;

    if (currentDataSize < 2) {
        errorState = true;
        strncpy(errorMessage, "Not enough training data", 49);
        errorMessage[49] = '\0';
        return false;
    }

    delete rootNode;
    rootNode = nullptr;

    criterion = splitCriterion;

    findUniqueLabelValues();
    calculateFeatureRanges();

    float **data = new float *[currentDataSize];
    char **labels = new char *[currentDataSize];

    for (int i = 0; i < currentDataSize; i++) {
        data[i] = new float[maxFeatures];
        for (int j = 0; j < maxFeatures; j++) {
            data[i][j] = trainingData[i][j];
        }

        labels[i] = new char[20];
        strncpy(labels[i], trainingLabels[i], 19);
        labels[i][19] = '\0';
    }

    rootNode = buildTree(data, labels, currentDataSize, 0);

    // Prune the tree if desired
    // pruneTree(rootNode);

    for (int i = 0; i < currentDataSize; i++) {
        delete[] data[i];
        delete[] labels[i];
    }
    delete[] data;
    delete[] labels;

    return true;
}

const char *DecisionTree::predict(const float dataPoint[]) {
    if (errorState) return "ERROR";

    if (rootNode == nullptr) {
        errorState = true;
        strncpy(errorMessage, "Model not trained", 49);
        errorMessage[49] = '\0';
        return "ERROR";
    }

    if (dataPoint == nullptr) {
        errorState = true;
        strncpy(errorMessage, "Null data point provided", 49);
        errorMessage[49] = '\0';
        return "ERROR";
    }

    DTNode *current = rootNode;
    while (!current->isLeaf) {
        if (dataPoint[current->featureIndex] <= current->splitValue) {
            if (current->left == nullptr) break;
            current = current->left;
        } else {
            if (current->right == nullptr) break;
            current = current->right;
        }
    }

    if (debugMode) {
        Serial.print("Predicted label: ");
        Serial.println(current->label);
    }

    return current->label;
}

void DecisionTree::setDebugMode(bool enable) {
    debugMode = enable;
}

void DecisionTree::printTree() {
    if (debugMode) {
        Serial.println("Decision Tree structure:");
        Serial.print("Unique labels: ");
        Serial.println(uniqueLabelCount);

        for (int i = 0; i < uniqueLabelCount; i++) {
            Serial.print("  ");
            Serial.println(uniqueLabels[i]);
        }
    }
}

float DecisionTree::evaluateAccuracy(const float **testFeatures, const char **testLabels, int testCount) {
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

float DecisionTree::crossValidate(int folds) {
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
        DecisionTree tempModel(maxFeatures, currentDataSize, maxDepth, minSamplesSplit);

        int testStart = fold * foldSize;
        int testEnd = (fold == folds - 1) ? currentDataSize : (fold + 1) * foldSize;

        for (int i = 0; i < currentDataSize; i++) {
            int idx = indices[i];
            if (i >= testStart && i < testEnd) continue;

            tempModel.addTrainingData(trainingLabels[idx], trainingData[idx]);
        }

        tempModel.train(criterion);

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

float DecisionTree::getPredictionConfidence(const float dataPoint[]) {
    if (errorState || rootNode == nullptr || dataPoint == nullptr) return 0.0f;
    return 1.0f;
}

bool DecisionTree::hasError() const {
    return errorState;
}

const char *DecisionTree::getErrorMessage() const {
    return errorState ? errorMessage : "No error";
}

void DecisionTree::clearError() {
    errorState = false;
    errorMessage[0] = '\0';
}

void DecisionTree::findUniqueLabelValues() {
    if (uniqueLabels != nullptr) {
        for (int i = 0; i < uniqueLabelCount; ++i) {
            delete[] uniqueLabels[i];
        }
        delete[] uniqueLabels;
        uniqueLabels = nullptr;
    }

    uniqueLabelCount = 0;

    for (int i = 0; i < currentDataSize; i++) {
        bool found = false;
        for (int j = 0; j < uniqueLabelCount; j++) {
            if (uniqueLabels != nullptr && strcmp(trainingLabels[i], uniqueLabels[j]) == 0) {
                found = true;
                break;
            }
        }

        if (!found) {
            uniqueLabelCount++;
        }
    }

    uniqueLabels = new char *[uniqueLabelCount];
    for (int i = 0; i < uniqueLabelCount; i++) {
        uniqueLabels[i] = new char[20];
        uniqueLabels[i][0] = '\0';
    }

    int uniqueIndex = 0;
    for (int i = 0; i < currentDataSize; i++) {
        bool found = false;
        for (int j = 0; j < uniqueIndex; j++) {
            if (strcmp(trainingLabels[i], uniqueLabels[j]) == 0) {
                found = true;
                break;
            }
        }

        if (!found) {
            strncpy(uniqueLabels[uniqueIndex], trainingLabels[i], 19);
            uniqueLabels[uniqueIndex][19] = '\0';
            uniqueIndex++;
        }
    }
}

void DecisionTree::calculateFeatureRanges() {
    if (currentDataSize == 0) return;

    for (int i = 0; i < maxFeatures; i++) {
        if (featureTypes[i] == NUMERIC) {
            featureMin[i] = trainingData[0][i];
            featureMax[i] = trainingData[0][i];

            for (int j = 1; j < currentDataSize; j++) {
                if (trainingData[j][i] < featureMin[i]) {
                    featureMin[i] = trainingData[j][i];
                }
                if (trainingData[j][i] > featureMax[i]) {
                    featureMax[i] = trainingData[j][i];
                }
            }
        }
    }
}

float DecisionTree::calculateGiniImpurity(char **labels, int dataSize) {
    if (dataSize == 0) return 0.0f;

    int *counts = new int[uniqueLabelCount]();

    for (int i = 0; i < dataSize; i++) {
        for (int j = 0; j < uniqueLabelCount; j++) {
            if (strcmp(labels[i], uniqueLabels[j]) == 0) {
                counts[j]++;
                break;
            }
        }
    }

    float gini = 1.0f;
    for (int i = 0; i < uniqueLabelCount; i++) {
        float p = (float) counts[i] / dataSize;
        gini -= p * p;
    }

    delete[] counts;
    return gini;
}

float DecisionTree::calculateInformationGain(char **labels, int dataSize) {
    if (dataSize == 0) return 0.0f;

    int *counts = new int[uniqueLabelCount]();

    for (int i = 0; i < dataSize; i++) {
        for (int j = 0; j < uniqueLabelCount; j++) {
            if (strcmp(labels[i], uniqueLabels[j]) == 0) {
                counts[j]++;
                break;
            }
        }
    }

    float entropy = 0.0f;
    for (int i = 0; i < uniqueLabelCount; i++) {
        if (counts[i] > 0) {
            float p = (float) counts[i] / dataSize;
            entropy -= p * log(p) / log(2.0f);
        }
    }

    delete[] counts;
    return entropy;
}

int DecisionTree::findBestSplit(float **data, char **labels, int dataSize, float *splitValue) {
    if (dataSize < minSamplesSplit) return -1;

    int bestFeatureIndex = -1;
    float bestScore = -1.0f;
    float bestSplitVal = 0.0f;

    for (int featureIdx = 0; featureIdx < maxFeatures; featureIdx++) {
        if (featureTypes[featureIdx] == NUMERIC) {
            float min = featureMin[featureIdx];
            float max = featureMax[featureIdx];

            int numSplits = 10;
            for (int i = 1; i < numSplits; i++) {
                float split = min + ((max - min) * i) / numSplits;

                int leftCount = 0, rightCount = 0;

                for (int j = 0; j < dataSize; j++) {
                    if (data[j][featureIdx] <= split) {
                        leftCount++;
                    } else {
                        rightCount++;
                    }
                }

                if (leftCount > 0 && rightCount > 0) {
                    char **leftLabels = new char *[leftCount];
                    char **rightLabels = new char *[rightCount];

                    int leftIdx = 0, rightIdx = 0;
                    for (int j = 0; j < dataSize; j++) {
                        if (data[j][featureIdx] <= split) {
                            leftLabels[leftIdx++] = labels[j];
                        } else {
                            rightLabels[rightIdx++] = labels[j];
                        }
                    }

                    float score;
                    if (criterion == GINI) {
                        float giniParent = calculateGiniImpurity(labels, dataSize);
                        float giniLeft = calculateGiniImpurity(leftLabels, leftCount);
                        float giniRight = calculateGiniImpurity(rightLabels, rightCount);

                        score = giniParent - ((leftCount * giniLeft + rightCount * giniRight) / dataSize);
                    } else {
                        float entropyParent = calculateInformationGain(labels, dataSize);
                        float entropyLeft = calculateInformationGain(leftLabels, leftCount);
                        float entropyRight = calculateInformationGain(rightLabels, rightCount);

                        score = entropyParent - ((leftCount * entropyLeft + rightCount * entropyRight) / dataSize);
                    }

                    delete[] leftLabels;
                    delete[] rightLabels;

                    if (score > bestScore) {
                        bestScore = score;
                        bestFeatureIndex = featureIdx;
                        bestSplitVal = split;
                    }
                }
            }
        }
    }

    if (splitValue != nullptr) {
        *splitValue = bestSplitVal;
    }

    return bestFeatureIndex;
}

void DecisionTree::splitData(float **data, char **labels, int dataSize, int featureIndex, float splitValue,
                             float ***leftData, char ***leftLabels, int &leftSize,
                             float ***rightData, char ***rightLabels, int &rightSize) {
    leftSize = 0;
    rightSize = 0;

    for (int i = 0; i < dataSize; i++) {
        if (data[i][featureIndex] <= splitValue) {
            leftSize++;
        } else {
            rightSize++;
        }
    }

    *leftData = new float *[leftSize];
    *leftLabels = new char *[leftSize];
    *rightData = new float *[rightSize];
    *rightLabels = new char *[rightSize];

    for (int i = 0; i < leftSize; i++) {
        (*leftData)[i] = new float[maxFeatures];
        (*leftLabels)[i] = new char[20];
    }

    for (int i = 0; i < rightSize; i++) {
        (*rightData)[i] = new float[maxFeatures];
        (*rightLabels)[i] = new char[20];
    }

    int leftIdx = 0, rightIdx = 0;
    for (int i = 0; i < dataSize; i++) {
        if (data[i][featureIndex] <= splitValue) {
            for (int j = 0; j < maxFeatures; j++) {
                (*leftData)[leftIdx][j] = data[i][j];
            }
            strcpy((*leftLabels)[leftIdx], labels[i]);
            leftIdx++;
        } else {
            for (int j = 0; j < maxFeatures; j++) {
                (*rightData)[rightIdx][j] = data[i][j];
            }
            strcpy((*rightLabels)[rightIdx], labels[i]);
            rightIdx++;
        }
    }
}

bool DecisionTree::isHomogeneous(char **labels, int dataSize) {
    if (dataSize <= 1) return true;

    for (int i = 1; i < dataSize; i++) {
        if (strcmp(labels[0], labels[i]) != 0) {
            return false;
        }
    }

    return true;
}

char *DecisionTree::getMajorityLabel(char **labels, int dataSize) {
    if (dataSize == 0) return nullptr;

    int *counts = new int[uniqueLabelCount]();

    for (int i = 0; i < dataSize; i++) {
        for (int j = 0; j < uniqueLabelCount; j++) {
            if (strcmp(labels[i], uniqueLabels[j]) == 0) {
                counts[j]++;
                break;
            }
        }
    }

    int maxCount = 0;
    int maxIndex = 0;

    for (int i = 0; i < uniqueLabelCount; i++) {
        if (counts[i] > maxCount) {
            maxCount = counts[i];
            maxIndex = i;
        }
    }

    delete[] counts;
    return uniqueLabels[maxIndex];
}

DTNode *DecisionTree::buildTree(float **data, char **labels, int dataSize, int depth) {
    DTNode *node = new DTNode();

    if (dataSize < minSamplesSplit || depth >= maxDepth || isHomogeneous(labels, dataSize)) {
        node->isLeaf = true;
        char *majorityLabel = getMajorityLabel(labels, dataSize);
        if (majorityLabel != nullptr) {
            strncpy(node->label, majorityLabel, 19);
            node->label[19] = '\0';
        }
        return node;
    }

    float splitValue;
    int featureIndex = findBestSplit(data, labels, dataSize, &splitValue);

    if (featureIndex == -1) {
        node->isLeaf = true;
        char *majorityLabel = getMajorityLabel(labels, dataSize);
        if (majorityLabel != nullptr) {
            strncpy(node->label, majorityLabel, 19);
            node->label[19] = '\0';
        }
        return node;
    }

    node->featureIndex = featureIndex;
    node->splitValue = splitValue;

    float **leftData, **rightData;
    char **leftLabels, **rightLabels;
    int leftSize, rightSize;

    splitData(data, labels, dataSize, featureIndex, splitValue,
              &leftData, &leftLabels, leftSize,
              &rightData, &rightLabels, rightSize);

    if (leftSize > 0) {
        node->left = buildTree(leftData, leftLabels, leftSize, depth + 1);
    } else {
        node->left = new DTNode();
        node->left->isLeaf = true;
        char *majorityLabel = getMajorityLabel(labels, dataSize);
        if (majorityLabel != nullptr) {
            strncpy(node->left->label, majorityLabel, 19);
            node->left->label[19] = '\0';
        }
    }

    if (rightSize > 0) {
        node->right = buildTree(rightData, rightLabels, rightSize, depth + 1);
    } else {
        node->right = new DTNode();
        node->right->isLeaf = true;
        char *majorityLabel = getMajorityLabel(labels, dataSize);
        if (majorityLabel != nullptr) {
            strncpy(node->right->label, majorityLabel, 19);
            node->right->label[19] = '\0';
        }
    }

    for (int i = 0; i < leftSize; i++) {
        delete[] leftData[i];
        delete[] leftLabels[i];
    }
    delete[] leftData;
    delete[] leftLabels;

    for (int i = 0; i < rightSize; i++) {
        delete[] rightData[i];
        delete[] rightLabels[i];
    }
    delete[] rightData;
    delete[] rightLabels;

    return node;
}

void DecisionTree::pruneTree(DTNode *node) {
    if (node == nullptr || node->isLeaf) {
        return;
    }

    pruneTree(node->left);
    pruneTree(node->right);

    if (node->left->isLeaf && node->right->isLeaf) {
        float **data = nullptr;
        char **labels = nullptr;
        int dataSize = 0;

        if (currentDataSize > 0) {
            char majorityLabel[20];
            strncpy(majorityLabel, getMajorityLabel(trainingLabels, currentDataSize), 19);
            majorityLabel[19] = '\0';

            int correctBefore = 0;
            int correctAfter = 0;

            for (int i = 0; i < currentDataSize; i++) {
                DTNode *current = node;
                bool reachesThisNode = true;
                DTNode *parent = nullptr;

                while (!current->isLeaf && current != node) {
                    parent = current;
                    if (trainingData[i][current->featureIndex] <= current->splitValue) {
                        current = current->left;
                    } else {
                        current = current->right;
                    }
                }

                if (reachesThisNode) {
                    const char *prediction = predict(trainingData[i]);
                    if (strcmp(prediction, trainingLabels[i]) == 0) {
                        correctBefore++;
                    }

                    if (strcmp(majorityLabel, trainingLabels[i]) == 0) {
                        correctAfter++;
                    }
                }
            }

            if (correctAfter >= correctBefore) {
                delete node->left;
                delete node->right;
                node->left = nullptr;
                node->right = nullptr;
                node->isLeaf = true;
                strncpy(node->label, majorityLabel, 19);
                node->label[19] = '\0';
            }
        }
    }
}

void DecisionTree::freeNode(DTNode *node) {
    if (node == nullptr) {
        return;
    }

    freeNode(node->left);
    freeNode(node->right);

    delete node;
}

#ifdef ESP32

bool DecisionTree::saveModel(const char *filename) {
    if (errorState || filename == nullptr || rootNode == nullptr) return false;

    File file = SPIFFS.open(filename, "w");
    if (!file) {
        errorState = true;
        strncpy(errorMessage, "Failed to open file for writing", 49);
        errorMessage[49] = '\0';
        return false;
    }

    file.write((uint8_t *) &maxFeatures, sizeof(int));
    file.write((uint8_t *) &maxDepth, sizeof(int));
    file.write((uint8_t *) &minSamplesSplit, sizeof(int));
    file.write((uint8_t *) &criterion, sizeof(SplitCriterion));
    file.write((uint8_t *) &uniqueLabelCount, sizeof(int));

    file.write((uint8_t *) featureTypes, maxFeatures * sizeof(FeatureType));
    file.write((uint8_t *) featureMin, maxFeatures * sizeof(float));
    file.write((uint8_t *) featureMax, maxFeatures * sizeof(float));

    for (int i = 0; i < uniqueLabelCount; i++) {
        size_t len = strlen(uniqueLabels[i]) + 1;
        file.write((uint8_t *) &len, sizeof(size_t));
        file.write((uint8_t *) uniqueLabels[i], len);
    }

    file.close();
    return true;
}

bool DecisionTree::loadModel(const char *filename) {
    if (filename == nullptr) return false;

    File file = SPIFFS.open(filename, "r");
    if (!file) {
        errorState = true;
        strncpy(errorMessage, "Failed to open file for reading", 49);
        errorMessage[49] = '\0';
        return false;
    }

    delete rootNode;
    rootNode = nullptr;

    int storedMaxFeatures, storedMaxDepth, storedMinSamplesSplit;
    file.read((uint8_t *) &storedMaxFeatures, sizeof(int));
    file.read((uint8_t *) &storedMaxDepth, sizeof(int));
    file.read((uint8_t *) &storedMinSamplesSplit, sizeof(int));

    if (storedMaxFeatures != maxFeatures) {
        errorState = true;
        strncpy(errorMessage, "Incompatible model dimensions", 49);
        errorMessage[49] = '\0';
        file.close();
        return false;
    }

    maxDepth = storedMaxDepth;
    minSamplesSplit = storedMinSamplesSplit;

    file.read((uint8_t *) &criterion, sizeof(SplitCriterion));
    file.read((uint8_t *) &uniqueLabelCount, sizeof(int));

    file.read((uint8_t *) featureTypes, maxFeatures * sizeof(FeatureType));
    file.read((uint8_t *) featureMin, maxFeatures * sizeof(float));
    file.read((uint8_t *) featureMax, maxFeatures * sizeof(float));

    if (uniqueLabels != nullptr) {
        for (int i = 0; i < uniqueLabelCount; i++) {
            delete[] uniqueLabels[i];
        }
        delete[] uniqueLabels;
    }

    uniqueLabels = new char *[uniqueLabelCount];
    for (int i = 0; i < uniqueLabelCount; i++) {
        size_t len;
        file.read((uint8_t *) &len, sizeof(size_t));

        uniqueLabels[i] = new char[len];
        file.read((uint8_t *) uniqueLabels[i], len);
    }

    file.close();
    return true;
}

#endif
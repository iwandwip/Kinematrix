/*
 *  DecisionTree.h
 *
 *  Decision Tree library for embedded systems
 *  Created on: 2025. 3. 29
 */

#pragma once

#ifndef DECISION_TREE_LIB_H
#define DECISION_TREE_LIB_H

#pragma message("[COMPILED]: DecisionTree.h")

#include "Arduino.h"
#ifdef ESP32
#include "SPIFFS.h"
#endif

enum SplitCriterion {
    GINI,
    INFORMATION_GAIN
};

enum FeatureType {
    NUMERIC,
    CATEGORICAL
};

// Forward declaration for Node
class DTNode;

class DecisionTree {
private:
    int maxFeatures;
    int maxData;
    int maxDepth;
    int minSamplesSplit;
    int currentDataSize;

    float **trainingData;
    char **trainingLabels;

    FeatureType *featureTypes;
    SplitCriterion criterion;
    bool debugMode;
    DTNode *rootNode;

    char **uniqueLabels;
    int uniqueLabelCount;
    float *featureMin;
    float *featureMax;

    bool errorState;
    char errorMessage[50];

    // Private methods for building the tree
    DTNode *buildTree(float **data, char **labels, int dataSize, int depth);
    int findBestSplit(float **data, char **labels, int dataSize, float *splitValue);
    float calculateGiniImpurity(char **labels, int dataSize);
    float calculateInformationGain(char **labels, int dataSize);
    void splitData(float **data, char **labels, int dataSize, int featureIndex, float splitValue,
                   float ***leftData, char ***leftLabels, int &leftSize,
                   float ***rightData, char ***rightLabels, int &rightSize);
    bool isHomogeneous(char **labels, int dataSize);
    char *getMajorityLabel(char **labels, int dataSize);
    void pruneTree(DTNode *node);
    void findUniqueLabelValues();
    void calculateFeatureRanges();
    void freeNode(DTNode *node);

public:
    DecisionTree(int maxFeatures, int maxData, int maxDepth = 10, int minSamplesSplit = 2);
    ~DecisionTree();

    // Data management methods
    bool addTrainingData(const char *label, const float features[]);
    void setFeatureType(int featureIndex, FeatureType type);
    void clearTrainingData();
    bool removeTrainingData(int index);
    int getDataCount() const;
    int getDataCountByLabel(const char *label) const;

    // Training and prediction methods
    bool train(SplitCriterion criterion = GINI);
    const char *predict(const float dataPoint[]);

    // Debug and evaluation methods
    void setDebugMode(bool enable);
    void printTree();
    float evaluateAccuracy(const float **testFeatures, const char **testLabels, int testCount);
    float crossValidate(int folds);
    float getPredictionConfidence(const float dataPoint[]);

    // Error handling
    bool hasError() const;
    const char *getErrorMessage() const;
    void clearError();

#ifdef ESP32
    bool saveModel(const char *filename);
    bool loadModel(const char *filename);
#endif
};

// Node class for the decision tree
class DTNode {
public:
    bool isLeaf;
    int featureIndex;
    float splitValue;
    char label[20];
    DTNode *left;
    DTNode *right;

    DTNode() : isLeaf(false), featureIndex(-1), splitValue(0.0f), left(nullptr), right(nullptr) {
        label[0] = '\0';
    }

    ~DTNode() {
        delete left;
        delete right;
    }
};

#endif
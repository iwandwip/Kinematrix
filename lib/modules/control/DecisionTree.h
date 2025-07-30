/*
 *  DecisionTree.h
 *
 *  Flexible Decision Tree library for embedded systems
 *  Supports mixed input types (numeric + categorical) and mixed outputs (classification + regression)
 *  Created on: 2025. 3. 29
 */

#pragma once

#ifndef DECISION_TREE_H
#define DECISION_TREE_H

#pragma message("[COMPILED]: DecisionTree.h")

#include "Arduino.h"
#ifdef ESP32
#include "SPIFFS.h"
#endif

enum SplitCriterion {
    GINI,                    // For classification
    ENTROPY,                 // For classification 
    MSE,                     // For regression
    MAE,                     // Mean Absolute Error for regression
    MIXED_CRITERION          // Automatically choose based on target type
};

enum FeatureType {
    NUMERIC,                 // Continuous or discrete numeric values
    CATEGORICAL,             // Nominal discrete values
    ORDINAL,                 // Ordered categorical values
    BINARY                   // Binary values (true/false)
};

enum TreeType {
    CLASSIFICATION,          // Classification tree
    REGRESSION,              // Regression tree
    MIXED                    // Mixed classification and regression
};

enum PruningMethod {
    NO_PRUNING,
    COST_COMPLEXITY,         // CART α-pruning
    REDUCED_ERROR
};

// Forward declarations
class DTNode;

// Flexible feature value container
class FeatureValue {
public:
    FeatureType type;
    float numericValue;      // For numeric features
    char categoricalValue[32]; // For categorical features
    int ordinalValue;        // For ordinal features
    bool binaryValue;        // For binary features

    FeatureValue() : type(NUMERIC), numericValue(0.0f), ordinalValue(0), binaryValue(false) {
        categoricalValue[0] = '\0';
    }

    FeatureValue(float value) : type(NUMERIC), numericValue(value), ordinalValue(0), binaryValue(false) {
        categoricalValue[0] = '\0';
    }

    FeatureValue(const char *value) : type(CATEGORICAL), numericValue(0.0f), ordinalValue(0), binaryValue(false) {
        strncpy(categoricalValue, value, 31);
        categoricalValue[31] = '\0';
    }

    FeatureValue(int value, bool isOrdinal = false) {
        if (isOrdinal) {
            type = ORDINAL;
            ordinalValue = value;
            numericValue = 0.0f;
            binaryValue = false;
            categoricalValue[0] = '\0';
        } else {
            type = BINARY;
            binaryValue = (value != 0);
            ordinalValue = 0;
            numericValue = 0.0f;
            categoricalValue[0] = '\0';
        }
    }

    FeatureValue(bool value) : type(BINARY), binaryValue(value), numericValue(0.0f), ordinalValue(0) {
        categoricalValue[0] = '\0';
    }

    // Comparison operators for splitting
    bool operator<=(const FeatureValue &other) const;
    bool operator==(const FeatureValue &other) const;
    bool operator!=(const FeatureValue &other) const;
};

// Flexible target value container
class TargetValue {
public:
    bool isClassification;   // true for class labels, false for regression
    char classLabel[32];     // For classification
    float regressionValue;   // For regression

    TargetValue() : isClassification(true), regressionValue(0.0f) {
        classLabel[0] = '\0';
    }

    TargetValue(const char *label) : isClassification(true), regressionValue(0.0f) {
        strncpy(classLabel, label, 31);
        classLabel[31] = '\0';
    }

    TargetValue(float value) : isClassification(false), regressionValue(value) {
        classLabel[0] = '\0';
    }
};

// Training sample container
struct TrainingSample {
    FeatureValue *features;  // Array of feature values
    TargetValue target;      // Target value
    int sampleId;           // Unique identifier

    TrainingSample(int numFeatures) : sampleId(0) {
        features = new FeatureValue[numFeatures];
    }

    ~TrainingSample() {
        delete[] features;
    }
};

// Enhanced Node class for flexible decision tree
class DTNode {
public:
    bool isLeaf;
    int featureIndex;        // Index of feature used for split
    FeatureValue splitValue; // Split threshold/value
    TargetValue prediction;  // Prediction for leaf nodes

    DTNode *left;            // Left child (≤ splitValue for numeric, == splitValue for categorical)
    DTNode *right;           // Right child (> splitValue for numeric, != splitValue for categorical)
    DTNode **children;       // For multi-way categorical splits
    int numChildren;         // Number of children for categorical splits

    // Node statistics
    int sampleCount;         // Number of samples in this node
    float impurity;          // Node impurity (Gini, Entropy, MSE, etc.)
    float *classProbabilities; // Class probability distribution (for classification)
    char **classLabels;      // Unique class labels in this node
    int numClasses;          // Number of unique classes
    float regressionMean;    // Mean regression value in this node
    float regressionVariance; // Variance of regression values

    // Categorical split metadata
    char **categoricalValues; // Values for categorical multi-way split
    int numCategoricalValues;

    DTNode() : isLeaf(false), featureIndex(-1), left(nullptr), right(nullptr),
               children(nullptr), numChildren(0), sampleCount(0), impurity(0.0f),
               classProbabilities(nullptr), classLabels(nullptr), numClasses(0),
               regressionMean(0.0f), regressionVariance(0.0f),
               categoricalValues(nullptr), numCategoricalValues(0) {}

    ~DTNode() {
        delete left;
        delete right;

        if (children != nullptr) {
            for (int i = 0; i < numChildren; i++) {
                delete children[i];
            }
            delete[] children;
        }

        delete[] classProbabilities;

        if (classLabels != nullptr) {
            for (int i = 0; i < numClasses; i++) {
                delete[] classLabels[i];
            }
            delete[] classLabels;
        }

        if (categoricalValues != nullptr) {
            for (int i = 0; i < numCategoricalValues; i++) {
                delete[] categoricalValues[i];
            }
            delete[] categoricalValues;
        }
    }
};

// Main Decision Tree class
class DecisionTree {
private:
    int maxFeatures;
    int maxSamples;
    int maxDepth;
    int minSamplesSplit;
    int minSamplesLeaf;
    int currentSampleCount;

    TrainingSample **samples;    // Training samples
    FeatureType *featureTypes;   // Type of each feature
    char **featureNames;         // Names of features
    SplitCriterion criterion;    // Split criterion
    TreeType treeType;           // Tree type
    PruningMethod pruningMethod; // Pruning method
    bool debugMode;
    DTNode *rootNode;

    // Feature metadata
    char ***categoricalFeatureValues;  // Possible values for categorical features
    int *numCategoricalValues;         // Number of values per categorical feature
    float *featureImportance;          // Feature importance scores

    // Target metadata
    char **uniqueClassLabels;          // Unique class labels
    int numUniqueClasses;              // Number of unique classes
    float minRegressionValue;          // Min regression target
    float maxRegressionValue;          // Max regression target

    bool errorState;
    char errorMessage[128];

    // Core tree building methods
    DTNode *buildTree(TrainingSample **samples, int numSamples, int depth);
    int findBestSplit(TrainingSample **samples, int numSamples, FeatureValue &bestSplitValue);

    // Impurity calculations
    float calculateImpurity(TrainingSample **samples, int numSamples);
    float calculateGiniImpurity(TrainingSample **samples, int numSamples);
    float calculateEntropy(TrainingSample **samples, int numSamples);
    float calculateMSE(TrainingSample **samples, int numSamples);
    float calculateMAE(TrainingSample **samples, int numSamples);

    // Splitting methods
    void splitSamples(TrainingSample **samples, int numSamples, int featureIndex,
                      const FeatureValue &splitValue, TrainingSample ***leftSamples,
                      int &leftCount, TrainingSample ***rightSamples, int &rightCount);
    void splitSamplesMultiway(TrainingSample **samples, int numSamples, int featureIndex,
                              TrainingSample ****childSamples, int *&childCounts, int &numChildren);

    // Utility methods
    bool isHomogeneous(TrainingSample **samples, int numSamples);
    TargetValue getMajorityTarget(TrainingSample **samples, int numSamples);
    void calculateNodeStatistics(DTNode *node, TrainingSample **samples, int numSamples);
    void calculateFeatureImportance();
    void calculateFeatureImportanceRecursive(DTNode *node);
    void analyzeDataTypes();

    // Pruning methods
    void performPruning();
    void costComplexityPruning(DTNode *node, float alpha);
    float calculateNodeComplexity(DTNode *node);

    // Memory management
    void freeNode(DTNode *node);
    void freeSamples();

    // Visualization helpers
    void printNodeRecursive(DTNode *node, String prefix, bool isLast, int depth, String branchLabel = "");
    int countNodes(DTNode *node);
    int countLeafNodes(DTNode *node);
    int getTreeDepth(DTNode *node);

public:
    DecisionTree(int maxFeatures, int maxSamples, int maxDepth = 10, int minSamplesSplit = 2, int minSamplesLeaf = 1);
    ~DecisionTree();

    // Data management methods
    bool addTrainingSample(const FeatureValue features[], const TargetValue &target);
    bool addTrainingSample(const FeatureValue features[], const char *classLabel);
    bool addTrainingSample(const FeatureValue features[], float regressionTarget);

    // Convenience methods for mixed input types
    bool addTrainingSample(const char *classLabel, const float numericFeatures[], const char *categoricalFeatures[]);
    bool addTrainingSample(float regressionTarget, const float numericFeatures[], const char *categoricalFeatures[]);

    // Feature configuration
    void setFeatureType(int featureIndex, FeatureType type);
    void setFeatureName(int featureIndex, const char *name);
    void addCategoricalValue(int featureIndex, const char *value);

    // Training methods
    bool train(SplitCriterion criterion = MIXED_CRITERION, PruningMethod pruning = COST_COMPLEXITY);

    // Prediction methods
    TargetValue predict(const FeatureValue features[]);
    const char *predictClass(const FeatureValue features[]);
    float predictRegression(const FeatureValue features[]);
    float *getClassProbabilities(const FeatureValue features[]);
    float getPredictionConfidence(const FeatureValue features[]);

    // Convenience prediction methods for mixed input types
    TargetValue predict(const float numericFeatures[], const char *categoricalFeatures[]);
    const char *predictClass(const float numericFeatures[], const char *categoricalFeatures[]);
    float predictRegression(const float numericFeatures[], const char *categoricalFeatures[]);

    // Model analysis
    float *getFeatureImportance();
    void printTreeStructure();
    void printTreeStatistics();
    void printModelSummary();

    // Evaluation methods
    float evaluateAccuracy(TrainingSample **testSamples, int testCount);
    float evaluateRegressionError(TrainingSample **testSamples, int testCount);
    float crossValidate(int folds = 5);

    // Configuration methods
    void setDebugMode(bool enable);
    void setPruningMethod(PruningMethod method);
    void clearTrainingData();
    int getSampleCount() const;

    // Error handling
    bool hasError() const;
    const char *getErrorMessage() const;
    void clearError();

#ifdef ESP32
    bool saveModel(const char *filename);
    bool loadModel(const char *filename);
#endif

    // Tree type detection
    TreeType getTreeType() const;
    void setTreeType(TreeType type);
};

#endif
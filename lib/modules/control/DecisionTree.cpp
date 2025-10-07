/*
 *  DecisionTree.cpp
 *
 *  Flexible Decision Tree library for embedded systems
 *  Supports mixed input types (numeric + categorical) and mixed outputs (classification + regression)
 *  Created on: 2025. 3. 29
 */

#include "DecisionTree.h"
#include <math.h>

// FeatureValue comparison operators implementation
bool FeatureValue::operator<=(const FeatureValue &other) const {
    if (type != other.type) return false;

    switch (type) {
        case NUMERIC:
            return numericValue <= other.numericValue;
        case CATEGORICAL:
            return strcmp(categoricalValue, other.categoricalValue) <= 0;
        case ORDINAL:
            return ordinalValue <= other.ordinalValue;
        case BINARY:
            return binaryValue <= other.binaryValue;
        default:
            return false;
    }
}

bool FeatureValue::operator==(const FeatureValue &other) const {
    if (type != other.type) return false;

    switch (type) {
        case NUMERIC:
            return abs(numericValue - other.numericValue) < 0.0001f;
        case CATEGORICAL:
            return strcmp(categoricalValue, other.categoricalValue) == 0;
        case ORDINAL:
            return ordinalValue == other.ordinalValue;
        case BINARY:
            return binaryValue == other.binaryValue;
        default:
            return false;
    }
}

bool FeatureValue::operator!=(const FeatureValue &other) const {
    return !(*this == other);
}

// DecisionTree constructor
DecisionTree::DecisionTree(int maxFeatures, int maxSamples, int maxDepth, int minSamplesSplit, int minSamplesLeaf) :
        maxFeatures(maxFeatures), maxSamples(maxSamples), maxDepth(maxDepth),
        minSamplesSplit(minSamplesSplit), minSamplesLeaf(minSamplesLeaf), currentSampleCount(0),
        criterion(MIXED_CRITERION), treeType(MIXED), pruningMethod(COST_COMPLEXITY),
        debugMode(false), rootNode(nullptr), featureImportance(nullptr),
        uniqueClassLabels(nullptr), numUniqueClasses(0),
        minRegressionValue(0.0f), maxRegressionValue(0.0f), errorState(false) {

    if (maxFeatures <= 0 || maxSamples <= 0 || maxDepth <= 0 || minSamplesSplit < 2 || minSamplesLeaf < 1) {
        errorState = true;
        strncpy(errorMessage, "Invalid parameters", 127);
        errorMessage[127] = '\0';
        return;
    }

    // Allocate training samples
    samples = new TrainingSample *[maxSamples];
    if (samples == nullptr) {
        errorState = true;
        strncpy(errorMessage, "Memory allocation failed for samples", 127);
        errorMessage[127] = '\0';
        return;
    }

    for (int i = 0; i < maxSamples; i++) {
        samples[i] = nullptr;
    }

    // Allocate feature types
    featureTypes = new FeatureType[maxFeatures];
    if (featureTypes == nullptr) {
        delete[] samples;
        errorState = true;
        strncpy(errorMessage, "Memory allocation failed for feature types", 127);
        errorMessage[127] = '\0';
        return;
    }

    // Initialize feature types as numeric by default
    for (int i = 0; i < maxFeatures; i++) {
        featureTypes[i] = NUMERIC;
    }

    // Allocate feature names
    featureNames = new char *[maxFeatures];
    if (featureNames == nullptr) {
        delete[] samples;
        delete[] featureTypes;
        errorState = true;
        strncpy(errorMessage, "Memory allocation failed for feature names", 127);
        errorMessage[127] = '\0';
        return;
    }

    for (int i = 0; i < maxFeatures; i++) {
        featureNames[i] = new char[32];
        if (featureNames[i] != nullptr) {
            snprintf(featureNames[i], 32, "Feature_%d", i);
        }
    }

    // Allocate categorical feature values
    categoricalFeatureValues = new char **[maxFeatures];
    numCategoricalValues = new int[maxFeatures];

    if (categoricalFeatureValues == nullptr || numCategoricalValues == nullptr) {
        // Cleanup on failure
        delete[] samples;
        delete[] featureTypes;
        for (int i = 0; i < maxFeatures; i++) {
            delete[] featureNames[i];
        }
        delete[] featureNames;
        delete[] categoricalFeatureValues;
        delete[] numCategoricalValues;
        errorState = true;
        strncpy(errorMessage, "Memory allocation failed for categorical features", 127);
        errorMessage[127] = '\0';
        return;
    }

    for (int i = 0; i < maxFeatures; i++) {
        categoricalFeatureValues[i] = nullptr;
        numCategoricalValues[i] = 0;
    }

    // Allocate feature importance
    featureImportance = new float[maxFeatures];
    if (featureImportance == nullptr) {
        // Cleanup on failure
        freeSamples();
        errorState = true;
        strncpy(errorMessage, "Memory allocation failed for feature importance", 127);
        errorMessage[127] = '\0';
        return;
    }

    for (int i = 0; i < maxFeatures; i++) {
        featureImportance[i] = 0.0f;
    }
}

DecisionTree::~DecisionTree() {
    freeSamples();
    delete rootNode;
    delete[] featureImportance;

    if (uniqueClassLabels != nullptr) {
        for (int i = 0; i < numUniqueClasses; i++) {
            delete[] uniqueClassLabels[i];
        }
        delete[] uniqueClassLabels;
    }
}

void DecisionTree::freeSamples() {
    if (samples != nullptr) {
        for (int i = 0; i < maxSamples; i++) {
            delete samples[i];
        }
        delete[] samples;
        samples = nullptr;
    }

    delete[] featureTypes;
    featureTypes = nullptr;

    if (featureNames != nullptr) {
        for (int i = 0; i < maxFeatures; i++) {
            delete[] featureNames[i];
        }
        delete[] featureNames;
        featureNames = nullptr;
    }

    if (categoricalFeatureValues != nullptr) {
        for (int i = 0; i < maxFeatures; i++) {
            if (categoricalFeatureValues[i] != nullptr) {
                for (int j = 0; j < numCategoricalValues[i]; j++) {
                    delete[] categoricalFeatureValues[i][j];
                }
                delete[] categoricalFeatureValues[i];
            }
        }
        delete[] categoricalFeatureValues;
        categoricalFeatureValues = nullptr;
    }

    delete[] numCategoricalValues;
    numCategoricalValues = nullptr;
}

bool DecisionTree::addTrainingSample(const FeatureValue features[], const TargetValue &target) {
    if (errorState) return false;

    if (currentSampleCount >= maxSamples) {
        errorState = true;
        strncpy(errorMessage, "Maximum samples exceeded", 127);
        errorMessage[127] = '\0';
        return false;
    }

    if (features == nullptr) {
        errorState = true;
        strncpy(errorMessage, "Null features provided", 127);
        errorMessage[127] = '\0';
        return false;
    }

    // Create new training sample
    TrainingSample *sample = new TrainingSample(maxFeatures);
    if (sample == nullptr) {
        errorState = true;
        strncpy(errorMessage, "Memory allocation failed for sample", 127);
        errorMessage[127] = '\0';
        return false;
    }

    // Copy features
    for (int i = 0; i < maxFeatures; i++) {
        sample->features[i] = features[i];
    }

    // Copy target
    sample->target = target;
    sample->sampleId = currentSampleCount;

    // Store sample
    samples[currentSampleCount] = sample;
    currentSampleCount++;

    return true;
}

bool DecisionTree::addTrainingSample(const FeatureValue features[], const char *classLabel) {
    TargetValue target(classLabel);
    return addTrainingSample(features, target);
}

bool DecisionTree::addTrainingSample(const FeatureValue features[], float regressionTarget) {
    TargetValue target(regressionTarget);
    return addTrainingSample(features, target);
}

bool DecisionTree::addTrainingSample(const char *classLabel, const float numericFeatures[], const char *categoricalFeatures[]) {
    if (numericFeatures == nullptr || classLabel == nullptr) return false;

    FeatureValue *features = new FeatureValue[maxFeatures];

    int numericIndex = 0;
    int categoricalIndex = 0;

    for (int i = 0; i < maxFeatures; i++) {
        switch (featureTypes[i]) {
            case NUMERIC:
                features[i] = FeatureValue(numericFeatures[numericIndex++]);
                break;
            case CATEGORICAL:
                if (categoricalFeatures != nullptr) {
                    features[i] = FeatureValue(categoricalFeatures[categoricalIndex++]);
                }
                break;
            case ORDINAL:
                features[i] = FeatureValue((int) numericFeatures[numericIndex++], true);
                break;
            case BINARY:
                features[i] = FeatureValue(numericFeatures[numericIndex++] != 0.0f);
                break;
        }
    }

    bool result = addTrainingSample(features, classLabel);
    delete[] features;
    return result;
}

bool DecisionTree::addTrainingSample(float regressionTarget, const float numericFeatures[], const char *categoricalFeatures[]) {
    if (numericFeatures == nullptr) return false;

    FeatureValue *features = new FeatureValue[maxFeatures];

    int numericIndex = 0;
    int categoricalIndex = 0;

    for (int i = 0; i < maxFeatures; i++) {
        switch (featureTypes[i]) {
            case NUMERIC:
                features[i] = FeatureValue(numericFeatures[numericIndex++]);
                break;
            case CATEGORICAL:
                if (categoricalFeatures != nullptr) {
                    features[i] = FeatureValue(categoricalFeatures[categoricalIndex++]);
                }
                break;
            case ORDINAL:
                features[i] = FeatureValue((int) numericFeatures[numericIndex++], true);
                break;
            case BINARY:
                features[i] = FeatureValue(numericFeatures[numericIndex++] != 0.0f);
                break;
        }
    }

    bool result = addTrainingSample(features, regressionTarget);
    delete[] features;
    return result;
}

void DecisionTree::setFeatureType(int featureIndex, FeatureType type) {
    if (featureIndex >= 0 && featureIndex < maxFeatures) {
        featureTypes[featureIndex] = type;
    }
}

void DecisionTree::setFeatureName(int featureIndex, const char *name) {
    if (featureIndex >= 0 && featureIndex < maxFeatures && name != nullptr && featureNames[featureIndex] != nullptr) {
        strncpy(featureNames[featureIndex], name, 31);
        featureNames[featureIndex][31] = '\0';
    }
}

void DecisionTree::addCategoricalValue(int featureIndex, const char *value) {
    if (featureIndex < 0 || featureIndex >= maxFeatures || value == nullptr) return;

    // Initialize categorical values array if needed
    if (categoricalFeatureValues[featureIndex] == nullptr) {
        categoricalFeatureValues[featureIndex] = new char *[20]; // Initial capacity
        numCategoricalValues[featureIndex] = 0;
    }

    // Add the new value
    int count = numCategoricalValues[featureIndex];
    if (count < 20) { // Prevent overflow
        categoricalFeatureValues[featureIndex][count] = new char[32];
        strncpy(categoricalFeatureValues[featureIndex][count], value, 31);
        categoricalFeatureValues[featureIndex][count][31] = '\0';
        numCategoricalValues[featureIndex]++;
    }
}

bool DecisionTree::train(SplitCriterion criterion, PruningMethod pruning) {
    if (errorState) return false;

    if (currentSampleCount < minSamplesSplit) {
        errorState = true;
        strncpy(errorMessage, "Not enough training samples", 127);
        errorMessage[127] = '\0';
        return false;
    }

    // Set parameters
    this->criterion = criterion;
    this->pruningMethod = pruning;

    // Analyze data types if criterion is automatic
    if (criterion == MIXED_CRITERION) {
        analyzeDataTypes();
    }

    // Clean up previous tree
    delete rootNode;
    rootNode = nullptr;

    // Build tree
    rootNode = buildTree(samples, currentSampleCount, 0);

    if (rootNode == nullptr) {
        errorState = true;
        strncpy(errorMessage, "Tree building failed", 127);
        errorMessage[127] = '\0';
        return false;
    }

    // Calculate feature importance
    calculateFeatureImportance();

    // Apply pruning if requested
    if (pruning != NO_PRUNING) {
        performPruning();
    }

    return true;
}

void DecisionTree::analyzeDataTypes() {
    if (currentSampleCount == 0) return;

    bool hasClassification = false;
    bool hasRegression = false;

    // Analyze target types
    for (int i = 0; i < currentSampleCount; i++) {
        if (samples[i] != nullptr) {
            if (samples[i]->target.isClassification) {
                hasClassification = true;
            } else {
                hasRegression = true;
            }
        }
    }

    // Set appropriate tree type and criterion
    if (hasClassification && hasRegression) {
        treeType = MIXED;
        criterion = GINI; // Default for mixed data
    } else if (hasClassification) {
        treeType = CLASSIFICATION;
        criterion = GINI;
    } else if (hasRegression) {
        treeType = REGRESSION;
        criterion = MSE;
    }
}

DTNode *DecisionTree::buildTree(TrainingSample **samples, int numSamples, int depth) {
    if (samples == nullptr || numSamples == 0) return nullptr;

    DTNode *node = new DTNode();
    if (node == nullptr) return nullptr;

    node->sampleCount = numSamples;

    // Calculate node statistics
    calculateNodeStatistics(node, samples, numSamples);

    // Check stopping criteria
    if (depth >= maxDepth || numSamples < minSamplesSplit ||
        numSamples < 2 * minSamplesLeaf || isHomogeneous(samples, numSamples)) {

        // Create leaf node
        node->isLeaf = true;
        node->prediction = getMajorityTarget(samples, numSamples);
        return node;
    }

    // Find best split
    FeatureValue bestSplitValue;
    int bestFeatureIndex = findBestSplit(samples, numSamples, bestSplitValue);

    if (bestFeatureIndex == -1) {
        // No valid split found, create leaf
        node->isLeaf = true;
        node->prediction = getMajorityTarget(samples, numSamples);
        return node;
    }

    // Set split parameters
    node->featureIndex = bestFeatureIndex;
    node->splitValue = bestSplitValue;

    // Split samples
    TrainingSample **leftSamples = nullptr;
    TrainingSample **rightSamples = nullptr;
    int leftCount = 0, rightCount = 0;

    splitSamples(samples, numSamples, bestFeatureIndex, bestSplitValue,
                 &leftSamples, leftCount, &rightSamples, rightCount);

    // Check if split is valid
    if (leftCount < minSamplesLeaf || rightCount < minSamplesLeaf) {
        // Invalid split, create leaf
        node->isLeaf = true;
        node->prediction = getMajorityTarget(samples, numSamples);
        delete[] leftSamples;
        delete[] rightSamples;
        return node;
    }

    // Recursively build children
    node->left = buildTree(leftSamples, leftCount, depth + 1);
    node->right = buildTree(rightSamples, rightCount, depth + 1);

    delete[] leftSamples;
    delete[] rightSamples;

    return node;
}

int DecisionTree::findBestSplit(TrainingSample **samples, int numSamples, FeatureValue &bestSplitValue) {
    if (samples == nullptr || numSamples < 2) return -1;

    float bestGain = -1.0f;
    int bestFeatureIndex = -1;
    FeatureValue bestThreshold;

    float parentImpurity = calculateImpurity(samples, numSamples);

    // Try each feature
    for (int featureIndex = 0; featureIndex < maxFeatures; featureIndex++) {
        FeatureType featureType = featureTypes[featureIndex];

        if (featureType == NUMERIC || featureType == ORDINAL) {
            // For numeric/ordinal features, try different thresholds

            // Collect unique values
            float *values = new float[numSamples];
            int uniqueCount = 0;

            for (int i = 0; i < numSamples; i++) {
                float value = (featureType == NUMERIC) ?
                              samples[i]->features[featureIndex].numericValue :
                              (float) samples[i]->features[featureIndex].ordinalValue;

                // Check if value is already in array
                bool found = false;
                for (int j = 0; j < uniqueCount; j++) {
                    if (abs(values[j] - value) < 0.0001f) {
                        found = true;
                        break;
                    }
                }

                if (!found) {
                    values[uniqueCount++] = value;
                }
            }

            // Sort values
            for (int i = 0; i < uniqueCount - 1; i++) {
                for (int j = i + 1; j < uniqueCount; j++) {
                    if (values[i] > values[j]) {
                        float temp = values[i];
                        values[i] = values[j];
                        values[j] = temp;
                    }
                }
            }

            // Try thresholds between consecutive values
            for (int i = 0; i < uniqueCount - 1; i++) {
                float threshold = (values[i] + values[i + 1]) / 2.0f;

                // Split samples using this threshold
                TrainingSample **leftSamples = nullptr;
                TrainingSample **rightSamples = nullptr;
                int leftCount = 0, rightCount = 0;

                FeatureValue splitValue = (featureType == NUMERIC) ?
                                          FeatureValue(threshold) :
                                          FeatureValue((int) threshold, true);

                splitSamples(samples, numSamples, featureIndex, splitValue,
                             &leftSamples, leftCount, &rightSamples, rightCount);

                // Calculate information gain
                if (leftCount >= minSamplesLeaf && rightCount >= minSamplesLeaf) {
                    float leftImpurity = calculateImpurity(leftSamples, leftCount);
                    float rightImpurity = calculateImpurity(rightSamples, rightCount);

                    float weightedImpurity = (leftCount * leftImpurity + rightCount * rightImpurity) / numSamples;
                    float gain = parentImpurity - weightedImpurity;

                    if (gain > bestGain) {
                        bestGain = gain;
                        bestFeatureIndex = featureIndex;
                        bestThreshold = splitValue;
                    }
                }

                delete[] leftSamples;
                delete[] rightSamples;
            }

            delete[] values;

        } else if (featureType == CATEGORICAL || featureType == BINARY) {
            // For categorical features, try each unique value

            // Collect unique categorical values
            char uniqueValues[20][32];
            int uniqueCount = 0;

            for (int i = 0; i < numSamples; i++) {
                const char *value = (featureType == CATEGORICAL) ?
                                    samples[i]->features[featureIndex].categoricalValue :
                                    (samples[i]->features[featureIndex].binaryValue ? "true" : "false");

                // Check if value is already in array
                bool found = false;
                for (int j = 0; j < uniqueCount; j++) {
                    if (strcmp(uniqueValues[j], value) == 0) {
                        found = true;
                        break;
                    }
                }

                if (!found && uniqueCount < 20) {
                    strncpy(uniqueValues[uniqueCount], value, 31);
                    uniqueValues[uniqueCount][31] = '\0';
                    uniqueCount++;
                }
            }

            // Try each unique value as split point
            for (int i = 0; i < uniqueCount; i++) {
                TrainingSample **leftSamples = nullptr;
                TrainingSample **rightSamples = nullptr;
                int leftCount = 0, rightCount = 0;

                FeatureValue splitValue = (featureType == CATEGORICAL) ?
                                          FeatureValue(uniqueValues[i]) :
                                          FeatureValue(strcmp(uniqueValues[i], "true") == 0);

                splitSamples(samples, numSamples, featureIndex, splitValue,
                             &leftSamples, leftCount, &rightSamples, rightCount);

                // Calculate information gain
                if (leftCount >= minSamplesLeaf && rightCount >= minSamplesLeaf) {
                    float leftImpurity = calculateImpurity(leftSamples, leftCount);
                    float rightImpurity = calculateImpurity(rightSamples, rightCount);

                    float weightedImpurity = (leftCount * leftImpurity + rightCount * rightImpurity) / numSamples;
                    float gain = parentImpurity - weightedImpurity;

                    if (gain > bestGain) {
                        bestGain = gain;
                        bestFeatureIndex = featureIndex;
                        bestThreshold = splitValue;
                    }
                }

                delete[] leftSamples;
                delete[] rightSamples;
            }
        }
    }

    if (bestFeatureIndex != -1) {
        bestSplitValue = bestThreshold;
    }

    return bestFeatureIndex;
}

float DecisionTree::calculateImpurity(TrainingSample **samples, int numSamples) {
    if (samples == nullptr || numSamples == 0) return 0.0f;

    switch (criterion) {
        case GINI:
            return calculateGiniImpurity(samples, numSamples);
        case ENTROPY:
            return calculateEntropy(samples, numSamples);
        case MSE:
            return calculateMSE(samples, numSamples);
        case MAE:
            return calculateMAE(samples, numSamples);
        default:
            // Auto-detect based on data type
            bool hasClassification = false;
            bool hasRegression = false;

            for (int i = 0; i < numSamples; i++) {
                if (samples[i]->target.isClassification) {
                    hasClassification = true;
                } else {
                    hasRegression = true;
                }
            }

            if (hasClassification && !hasRegression) {
                return calculateGiniImpurity(samples, numSamples);
            } else if (hasRegression && !hasClassification) {
                return calculateMSE(samples, numSamples);
            } else {
                // Mixed targets - use Gini as default
                return calculateGiniImpurity(samples, numSamples);
            }
    }
}

float DecisionTree::calculateGiniImpurity(TrainingSample **samples, int numSamples) {
    if (samples == nullptr || numSamples == 0) return 0.0f;

    // Count class frequencies
    char classLabels[50][32];
    int classCounts[50];
    int numClasses = 0;

    for (int i = 0; i < numSamples; i++) {
        if (samples[i]->target.isClassification) {
            const char *label = samples[i]->target.classLabel;

            // Find or create class
            int classIndex = -1;
            for (int j = 0; j < numClasses; j++) {
                if (strcmp(classLabels[j], label) == 0) {
                    classIndex = j;
                    break;
                }
            }

            if (classIndex == -1 && numClasses < 50) {
                strncpy(classLabels[numClasses], label, 31);
                classLabels[numClasses][31] = '\0';
                classCounts[numClasses] = 1;
                numClasses++;
            } else if (classIndex != -1) {
                classCounts[classIndex]++;
            }
        }
    }

    // Calculate Gini impurity
    float gini = 1.0f;
    for (int i = 0; i < numClasses; i++) {
        float probability = (float) classCounts[i] / numSamples;
        gini -= probability * probability;
    }

    return gini;
}

float DecisionTree::calculateEntropy(TrainingSample **samples, int numSamples) {
    if (samples == nullptr || numSamples == 0) return 0.0f;

    // Count class frequencies
    char classLabels[50][32];
    int classCounts[50];
    int numClasses = 0;

    for (int i = 0; i < numSamples; i++) {
        if (samples[i]->target.isClassification) {
            const char *label = samples[i]->target.classLabel;

            // Find or create class
            int classIndex = -1;
            for (int j = 0; j < numClasses; j++) {
                if (strcmp(classLabels[j], label) == 0) {
                    classIndex = j;
                    break;
                }
            }

            if (classIndex == -1 && numClasses < 50) {
                strncpy(classLabels[numClasses], label, 31);
                classLabels[numClasses][31] = '\0';
                classCounts[numClasses] = 1;
                numClasses++;
            } else if (classIndex != -1) {
                classCounts[classIndex]++;
            }
        }
    }

    // Calculate entropy
    float entropy = 0.0f;
    for (int i = 0; i < numClasses; i++) {
        if (classCounts[i] > 0) {
            float probability = (float) classCounts[i] / numSamples;
            entropy -= probability * log2(probability);
        }
    }

    return entropy;
}

float DecisionTree::calculateMSE(TrainingSample **samples, int numSamples) {
    if (samples == nullptr || numSamples == 0) return 0.0f;

    // Calculate mean
    float sum = 0.0f;
    int count = 0;

    for (int i = 0; i < numSamples; i++) {
        if (!samples[i]->target.isClassification) {
            sum += samples[i]->target.regressionValue;
            count++;
        }
    }

    if (count == 0) return 0.0f;

    float mean = sum / count;

    // Calculate MSE
    float mse = 0.0f;
    for (int i = 0; i < numSamples; i++) {
        if (!samples[i]->target.isClassification) {
            float diff = samples[i]->target.regressionValue - mean;
            mse += diff * diff;
        }
    }

    return mse / count;
}

float DecisionTree::calculateMAE(TrainingSample **samples, int numSamples) {
    if (samples == nullptr || numSamples == 0) return 0.0f;

    // Calculate median for MAE
    float *values = new float[numSamples];
    int count = 0;

    for (int i = 0; i < numSamples; i++) {
        if (!samples[i]->target.isClassification) {
            values[count++] = samples[i]->target.regressionValue;
        }
    }

    if (count == 0) {
        delete[] values;
        return 0.0f;
    }

    // Sort values to find median
    for (int i = 0; i < count - 1; i++) {
        for (int j = i + 1; j < count; j++) {
            if (values[i] > values[j]) {
                float temp = values[i];
                values[i] = values[j];
                values[j] = temp;
            }
        }
    }

    float median = (count % 2 == 0) ?
                   (values[count / 2 - 1] + values[count / 2]) / 2.0f :
                   values[count / 2];

    // Calculate MAE
    float mae = 0.0f;
    for (int i = 0; i < count; i++) {
        mae += abs(values[i] - median);
    }

    delete[] values;
    return mae / count;
}

void DecisionTree::splitSamples(TrainingSample **samples, int numSamples, int featureIndex,
                                const FeatureValue &splitValue, TrainingSample ***leftSamples,
                                int &leftCount, TrainingSample ***rightSamples, int &rightCount) {

    // Count samples for each side
    leftCount = 0;
    rightCount = 0;

    for (int i = 0; i < numSamples; i++) {
        if (samples[i] != nullptr) {
            bool goLeft = false;

            FeatureValue &feature = samples[i]->features[featureIndex];

            switch (feature.type) {
                case NUMERIC:
                    goLeft = (feature.numericValue <= splitValue.numericValue);
                    break;
                case CATEGORICAL:
                    goLeft = (strcmp(feature.categoricalValue, splitValue.categoricalValue) == 0);
                    break;
                case ORDINAL:
                    goLeft = (feature.ordinalValue <= splitValue.ordinalValue);
                    break;
                case BINARY:
                    goLeft = (feature.binaryValue == splitValue.binaryValue);
                    break;
            }

            if (goLeft) {
                leftCount++;
            } else {
                rightCount++;
            }
        }
    }

    // Allocate arrays
    *leftSamples = new TrainingSample *[leftCount];
    *rightSamples = new TrainingSample *[rightCount];

    // Fill arrays
    int leftIndex = 0, rightIndex = 0;

    for (int i = 0; i < numSamples; i++) {
        if (samples[i] != nullptr) {
            bool goLeft = false;

            FeatureValue &feature = samples[i]->features[featureIndex];

            switch (feature.type) {
                case NUMERIC:
                    goLeft = (feature.numericValue <= splitValue.numericValue);
                    break;
                case CATEGORICAL:
                    goLeft = (strcmp(feature.categoricalValue, splitValue.categoricalValue) == 0);
                    break;
                case ORDINAL:
                    goLeft = (feature.ordinalValue <= splitValue.ordinalValue);
                    break;
                case BINARY:
                    goLeft = (feature.binaryValue == splitValue.binaryValue);
                    break;
            }

            if (goLeft) {
                (*leftSamples)[leftIndex++] = samples[i];
            } else {
                (*rightSamples)[rightIndex++] = samples[i];
            }
        }
    }
}

bool DecisionTree::isHomogeneous(TrainingSample **samples, int numSamples) {
    if (samples == nullptr || numSamples <= 1) return true;

    bool isClassification = samples[0]->target.isClassification;

    if (isClassification) {
        // Check if all class labels are the same
        const char *firstLabel = samples[0]->target.classLabel;
        for (int i = 1; i < numSamples; i++) {
            if (!samples[i]->target.isClassification ||
                strcmp(samples[i]->target.classLabel, firstLabel) != 0) {
                return false;
            }
        }
        return true;
    } else {
        // For regression, check if variance is very small
        float sum = 0.0f;
        for (int i = 0; i < numSamples; i++) {
            if (!samples[i]->target.isClassification) {
                sum += samples[i]->target.regressionValue;
            }
        }
        float mean = sum / numSamples;

        float variance = 0.0f;
        for (int i = 0; i < numSamples; i++) {
            if (!samples[i]->target.isClassification) {
                float diff = samples[i]->target.regressionValue - mean;
                variance += diff * diff;
            }
        }
        variance /= numSamples;

        return variance < 0.001f; // Very small variance threshold
    }
}

TargetValue DecisionTree::getMajorityTarget(TrainingSample **samples, int numSamples) {
    if (samples == nullptr || numSamples == 0) {
        return TargetValue();
    }

    // Check if we have classification or regression samples
    bool hasClassification = false;
    bool hasRegression = false;

    for (int i = 0; i < numSamples; i++) {
        if (samples[i]->target.isClassification) {
            hasClassification = true;
        } else {
            hasRegression = true;
        }
    }

    if (hasClassification) {
        // Find majority class
        char classLabels[50][32];
        int classCounts[50];
        int numClasses = 0;

        for (int i = 0; i < numSamples; i++) {
            if (samples[i]->target.isClassification) {
                const char *label = samples[i]->target.classLabel;

                // Find or create class
                int classIndex = -1;
                for (int j = 0; j < numClasses; j++) {
                    if (strcmp(classLabels[j], label) == 0) {
                        classIndex = j;
                        break;
                    }
                }

                if (classIndex == -1 && numClasses < 50) {
                    strncpy(classLabels[numClasses], label, 31);
                    classLabels[numClasses][31] = '\0';
                    classCounts[numClasses] = 1;
                    numClasses++;
                } else if (classIndex != -1) {
                    classCounts[classIndex]++;
                }
            }
        }

        // Find class with maximum count
        int maxCount = 0;
        int maxIndex = 0;
        for (int i = 0; i < numClasses; i++) {
            if (classCounts[i] > maxCount) {
                maxCount = classCounts[i];
                maxIndex = i;
            }
        }

        return TargetValue(classLabels[maxIndex]);

    } else if (hasRegression) {
        // Calculate mean for regression
        float sum = 0.0f;
        int count = 0;

        for (int i = 0; i < numSamples; i++) {
            if (!samples[i]->target.isClassification) {
                sum += samples[i]->target.regressionValue;
                count++;
            }
        }

        return TargetValue(sum / count);
    }

    return TargetValue();
}

void DecisionTree::calculateNodeStatistics(DTNode *node, TrainingSample **samples, int numSamples) {
    if (node == nullptr || samples == nullptr || numSamples == 0) return;

    node->sampleCount = numSamples;
    node->impurity = calculateImpurity(samples, numSamples);

    // Count classes for classification
    char classLabels[50][32];
    int classCounts[50];
    int numClasses = 0;

    // Calculate regression statistics
    float regressionSum = 0.0f;
    int regressionCount = 0;

    for (int i = 0; i < numSamples; i++) {
        if (samples[i]->target.isClassification) {
            const char *label = samples[i]->target.classLabel;

            // Find or create class
            int classIndex = -1;
            for (int j = 0; j < numClasses; j++) {
                if (strcmp(classLabels[j], label) == 0) {
                    classIndex = j;
                    break;
                }
            }

            if (classIndex == -1 && numClasses < 50) {
                strncpy(classLabels[numClasses], label, 31);
                classLabels[numClasses][31] = '\0';
                classCounts[numClasses] = 1;
                numClasses++;
            } else if (classIndex != -1) {
                classCounts[classIndex]++;
            }
        } else {
            regressionSum += samples[i]->target.regressionValue;
            regressionCount++;
        }
    }

    // Set classification statistics
    if (numClasses > 0) {
        node->numClasses = numClasses;
        node->classLabels = new char *[numClasses];
        node->classProbabilities = new float[numClasses];

        for (int i = 0; i < numClasses; i++) {
            node->classLabels[i] = new char[32];
            strncpy(node->classLabels[i], classLabels[i], 31);
            node->classLabels[i][31] = '\0';
            node->classProbabilities[i] = (float) classCounts[i] / numSamples;
        }
    }

    // Set regression statistics
    if (regressionCount > 0) {
        node->regressionMean = regressionSum / regressionCount;

        // Calculate variance
        float varianceSum = 0.0f;
        for (int i = 0; i < numSamples; i++) {
            if (!samples[i]->target.isClassification) {
                float diff = samples[i]->target.regressionValue - node->regressionMean;
                varianceSum += diff * diff;
            }
        }
        node->regressionVariance = varianceSum / regressionCount;
    }
}

void DecisionTree::calculateFeatureImportance() {
    if (featureImportance == nullptr || rootNode == nullptr) return;

    // Initialize importance scores
    for (int i = 0; i < maxFeatures; i++) {
        featureImportance[i] = 0.0f;
    }

    // Calculate importance recursively
    calculateFeatureImportanceRecursive(rootNode);

    // Normalize importance scores
    float totalImportance = 0.0f;
    for (int i = 0; i < maxFeatures; i++) {
        totalImportance += featureImportance[i];
    }

    if (totalImportance > 0.0f) {
        for (int i = 0; i < maxFeatures; i++) {
            featureImportance[i] /= totalImportance;
        }
    }
}

void DecisionTree::calculateFeatureImportanceRecursive(DTNode *node) {
    if (node == nullptr || node->isLeaf) return;

    // Calculate weighted impurity decrease
    float impurityDecrease = node->impurity;

    if (node->left != nullptr) {
        impurityDecrease -= (float) node->left->sampleCount / node->sampleCount * node->left->impurity;
    }

    if (node->right != nullptr) {
        impurityDecrease -= (float) node->right->sampleCount / node->sampleCount * node->right->impurity;
    }

    // Add to feature importance
    if (node->featureIndex >= 0 && node->featureIndex < maxFeatures) {
        featureImportance[node->featureIndex] += impurityDecrease * node->sampleCount;
    }

    // Recurse to children
    calculateFeatureImportanceRecursive(node->left);
    calculateFeatureImportanceRecursive(node->right);
}

TargetValue DecisionTree::predict(const FeatureValue features[]) {
    if (errorState || rootNode == nullptr || features == nullptr) {
        return TargetValue();
    }

    DTNode *current = rootNode;

    while (!current->isLeaf && current != nullptr) {
        bool goLeft = false;

        FeatureValue &splitValue = current->splitValue;
        FeatureValue &feature = (FeatureValue &) features[current->featureIndex];

        switch (feature.type) {
            case NUMERIC:
                goLeft = (feature.numericValue <= splitValue.numericValue);
                break;
            case CATEGORICAL:
                goLeft = (strcmp(feature.categoricalValue, splitValue.categoricalValue) == 0);
                break;
            case ORDINAL:
                goLeft = (feature.ordinalValue <= splitValue.ordinalValue);
                break;
            case BINARY:
                goLeft = (feature.binaryValue == splitValue.binaryValue);
                break;
        }

        current = goLeft ? current->left : current->right;

        if (current == nullptr) break;
    }

    return (current != nullptr) ? current->prediction : TargetValue();
}

const char *DecisionTree::predictClass(const FeatureValue features[]) {
    TargetValue result = predict(features);
    return result.isClassification ? result.classLabel : "";
}

float DecisionTree::predictRegression(const FeatureValue features[]) {
    TargetValue result = predict(features);
    return result.isClassification ? 0.0f : result.regressionValue;
}

TargetValue DecisionTree::predict(const float numericFeatures[], const char *categoricalFeatures[]) {
    if (numericFeatures == nullptr) return TargetValue();

    FeatureValue *features = new FeatureValue[maxFeatures];

    int numericIndex = 0;
    int categoricalIndex = 0;

    for (int i = 0; i < maxFeatures; i++) {
        switch (featureTypes[i]) {
            case NUMERIC:
                features[i] = FeatureValue(numericFeatures[numericIndex++]);
                break;
            case CATEGORICAL:
                if (categoricalFeatures != nullptr) {
                    features[i] = FeatureValue(categoricalFeatures[categoricalIndex++]);
                }
                break;
            case ORDINAL:
                features[i] = FeatureValue((int) numericFeatures[numericIndex++], true);
                break;
            case BINARY:
                features[i] = FeatureValue(numericFeatures[numericIndex++] != 0.0f);
                break;
        }
    }

    TargetValue result = predict(features);
    delete[] features;
    return result;
}

const char *DecisionTree::predictClass(const float numericFeatures[], const char *categoricalFeatures[]) {
    TargetValue result = predict(numericFeatures, categoricalFeatures);
    return result.isClassification ? result.classLabel : "";
}

float DecisionTree::predictRegression(const float numericFeatures[], const char *categoricalFeatures[]) {
    TargetValue result = predict(numericFeatures, categoricalFeatures);
    return result.isClassification ? 0.0f : result.regressionValue;
}

float *DecisionTree::getFeatureImportance() {
    if (featureImportance == nullptr) return nullptr;

    float *importance = new float[maxFeatures];
    for (int i = 0; i < maxFeatures; i++) {
        importance[i] = featureImportance[i];
    }
    return importance;
}

void DecisionTree::printTreeStructure() {
    if (rootNode == nullptr) {
        Serial.println("Empty tree");
        return;
    }

    Serial.println("\n=== DECISION TREE STRUCTURE ===");
    Serial.println("Root:");
    printNodeRecursive(rootNode, "", true, 0, "");
    Serial.println("==============================\n");
}

void DecisionTree::printTreeStatistics() {
    if (rootNode == nullptr) {
        Serial.println("No tree statistics available");
        return;
    }

    int totalNodes = countNodes(rootNode);
    int leafNodes = countLeafNodes(rootNode);
    int internalNodes = totalNodes - leafNodes;
    int treeDepth = getTreeDepth(rootNode);

    Serial.println("\n=== TREE STATISTICS ===");
    Serial.println("Total Nodes: " + String(totalNodes));
    Serial.println("Leaf Nodes: " + String(leafNodes));
    Serial.println("Internal Nodes: " + String(internalNodes));
    Serial.println("Tree Depth: " + String(treeDepth));
    Serial.println("Training Samples: " + String(currentSampleCount));
    Serial.println("Features: " + String(maxFeatures));

    // Print tree type
    Serial.print("Tree Type: ");
    switch (treeType) {
        case CLASSIFICATION:
            Serial.println("Classification");
            break;
        case REGRESSION:
            Serial.println("Regression");
            break;
        case MIXED:
            Serial.println("Mixed");
            break;
    }

    // Print split criterion
    Serial.print("Split Criterion: ");
    switch (criterion) {
        case GINI:
            Serial.println("Gini");
            break;
        case ENTROPY:
            Serial.println("Entropy");
            break;
        case MSE:
            Serial.println("MSE");
            break;
        case MAE:
            Serial.println("MAE");
            break;
        case MIXED_CRITERION:
            Serial.println("Mixed");
            break;
    }

    Serial.println("=======================\n");
}

void DecisionTree::printNodeRecursive(DTNode *node, String prefix, bool isLast, int depth, String branchLabel) {
    if (node == nullptr) return;

    Serial.print(prefix);
    Serial.print(isLast ? "└── " : "├── ");

    if (branchLabel != "") {
        Serial.print(branchLabel + " → ");
    }

    if (node->isLeaf) {
        Serial.print("🍃 [");
        if (node->prediction.isClassification) {
            Serial.print(node->prediction.classLabel);
        } else {
            Serial.print(node->prediction.regressionValue, 2);
        }
        Serial.print("] (samples=");
        Serial.print(node->sampleCount);
        Serial.println(")");
    } else {
        Serial.print("🌳 ");
        
        String featureName = "";
        if (node->featureIndex == 0) featureName = "pH";
        else if (node->featureIndex == 1) featureName = "TDS";
        else if (node->featureIndex == 2) featureName = "UV";
        else featureName = "Feature" + String(node->featureIndex);
        
        Serial.print(featureName);

        switch (node->splitValue.type) {
            case NUMERIC:
                Serial.print(" <= ");
                Serial.print(node->splitValue.numericValue, 2);
                break;
            case CATEGORICAL:
                Serial.print(" == ");
                Serial.print(node->splitValue.categoricalValue);
                break;
            case ORDINAL:
                Serial.print(" <= ");
                Serial.print(node->splitValue.ordinalValue);
                break;
            case BINARY:
                Serial.print(" == ");
                Serial.print(node->splitValue.binaryValue ? "true" : "false");
                break;
        }

        Serial.print(" (samples=");
        Serial.print(node->sampleCount);
        Serial.print(", impurity=");
        Serial.print(node->impurity, 3);
        Serial.println(")");

        String childPrefix = prefix + (isLast ? "    " : "│   ");

        if (node->left != nullptr && node->right != nullptr) {
            printNodeRecursive(node->left, childPrefix, false, depth + 1, "Ya");
            printNodeRecursive(node->right, childPrefix, true, depth + 1, "Tidak");
        } else if (node->left != nullptr) {
            printNodeRecursive(node->left, childPrefix, true, depth + 1, "Ya");
        } else if (node->right != nullptr) {
            printNodeRecursive(node->right, childPrefix, true, depth + 1, "Tidak");
        }
    }
}

int DecisionTree::countNodes(DTNode *node) {
    if (node == nullptr) return 0;
    return 1 + countNodes(node->left) + countNodes(node->right);
}

int DecisionTree::countLeafNodes(DTNode *node) {
    if (node == nullptr) return 0;
    if (node->isLeaf) return 1;
    return countLeafNodes(node->left) + countLeafNodes(node->right);
}

int DecisionTree::getTreeDepth(DTNode *node) {
    if (node == nullptr || node->isLeaf) return 0;
    int leftDepth = (node->left != nullptr) ? getTreeDepth(node->left) : 0;
    int rightDepth = (node->right != nullptr) ? getTreeDepth(node->right) : 0;
    return 1 + max(leftDepth, rightDepth);
}

void DecisionTree::performPruning() {
    // Simple post-pruning implementation
    if (pruningMethod == COST_COMPLEXITY && rootNode != nullptr) {
        costComplexityPruning(rootNode, 0.01f); // Simple alpha value
    }
}

void DecisionTree::costComplexityPruning(DTNode *node, float alpha) {
    if (node == nullptr || node->isLeaf) return;

    // Recursively prune children first
    costComplexityPruning(node->left, alpha);
    costComplexityPruning(node->right, alpha);

    // Check if this node should be pruned
    float subtreeComplexity = calculateNodeComplexity(node);
    float leafComplexity = node->impurity;

    if (subtreeComplexity + alpha >= leafComplexity) {
        // Prune this subtree
        delete node->left;
        delete node->right;
        node->left = nullptr;
        node->right = nullptr;
        node->isLeaf = true;
    }
}

float DecisionTree::calculateNodeComplexity(DTNode *node) {
    if (node == nullptr) return 0.0f;
    if (node->isLeaf) return node->impurity;

    float leftComplexity = (node->left != nullptr) ? calculateNodeComplexity(node->left) : 0.0f;
    float rightComplexity = (node->right != nullptr) ? calculateNodeComplexity(node->right) : 0.0f;

    return leftComplexity + rightComplexity;
}

// Remaining utility methods
void DecisionTree::setDebugMode(bool enable) {
    debugMode = enable;
}

void DecisionTree::setPruningMethod(PruningMethod method) {
    pruningMethod = method;
}

void DecisionTree::clearTrainingData() {
    for (int i = 0; i < currentSampleCount; i++) {
        delete samples[i];
        samples[i] = nullptr;
    }
    currentSampleCount = 0;

    delete rootNode;
    rootNode = nullptr;

    clearError();
}

int DecisionTree::getSampleCount() const {
    return currentSampleCount;
}

bool DecisionTree::hasError() const {
    return errorState;
}

const char *DecisionTree::getErrorMessage() const {
    return errorMessage;
}

void DecisionTree::clearError() {
    errorState = false;
    errorMessage[0] = '\0';
}

TreeType DecisionTree::getTreeType() const {
    return treeType;
}

void DecisionTree::setTreeType(TreeType type) {
    treeType = type;
}
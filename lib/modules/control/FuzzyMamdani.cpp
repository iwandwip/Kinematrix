/*
 *  FuzzyMamdani.cpp
 *
 *  Fuzzy Mamdani library for embedded systems
 *  Created on: 2025. 3. 30
 */

#include "FuzzyMamdani.h"

FuzzyMamdani::FuzzyMamdani(int maxInputs, int maxOutputs, int maxRules, int maxSetsPerVar) :
        maxInputs(maxInputs), maxOutputs(maxOutputs), maxRules(maxRules), maxSetsPerVar(maxSetsPerVar),
        numInputs(0), numOutputs(0), numRules(0), defuzzMethod(CENTROID), debugMode(false),
        errorState(false) {

    if (maxInputs <= 0 || maxOutputs <= 0 || maxRules <= 0 || maxSetsPerVar <= 0) {
        errorState = true;
        strncpy(errorMessage, "Invalid parameters", 49);
        errorMessage[49] = '\0';
        return;
    }

    inputVars = new FuzzyMamdaniVariable[maxInputs];
    if (inputVars == nullptr) {
        errorState = true;
        strncpy(errorMessage, "Memory allocation failed", 49);
        errorMessage[49] = '\0';
        return;
    }

    outputVars = new FuzzyMamdaniVariable[maxOutputs];
    if (outputVars == nullptr) {
        delete[] inputVars;
        inputVars = nullptr;

        errorState = true;
        strncpy(errorMessage, "Memory allocation failed", 49);
        errorMessage[49] = '\0';
        return;
    }

    rules = new FuzzyMamdaniRule[maxRules];
    if (rules == nullptr) {
        delete[] inputVars;
        delete[] outputVars;
        inputVars = nullptr;
        outputVars = nullptr;

        errorState = true;
        strncpy(errorMessage, "Memory allocation failed", 49);
        errorMessage[49] = '\0';
        return;
    }

    for (int i = 0; i < maxInputs; i++) {
        inputVars[i].numSets = 0;
        inputVars[i].sets = nullptr;
        inputVars[i].name[0] = '\0';
    }

    for (int i = 0; i < maxOutputs; i++) {
        outputVars[i].numSets = 0;
        outputVars[i].sets = nullptr;
        outputVars[i].name[0] = '\0';
    }

    for (int i = 0; i < maxRules; i++) {
        rules[i].antecedentVars = nullptr;
        rules[i].antecedentSets = nullptr;
        rules[i].numAntecedents = 0;
    }
}

FuzzyMamdani::~FuzzyMamdani() {
    for (int i = 0; i < numInputs; i++) {
        delete[] inputVars[i].sets;
    }

    for (int i = 0; i < numOutputs; i++) {
        delete[] outputVars[i].sets;
    }

    for (int i = 0; i < numRules; i++) {
        delete[] rules[i].antecedentVars;
        delete[] rules[i].antecedentSets;
    }

    delete[] inputVars;
    delete[] outputVars;
    delete[] rules;
}

bool FuzzyMamdani::addInputVariable(const char *name, float min, float max) {
    if (errorState) return false;

    if (numInputs >= maxInputs) {
        errorState = true;
        strncpy(errorMessage, "Max inputs exceeded", 49);
        errorMessage[49] = '\0';
        return false;
    }

    if (name == nullptr || min >= max) {
        errorState = true;
        strncpy(errorMessage, "Invalid input variable parameters", 49);
        errorMessage[49] = '\0';
        return false;
    }

    strncpy(inputVars[numInputs].name, name, 19);
    inputVars[numInputs].name[19] = '\0';
    inputVars[numInputs].min = min;
    inputVars[numInputs].max = max;
    inputVars[numInputs].numSets = 0;

    inputVars[numInputs].sets = new FuzzyMamdaniSet[maxSetsPerVar];
    if (inputVars[numInputs].sets == nullptr) {
        errorState = true;
        strncpy(errorMessage, "Memory allocation failed", 49);
        errorMessage[49] = '\0';
        return false;
    }

    numInputs++;
    return true;
}

bool FuzzyMamdani::addOutputVariable(const char *name, float min, float max) {
    if (errorState) return false;

    if (numOutputs >= maxOutputs) {
        errorState = true;
        strncpy(errorMessage, "Max outputs exceeded", 49);
        errorMessage[49] = '\0';
        return false;
    }

    if (name == nullptr || min >= max) {
        errorState = true;
        strncpy(errorMessage, "Invalid output variable parameters", 49);
        errorMessage[49] = '\0';
        return false;
    }

    strncpy(outputVars[numOutputs].name, name, 19);
    outputVars[numOutputs].name[19] = '\0';
    outputVars[numOutputs].min = min;
    outputVars[numOutputs].max = max;
    outputVars[numOutputs].numSets = 0;

    outputVars[numOutputs].sets = new FuzzyMamdaniSet[maxSetsPerVar];
    if (outputVars[numOutputs].sets == nullptr) {
        errorState = true;
        strncpy(errorMessage, "Memory allocation failed", 49);
        errorMessage[49] = '\0';
        return false;
    }

    numOutputs++;
    return true;
}

bool FuzzyMamdani::addFuzzySet(int varIndex, bool isInput, const char *name, FuzzyMamdaniMembershipType type, const float params[]) {
    if (errorState) return false;

    FuzzyMamdaniVariable *vars = isInput ? inputVars : outputVars;
    int numVars = isInput ? numInputs : numOutputs;

    if (varIndex < 0 || varIndex >= numVars) {
        errorState = true;
        strncpy(errorMessage, "Invalid variable index", 49);
        errorMessage[49] = '\0';
        return false;
    }

    if (vars[varIndex].numSets >= maxSetsPerVar) {
        errorState = true;
        strncpy(errorMessage, "Max sets per variable exceeded", 49);
        errorMessage[49] = '\0';
        return false;
    }

    if (name == nullptr || params == nullptr) {
        errorState = true;
        strncpy(errorMessage, "Invalid fuzzy set parameters", 49);
        errorMessage[49] = '\0';
        return false;
    }

    int setIndex = vars[varIndex].numSets;
    strncpy(vars[varIndex].sets[setIndex].name, name, 19);
    vars[varIndex].sets[setIndex].name[19] = '\0';
    vars[varIndex].sets[setIndex].type = type;

    switch (type) {
        case TRIANGULAR:
            if (params[0] > params[1] || params[1] > params[2]) {
                errorState = true;
                strncpy(errorMessage, "Invalid triangular parameters", 49);
                errorMessage[49] = '\0';
                return false;
            }
            vars[varIndex].sets[setIndex].params[0] = params[0];
            vars[varIndex].sets[setIndex].params[1] = params[1];
            vars[varIndex].sets[setIndex].params[2] = params[2];
            break;

        case TRAPEZOIDAL:
            if (params[0] > params[1] || params[1] > params[2] || params[2] > params[3]) {
                errorState = true;
                strncpy(errorMessage, "Invalid trapezoidal parameters", 49);
                errorMessage[49] = '\0';
                return false;
            }
            vars[varIndex].sets[setIndex].params[0] = params[0];
            vars[varIndex].sets[setIndex].params[1] = params[1];
            vars[varIndex].sets[setIndex].params[2] = params[2];
            vars[varIndex].sets[setIndex].params[3] = params[3];
            break;

        case GAUSSIAN:
            if (params[1] <= 0) {
                errorState = true;
                strncpy(errorMessage, "Invalid gaussian parameters", 49);
                errorMessage[49] = '\0';
                return false;
            }
            vars[varIndex].sets[setIndex].params[0] = params[0]; // Mean
            vars[varIndex].sets[setIndex].params[1] = params[1]; // Sigma
            break;

        case SINGLETON:
            vars[varIndex].sets[setIndex].params[0] = params[0]; // Center
            break;
    }

    vars[varIndex].numSets++;
    return true;
}

bool FuzzyMamdani::addRule(int *antecedentVars, int *antecedentSets, int numAntecedents,
                           int consequentVar, int consequentSet, bool useAND) {
    if (errorState) return false;

    if (numRules >= maxRules) {
        errorState = true;
        strncpy(errorMessage, "Max rules exceeded", 49);
        errorMessage[49] = '\0';
        return false;
    }

    if (numAntecedents <= 0 || antecedentVars == nullptr || antecedentSets == nullptr) {
        errorState = true;
        strncpy(errorMessage, "Invalid rule parameters", 49);
        errorMessage[49] = '\0';
        return false;
    }

    if (consequentVar < 0 || consequentVar >= numOutputs) {
        errorState = true;
        strncpy(errorMessage, "Invalid consequent variable", 49);
        errorMessage[49] = '\0';
        return false;
    }

    if (consequentSet < 0 || consequentSet >= outputVars[consequentVar].numSets) {
        errorState = true;
        strncpy(errorMessage, "Invalid consequent set", 49);
        errorMessage[49] = '\0';
        return false;
    }

    for (int i = 0; i < numAntecedents; i++) {
        if (antecedentVars[i] < 0 || antecedentVars[i] >= numInputs) {
            errorState = true;
            strncpy(errorMessage, "Invalid antecedent variable", 49);
            errorMessage[49] = '\0';
            return false;
        }

        if (antecedentSets[i] < 0 || antecedentSets[i] >= inputVars[antecedentVars[i]].numSets) {
            errorState = true;
            strncpy(errorMessage, "Invalid antecedent set", 49);
            errorMessage[49] = '\0';
            return false;
        }
    }

    rules[numRules].antecedentVars = new int[numAntecedents];
    rules[numRules].antecedentSets = new int[numAntecedents];

    if (rules[numRules].antecedentVars == nullptr || rules[numRules].antecedentSets == nullptr) {
        delete[] rules[numRules].antecedentVars;
        delete[] rules[numRules].antecedentSets;
        rules[numRules].antecedentVars = nullptr;
        rules[numRules].antecedentSets = nullptr;

        errorState = true;
        strncpy(errorMessage, "Memory allocation failed", 49);
        errorMessage[49] = '\0';
        return false;
    }

    for (int i = 0; i < numAntecedents; i++) {
        rules[numRules].antecedentVars[i] = antecedentVars[i];
        rules[numRules].antecedentSets[i] = antecedentSets[i];
    }

    rules[numRules].numAntecedents = numAntecedents;
    rules[numRules].consequentVar = consequentVar;
    rules[numRules].consequentSet = consequentSet;
    rules[numRules].useAND = useAND;

    numRules++;
    return true;
}

float *FuzzyMamdani::evaluate(const float *inputs) {
    if (errorState) return nullptr;

    if (inputs == nullptr) {
        errorState = true;
        strncpy(errorMessage, "Null input provided", 49);
        errorMessage[49] = '\0';
        return nullptr;
    }

    if (numInputs == 0 || numOutputs == 0 || numRules == 0) {
        errorState = true;
        strncpy(errorMessage, "Fuzzy system not fully defined", 49);
        errorMessage[49] = '\0';
        return nullptr;
    }

    int *activatedRules = new int[numRules];
    for (int i = 0; i < numRules; i++) {
        activatedRules[i] = 0;
    }

    float *ruleStrengths = evaluateRules(inputs, activatedRules);
    if (ruleStrengths == nullptr) {
        delete[] activatedRules;
        return nullptr;
    }

    float *outputs = new float[numOutputs];
    if (outputs == nullptr) {
        delete[] ruleStrengths;
        delete[] activatedRules;

        errorState = true;
        strncpy(errorMessage, "Memory allocation failed", 49);
        errorMessage[49] = '\0';
        return nullptr;
    }

    for (int i = 0; i < numOutputs; i++) {
        outputs[i] = 0.0f;

        bool hasRule = false;
        for (int j = 0; j < numRules; j++) {
            if (rules[j].consequentVar == i && activatedRules[j] > 0) {
                hasRule = true;
                break;
            }
        }

        if (hasRule) {
            switch (defuzzMethod) {
                case CENTROID:
                    outputs[i] = defuzzifyCentroid(i, ruleStrengths);
                    break;
                case BISECTOR:
                    outputs[i] = defuzzifyBisector(i, ruleStrengths);
                    break;
                case MOM:
                    outputs[i] = defuzzifyMOM(i, ruleStrengths);
                    break;
                case SOM:
                    outputs[i] = defuzzifySOM(i, ruleStrengths);
                    break;
                case LOM:
                    outputs[i] = defuzzifyLOM(i, ruleStrengths);
                    break;
            }
        } else {
            outputs[i] = (outputVars[i].min + outputVars[i].max) / 2.0f;
        }

        if (outputs[i] < outputVars[i].min) outputs[i] = outputVars[i].min;
        if (outputs[i] > outputVars[i].max) outputs[i] = outputVars[i].max;
    }

    delete[] ruleStrengths;
    delete[] activatedRules;

    return outputs;
}

float *FuzzyMamdani::evaluateRules(const float *inputs, int *activatedRules) {
    float *ruleStrengths = new float[numRules];
    if (ruleStrengths == nullptr) {
        return nullptr;
    }

    for (int i = 0; i < numRules; i++) {
        const FuzzyMamdaniRule &rule = rules[i];

        if (rule.numAntecedents > 0) {
            int varIndex = rule.antecedentVars[0];
            int setIndex = rule.antecedentSets[0];

            float value = inputs[varIndex];
            const FuzzyMamdaniSet &set = inputVars[varIndex].sets[setIndex];

            float strength = calculateMembership(value, set);

            for (int j = 1; j < rule.numAntecedents; j++) {
                varIndex = rule.antecedentVars[j];
                setIndex = rule.antecedentSets[j];

                value = inputs[varIndex];
                const FuzzyMamdaniSet &nextSet = inputVars[varIndex].sets[setIndex];

                float membershipDegree = calculateMembership(value, nextSet);
                strength = applyFuzzyOperator(strength, membershipDegree, rule.useAND);
            }

            ruleStrengths[i] = strength;
            activatedRules[i] = (strength > 0.0f) ? 1 : 0;
        } else {
            ruleStrengths[i] = 0.0f;
            activatedRules[i] = 0;
        }
    }

    return ruleStrengths;
}

float FuzzyMamdani::applyFuzzyOperator(float a, float b, bool useAND) const {
    if (useAND) {
        return (a < b) ? a : b;  // min for AND
    } else {
        return (a > b) ? a : b;  // max for OR
    }
}

float FuzzyMamdani::calculateMembership(float value, const FuzzyMamdaniSet &set) const {
    switch (set.type) {
        case TRIANGULAR:
            return calculateTriangularMembership(value, set.params[0], set.params[1], set.params[2]);
        case TRAPEZOIDAL:
            return calculateTrapezoidalMembership(value, set.params[0], set.params[1], set.params[2], set.params[3]);
        case GAUSSIAN:
            return calculateGaussianMembership(value, set.params[0], set.params[1]);
        case SINGLETON:
            return calculateSingletonMembership(value, set.params[0]);
        default:
            return 0.0f;
    }
}

float FuzzyMamdani::calculateTriangularMembership(float value, float a, float b, float c) const {
    if (value == a && a == b) return 1.0f;  // Left shoulder case
    if (value == c && b == c) return 1.0f;  // Right shoulder case
    if (value == b) return 1.0f;
    if (value <= a || value >= c) {
        return 0.0f;
    } else if (value < b) {
        return (value - a) / (b - a);
    } else {
        return (c - value) / (c - b);
    }
}

float FuzzyMamdani::calculateTrapezoidalMembership(float value, float a, float b, float c, float d) const {
    if (value == d) {
        if (c == d) {
            return 1.0f;
        }
        return 0.0f;
    }
    if (value < a || value > d) {
        return 0.0f;
    } else if (value < b) {
        return (value - a) / (b - a);
    } else if (value <= c) {
        return 1.0f;
    } else {
        return (d - value) / (d - c);
    }
}

float FuzzyMamdani::calculateGaussianMembership(float value, float mean, float sigma) const {
    float diff = value - mean;
    return exp(-(diff * diff) / (2 * sigma * sigma));
}

float FuzzyMamdani::calculateSingletonMembership(float value, float center) const {
    return (abs(value - center) < 0.0001f) ? 1.0f : 0.0f;
}

float FuzzyMamdani::defuzzifyCentroid(int outputIndex, const float *ruleStrengths) const {
    const int RESOLUTION = 100;  // Discretization resolution

    const FuzzyMamdaniVariable &outVar = outputVars[outputIndex];
    float min = outVar.min;
    float max = outVar.max;
    float step = (max - min) / RESOLUTION;

    float weightedSum = 0.0f;
    float membershipSum = 0.0f;

    for (float x = min; x <= max; x += step) {
        float membership = 0.0f;

        for (int i = 0; i < numRules; i++) {
            if (rules[i].consequentVar == outputIndex) {
                int setIndex = rules[i].consequentSet;
                const FuzzyMamdaniSet &set = outVar.sets[setIndex];

                float setMembership = calculateMembership(x, set);
                float ruleMembership = (setMembership < ruleStrengths[i]) ? setMembership : ruleStrengths[i];

                if (ruleMembership > membership) {
                    membership = ruleMembership;  // max aggregation method
                }
            }
        }

        weightedSum += x * membership;
        membershipSum += membership;
    }

    if (membershipSum < 0.0001f) {
        return (min + max) / 2.0f;  // Default to center if no membership
    }

    return weightedSum / membershipSum;
}

float FuzzyMamdani::defuzzifyMOM(int outputIndex, const float *ruleStrengths) const {
    const int RESOLUTION = 100;

    const FuzzyMamdaniVariable &outVar = outputVars[outputIndex];
    float min = outVar.min;
    float max = outVar.max;
    float step = (max - min) / RESOLUTION;

    float maxMembership = -1.0f;
    float sum = 0.0f;
    int count = 0;

    for (float x = min; x <= max; x += step) {
        float membership = 0.0f;

        for (int i = 0; i < numRules; i++) {
            if (rules[i].consequentVar == outputIndex) {
                int setIndex = rules[i].consequentSet;
                const FuzzyMamdaniSet &set = outVar.sets[setIndex];

                float setMembership = calculateMembership(x, set);
                float ruleMembership = (setMembership < ruleStrengths[i]) ? setMembership : ruleStrengths[i];

                if (ruleMembership > membership) {
                    membership = ruleMembership;
                }
            }
        }

        if (membership > maxMembership) {
            maxMembership = membership;
            sum = x;
            count = 1;
        } else if (abs(membership - maxMembership) < 0.0001f) {
            sum += x;
            count++;
        }
    }

    if (count == 0) {
        return (min + max) / 2.0f;
    }

    return sum / count;
}

float FuzzyMamdani::defuzzifySOM(int outputIndex, const float *ruleStrengths) const {
    const int RESOLUTION = 100;

    const FuzzyMamdaniVariable &outVar = outputVars[outputIndex];
    float min = outVar.min;
    float max = outVar.max;
    float step = (max - min) / RESOLUTION;

    float maxMembership = -1.0f;
    float result = min;

    for (float x = min; x <= max; x += step) {
        float membership = 0.0f;

        for (int i = 0; i < numRules; i++) {
            if (rules[i].consequentVar == outputIndex) {
                int setIndex = rules[i].consequentSet;
                const FuzzyMamdaniSet &set = outVar.sets[setIndex];

                float setMembership = calculateMembership(x, set);
                float ruleMembership = (setMembership < ruleStrengths[i]) ? setMembership : ruleStrengths[i];

                if (ruleMembership > membership) {
                    membership = ruleMembership;
                }
            }
        }

        if (membership > maxMembership) {
            maxMembership = membership;
            result = x;
        }
    }

    return result;
}

float FuzzyMamdani::defuzzifyLOM(int outputIndex, const float *ruleStrengths) const {
    const int RESOLUTION = 100;

    const FuzzyMamdaniVariable &outVar = outputVars[outputIndex];
    float min = outVar.min;
    float max = outVar.max;
    float step = (max - min) / RESOLUTION;

    float maxMembership = -1.0f;
    float result = max;

    for (float x = max; x >= min; x -= step) {
        float membership = 0.0f;

        for (int i = 0; i < numRules; i++) {
            if (rules[i].consequentVar == outputIndex) {
                int setIndex = rules[i].consequentSet;
                const FuzzyMamdaniSet &set = outVar.sets[setIndex];

                float setMembership = calculateMembership(x, set);
                float ruleMembership = (setMembership < ruleStrengths[i]) ? setMembership : ruleStrengths[i];

                if (ruleMembership > membership) {
                    membership = ruleMembership;
                }
            }
        }

        if (membership > maxMembership) {
            maxMembership = membership;
            result = x;
        }
    }

    return result;
}

float FuzzyMamdani::defuzzifyBisector(int outputIndex, const float *ruleStrengths) const {
    const int RESOLUTION = 100;

    const FuzzyMamdaniVariable &outVar = outputVars[outputIndex];
    float min = outVar.min;
    float max = outVar.max;
    float step = (max - min) / RESOLUTION;

    float totalArea = 0.0f;

    for (float x = min; x <= max; x += step) {
        float membership = 0.0f;

        for (int i = 0; i < numRules; i++) {
            if (rules[i].consequentVar == outputIndex) {
                int setIndex = rules[i].consequentSet;
                const FuzzyMamdaniSet &set = outVar.sets[setIndex];

                float setMembership = calculateMembership(x, set);
                float ruleMembership = (setMembership < ruleStrengths[i]) ? setMembership : ruleStrengths[i];

                if (ruleMembership > membership) {
                    membership = ruleMembership;
                }
            }
        }

        totalArea += membership * step;
    }

    float halfArea = totalArea / 2.0f;
    float currentArea = 0.0f;

    for (float x = min; x <= max; x += step) {
        float membership = 0.0f;

        for (int i = 0; i < numRules; i++) {
            if (rules[i].consequentVar == outputIndex) {
                int setIndex = rules[i].consequentSet;
                const FuzzyMamdaniSet &set = outVar.sets[setIndex];

                float setMembership = calculateMembership(x, set);
                float ruleMembership = (setMembership < ruleStrengths[i]) ? setMembership : ruleStrengths[i];

                if (ruleMembership > membership) {
                    membership = ruleMembership;
                }
            }
        }

        currentArea += membership * step;
        if (currentArea >= halfArea) {
            return x;
        }
    }

    return (min + max) / 2.0f;
}

void FuzzyMamdani::setDefuzzificationMethod(FuzzyMamdaniDefuzzificationMethod method) {
    defuzzMethod = method;
}

void FuzzyMamdani::setDebugMode(bool enable) {
    debugMode = enable;
}

void FuzzyMamdani::clearVariables() {
    for (int i = 0; i < numInputs; i++) {
        delete[] inputVars[i].sets;
    }

    for (int i = 0; i < numOutputs; i++) {
        delete[] outputVars[i].sets;
    }

    numInputs = 0;
    numOutputs = 0;
    clearRules();
    clearError();
}

void FuzzyMamdani::clearRules() {
    for (int i = 0; i < numRules; i++) {
        delete[] rules[i].antecedentVars;
        delete[] rules[i].antecedentSets;
    }

    numRules = 0;
    clearError();
}

int FuzzyMamdani::getInputCount() const {
    return numInputs;
}

int FuzzyMamdani::getOutputCount() const {
    return numOutputs;
}

int FuzzyMamdani::getRuleCount() const {
    return numRules;
}

const FuzzyMamdaniVariable *FuzzyMamdani::getInputVariable(int index) const {
    if (index < 0 || index >= numInputs) {
        return nullptr;
    }

    return &inputVars[index];
}

const FuzzyMamdaniVariable *FuzzyMamdani::getOutputVariable(int index) const {
    if (index < 0 || index >= numOutputs) {
        return nullptr;
    }

    return &outputVars[index];
}

const FuzzyMamdaniRule *FuzzyMamdani::getRule(int index) const {
    if (index < 0 || index >= numRules) {
        return nullptr;
    }

    return &rules[index];
}

bool FuzzyMamdani::hasError() const {
    return errorState;
}

const char *FuzzyMamdani::getErrorMessage() const {
    return errorState ? errorMessage : "No error";
}

void FuzzyMamdani::clearError() {
    errorState = false;
    errorMessage[0] = '\0';
}

#ifdef ESP32

bool FuzzyMamdani::saveModel(const char *filename) {
    if (errorState || filename == nullptr) return false;

    File file = SPIFFS.open(filename, "w");
    if (!file) {
        errorState = true;
        strncpy(errorMessage, "Failed to open file for writing", 49);
        errorMessage[49] = '\0';
        return false;
    }

    file.write((uint8_t *) &maxInputs, sizeof(int));
    file.write((uint8_t *) &maxOutputs, sizeof(int));
    file.write((uint8_t *) &maxRules, sizeof(int));
    file.write((uint8_t *) &maxSetsPerVar, sizeof(int));
    file.write((uint8_t *) &numInputs, sizeof(int));
    file.write((uint8_t *) &numOutputs, sizeof(int));
    file.write((uint8_t *) &numRules, sizeof(int));
    file.write((uint8_t *) &defuzzMethod, sizeof(FuzzyMamdaniDefuzzificationMethod));

    for (int i = 0; i < numInputs; i++) {
        file.write((uint8_t *) &inputVars[i].min, sizeof(float));
        file.write((uint8_t *) &inputVars[i].max, sizeof(float));
        file.write((uint8_t *) &inputVars[i].numSets, sizeof(int));
        file.write((uint8_t *) inputVars[i].name, sizeof(char) * 20);

        for (int j = 0; j < inputVars[i].numSets; j++) {
            file.write((uint8_t *) &inputVars[i].sets[j].type, sizeof(FuzzyMamdaniMembershipType));
            file.write((uint8_t *) inputVars[i].sets[j].name, sizeof(char) * 20);
            file.write((uint8_t *) inputVars[i].sets[j].params, sizeof(float) * 4);
        }
    }

    for (int i = 0; i < numOutputs; i++) {
        file.write((uint8_t *) &outputVars[i].min, sizeof(float));
        file.write((uint8_t *) &outputVars[i].max, sizeof(float));
        file.write((uint8_t *) &outputVars[i].numSets, sizeof(int));
        file.write((uint8_t *) outputVars[i].name, sizeof(char) * 20);

        for (int j = 0; j < outputVars[i].numSets; j++) {
            file.write((uint8_t *) &outputVars[i].sets[j].type, sizeof(FuzzyMamdaniMembershipType));
            file.write((uint8_t *) outputVars[i].sets[j].name, sizeof(char) * 20);
            file.write((uint8_t *) outputVars[i].sets[j].params, sizeof(float) * 4);
        }
    }

    for (int i = 0; i < numRules; i++) {
        file.write((uint8_t *) &rules[i].numAntecedents, sizeof(int));
        file.write((uint8_t *) &rules[i].consequentVar, sizeof(int));
        file.write((uint8_t *) &rules[i].consequentSet, sizeof(int));
        file.write((uint8_t *) &rules[i].useAND, sizeof(bool));

        for (int j = 0; j < rules[i].numAntecedents; j++) {
            file.write((uint8_t *) &rules[i].antecedentVars[j], sizeof(int));
            file.write((uint8_t *) &rules[i].antecedentSets[j], sizeof(int));
        }
    }

    file.close();
    return true;
}

bool FuzzyMamdani::loadModel(const char *filename) {
    if (filename == nullptr) return false;

    clearVariables();

    File file = SPIFFS.open(filename, "r");
    if (!file) {
        errorState = true;
        strncpy(errorMessage, "Failed to open file for reading", 49);
        errorMessage[49] = '\0';
        return false;
    }

    int loadedMaxInputs, loadedMaxOutputs, loadedMaxRules, loadedMaxSetsPerVar;

    file.read((uint8_t *) &loadedMaxInputs, sizeof(int));
    file.read((uint8_t *) &loadedMaxOutputs, sizeof(int));
    file.read((uint8_t *) &loadedMaxRules, sizeof(int));
    file.read((uint8_t *) &loadedMaxSetsPerVar, sizeof(int));

    if (loadedMaxInputs > maxInputs || loadedMaxOutputs > maxOutputs ||
        loadedMaxRules > maxRules || loadedMaxSetsPerVar > maxSetsPerVar) {
        errorState = true;
        strncpy(errorMessage, "Model exceeds current capacity", 49);
        errorMessage[49] = '\0';
        file.close();
        return false;
    }

    file.read((uint8_t *) &numInputs, sizeof(int));
    file.read((uint8_t *) &numOutputs, sizeof(int));
    file.read((uint8_t *) &numRules, sizeof(int));
    file.read((uint8_t *) &defuzzMethod, sizeof(FuzzyMamdaniDefuzzificationMethod));

    for (int i = 0; i < numInputs; i++) {
        file.read((uint8_t *) &inputVars[i].min, sizeof(float));
        file.read((uint8_t *) &inputVars[i].max, sizeof(float));
        file.read((uint8_t *) &inputVars[i].numSets, sizeof(int));
        file.read((uint8_t *) inputVars[i].name, sizeof(char) * 20);

        inputVars[i].sets = new FuzzyMamdaniSet[maxSetsPerVar];
        if (inputVars[i].sets == nullptr) {
            errorState = true;
            strncpy(errorMessage, "Memory allocation failed", 49);
            errorMessage[49] = '\0';
            file.close();
            return false;
        }

        for (int j = 0; j < inputVars[i].numSets; j++) {
            file.read((uint8_t *) &inputVars[i].sets[j].type, sizeof(FuzzyMamdaniMembershipType));
            file.read((uint8_t *) inputVars[i].sets[j].name, sizeof(char) * 20);
            file.read((uint8_t *) inputVars[i].sets[j].params, sizeof(float) * 4);
        }
    }

    for (int i = 0; i < numOutputs; i++) {
        file.read((uint8_t *) &outputVars[i].min, sizeof(float));
        file.read((uint8_t *) &outputVars[i].max, sizeof(float));
        file.read((uint8_t *) &outputVars[i].numSets, sizeof(int));
        file.read((uint8_t *) outputVars[i].name, sizeof(char) * 20);

        outputVars[i].sets = new FuzzyMamdaniSet[maxSetsPerVar];
        if (outputVars[i].sets == nullptr) {
            errorState = true;
            strncpy(errorMessage, "Memory allocation failed", 49);
            errorMessage[49] = '\0';
            file.close();
            return false;
        }

        for (int j = 0; j < outputVars[i].numSets; j++) {
            file.read((uint8_t *) &outputVars[i].sets[j].type, sizeof(FuzzyMamdaniMembershipType));
            file.read((uint8_t *) outputVars[i].sets[j].name, sizeof(char) * 20);
            file.read((uint8_t *) outputVars[i].sets[j].params, sizeof(float) * 4);
        }
    }

    for (int i = 0; i < numRules; i++) {
        int numAntecedents;
        file.read((uint8_t *) &numAntecedents, sizeof(int));
        file.read((uint8_t *) &rules[i].consequentVar, sizeof(int));
        file.read((uint8_t *) &rules[i].consequentSet, sizeof(int));
        file.read((uint8_t *) &rules[i].useAND, sizeof(bool));

        rules[i].numAntecedents = numAntecedents;
        rules[i].antecedentVars = new int[numAntecedents];
        rules[i].antecedentSets = new int[numAntecedents];

        if (rules[i].antecedentVars == nullptr || rules[i].antecedentSets == nullptr) {
            errorState = true;
            strncpy(errorMessage, "Memory allocation failed", 49);
            errorMessage[49] = '\0';
            file.close();
            return false;
        }

        for (int j = 0; j < numAntecedents; j++) {
            file.read((uint8_t *) &rules[i].antecedentVars[j], sizeof(int));
            file.read((uint8_t *) &rules[i].antecedentSets[j], sizeof(int));
        }
    }

    file.close();
    return true;
}

#endif
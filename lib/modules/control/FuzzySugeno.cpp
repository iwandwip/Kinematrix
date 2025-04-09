/*
 *  FuzzySugeno.cpp
 *
 *  Fuzzy Sugeno library for embedded systems
 *  Created on: 2025. 3. 30
 */

#include "FuzzySugeno.h"

FuzzySugeno::FuzzySugeno(int maxInputs, int maxOutputs, int maxRules, int maxSetsPerVar) :
        maxInputs(maxInputs), maxOutputs(maxOutputs), maxRules(maxRules), maxSetsPerVar(maxSetsPerVar),
        numInputs(0), numOutputs(0), numRules(0), debugMode(false), errorState(false) {

    if (maxInputs <= 0 || maxOutputs <= 0 || maxRules <= 0 || maxSetsPerVar <= 0) {
        errorState = true;
        strncpy(errorMessage, "Invalid parameters", 49);
        errorMessage[49] = '\0';
        return;
    }

    inputVars = new FuzzySugenoVariable[maxInputs];
    if (inputVars == nullptr) {
        errorState = true;
        strncpy(errorMessage, "Memory allocation failed", 49);
        errorMessage[49] = '\0';
        return;
    }

    outputVars = new FuzzySugenoVariable[maxOutputs];
    if (outputVars == nullptr) {
        delete[] inputVars;
        inputVars = nullptr;
        errorState = true;
        strncpy(errorMessage, "Memory allocation failed", 49);
        errorMessage[49] = '\0';
        return;
    }

    outputSets = new FuzzySugenoOutput *[maxOutputs];
    if (outputSets == nullptr) {
        delete[] inputVars;
        delete[] outputVars;
        inputVars = nullptr;
        outputVars = nullptr;
        errorState = true;
        strncpy(errorMessage, "Memory allocation failed", 49);
        errorMessage[49] = '\0';
        return;
    }

    numOutputSets = new int[maxOutputs];
    if (numOutputSets == nullptr) {
        delete[] inputVars;
        delete[] outputVars;
        delete[] outputSets;
        inputVars = nullptr;
        outputVars = nullptr;
        outputSets = nullptr;
        errorState = true;
        strncpy(errorMessage, "Memory allocation failed", 49);
        errorMessage[49] = '\0';
        return;
    }

    for (int i = 0; i < maxOutputs; i++) {
        outputSets[i] = new FuzzySugenoOutput[maxSetsPerVar];
        if (outputSets[i] == nullptr) {
            for (int j = 0; j < i; j++) {
                delete[] outputSets[j];
            }
            delete[] inputVars;
            delete[] outputVars;
            delete[] outputSets;
            delete[] numOutputSets;
            inputVars = nullptr;
            outputVars = nullptr;
            outputSets = nullptr;
            numOutputSets = nullptr;
            errorState = true;
            strncpy(errorMessage, "Memory allocation failed", 49);
            errorMessage[49] = '\0';
            return;
        }
        numOutputSets[i] = 0;
    }

    rules = new FuzzySugenoRule[maxRules];
    if (rules == nullptr) {
        for (int i = 0; i < maxOutputs; i++) {
            delete[] outputSets[i];
        }
        delete[] inputVars;
        delete[] outputVars;
        delete[] outputSets;
        delete[] numOutputSets;
        inputVars = nullptr;
        outputVars = nullptr;
        outputSets = nullptr;
        numOutputSets = nullptr;
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

FuzzySugeno::~FuzzySugeno() {
    for (int i = 0; i < numInputs; i++) {
        delete[] inputVars[i].sets;
    }

    for (int i = 0; i < numOutputs; i++) {
        for (int j = 0; j < numOutputSets[i]; j++) {
            delete[] outputSets[i][j].coefficients;
        }
        delete[] outputSets[i];
    }

    for (int i = 0; i < numRules; i++) {
        delete[] rules[i].antecedentVars;
        delete[] rules[i].antecedentSets;
    }

    delete[] inputVars;
    delete[] outputVars;
    delete[] outputSets;
    delete[] numOutputSets;
    delete[] rules;
}

bool FuzzySugeno::addInputVariable(const char *name, float min, float max) {
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

    inputVars[numInputs].sets = new FuzzySugenoSet[maxSetsPerVar];
    if (inputVars[numInputs].sets == nullptr) {
        errorState = true;
        strncpy(errorMessage, "Memory allocation failed", 49);
        errorMessage[49] = '\0';
        return false;
    }

    numInputs++;
    return true;
}

bool FuzzySugeno::addOutputVariable(const char *name, float min, float max) {
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

    numOutputs++;
    return true;
}

bool FuzzySugeno::addFuzzySet(int varIndex, bool isInput, const char *name, MembershipType type, const float params[]) {
    if (errorState) return false;

    if (isInput) {
        if (varIndex < 0 || varIndex >= numInputs) {
            errorState = true;
            strncpy(errorMessage, "Invalid input variable index", 49);
            errorMessage[49] = '\0';
            return false;
        }

        if (inputVars[varIndex].numSets >= maxSetsPerVar) {
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

        int setIndex = inputVars[varIndex].numSets;
        strncpy(inputVars[varIndex].sets[setIndex].name, name, 19);
        inputVars[varIndex].sets[setIndex].name[19] = '\0';
        inputVars[varIndex].sets[setIndex].type = type;

        switch (type) {
            case Fuzzy::TRIANGULAR:
                if (params[0] > params[1] || params[1] > params[2]) {
                    errorState = true;
                    strncpy(errorMessage, "Invalid triangular parameters", 49);
                    errorMessage[49] = '\0';
                    return false;
                }
                inputVars[varIndex].sets[setIndex].params[0] = params[0];
                inputVars[varIndex].sets[setIndex].params[1] = params[1];
                inputVars[varIndex].sets[setIndex].params[2] = params[2];
                break;

            case Fuzzy::TRAPEZOIDAL:
                if (params[0] > params[1] || params[1] > params[2] || params[2] > params[3]) {
                    errorState = true;
                    strncpy(errorMessage, "Invalid trapezoidal parameters", 49);
                    errorMessage[49] = '\0';
                    return false;
                }
                inputVars[varIndex].sets[setIndex].params[0] = params[0];
                inputVars[varIndex].sets[setIndex].params[1] = params[1];
                inputVars[varIndex].sets[setIndex].params[2] = params[2];
                inputVars[varIndex].sets[setIndex].params[3] = params[3];
                break;

            case Fuzzy::GAUSSIAN:
                if (params[1] <= 0) {
                    errorState = true;
                    strncpy(errorMessage, "Invalid gaussian parameters", 49);
                    errorMessage[49] = '\0';
                    return false;
                }
                inputVars[varIndex].sets[setIndex].params[0] = params[0]; // Mean
                inputVars[varIndex].sets[setIndex].params[1] = params[1]; // Sigma
                break;

            case Fuzzy::SINGLETON:
                inputVars[varIndex].sets[setIndex].params[0] = params[0]; // Center
                break;

            default:
                errorState = true;
                strncpy(errorMessage, "Unsupported membership function type", 49);
                errorMessage[49] = '\0';
                return false;
        }

        inputVars[varIndex].numSets++;
        return true;
    } else {
        errorState = true;
        strncpy(errorMessage, "addFuzzySet for outputs not supported in Sugeno", 49);
        errorMessage[49] = '\0';
        return false;
    }
}

bool FuzzySugeno::addConstantOutput(int varIndex, const char *name, float value) {
    if (errorState) return false;

    if (varIndex < 0 || varIndex >= numOutputs) {
        errorState = true;
        strncpy(errorMessage, "Invalid output variable index", 49);
        errorMessage[49] = '\0';
        return false;
    }

    if (numOutputSets[varIndex] >= maxSetsPerVar) {
        errorState = true;
        strncpy(errorMessage, "Max output sets exceeded for variable", 49);
        errorMessage[49] = '\0';
        return false;
    }

    if (name == nullptr) {
        errorState = true;
        strncpy(errorMessage, "Invalid output name", 49);
        errorMessage[49] = '\0';
        return false;
    }

    int setIndex = numOutputSets[varIndex];
    strncpy(outputSets[varIndex][setIndex].name, name, 19);
    outputSets[varIndex][setIndex].name[19] = '\0';
    outputSets[varIndex][setIndex].type = Fuzzy::CONSTANT;
    outputSets[varIndex][setIndex].numCoefficients = 1;

    outputSets[varIndex][setIndex].coefficients = new float[1];
    if (outputSets[varIndex][setIndex].coefficients == nullptr) {
        errorState = true;
        strncpy(errorMessage, "Memory allocation failed", 49);
        errorMessage[49] = '\0';
        return false;
    }

    outputSets[varIndex][setIndex].coefficients[0] = value;
    numOutputSets[varIndex]++;
    return true;
}

bool FuzzySugeno::addLinearOutput(int varIndex, const char *name, const float coefficients[], int numCoefficients) {
    if (errorState) return false;

    if (varIndex < 0 || varIndex >= numOutputs) {
        errorState = true;
        strncpy(errorMessage, "Invalid output variable index", 49);
        errorMessage[49] = '\0';
        return false;
    }

    if (numOutputSets[varIndex] >= maxSetsPerVar) {
        errorState = true;
        strncpy(errorMessage, "Max output sets exceeded for variable", 49);
        errorMessage[49] = '\0';
        return false;
    }

    if (name == nullptr || coefficients == nullptr || numCoefficients <= 0) {
        errorState = true;
        strncpy(errorMessage, "Invalid output parameters", 49);
        errorMessage[49] = '\0';
        return false;
    }

    if (numCoefficients > numInputs + 1) {
        errorState = true;
        strncpy(errorMessage, "Too many coefficients for inputs", 49);
        errorMessage[49] = '\0';
        return false;
    }

    int setIndex = numOutputSets[varIndex];
    strncpy(outputSets[varIndex][setIndex].name, name, 19);
    outputSets[varIndex][setIndex].name[19] = '\0';
    outputSets[varIndex][setIndex].type = Fuzzy::LINEAR;
    outputSets[varIndex][setIndex].numCoefficients = numCoefficients;

    outputSets[varIndex][setIndex].coefficients = new float[numCoefficients];
    if (outputSets[varIndex][setIndex].coefficients == nullptr) {
        errorState = true;
        strncpy(errorMessage, "Memory allocation failed", 49);
        errorMessage[49] = '\0';
        return false;
    }

    for (int i = 0; i < numCoefficients; i++) {
        outputSets[varIndex][setIndex].coefficients[i] = coefficients[i];
    }

    numOutputSets[varIndex]++;
    return true;
}

bool FuzzySugeno::addRule(int *antecedentVars, int *antecedentSets, int numAntecedents,
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

    if (consequentSet < 0 || consequentSet >= numOutputSets[consequentVar]) {
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

float *FuzzySugeno::evaluate(const float *inputs) {
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

    float *ruleFiringStrengths = new float[numRules];
    if (ruleFiringStrengths == nullptr) {
        errorState = true;
        strncpy(errorMessage, "Memory allocation failed", 49);
        errorMessage[49] = '\0';
        return nullptr;
    }

    float *numerators = new float[numOutputs];
    float *denominators = new float[numOutputs];

    if (numerators == nullptr || denominators == nullptr) {
        delete[] ruleFiringStrengths;
        if (numerators != nullptr) delete[] numerators;
        if (denominators != nullptr) delete[] denominators;

        errorState = true;
        strncpy(errorMessage, "Memory allocation failed", 49);
        errorMessage[49] = '\0';
        return nullptr;
    }

    for (int i = 0; i < numOutputs; i++) {
        numerators[i] = 0.0f;
        denominators[i] = 0.0f;
    }

    for (int i = 0; i < numRules; i++) {
        const FuzzySugenoRule &rule = rules[i];

        if (rule.numAntecedents > 0) {
            int varIndex = rule.antecedentVars[0];
            int setIndex = rule.antecedentSets[0];

            float value = inputs[varIndex];
            const FuzzySugenoSet &set = inputVars[varIndex].sets[setIndex];

            float strength = calculateMembership(value, set);

            for (int j = 1; j < rule.numAntecedents; j++) {
                varIndex = rule.antecedentVars[j];
                setIndex = rule.antecedentSets[j];

                value = inputs[varIndex];
                const FuzzySugenoSet &nextSet = inputVars[varIndex].sets[setIndex];

                float membershipDegree = calculateMembership(value, nextSet);
                strength = applyFuzzyOperator(strength, membershipDegree, rule.useAND);
            }

            ruleFiringStrengths[i] = strength;

            if (strength > 0.0f) {
                float outputValue = evaluateSugenoOutput(rule.consequentVar, rule.consequentSet, inputs);
                numerators[rule.consequentVar] += strength * outputValue;
                denominators[rule.consequentVar] += strength;
            }
        } else {
            ruleFiringStrengths[i] = 0.0f;
        }
    }

    float *outputs = new float[numOutputs];
    if (outputs == nullptr) {
        delete[] ruleFiringStrengths;
        delete[] numerators;
        delete[] denominators;

        errorState = true;
        strncpy(errorMessage, "Memory allocation failed", 49);
        errorMessage[49] = '\0';
        return nullptr;
    }

    for (int i = 0; i < numOutputs; i++) {
        if (denominators[i] > 0.0001f) {
            outputs[i] = numerators[i] / denominators[i];
        } else {
            // Default output if no rules fire significantly
            if (numOutputSets[i] > 0 && outputSets[i][0].type == Fuzzy::CONSTANT) {
                outputs[i] = outputSets[i][0].coefficients[0];
            } else {
                outputs[i] = (outputVars[i].min + outputVars[i].max) / 2.0f;
            }
        }

        if (outputs[i] < outputVars[i].min) outputs[i] = outputVars[i].min;
        if (outputs[i] > outputVars[i].max) outputs[i] = outputVars[i].max;
    }

    delete[] ruleFiringStrengths;
    delete[] numerators;
    delete[] denominators;

    return outputs;
}

float FuzzySugeno::evaluateSugenoOutput(int outputVarIndex, int outputSetIndex, const float *inputs) const {
    if (outputVarIndex < 0 || outputVarIndex >= numOutputs ||
        outputSetIndex < 0 || outputSetIndex >= numOutputSets[outputVarIndex]) {
        return 0.0f;
    }

    const FuzzySugenoOutput &output = outputSets[outputVarIndex][outputSetIndex];

    if (output.type == Fuzzy::CONSTANT) {
        return output.coefficients[0];
    } else { // LINEAR
        float result = output.numCoefficients > 0 ? output.coefficients[0] : 0.0f; // Constant term

        for (int i = 1; i < output.numCoefficients && i <= numInputs; i++) {
            result += output.coefficients[i] * inputs[i - 1];
        }

        return result;
    }
}

float FuzzySugeno::applyFuzzyOperator(float a, float b, bool useAND) const {
    if (useAND) {
        return (a < b) ? a : b;  // min for AND
    } else {
        return (a > b) ? a : b;  // max for OR
    }
}

float FuzzySugeno::calculateMembership(float value, const FuzzySugenoSet &set) const {
    switch (set.type) {
        case Fuzzy::TRIANGULAR:
            return calculateTriangularMembership(value, set.params[0], set.params[1], set.params[2]);
        case Fuzzy::TRAPEZOIDAL:
            return calculateTrapezoidalMembership(value, set.params[0], set.params[1], set.params[2], set.params[3]);
        case Fuzzy::GAUSSIAN:
            return calculateGaussianMembership(value, set.params[0], set.params[1]);
        case Fuzzy::SINGLETON:
            return calculateSingletonMembership(value, set.params[0]);
        default:
            return 0.0f;
    }
}

float FuzzySugeno::calculateTriangularMembership(float value, float a, float b, float c) const {
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

float FuzzySugeno::calculateTrapezoidalMembership(float value, float a, float b, float c, float d) const {
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

float FuzzySugeno::calculateGaussianMembership(float value, float mean, float sigma) const {
    float diff = value - mean;
    return exp(-(diff * diff) / (2 * sigma * sigma));
}

float FuzzySugeno::calculateSingletonMembership(float value, float center) const {
    return (abs(value - center) < 0.0001f) ? 1.0f : 0.0f;
}

void FuzzySugeno::setDebugMode(bool enable) {
    debugMode = enable;
}

void FuzzySugeno::clearVariables() {
    for (int i = 0; i < numInputs; i++) {
        delete[] inputVars[i].sets;
    }

    for (int i = 0; i < numOutputs; i++) {
        for (int j = 0; j < numOutputSets[i]; j++) {
            delete[] outputSets[i][j].coefficients;
        }
    }

    numInputs = 0;
    numOutputs = 0;
    clearRules();
    clearError();
}

void FuzzySugeno::clearRules() {
    for (int i = 0; i < numRules; i++) {
        delete[] rules[i].antecedentVars;
        delete[] rules[i].antecedentSets;
    }

    numRules = 0;
    clearError();
}

int FuzzySugeno::getInputCount() const {
    return numInputs;
}

int FuzzySugeno::getOutputCount() const {
    return numOutputs;
}

int FuzzySugeno::getRuleCount() const {
    return numRules;
}

const FuzzySugenoVariable *FuzzySugeno::getInputVariable(int index) const {
    if (index < 0 || index >= numInputs) {
        return nullptr;
    }

    return &inputVars[index];
}

const FuzzySugenoVariable *FuzzySugeno::getOutputVariable(int index) const {
    if (index < 0 || index >= numOutputs) {
        return nullptr;
    }

    return &outputVars[index];
}

const FuzzySugenoOutput *FuzzySugeno::getOutputSet(int varIndex, int setIndex) const {
    if (varIndex < 0 || varIndex >= numOutputs || setIndex < 0 || setIndex >= numOutputSets[varIndex]) {
        return nullptr;
    }

    return &outputSets[varIndex][setIndex];
}

const FuzzySugenoRule *FuzzySugeno::getRule(int index) const {
    if (index < 0 || index >= numRules) {
        return nullptr;
    }

    return &rules[index];
}

bool FuzzySugeno::hasError() const {
    return errorState;
}

const char *FuzzySugeno::getErrorMessage() const {
    return errorState ? errorMessage : "No error";
}

void FuzzySugeno::clearError() {
    errorState = false;
    errorMessage[0] = '\0';
}

#ifdef ESP32

bool FuzzySugeno::saveModel(const char *filename) {
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

    for (int i = 0; i < numInputs; i++) {
        file.write((uint8_t *) &inputVars[i].min, sizeof(float));
        file.write((uint8_t *) &inputVars[i].max, sizeof(float));
        file.write((uint8_t *) &inputVars[i].numSets, sizeof(int));
        file.write((uint8_t *) inputVars[i].name, sizeof(char) * 20);

        for (int j = 0; j < inputVars[i].numSets; j++) {
            file.write((uint8_t *) &inputVars[i].sets[j].type, sizeof(FuzzySugenoMembershipType));
            file.write((uint8_t *) inputVars[i].sets[j].name, sizeof(char) * 20);
            file.write((uint8_t *) inputVars[i].sets[j].params, sizeof(float) * 4);
        }
    }

    for (int i = 0; i < numOutputs; i++) {
        file.write((uint8_t *) &outputVars[i].min, sizeof(float));
        file.write((uint8_t *) &outputVars[i].max, sizeof(float));
        file.write((uint8_t *) &numOutputSets[i], sizeof(int));
        file.write((uint8_t *) outputVars[i].name, sizeof(char) * 20);

        for (int j = 0; j < numOutputSets[i]; j++) {
            file.write((uint8_t *) &outputSets[i][j].type, sizeof(FuzzySugenoOutputType));
            file.write((uint8_t *) &outputSets[i][j].numCoefficients, sizeof(int));
            file.write((uint8_t *) outputSets[i][j].name, sizeof(char) * 20);
            file.write((uint8_t *) outputSets[i][j].coefficients, sizeof(float) * outputSets[i][j].numCoefficients);
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

bool FuzzySugeno::loadModel(const char *filename) {
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

    for (int i = 0; i < numInputs; i++) {
        file.read((uint8_t *) &inputVars[i].min, sizeof(float));
        file.read((uint8_t *) &inputVars[i].max, sizeof(float));
        file.read((uint8_t *) &inputVars[i].numSets, sizeof(int));
        file.read((uint8_t *) inputVars[i].name, sizeof(char) * 20);

        inputVars[i].sets = new FuzzySugenoSet[maxSetsPerVar];
        if (inputVars[i].sets == nullptr) {
            errorState = true;
            strncpy(errorMessage, "Memory allocation failed", 49);
            errorMessage[49] = '\0';
            file.close();
            return false;
        }

        for (int j = 0; j < inputVars[i].numSets; j++) {
            file.read((uint8_t *) &inputVars[i].sets[j].type, sizeof(FuzzySugenoMembershipType));
            file.read((uint8_t *) inputVars[i].sets[j].name, sizeof(char) * 20);
            file.read((uint8_t *) inputVars[i].sets[j].params, sizeof(float) * 4);
        }
    }

    for (int i = 0; i < numOutputs; i++) {
        file.read((uint8_t *) &outputVars[i].min, sizeof(float));
        file.read((uint8_t *) &outputVars[i].max, sizeof(float));
        file.read((uint8_t *) &numOutputSets[i], sizeof(int));
        file.read((uint8_t *) outputVars[i].name, sizeof(char) * 20);

        for (int j = 0; j < numOutputSets[i]; j++) {
            file.read((uint8_t *) &outputSets[i][j].type, sizeof(FuzzySugenoOutputType));
            file.read((uint8_t *) &outputSets[i][j].numCoefficients, sizeof(int));
            file.read((uint8_t *) outputSets[i][j].name, sizeof(char) * 20);

            outputSets[i][j].coefficients = new float[outputSets[i][j].numCoefficients];
            if (outputSets[i][j].coefficients == nullptr) {
                errorState = true;
                strncpy(errorMessage, "Memory allocation failed", 49);
                errorMessage[49] = '\0';
                file.close();
                return false;
            }

            file.read((uint8_t *) outputSets[i][j].coefficients, sizeof(float) * outputSets[i][j].numCoefficients);
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
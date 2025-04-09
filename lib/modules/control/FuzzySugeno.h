/*
 *  FuzzySugeno.h
 *
 *  Fuzzy Sugeno library for embedded systems
 *  Created on: 2025. 3. 30
 */

#pragma once

#ifndef FUZZY_SUGENO_LIB_H
#define FUZZY_SUGENO_LIB_H

#pragma message("[COMPILED]: FuzzySugeno.h")

#include "FuzzyHeader.h"

using Fuzzy::MembershipType;
using Fuzzy::OutputType;
using Fuzzy::FuzzySet;
using Fuzzy::FuzzyVariable;

typedef FuzzySet FuzzySugenoSet;
typedef FuzzyVariable FuzzySugenoVariable;
typedef OutputType FuzzySugenoOutputType;
typedef MembershipType FuzzySugenoMembershipType;

struct FuzzySugenoOutput {
    char name[20];
    OutputType type;
    int numCoefficients;
    float *coefficients;
};

struct FuzzySugenoRule {
    int *antecedentVars;
    int *antecedentSets;
    int consequentVar;
    int consequentSet;
    int numAntecedents;
    bool useAND;
};

class FuzzySugeno {
private:
    int maxInputs;
    int maxOutputs;
    int maxRules;
    int maxSetsPerVar;

    int numInputs;
    int numOutputs;
    int numRules;

    FuzzySugenoVariable *inputVars;
    FuzzySugenoVariable *outputVars;
    FuzzySugenoOutput **outputSets;
    int *numOutputSets;
    FuzzySugenoRule *rules;

    bool debugMode;
    bool errorState;
    char errorMessage[50];

    float calculateMembership(float value, const FuzzySugenoSet &set) const;
    float calculateTriangularMembership(float value, float a, float b, float c) const;
    float calculateTrapezoidalMembership(float value, float a, float b, float c, float d) const;
    float calculateGaussianMembership(float value, float mean, float sigma) const;
    float calculateSingletonMembership(float value, float center) const;

    float applyFuzzyOperator(float a, float b, bool useAND) const;
    float evaluateSugenoOutput(int outputVarIndex, int outputSetIndex, const float *inputs) const;

public:
    FuzzySugeno(int maxInputs, int maxOutputs, int maxRules, int maxSetsPerVar);
    ~FuzzySugeno();

    bool addInputVariable(const char *name, float min, float max);
    bool addOutputVariable(const char *name, float min, float max);
    bool addFuzzySet(int varIndex, bool isInput, const char *name, MembershipType type, const float params[]);

    bool addConstantOutput(int varIndex, const char *name, float value);
    bool addLinearOutput(int varIndex, const char *name, const float coefficients[], int numCoefficients);

    bool addRule(int *antecedentVars, int *antecedentSets, int numAntecedents, int consequentVar, int consequentSet, bool useAND);

    float *evaluate(const float *inputs);

    void setDebugMode(bool enable);
    void clearVariables();
    void clearRules();

    int getInputCount() const;
    int getOutputCount() const;
    int getRuleCount() const;

    const FuzzySugenoVariable *getInputVariable(int index) const;
    const FuzzySugenoVariable *getOutputVariable(int index) const;
    const FuzzySugenoOutput *getOutputSet(int varIndex, int setIndex) const;
    const FuzzySugenoRule *getRule(int index) const;

    bool hasError() const;
    const char *getErrorMessage() const;
    void clearError();

#ifdef ESP32
    bool saveModel(const char *filename);
    bool loadModel(const char *filename);
#endif
};

#endif
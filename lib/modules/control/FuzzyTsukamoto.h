/*
 *  FuzzyTsukamoto.h
 *
 *  Fuzzy Tsukamoto library for embedded systems
 *  Created on: 2025. 3. 30
 */

#pragma once

#ifndef FUZZY_TSUKAMOTO_LIB_H
#define FUZZY_TSUKAMOTO_LIB_H

#pragma message("[COMPILED]: FuzzyTsukamoto.h")

#include "FuzzyHeader.h"

// Using the common Fuzzy namespace

// Typedef for backward compatibility
typedef FuzzySet FuzzyTsukamotoSet;
typedef FuzzyVariable FuzzyTsukamotoVariable;
typedef MembershipType FuzzyTsukamotoMembershipType;

struct FuzzyTsukamotoRule {
    int *antecedentVars;
    int *antecedentSets;
    int consequentVar;
    int consequentSet;
    int numAntecedents;
    bool useAND;
};

class FuzzyTsukamoto {
private:
    int maxInputs;
    int maxOutputs;
    int maxRules;
    int maxSetsPerVar;

    int numInputs;
    int numOutputs;
    int numRules;

    FuzzyTsukamotoVariable *inputVars;
    FuzzyTsukamotoVariable *outputVars;
    FuzzyTsukamotoRule *rules;

    bool debugMode;
    bool errorState;
    char errorMessage[50];

    float calculateMembership(float value, const FuzzyTsukamotoSet &set) const;
    float calculateTriangularMembership(float value, float a, float b, float c) const;
    float calculateTrapezoidalMembership(float value, float a, float b, float c, float d) const;
    float calculateGaussianMembership(float value, float mean, float sigma) const;
    float calculateSingletonMembership(float value, float center) const;
    float calculateMonotonicIncreasingMembership(float value, float a, float b) const;
    float calculateMonotonicDecreasingMembership(float value, float a, float b) const;

    float applyFuzzyOperator(float a, float b, bool useAND) const;
    float findInverseMembershipValue(float membershipDegree, const FuzzyTsukamotoSet &set, float min, float max) const;
    float *evaluateRules(const float *inputs);

public:
    FuzzyTsukamoto(int maxInputs, int maxOutputs, int maxRules, int maxSetsPerVar);
    ~FuzzyTsukamoto();

    bool addInputVariable(const char *name, float min, float max);
    bool addOutputVariable(const char *name, float min, float max);
    bool addFuzzySet(int varIndex, bool isInput, const char *name, MembershipType type, const float params[]);
    bool addRule(int *antecedentVars, int *antecedentSets, int numAntecedents, int consequentVar, int consequentSet, bool useAND);

    float *evaluate(const float *inputs);

    void setDebugMode(bool enable);
    void clearVariables();
    void clearRules();

    int getInputCount() const;
    int getOutputCount() const;
    int getRuleCount() const;

    const FuzzyTsukamotoVariable *getInputVariable(int index) const;
    const FuzzyTsukamotoVariable *getOutputVariable(int index) const;
    const FuzzyTsukamotoRule *getRule(int index) const;

    bool hasError() const;
    const char *getErrorMessage() const;
    void clearError();

#ifdef ESP32
    bool saveModel(const char *filename);
    bool loadModel(const char *filename);
#endif
};

#endif
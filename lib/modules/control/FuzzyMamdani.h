#pragma once

#ifndef FUZZY_MAMDANI_LIB_H
#define FUZZY_MAMDANI_LIB_H

#pragma message("[COMPILED]: FuzzyMamdani.h")

#include "FuzzyHeader.h"

typedef FuzzySet FuzzyMamdaniSet;
typedef FuzzyVariable FuzzyMamdaniVariable;
typedef DefuzzificationMethod FuzzyMamdaniDefuzzificationMethod;
typedef MembershipType FuzzyMamdaniMembershipType;

struct FuzzyMamdaniRule {
    int *antecedentVars;
    int *antecedentSets;
    int consequentVar;
    int consequentSet;
    int numAntecedents;
    bool useAND;
};

class FuzzyMamdani {
private:
    int maxInputs;
    int maxOutputs;
    int maxRules;
    int maxSetsPerVar;

    int numInputs;
    int numOutputs;
    int numRules;

    FuzzyMamdaniVariable *inputVars;
    FuzzyMamdaniVariable *outputVars;
    FuzzyMamdaniRule *rules;

    DefuzzificationMethod defuzzMethod;
    bool debugMode;

    bool errorState;
    char errorMessage[50];

    float calculateMembership(float value, const FuzzyMamdaniSet &set) const;
    float calculateTriangularMembership(float value, float a, float b, float c) const;
    float calculateTrapezoidalMembership(float value, float a, float b, float c, float d) const;
    float calculateGaussianMembership(float value, float mean, float sigma) const;
    float calculateSingletonMembership(float value, float center) const;

    float applyFuzzyOperator(float a, float b, bool useAND) const;
    float *evaluateRules(const float *inputs, int *activatedRules);
    float defuzzifyCentroid(int outputIndex, const float *ruleStrengths) const;
    float defuzzifyMOM(int outputIndex, const float *ruleStrengths) const;
    float defuzzifySOM(int outputIndex, const float *ruleStrengths) const;
    float defuzzifyLOM(int outputIndex, const float *ruleStrengths) const;
    float defuzzifyBisector(int outputIndex, const float *ruleStrengths) const;

public:
    FuzzyMamdani(int maxInputs, int maxOutputs, int maxRules, int maxSetsPerVar);
    ~FuzzyMamdani();

    bool addInputVariable(const char *name, float min, float max);
    bool addOutputVariable(const char *name, float min, float max);
    bool addFuzzySet(int varIndex, bool isInput, const char *name, MembershipType type, const float params[]);
    bool addRule(int *antecedentVars, int *antecedentSets, int numAntecedents, int consequentVar, int consequentSet, bool useAND);

    float *evaluate(const float *inputs);

    void setDefuzzificationMethod(DefuzzificationMethod method);
    void setDebugMode(bool enable);

    void clearVariables();
    void clearRules();

    int getInputCount() const;
    int getOutputCount() const;
    int getRuleCount() const;

    const FuzzyMamdaniVariable *getInputVariable(int index) const;
    const FuzzyMamdaniVariable *getOutputVariable(int index) const;
    const FuzzyMamdaniRule *getRule(int index) const;

    bool hasError() const;
    const char *getErrorMessage() const;
    void clearError();

#ifdef ESP32
    bool saveModel(const char *filename);
    bool loadModel(const char *filename);
#endif
};

#endif
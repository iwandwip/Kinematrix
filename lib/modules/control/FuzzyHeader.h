#pragma once

#ifndef FUZZY_COMMON_HEADER_H
#define FUZZY_COMMON_HEADER_H

#include "Arduino.h"
#ifdef ESP32
#include "SPIFFS.h"
#endif

enum MembershipType {
    TRIANGULAR,
    TRAPEZOIDAL,
    GAUSSIAN,
    SINGLETON,
    MONOTONIC_INCREASING,
    MONOTONIC_DECREASING
};

enum DefuzzificationMethod {
    CENTROID,
    BISECTOR,
    MOM,
    SOM,
    LOM
};

enum OutputType {
    CONSTANT,
    LINEAR
};

struct FuzzySet {
    char name[20];
    MembershipType type;
    float params[4];
};

struct FuzzyVariable {
    char name[20];
    float min;
    float max;
    int numSets;
    FuzzySet *sets;
};

#endif
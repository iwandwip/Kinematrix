/*
 *  FuzzyHeader.h
 *
 *  Common header file for fuzzy logic libraries
 *  Created on: 2025. 3. 30
 */

#pragma once

#ifndef FUZZY_COMMON_HEADER_H
#define FUZZY_COMMON_HEADER_H

#include "Arduino.h"
#ifdef ESP32
#include "SPIFFS.h"
#endif

namespace Fuzzy {
    // Common membership function types
    enum MembershipType {
        TRIANGULAR,
        TRAPEZOIDAL,
        GAUSSIAN,
        SINGLETON,
        MONOTONIC_INCREASING,  // Used by Tsukamoto
        MONOTONIC_DECREASING   // Used by Tsukamoto
    };

    // Common defuzzification methods for Mamdani
    enum DefuzzificationMethod {
        CENTROID,
        BISECTOR,
        MOM,   // Mean of Maximum
        SOM,   // Smallest of Maximum
        LOM    // Largest of Maximum
    };

    // Output types for Sugeno
    enum OutputType {
        CONSTANT,
        LINEAR
    };

    // Base fuzzy set structure
    struct FuzzySet {
        char name[20];
        MembershipType type;
        float params[4];  // Parameters for the membership function
    };

    // Base variable structure
    struct FuzzyVariable {
        char name[20];
        float min;
        float max;
        int numSets;
        FuzzySet *sets;
    };
}

#endif /* FUZZY_COMMON_HEADER_H */
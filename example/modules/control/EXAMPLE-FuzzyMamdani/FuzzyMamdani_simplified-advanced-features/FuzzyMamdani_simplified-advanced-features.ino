/*
 * FuzzyMamdani_Simplified_Advanced_Features.ino
 * 
 * Example demonstrating advanced features of the FuzzyMamdani library
 * All code contained in setup() and loop() only
 */

#define ENABLE_MODULE_FUZZY_MAMDANI
#include "Kinematrix.h"

// Create fuzzy system with:
// - 2 input variables
// - 2 output variables
// - 10 rules maximum
// - 4 fuzzy sets per variable
FuzzyMamdani fuzzy(2, 2, 10, 4);

// State machine for demonstrations
enum DemoState {
    MEMBERSHIP_FUNCTIONS,
    DEFUZZIFICATION_METHODS,
    ERROR_HANDLING,
    DEMO_COMPLETE
};

DemoState currentState = MEMBERSHIP_FUNCTIONS;
int demoStep = 0;
unsigned long lastUpdateTime = 0;

// For temporary error testing
FuzzyMamdani *errorTest = nullptr;

void demonstrateMembershipFunctions();
void demonstrateDefuzzificationMethods();
void demonstrateErrorHandling();

void setup() {
    Serial.begin(115200);
    delay(2000);
    Serial.println("FuzzyMamdani Advanced Features Demo");
    Serial.println("----------------------------------");

    // Add input variables
    fuzzy.addInputVariable("input1", 0, 100);
    fuzzy.addInputVariable("input2", 0, 100);

    // Add output variables
    fuzzy.addOutputVariable("output1", 0, 100);
    fuzzy.addOutputVariable("output2", -50, 50);

    // Add various types of membership functions for input1

    // Triangular function
    float triangularParams[3] = {0, 25, 50};
    fuzzy.addFuzzySet(0, true, "triangular", TRIANGULAR, triangularParams);

    // Trapezoidal function
    float trapezoidalParams[4] = {25, 40, 60, 75};
    fuzzy.addFuzzySet(0, true, "trapezoidal", TRAPEZOIDAL, trapezoidalParams);

    // Gaussian function
    float gaussianParams[2] = {75, 10}; // mean, sigma
    fuzzy.addFuzzySet(0, true, "gaussian", GAUSSIAN, gaussianParams);

    // Singleton function
    float singletonParams[1] = {100};
    fuzzy.addFuzzySet(0, true, "singleton", SINGLETON, singletonParams);

    // Add fuzzy sets for input2 (just using triangular for simplicity)
    float lowParams[3] = {0, 0, 50};
    float highParams[3] = {50, 100, 100};

    fuzzy.addFuzzySet(1, true, "low", TRIANGULAR, lowParams);
    fuzzy.addFuzzySet(1, true, "high", TRIANGULAR, highParams);

    // Add fuzzy sets for output1
    float smallParams[3] = {0, 0, 50};
    float largeParams[3] = {50, 100, 100};

    fuzzy.addFuzzySet(0, false, "small", TRIANGULAR, smallParams);
    fuzzy.addFuzzySet(0, false, "large", TRIANGULAR, largeParams);

    // Add fuzzy sets for output2
    float negativeParams[3] = {-50, -50, 0};
    float zeroParams[3] = {-25, 0, 25};
    float positiveParams[3] = {0, 50, 50};

    fuzzy.addFuzzySet(1, false, "negative", TRIANGULAR, negativeParams);
    fuzzy.addFuzzySet(1, false, "zero", TRIANGULAR, zeroParams);
    fuzzy.addFuzzySet(1, false, "positive", TRIANGULAR, positiveParams);

    // Add some rules

    // Rule 1: IF input1 is triangular THEN output1 is small
    int vars1[1] = {0};
    int sets1[1] = {0}; // triangular
    fuzzy.addRule(vars1, sets1, 1, 0, 0, true); // output1 = small

    // Rule 2: IF input1 is gaussian THEN output1 is large
    int vars2[1] = {0};
    int sets2[1] = {2}; // gaussian
    fuzzy.addRule(vars2, sets2, 1, 0, 1, true); // output1 = large

    // Rule 3: IF input2 is low THEN output2 is negative
    int vars3[1] = {1};
    int sets3[1] = {0}; // low
    fuzzy.addRule(vars3, sets3, 1, 1, 0, true); // output2 = negative

    // Rule 4: IF input2 is high THEN output2 is positive
    int vars4[1] = {1};
    int sets4[1] = {1}; // high
    fuzzy.addRule(vars4, sets4, 1, 1, 2, true); // output2 = positive

    // Rule 5: IF input1 is trapezoidal AND input2 is high THEN output2 is zero
    int vars5[2] = {0, 1};
    int sets5[2] = {1, 1}; // trapezoidal, high
    fuzzy.addRule(vars5, sets5, 2, 1, 1, true); // output2 = zero, AND

    // Rule 6: IF input1 is triangular OR input2 is low THEN output1 is small
    int vars6[2] = {0, 1};
    int sets6[2] = {0, 0}; // triangular, low
    fuzzy.addRule(vars6, sets6, 2, 0, 0, false); // output1 = small, OR

    // Setting default defuzzification method
    fuzzy.setDefuzzificationMethod(CENTROID);

    if (fuzzy.hasError()) {
        Serial.print("Error during setup: ");
        Serial.println(fuzzy.getErrorMessage());
        return;
    }

    Serial.println("\nDemonstrating advanced features:");

    // Initialize error testing fuzzy system for later use
    errorTest = new FuzzyMamdani(2, 1, 5, 3);

    lastUpdateTime = millis();
}

void loop() {
    // Advance the demonstration state every 3 seconds
    if (millis() - lastUpdateTime >= 3000) {
        lastUpdateTime = millis();

        // State machine for demonstrations
        switch (currentState) {
            case MEMBERSHIP_FUNCTIONS:
                demonstrateMembershipFunctions();
                break;

            case DEFUZZIFICATION_METHODS:
                demonstrateDefuzzificationMethods();
                break;

            case ERROR_HANDLING:
                demonstrateErrorHandling();
                break;

            case DEMO_COMPLETE:
                Serial.println("\nAll demonstrations complete. Restarting...");
                currentState = MEMBERSHIP_FUNCTIONS;
                demoStep = 0;
                break;
        }
    }

    // Small delay to avoid busy-looping
    delay(50);
}

// Function for membership functions demonstration
void demonstrateMembershipFunctions() {
    if (demoStep == 0) {
        Serial.println("\n1. MEMBERSHIP FUNCTION DEMONSTRATION");
        Serial.println("----------------------------------");
        Serial.println("Testing different membership functions with input1 = 25:");

        float inputs[2] = {25, 50}; // input1 = 25, input2 = 50
        float *outputs = fuzzy.evaluate(inputs);

        if (outputs != nullptr) {
            Serial.println("Result with input1 = 25 (should activate triangular function):");
            Serial.println("  output1 = " + String(outputs[0], 2));
            Serial.println("  output2 = " + String(outputs[1], 2));
            delete[] outputs;
        }

        demoStep++;
    } else if (demoStep == 1) {
        float inputs[2] = {50, 50}; // input1 = 50, input2 = 50
        float *outputs = fuzzy.evaluate(inputs);

        if (outputs != nullptr) {
            Serial.println("\nResult with input1 = 50 (should activate trapezoidal function):");
            Serial.println("  output1 = " + String(outputs[0], 2));
            Serial.println("  output2 = " + String(outputs[1], 2));
            delete[] outputs;
        }

        demoStep++;
    } else if (demoStep == 2) {
        float inputs[2] = {75, 50}; // input1 = 75, input2 = 50
        float *outputs = fuzzy.evaluate(inputs);

        if (outputs != nullptr) {
            Serial.println("\nResult with input1 = 75 (should activate gaussian function):");
            Serial.println("  output1 = " + String(outputs[0], 2));
            Serial.println("  output2 = " + String(outputs[1], 2));
            delete[] outputs;
        }

        demoStep++;
    } else if (demoStep == 3) {
        float inputs[2] = {100, 50}; // input1 = 100, input2 = 50
        float *outputs = fuzzy.evaluate(inputs);

        if (outputs != nullptr) {
            Serial.println("\nResult with input1 = 100 (should activate singleton function):");
            Serial.println("  output1 = " + String(outputs[0], 2));
            Serial.println("  output2 = " + String(outputs[1], 2));
            delete[] outputs;
        }

        // Move to next demonstration
        demoStep = 0;
        currentState = DEFUZZIFICATION_METHODS;
    }
}

// Function for defuzzification methods demonstration
void demonstrateDefuzzificationMethods() {
    float inputs[2] = {50, 75}; // input1 = 50, input2 = 75

    if (demoStep == 0) {
        Serial.println("\n2. DEFUZZIFICATION METHOD DEMONSTRATION");
        Serial.println("-------------------------------------");
        Serial.println("Testing different defuzzification methods with the same inputs:");

        fuzzy.setDefuzzificationMethod(CENTROID);
        float *outputs = fuzzy.evaluate(inputs);
        if (outputs != nullptr) {
            Serial.println("CENTROID (Center of Mass) Method:");
            Serial.println("  output1 = " + String(outputs[0], 2));
            Serial.println("  output2 = " + String(outputs[1], 2));
            delete[] outputs;
        }

        demoStep++;
    } else if (demoStep == 1) {
        fuzzy.setDefuzzificationMethod(BISECTOR);
        float *outputs = fuzzy.evaluate(inputs);
        if (outputs != nullptr) {
            Serial.println("\nBISECTOR Method:");
            Serial.println("  output1 = " + String(outputs[0], 2));
            Serial.println("  output2 = " + String(outputs[1], 2));
            delete[] outputs;
        }

        demoStep++;
    } else if (demoStep == 2) {
        fuzzy.setDefuzzificationMethod(MOM);
        float *outputs = fuzzy.evaluate(inputs);
        if (outputs != nullptr) {
            Serial.println("\nMEAN OF MAXIMUM Method:");
            Serial.println("  output1 = " + String(outputs[0], 2));
            Serial.println("  output2 = " + String(outputs[1], 2));
            delete[] outputs;
        }

        demoStep++;
    } else if (demoStep == 3) {
        fuzzy.setDefuzzificationMethod(SOM);
        float *outputs = fuzzy.evaluate(inputs);
        if (outputs != nullptr) {
            Serial.println("\nSMALLEST OF MAXIMUM Method:");
            Serial.println("  output1 = " + String(outputs[0], 2));
            Serial.println("  output2 = " + String(outputs[1], 2));
            delete[] outputs;
        }

        demoStep++;
    } else if (demoStep == 4) {
        fuzzy.setDefuzzificationMethod(LOM);
        float *outputs = fuzzy.evaluate(inputs);
        if (outputs != nullptr) {
            Serial.println("\nLARGEST OF MAXIMUM Method:");
            Serial.println("  output1 = " + String(outputs[0], 2));
            Serial.println("  output2 = " + String(outputs[1], 2));
            delete[] outputs;
        }

        // Reset defuzzification method for next tests
        fuzzy.setDefuzzificationMethod(CENTROID);

        // Move to next demonstration
        demoStep = 0;
        currentState = ERROR_HANDLING;
    }
}

// Function for error handling demonstration
void demonstrateErrorHandling() {
    if (demoStep == 0) {
        Serial.println("\n3. ERROR HANDLING DEMONSTRATION");
        Serial.println("------------------------------");

        // Test 1: Add variable with invalid range
        Serial.println("Test 1: Adding variable with invalid range (min >= max)");
        if (!errorTest->addInputVariable("invalid", 100, 0)) {
            Serial.println("  Error caught: " + String(errorTest->getErrorMessage()));
        } else {
            Serial.println("  Failed to catch error");
        }

        // Reset for next test
        errorTest->clearError();

        demoStep++;
    } else if (demoStep == 1) {
        // Test 2: Adding a fuzzy set to a non-existent variable
        Serial.println("\nTest 2: Adding fuzzy set to non-existent variable");
        float params[3] = {0, 50, 100};
        if (!errorTest->addFuzzySet(5, true, "test", TRIANGULAR, params)) {
            Serial.println("  Error caught: " + String(errorTest->getErrorMessage()));
        } else {
            Serial.println("  Failed to catch error");
        }

        // Reset for next test
        errorTest->clearError();

        demoStep++;
    } else if (demoStep == 2) {
        // Set up a valid variable and set for rule testing
        errorTest->addInputVariable("valid", 0, 100);
        errorTest->addOutputVariable("output", 0, 100);

        float params[3] = {0, 50, 100};
        errorTest->addFuzzySet(0, true, "test", TRIANGULAR, params);
        errorTest->addFuzzySet(0, false, "result", TRIANGULAR, params);

        // Test 3: Add rule with invalid consequent variable
        Serial.println("\nTest 3: Adding rule with invalid consequent variable");
        int vars[1] = {0};
        int sets[1] = {0};
        if (!errorTest->addRule(vars, sets, 1, 5, 0, true)) {
            Serial.println("  Error caught: " + String(errorTest->getErrorMessage()));
        } else {
            Serial.println("  Failed to catch error");
        }

        // Reset for next test
        errorTest->clearError();

        demoStep++;
    } else if (demoStep == 3) {
        // Test 4: Evaluate with null input
        Serial.println("\nTest 4: Evaluating with null input");
        if (errorTest->evaluate(nullptr) == nullptr) {
            Serial.println("  Error caught: " + String(errorTest->getErrorMessage()));
        } else {
            Serial.println("  Failed to catch error");
        }

        // Reset for next test
        errorTest->clearError();

        demoStep++;
    } else if (demoStep == 4) {
        // Test 5: Evaluate with no rules defined
        Serial.println("\nTest 5: Evaluating system with no rules");
        float testInput[1] = {50};
        if (errorTest->evaluate(testInput) == nullptr) {
            Serial.println("  Error caught: " + String(errorTest->getErrorMessage()));
        } else {
            Serial.println("  Failed to catch error");
        }

        Serial.println("\nError handling demonstration complete");

        // Move to completion state
        demoStep = 0;
        currentState = DEMO_COMPLETE;
    }
}

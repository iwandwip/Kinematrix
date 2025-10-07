/*
 * FuzzySugeno_Basic_Usage.ino
 * 
 * Basic example demonstrating FuzzySugeno library usage
 * Created on: 2025. 3. 30
 */

#define ENABLE_MODULE_FUZZY_SUGENO
#include "Kinematrix.h"

// Create a FuzzySugeno instance with max parameters
// (maxInputs, maxOutputs, maxRules, maxSetsPerVar)
FuzzySugeno fuzzy(2, 1, 5, 3);

void setup() {
    Serial.begin(115200);
    Serial.println("FuzzySugeno Basic Example");
    Serial.println("-------------------------");

    // Initialize the system

    // 1. Define input variables
    fuzzy.addInputVariable("temperature", 0, 40);

    // 2. Define fuzzy sets for temperature
    float coldParams[] = {0, 0, 15};
    float normalParams[] = {10, 20, 30};
    float hotParams[] = {25, 40, 40};

    fuzzy.addFuzzySet(0, "cold", TRIANGULAR, coldParams);
    fuzzy.addFuzzySet(0, "normal", TRIANGULAR, normalParams);
    fuzzy.addFuzzySet(0, "hot", TRIANGULAR, hotParams);

    // 3. Define output constants
    fuzzy.addConstantOutput("heater_power", 0); // Will be overridden by rules

    // 4. Define rules
    // Rule 1: IF temperature IS cold THEN heater_power = 0.8
    int antecedentVars1[] = {0};
    int antecedentSets1[] = {0}; // cold
    fuzzy.addRule(antecedentVars1, antecedentSets1, 1, 0, true);
    fuzzy.addConstantOutput("cold_power", 0.8);

    // Rule 2: IF temperature IS normal THEN heater_power = 0.5
    int antecedentVars2[] = {0};
    int antecedentSets2[] = {1}; // normal
    fuzzy.addRule(antecedentVars2, antecedentSets2, 1, 1, true);
    fuzzy.addConstantOutput("normal_power", 0.5);

    // Rule 3: IF temperature IS hot THEN heater_power = 0.0
    int antecedentVars3[] = {0};
    int antecedentSets3[] = {2}; // hot
    fuzzy.addRule(antecedentVars3, antecedentSets3, 1, 2, true);
    fuzzy.addConstantOutput("hot_power", 0.0);

    // Check for any errors
    if (fuzzy.hasError()) {
        Serial.print("Error: ");
        Serial.println(fuzzy.getErrorMessage());
    } else {
        Serial.println("Fuzzy system initialized successfully!");
    }
}

void loop() {
    // Test with different temperature values
    float testTemps[] = {5, 15, 25, 35};

    for (int i = 0; i < 4; i++) {
        float temperature = testTemps[i];

        // Create input array
        float inputs[] = {temperature};

        // Evaluate fuzzy system
        float *outputs = fuzzy.evaluate(inputs);

        if (outputs != nullptr) {
            // Display results
            Serial.print("Temperature: ");
            Serial.print(temperature);
            Serial.print(" °C, Heater Power: ");
            Serial.println(outputs[0] * 100);

            // Clean up
            delete[] outputs;
        } else {
            Serial.print("Evaluation error: ");
            Serial.println(fuzzy.getErrorMessage());
        }

        delay(1000);
    }

    // Loop back and repeat
    Serial.println("--------------------");
    delay(2000);
}

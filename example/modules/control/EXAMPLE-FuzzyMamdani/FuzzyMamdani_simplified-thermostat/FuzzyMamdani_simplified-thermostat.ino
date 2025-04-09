/*
 * FuzzyMamdani_Simplified_Thermostat.ino
 * 
 * Simple thermostat control using fuzzy logic with
 * just one input (room temperature) and one output (heating power)
 * All code contained in setup() and loop() only
 */

#define ENABLE_MODULE_FUZZY_MAMDANI
#include "Kinematrix.h"

// Create a simple fuzzy system with:
// - 1 input (temperature)
// - 1 output (heating power)
// - 3 rules maximum
// - 3 fuzzy sets per variable
FuzzyMamdani fuzzy(1, 1, 3, 3);

// Test temperatures to cycle through
float testTemperatures[] = {17, 22, 26};
int currentTestIndex = 0;
unsigned long lastTestTime = 0;

void setup() {
  Serial.begin(115200);
  delay(1000);
  Serial.println("Simple Fuzzy Thermostat Example");
  
  // Add the temperature input variable (15-30°C range)
  fuzzy.addInputVariable("temperature", 15, 30);
  
  // Add the heating power output variable (0-100%)
  fuzzy.addOutputVariable("heating_power", 0, 100);
  
  // Add fuzzy sets for temperature
  float coldParams[3] = {15, 15, 21};
  float comfortParams[3] = {19, 22, 25};
  float warmParams[3] = {23, 30, 30};
  
  fuzzy.addFuzzySet(0, true, "cold", TRIANGULAR, coldParams);
  fuzzy.addFuzzySet(0, true, "comfort", TRIANGULAR, comfortParams);
  fuzzy.addFuzzySet(0, true, "warm", TRIANGULAR, warmParams);
  
  // Add fuzzy sets for heating power
  float highParams[3] = {70, 100, 100};
  float mediumParams[3] = {30, 50, 70};
  float lowParams[3] = {0, 0, 30};
  
  fuzzy.addFuzzySet(0, false, "high", TRIANGULAR, highParams);
  fuzzy.addFuzzySet(0, false, "medium", TRIANGULAR, mediumParams);
  fuzzy.addFuzzySet(0, false, "low", TRIANGULAR, lowParams);
  
  // Add rules
  // Rule 1: IF temperature is cold THEN heating power is high
  int vars1[1] = {0};
  int sets1[1] = {0}; // cold
  fuzzy.addRule(vars1, sets1, 1, 0, 0, true); // heating = high
  
  // Rule 2: IF temperature is comfort THEN heating power is medium
  int vars2[1] = {0};
  int sets2[1] = {1}; // comfort
  fuzzy.addRule(vars2, sets2, 1, 0, 1, true); // heating = medium
  
  // Rule 3: IF temperature is warm THEN heating power is low
  int vars3[1] = {0};
  int sets3[1] = {2}; // warm
  fuzzy.addRule(vars3, sets3, 1, 0, 2, true); // heating = low
  
  // Set defuzzification method
  fuzzy.setDefuzzificationMethod(CENTROID);
  
  if (fuzzy.hasError()) {
    Serial.print("Error: ");
    Serial.println(fuzzy.getErrorMessage());
    return;
  }
  
  Serial.println("Thermostat initialized!");
  Serial.println("Testing different temperatures:");
  
  lastTestTime = millis();
}

void loop() {
  // Run a new test every 3 seconds
  if (millis() - lastTestTime >= 3000) {
    // Get the next temperature to test
    float temperature = testTemperatures[currentTestIndex];
    currentTestIndex = (currentTestIndex + 1) % 3;
    lastTestTime = millis();
    
    // Print test header
    Serial.println("\n-----------------------");
    Serial.print("Room temperature: ");
    Serial.print(temperature);
    Serial.println("°C");
    
    // Evaluate with fuzzy logic
    float inputs[1] = {temperature};
    float* outputs = fuzzy.evaluate(inputs);
    
    if (outputs != nullptr) {
      // Print results
      Serial.print("Heating power: ");
      Serial.print(outputs[0]);
      Serial.println("%");
      
      // Determine heating status
      String status;
      if (outputs[0] > 70) {
        status = "HEATING HIGH";
      } else if (outputs[0] > 30) {
        status = "HEATING MEDIUM";
      } else {
        status = "HEATING LOW";
      }
      
      Serial.println("Status: " + status);
      
      delete[] outputs; // Free memory
    } else {
      Serial.print("Error: ");
      Serial.println(fuzzy.getErrorMessage());
    }
  }
}

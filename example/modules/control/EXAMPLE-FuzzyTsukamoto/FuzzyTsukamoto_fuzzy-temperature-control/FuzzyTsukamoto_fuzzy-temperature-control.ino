/*
 * FuzzyTsukamoto - Temperature Control Example
 * 
 * This example demonstrates a more advanced temperature control system
 * with both heating and cooling outputs
 */

#define ENABLE_MODULE_FUZZY_TSUKAMOTO
#include "Kinematrix.h"

// Pin definitions
#define TEMP_SENSOR_PIN A0
#define HEAT_PIN 9
#define COOL_PIN 10

// Global variables
FuzzyTsukamoto fuzzy(2, 2, 4, 3);
unsigned long lastPrintTime = 0;
float targetTemp = 22.0; // Target temperature in Celsius

void setup() {
  Serial.begin(9600);
  pinMode(HEAT_PIN, OUTPUT);
  pinMode(COOL_PIN, OUTPUT);
  
  // Add temperature input (0-40 degrees Celsius)
  fuzzy.addInputVariable("Temperature", 0.0, 40.0);
  
  // Add temperature error input (-20 to +20 degrees)
  fuzzy.addInputVariable("Error", -20.0, 20.0);
  
  // Define input sets for temperature
  float cold[] = {0.0, 0.0, 20.0};
  float comfortable[] = {15.0, 22.0, 30.0};
  float hot[] = {25.0, 40.0, 40.0};
  fuzzy.addFuzzySet(0, true, "Cold", TRIANGULAR, cold);
  fuzzy.addFuzzySet(0, true, "Comfortable", TRIANGULAR, comfortable);
  fuzzy.addFuzzySet(0, true, "Hot", TRIANGULAR, hot);
  
  // Define input sets for error
  float negative[] = {-20.0, -20.0, 0.0};
  float zero[] = {-5.0, 0.0, 5.0};
  float positive[] = {0.0, 20.0, 20.0};
  fuzzy.addFuzzySet(1, true, "Negative", TRIANGULAR, negative);
  fuzzy.addFuzzySet(1, true, "Zero", TRIANGULAR, zero);
  fuzzy.addFuzzySet(1, true, "Positive", TRIANGULAR, positive);
  
  // Add heating output (0-255 for PWM)
  fuzzy.addOutputVariable("HeatingPower", 0.0, 255.0);
  
  // Add cooling output (0-255 for PWM)
  fuzzy.addOutputVariable("CoolingPower", 0.0, 255.0);
  
  // Define output sets for heating power
  float heating_low[] = {255.0, 0.0};
  float heating_high[] = {0.0, 255.0};
  fuzzy.addFuzzySet(0, false, "LowHeat", MONOTONIC_DECREASING, heating_low);
  fuzzy.addFuzzySet(0, false, "HighHeat", MONOTONIC_INCREASING, heating_high);
  
  // Define output sets for cooling power
  float cooling_low[] = {255.0, 0.0};
  float cooling_high[] = {0.0, 255.0};
  fuzzy.addFuzzySet(1, false, "LowCool", MONOTONIC_DECREASING, cooling_low);
  fuzzy.addFuzzySet(1, false, "HighCool", MONOTONIC_INCREASING, cooling_high);
  
  // Add the rules
  
  // Rule 1: IF temp is cold OR error is negative THEN heating is high AND cooling is low
  int antecedentVars1[] = {0, 1};
  int antecedentSets1[] = {0, 0}; // Cold, Negative
  fuzzy.addRule(antecedentVars1, antecedentSets1, 2, 0, 1, false); // OR operator, HighHeat
  fuzzy.addRule(antecedentVars1, antecedentSets1, 2, 1, 0, false); // OR operator, LowCool
  
  // Rule 2: IF temp is hot OR error is positive THEN heating is low AND cooling is high
  int antecedentVars2[] = {0, 1};
  int antecedentSets2[] = {2, 2}; // Hot, Positive
  fuzzy.addRule(antecedentVars2, antecedentSets2, 2, 0, 0, false); // OR operator, LowHeat
  fuzzy.addRule(antecedentVars2, antecedentSets2, 2, 1, 1, false); // OR operator, HighCool
  
  Serial.println("Temperature control system initialized");
  Serial.println("Target temperature: 22.0°C");
}

void loop() {
  // Read temperature (simulated or from sensor)
  // For simulation, we'll use a value that changes over time
  float temperature = 15.0 + 15.0 * sin(millis() / 30000.0);
  
  // Calculate error (difference from target)
  float error = targetTemp - temperature;
  
  // Prepare input array
  float inputs[] = {temperature, error};
  
  // Evaluate the fuzzy system
  float* outputs = fuzzy.evaluate(inputs);
  
  if (outputs != nullptr) {
    // Apply the outputs to heating and cooling systems
    analogWrite(HEAT_PIN, (int)outputs[0]);
    analogWrite(COOL_PIN, (int)outputs[1]);
    
    // Print results every second
    if (millis() - lastPrintTime > 1000) {
      Serial.print("Temperature: ");
      Serial.print(temperature, 1);
      Serial.print("°C, Error: ");
      Serial.print(error, 1);
      Serial.print("°C → Heating: ");
      Serial.print(outputs[0], 1);
      Serial.print(", Cooling: ");
      Serial.println(outputs[1], 1);
      lastPrintTime = millis();
    }
    
    delete[] outputs; // Free the memory
  } else {
    if (fuzzy.hasError()) {
      Serial.print("Error: ");
      Serial.println(fuzzy.getErrorMessage());
    }
  }
  
  delay(100);
}

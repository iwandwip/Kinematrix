/*
 * FuzzyTsukamoto - Basic Usage Example
 * 
 * This example shows the most basic usage of the FuzzyTsukamoto library
 * with one input (temperature) and one output (fan speed)
 */

#define ENABLE_MODULE_FUZZY_TSUKAMOTO
#include "Kinematrix.h"

// Pin definitions
#define TEMP_SENSOR_PIN A0
#define FAN_PIN 9

// Global variables
FuzzyTsukamoto fuzzy(1, 1, 2, 2);
unsigned long lastPrintTime = 0;

void setup() {
  Serial.begin(9600);
  pinMode(FAN_PIN, OUTPUT);
  
  // Add temperature input (0-100 degrees)
  fuzzy.addInputVariable("Temperature", 0.0, 100.0);
  
  // Define input sets for temperature
  float cold[] = {0.0, 0.0, 50.0};
  float hot[] = {30.0, 100.0, 100.0};
  fuzzy.addFuzzySet(0, true, "Cold", TRIANGULAR, cold);
  fuzzy.addFuzzySet(0, true, "Hot", TRIANGULAR, hot);
  
  // Add fan speed output (0-255 for PWM)
  fuzzy.addOutputVariable("FanSpeed", 0.0, 255.0);
  
  // Define output sets for fan speed (must be monotonic for Tsukamoto)
  float low[] = {255.0, 0.0};  // Decreasing (high to low)
  float high[] = {0.0, 255.0}; // Increasing (low to high)
  fuzzy.addFuzzySet(0, false, "Low", MONOTONIC_DECREASING, low);
  fuzzy.addFuzzySet(0, false, "High", MONOTONIC_INCREASING, high);
  
  // Add the rules
  int antecedentVars[] = {0}; // Input variable index
  
  // Rule 1: IF temperature is cold THEN fan speed is low
  int antecedentSets1[] = {0}; // Cold
  fuzzy.addRule(antecedentVars, antecedentSets1, 1, 0, 0, true); // Output: Low
  
  // Rule 2: IF temperature is hot THEN fan speed is high
  int antecedentSets2[] = {1}; // Hot
  fuzzy.addRule(antecedentVars, antecedentSets2, 1, 0, 1, true); // Output: High
  
  Serial.println("Fuzzy system initialized");
  Serial.println("Temperature (0-100) -> Fan Speed (0-255)");
}

void loop() {
  // Read temperature (simulated or from sensor)
  // For simulation, we'll use a value that oscillates between 0-100
  float temperature = 50.0 + 50.0 * sin(millis() / 5000.0);
  
  // Prepare input array
  float inputs[] = {temperature};
  
  // Evaluate the fuzzy system
  float* outputs = fuzzy.evaluate(inputs);
  
  if (outputs != nullptr) {
    // Apply the output to the fan
    analogWrite(FAN_PIN, (int)outputs[0]);
    
    // Print results every second
    if (millis() - lastPrintTime > 1000) {
      Serial.print("Temperature: ");
      Serial.print(temperature);
      Serial.print(" → Fan Speed: ");
      Serial.println(outputs[0]);
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

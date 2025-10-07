/*
 * FuzzyTsukamoto - Water Level Controller Example
 * 
 * This example demonstrates a water level controller that manages
 * a pump based on water level and its rate of change
 */

#define ENABLE_MODULE_FUZZY_TSUKAMOTO
#include "Kinematrix.h"

// Pin definitions
#define WATER_SENSOR_PIN A0
#define PUMP_PIN 9

// Global variables
FuzzyTsukamoto fuzzy(2, 1, 5, 3);
unsigned long lastPrintTime = 0;
unsigned long lastMeasureTime = 0;
float previousLevel = 0;
float rateOfChange = 0;

void setup() {
  Serial.begin(9600);
  pinMode(PUMP_PIN, OUTPUT);
  
  // Add water level input (0-100%)
  fuzzy.addInputVariable("WaterLevel", 0.0, 100.0);
  
  // Add rate of change input (-10 to +10 % per second)
  fuzzy.addInputVariable("RateOfChange", -10.0, 10.0);
  
  // Define input sets for water level
  float low[] = {0.0, 0.0, 40.0};
  float medium[] = {30.0, 50.0, 70.0};
  float high[] = {60.0, 100.0, 100.0};
  fuzzy.addFuzzySet(0, true, "Low", TRIANGULAR, low);
  fuzzy.addFuzzySet(0, true, "Medium", TRIANGULAR, medium);
  fuzzy.addFuzzySet(0, true, "High", TRIANGULAR, high);
  
  // Define input sets for rate of change
  float falling[] = {-10.0, -10.0, -1.0};
  float stable[] = {-2.0, 0.0, 2.0};
  float rising[] = {1.0, 10.0, 10.0};
  fuzzy.addFuzzySet(1, true, "Falling", TRIANGULAR, falling);
  fuzzy.addFuzzySet(1, true, "Stable", TRIANGULAR, stable);
  fuzzy.addFuzzySet(1, true, "Rising", TRIANGULAR, rising);
  
  // Add pump power output (0-255 for PWM)
  fuzzy.addOutputVariable("PumpPower", 0.0, 255.0);
  
  // Define output sets for pump power (must be monotonic for Tsukamoto)
  float off[] = {255.0, 0.0};
  float medium_power[] = {0.0, 127.0};
  float full_power[] = {127.0, 255.0};
  fuzzy.addFuzzySet(0, false, "Off", MONOTONIC_DECREASING, off);
  fuzzy.addFuzzySet(0, false, "Medium", MONOTONIC_INCREASING, medium_power);
  fuzzy.addFuzzySet(0, false, "Full", MONOTONIC_INCREASING, full_power);
  
  // Add the rules
  
  // Rule 1: IF level is high THEN pump is full power
  int antecedentVars1[] = {0};
  int antecedentSets1[] = {2}; // High
  fuzzy.addRule(antecedentVars1, antecedentSets1, 1, 0, 2, true); // Full
  
  // Rule 2: IF level is medium AND rate is stable THEN pump is medium power
  int antecedentVars2[] = {0, 1};
  int antecedentSets2[] = {1, 1}; // Medium, Stable
  fuzzy.addRule(antecedentVars2, antecedentSets2, 2, 0, 1, true); // Medium
  
  // Rule 3: IF level is medium AND rate is rising THEN pump is full power
  int antecedentVars3[] = {0, 1};
  int antecedentSets3[] = {1, 2}; // Medium, Rising
  fuzzy.addRule(antecedentVars3, antecedentSets3, 2, 0, 2, true); // Full
  
  // Rule 4: IF level is low THEN pump is off
  int antecedentVars4[] = {0};
  int antecedentSets4[] = {0}; // Low
  fuzzy.addRule(antecedentVars4, antecedentSets4, 1, 0, 0, true); // Off
  
  // Rule 5: IF level is medium AND rate is falling THEN pump is off
  int antecedentVars5[] = {0, 1};
  int antecedentSets5[] = {1, 0}; // Medium, Falling
  fuzzy.addRule(antecedentVars5, antecedentSets5, 2, 0, 0, true); // Off
  
  Serial.println("Water level controller initialized");
  lastMeasureTime = millis();
}

void loop() {
  // Simulated water level (oscillates between 20% and 80%)
  float waterLevel = 50.0 + 30.0 * sin(millis() / 15000.0);
  
  // Calculate rate of change
  unsigned long currentTime = millis();
  if (currentTime - lastMeasureTime >= 1000) {
    float elapsed = (currentTime - lastMeasureTime) / 1000.0;
    rateOfChange = (waterLevel - previousLevel) / elapsed;
    previousLevel = waterLevel;
    lastMeasureTime = currentTime;
  }
  
  // Prepare input array
  float inputs[] = {waterLevel, rateOfChange};
  
  // Evaluate the fuzzy system
  float* outputs = fuzzy.evaluate(inputs);
  
  if (outputs != nullptr) {
    // Apply the output to pump
    analogWrite(PUMP_PIN, (int)outputs[0]);
    
    // Print results every second
    if (millis() - lastPrintTime > 1000) {
      Serial.print("Water Level: ");
      Serial.print(waterLevel, 1);
      Serial.print("%, Rate: ");
      Serial.print(rateOfChange, 2);
      Serial.print("%/s → Pump Power: ");
      Serial.println(outputs[0], 1);
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

/*
 * FuzzyTsukamoto - Light Controller Example
 * 
 * This example demonstrates an adaptive lighting controller
 * that adjusts LED brightness based on ambient light and time of day
 */

#define ENABLE_MODULE_FUZZY_TSUKAMOTO
#include "Kinematrix.h"

// Pin definitions
#define LIGHT_SENSOR_PIN A0
#define LED_PIN 9

// Global variables
FuzzyTsukamoto fuzzy(2, 1, 4, 3);
unsigned long lastPrintTime = 0;
unsigned long startTime;

void setup() {
  Serial.begin(9600);
  pinMode(LED_PIN, OUTPUT);
  startTime = millis();
  
  // Add ambient light input (0-1023 from analog read)
  fuzzy.addInputVariable("AmbientLight", 0.0, 1023.0);
  
  // Add time of day input (0-24 hours)
  fuzzy.addInputVariable("TimeOfDay", 0.0, 24.0);
  
  // Define input sets for ambient light
  float dark[] = {0.0, 0.0, 400.0};
  float medium[] = {300.0, 600.0, 800.0};
  float bright[] = {700.0, 1023.0, 1023.0};
  fuzzy.addFuzzySet(0, true, "Dark", TRIANGULAR, dark);
  fuzzy.addFuzzySet(0, true, "Medium", TRIANGULAR, medium);
  fuzzy.addFuzzySet(0, true, "Bright", TRIANGULAR, bright);
  
  // Define input sets for time of day
  float morning[] = {5.0, 8.0, 12.0};
  float afternoon[] = {11.0, 15.0, 19.0};
  float night[] = {18.0, 22.0, 5.0}; // Note: wraps around midnight
  fuzzy.addFuzzySet(1, true, "Morning", TRIANGULAR, morning);
  fuzzy.addFuzzySet(1, true, "Afternoon", TRIANGULAR, afternoon);
  fuzzy.addFuzzySet(1, true, "Night", TRIANGULAR, night);
  
  // Add LED brightness output (0-255 for PWM)
  fuzzy.addOutputVariable("Brightness", 0.0, 255.0);
  
  // Define output sets for LED brightness
  float dim[] = {255.0, 0.0};
  float medium_bright[] = {0.0, 127.0};
  float full[] = {127.0, 255.0};
  fuzzy.addFuzzySet(0, false, "Dim", MONOTONIC_DECREASING, dim);
  fuzzy.addFuzzySet(0, false, "Medium", MONOTONIC_INCREASING, medium_bright);
  fuzzy.addFuzzySet(0, false, "Full", MONOTONIC_INCREASING, full);
  
  // Add the rules
  
  // Rule 1: IF ambient is dark OR time is night THEN brightness is full
  int antecedentVars1[] = {0, 1};
  int antecedentSets1[] = {0, 2}; // Dark, Night
  fuzzy.addRule(antecedentVars1, antecedentSets1, 2, 0, 2, false); // OR operator, Full
  
  // Rule 2: IF ambient is medium AND time is afternoon THEN brightness is medium
  int antecedentVars2[] = {0, 1};
  int antecedentSets2[] = {1, 1}; // Medium, Afternoon
  fuzzy.addRule(antecedentVars2, antecedentSets2, 2, 0, 1, true); // AND operator, Medium
  
  // Rule 3: IF ambient is bright AND time is morning THEN brightness is dim
  int antecedentVars3[] = {0, 1};
  int antecedentSets3[] = {2, 0}; // Bright, Morning
  fuzzy.addRule(antecedentVars3, antecedentSets3, 2, 0, 0, true); // AND operator, Dim
  
  // Rule 4: IF ambient is bright AND time is afternoon THEN brightness is dim
  int antecedentVars4[] = {0, 1};
  int antecedentSets4[] = {2, 1}; // Bright, Afternoon
  fuzzy.addRule(antecedentVars4, antecedentSets4, 2, 0, 0, true); // AND operator, Dim
  
  Serial.println("Adaptive lighting controller initialized");
}

void loop() {
  // Read ambient light
  float ambientLight = analogRead(LIGHT_SENSOR_PIN);
  
  // For simulation, use a value that changes
  if (!analogRead(LIGHT_SENSOR_PIN)) {
    ambientLight = 500.0 + 400.0 * sin(millis() / 10000.0);
  }
  
  // Get time of day (for simulation, cycle through 24 hours in 2 minutes)
  float timeOfDay = ((millis() - startTime) % 120000) / 5000.0;
  
  // Prepare input array
  float inputs[] = {ambientLight, timeOfDay};
  
  // Evaluate the fuzzy system
  float* outputs = fuzzy.evaluate(inputs);
  
  if (outputs != nullptr) {
    // Apply the output to LED
    analogWrite(LED_PIN, (int)outputs[0]);
    
    // Print results every second
    if (millis() - lastPrintTime > 1000) {
      Serial.print("Ambient Light: ");
      Serial.print(ambientLight, 1);
      Serial.print(", Time: ");
      Serial.print(timeOfDay, 1);
      Serial.print("h → LED Brightness: ");
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

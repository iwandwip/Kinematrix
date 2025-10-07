/*
 * FuzzyMamdani_Simplified_Light_Controller.ino
 * 
 * Simple automatic light controller using fuzzy logic
 * Adjusts LED brightness based on ambient light level
 * All code contained in setup() and loop() only
 */

#define ENABLE_MODULE_FUZZY_MAMDANI
#include "Kinematrix.h"

// Create a simple fuzzy system with:
// - 1 input (ambient light)
// - 1 output (LED brightness)
// - 3 rules maximum
// - 3 fuzzy sets per variable
FuzzyMamdani fuzzy(1, 1, 3, 3);

const int LIGHT_SENSOR_PIN = A0;  // Ambient light sensor on analog pin A0
const int LED_PIN = 9;            // LED on PWM pin 9

// For simulation, predefined values
int testValues[] = {10, 40, 75, 90, 60, 30};
int testIndex = 0;
unsigned long lastTestTime = 0;

void setup() {
  Serial.begin(115200);
  delay(1000);
  
  // Setup pins
  pinMode(LIGHT_SENSOR_PIN, INPUT);
  pinMode(LED_PIN, OUTPUT);
  
  Serial.println("Simple Fuzzy Light Controller Example");
  
  // Add input variable: ambient light level (0-100%)
  fuzzy.addInputVariable("ambient_light", 0, 100);
  
  // Add output variable: LED brightness (0-100%)
  fuzzy.addOutputVariable("led_brightness", 0, 100);
  
  // Add fuzzy sets for ambient light
  float darkParams[3] = {0, 0, 30};
  float mediumParams[3] = {20, 50, 80};
  float brightParams[3] = {70, 100, 100};
  
  fuzzy.addFuzzySet(0, true, "dark", TRIANGULAR, darkParams);
  fuzzy.addFuzzySet(0, true, "medium", TRIANGULAR, mediumParams);
  fuzzy.addFuzzySet(0, true, "bright", TRIANGULAR, brightParams);
  
  // Add fuzzy sets for LED brightness
  float highParams[3] = {70, 100, 100};
  float moderateParams[3] = {30, 50, 70};
  float lowParams[3] = {0, 0, 30};
  
  fuzzy.addFuzzySet(0, false, "high", TRIANGULAR, highParams);
  fuzzy.addFuzzySet(0, false, "moderate", TRIANGULAR, moderateParams);
  fuzzy.addFuzzySet(0, false, "low", TRIANGULAR, lowParams);
  
  // Add simple rules
  // Rule 1: IF ambient_light is dark THEN led_brightness is high
  int vars1[1] = {0};
  int sets1[1] = {0}; // dark
  fuzzy.addRule(vars1, sets1, 1, 0, 0, true); // brightness = high
  
  // Rule 2: IF ambient_light is medium THEN led_brightness is moderate
  int vars2[1] = {0};
  int sets2[1] = {1}; // medium
  fuzzy.addRule(vars2, sets2, 1, 0, 1, true); // brightness = moderate
  
  // Rule 3: IF ambient_light is bright THEN led_brightness is low
  int vars3[1] = {0};
  int sets3[1] = {2}; // bright
  fuzzy.addRule(vars3, sets3, 1, 0, 2, true); // brightness = low
  
  // Set defuzzification method
  fuzzy.setDefuzzificationMethod(CENTROID);
  
  if (fuzzy.hasError()) {
    Serial.print("Error: ");
    Serial.println(fuzzy.getErrorMessage());
    return;
  }
  
  Serial.println("Light controller initialized!");
  lastTestTime = millis();
}

void loop() {
  // Check if 2 seconds have passed to run the next test
  if (millis() - lastTestTime >= 2000) {
    lastTestTime = millis();
    
    // Get the next test value
    float lightPercent = testValues[testIndex];
    testIndex = (testIndex + 1) % 6;
    
    // In a real application, you would use:
    // int rawValue = analogRead(LIGHT_SENSOR_PIN);
    // float lightPercent = map(rawValue, 0, 1023, 0, 100);
    
    // Run the fuzzy logic controller
    float inputs[1] = {lightPercent};
    float* outputs = fuzzy.evaluate(inputs);
    
    if (outputs != nullptr) {
      float ledBrightness = outputs[0];
      
      // Set LED brightness (commented out for simulation)
      // analogWrite(LED_PIN, map(ledBrightness, 0, 100, 0, 255));
      
      // Print information
      Serial.println("\n-----------------------");
      Serial.print("Ambient light: ");
      Serial.print(lightPercent);
      Serial.println("%");
      
      Serial.print("LED brightness: ");
      Serial.print(ledBrightness);
      Serial.println("%");
      
      // Determine light conditions description
      String lightDescription;
      if (lightPercent < 30) {
        lightDescription = "DARK";
      } else if (lightPercent < 70) {
        lightDescription = "MEDIUM";
      } else {
        lightDescription = "BRIGHT";
      }
      
      Serial.println("Current conditions: " + lightDescription);
      
      delete[] outputs; // Free memory
    } else {
      Serial.print("Error: ");
      Serial.println(fuzzy.getErrorMessage());
    }
  }
}

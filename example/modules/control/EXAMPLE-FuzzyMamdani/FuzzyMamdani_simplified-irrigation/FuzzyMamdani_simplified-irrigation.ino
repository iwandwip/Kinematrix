/*
 * FuzzyMamdani_Simplified_Irrigation.ino
 * 
 * Example of using the FuzzyMamdani library for an irrigation system
 * All code contained in setup() and loop() only
 */

#define ENABLE_MODULE_FUZZY_MAMDANI
#include "Kinematrix.h"

// Create a fuzzy system
// - 2 input variables (soil moisture, temperature)
// - 1 output variable (watering duration)
// - 9 rules maximum (3x3 rule matrix)
// - 3 fuzzy sets per variable
FuzzyMamdani fuzzy(2, 1, 9, 3);

// Test cases for the irrigation system
struct TestCase {
  float moisture;
  float temperature;
  const char* description;
};

// Define test cases
TestCase testCases[] = {
  {10, 35, "Very dry soil, hot day"},
  {20, 25, "Dry soil, mild day"},
  {30, 10, "Somewhat dry soil, cold day"},
  {50, 30, "Medium moisture, warm day"},
  {70, 20, "Moist soil, mild day"},
  {90, 15, "Wet soil, cool day"}
};

int currentTestIndex = 0;
unsigned long lastTestTime = 0;

void setup() {
  Serial.begin(115200);
  delay(2000);
  Serial.println("Fuzzy Logic Irrigation Controller");
  Serial.println("--------------------------------");
  
  // Add input variables
  fuzzy.addInputVariable("moisture", 0, 100);  // Soil moisture as percentage
  fuzzy.addInputVariable("temperature", 0, 50); // Temperature in Celsius
  
  // Add output variable
  fuzzy.addOutputVariable("duration", 0, 60);  // Watering duration in minutes
  
  // Check for any initialization errors
  if (fuzzy.hasError()) {
    Serial.print("Error during initialization: ");
    Serial.println(fuzzy.getErrorMessage());
    return;
  }
  
  // Add fuzzy sets for soil moisture (using triangular membership functions)
  float dryParams[3] = {0, 0, 40};
  float moistParams[3] = {20, 50, 80};
  float wetParams[3] = {60, 100, 100};
  
  fuzzy.addFuzzySet(0, true, "dry", TRIANGULAR, dryParams);
  fuzzy.addFuzzySet(0, true, "moist", TRIANGULAR, moistParams);
  fuzzy.addFuzzySet(0, true, "wet", TRIANGULAR, wetParams);
  
  // Add fuzzy sets for temperature (using different membership function types)
  float coldParams[3] = {0, 0, 15};         // Triangular
  float mildParams[4] = {10, 20, 30, 40};   // Trapezoidal
  float hotParams[2] = {30, 5};             // Gaussian (mean=30, sigma=5)
  
  fuzzy.addFuzzySet(1, true, "cold", TRIANGULAR, coldParams);
  fuzzy.addFuzzySet(1, true, "mild", TRAPEZOIDAL, mildParams);
  fuzzy.addFuzzySet(1, true, "hot", GAUSSIAN, hotParams);
  
  // Add fuzzy sets for watering duration
  float shortParams[3] = {0, 0, 15};
  float mediumParams[3] = {10, 30, 50};
  float longParams[3] = {40, 60, 60};
  
  fuzzy.addFuzzySet(0, false, "short", TRIANGULAR, shortParams);
  fuzzy.addFuzzySet(0, false, "medium", TRIANGULAR, mediumParams);
  fuzzy.addFuzzySet(0, false, "long", TRIANGULAR, longParams);
  
  // Check for any errors during set definition
  if (fuzzy.hasError()) {
    Serial.print("Error during fuzzy set definition: ");
    Serial.println(fuzzy.getErrorMessage());
    return;
  }
  
  // Add fuzzy rules
  
  // Rule 1: IF soil is dry AND temperature is hot THEN watering duration is long
  int vars1[2] = {0, 1};
  int sets1[2] = {0, 2}; // dry, hot
  fuzzy.addRule(vars1, sets1, 2, 0, 2, true); // true = AND
  
  // Rule 2: IF soil is dry AND temperature is mild THEN watering duration is medium
  int vars2[2] = {0, 1};
  int sets2[2] = {0, 1}; // dry, mild
  fuzzy.addRule(vars2, sets2, 2, 0, 1, true); // true = AND
  
  // Rule 3: IF soil is dry AND temperature is cold THEN watering duration is medium
  int vars3[2] = {0, 1};
  int sets3[2] = {0, 0}; // dry, cold
  fuzzy.addRule(vars3, sets3, 2, 0, 1, true); // true = AND
  
  // Rule 4: IF soil is moist AND temperature is hot THEN watering duration is medium
  int vars4[2] = {0, 1};
  int sets4[2] = {1, 2}; // moist, hot
  fuzzy.addRule(vars4, sets4, 2, 0, 1, true); // true = AND
  
  // Rule 5: IF soil is moist AND temperature is mild THEN watering duration is short
  int vars5[2] = {0, 1};
  int sets5[2] = {1, 1}; // moist, mild
  fuzzy.addRule(vars5, sets5, 2, 0, 0, true); // true = AND
  
  // Rule 6: IF soil is moist AND temperature is cold THEN watering duration is short
  int vars6[2] = {0, 1};
  int sets6[2] = {1, 0}; // moist, cold
  fuzzy.addRule(vars6, sets6, 2, 0, 0, true); // true = AND
  
  // Rule 7: IF soil is wet THEN watering duration is short (regardless of temperature)
  int vars7[1] = {0};
  int sets7[1] = {2}; // wet
  fuzzy.addRule(vars7, sets7, 1, 0, 0, true);
  
  // Rule 8: IF temperature is hot OR soil is dry THEN watering duration is at least medium
  int vars8[2] = {1, 0};
  int sets8[2] = {2, 0}; // hot, dry
  fuzzy.addRule(vars8, sets8, 2, 0, 1, false); // false = OR
  
  // Check for any errors during rule definition
  if (fuzzy.hasError()) {
    Serial.print("Error during rule definition: ");
    Serial.println(fuzzy.getErrorMessage());
    return;
  }
  
  // Set defuzzification method
  fuzzy.setDefuzzificationMethod(CENTROID);
  
  Serial.println("Fuzzy irrigation controller initialized successfully");
  Serial.println("Rules defined: " + String(fuzzy.getRuleCount()));
  Serial.println("\nTesting with various soil conditions:");
  
  lastTestTime = millis();
}

void loop() {
  // Run a new test case every 4 seconds
  if (millis() - lastTestTime >= 4000) {
    lastTestTime = millis();
    
    // Get current test case
    TestCase currentTest = testCases[currentTestIndex];
    
    // Move to next test case for next iteration
    currentTestIndex = (currentTestIndex + 1) % (sizeof(testCases) / sizeof(TestCase));
    
    // Print test case information
    Serial.println("\n--- " + String(currentTest.description) + " ---");
    Serial.println("Soil Moisture: " + String(currentTest.moisture, 1) + "%");
    Serial.println("Temperature: " + String(currentTest.temperature, 1) + "°C");
    
    // Create input array and evaluate the fuzzy system
    float inputs[2] = {currentTest.moisture, currentTest.temperature};
    float* outputs = fuzzy.evaluate(inputs);
    
    if (outputs != nullptr) {
      float duration = outputs[0];
      
      Serial.println("Watering Duration: " + String(duration, 1) + " minutes");
      
      // Provide a human-readable interpretation
      String interpretation;
      if (duration < 10) {
        interpretation = "Minimal watering needed";
      } else if (duration < 25) {
        interpretation = "Light watering recommended";
      } else if (duration < 45) {
        interpretation = "Moderate watering needed";
      } else {
        interpretation = "Heavy watering required";
      }
      
      Serial.println("Recommendation: " + interpretation);
      
      // Calculate water usage (example: 2 liters per minute)
      float waterUsage = duration * 2.0;
      Serial.println("Estimated water usage: " + String(waterUsage, 1) + " liters");
      
      delete[] outputs; // Free the allocated memory
    } else {
      Serial.print("Error during evaluation: ");
      Serial.println(fuzzy.getErrorMessage());
    }
  }
  
  // Add a little wait to avoid busy-looping
  delay(100);
}

/*
 * FuzzyMamdani_Simplified_Fan_Control.ino
 * 
 * Simple fan speed controller based on temperature and humidity
 * All code contained in setup() and loop() only
 */

#define ENABLE_MODULE_FUZZY_MAMDANI
#include "Kinematrix.h"

// Create a fuzzy system with minimal configuration:
// - 2 inputs (temperature, humidity)
// - 1 output (fan speed)
// - 4 rules maximum
// - 2 fuzzy sets per variable
FuzzyMamdani fuzzy(2, 1, 4, 2);

// Test conditions to cycle through
float testTemperatures[] = {22, 22, 30, 32};
float testHumidities[] = {40, 75, 40, 80};
int currentTestIndex = 0;
unsigned long lastTestTime = 0;

void setup() {
  Serial.begin(115200);
  delay(1000);
  Serial.println("Simple Fan Speed Controller Example");
  
  // Add input variables
  fuzzy.addInputVariable("temperature", 15, 35);  // 15-35°C
  fuzzy.addInputVariable("humidity", 20, 90);     // 20-90%
  
  // Add output variable
  fuzzy.addOutputVariable("fan_speed", 0, 100);   // 0-100%
  
  // Define simple fuzzy sets (just low/high for each variable)
  // Temperature sets
  float coolParams[3] = {15, 15, 25};
  float hotParams[3] = {25, 35, 35};
  
  fuzzy.addFuzzySet(0, true, "cool", TRIANGULAR, coolParams);
  fuzzy.addFuzzySet(0, true, "hot", TRIANGULAR, hotParams);
  
  // Humidity sets
  float dryParams[3] = {20, 20, 55};
  float humidParams[3] = {55, 90, 90};
  
  fuzzy.addFuzzySet(1, true, "dry", TRIANGULAR, dryParams);
  fuzzy.addFuzzySet(1, true, "humid", TRIANGULAR, humidParams);
  
  // Fan speed sets
  float lowSpeedParams[3] = {0, 0, 50};
  float highSpeedParams[3] = {50, 100, 100};
  
  fuzzy.addFuzzySet(0, false, "low", TRIANGULAR, lowSpeedParams);
  fuzzy.addFuzzySet(0, false, "high", TRIANGULAR, highSpeedParams);
  
  // Define rules for fan speed
  
  // Rule 1: IF temperature is cool AND humidity is dry THEN fan_speed is low
  int vars1[2] = {0, 1};
  int sets1[2] = {0, 0}; // cool, dry
  fuzzy.addRule(vars1, sets1, 2, 0, 0, true); // fan_speed = low, AND operator
  
  // Rule 2: IF temperature is hot THEN fan_speed is high
  int vars2[1] = {0};
  int sets2[1] = {1}; // hot
  fuzzy.addRule(vars2, sets2, 1, 0, 1, true); // fan_speed = high
  
  // Rule 3: IF humidity is humid THEN fan_speed is high
  int vars3[1] = {1};
  int sets3[1] = {1}; // humid
  fuzzy.addRule(vars3, sets3, 1, 0, 1, true); // fan_speed = high
  
  // Set defuzzification method
  fuzzy.setDefuzzificationMethod(CENTROID);
  
  if (fuzzy.hasError()) {
    Serial.print("Error: ");
    Serial.println(fuzzy.getErrorMessage());
    return;
  }
  
  Serial.println("Fan controller initialized!");
  Serial.println("Testing with different temperature and humidity combinations:");
  
  lastTestTime = millis();
}

void loop() {
  // Run a new test every 3 seconds
  if (millis() - lastTestTime >= 3000) {
    lastTestTime = millis();
    
    // Get the next set of test values
    float temperature = testTemperatures[currentTestIndex];
    float humidity = testHumidities[currentTestIndex];
    
    // Descriptive labels for test cases
    String testLabels[] = {
      "Cool and dry",
      "Cool but humid",
      "Hot but dry",
      "Hot and humid"
    };
    
    // Move to next test
    String currentTest = testLabels[currentTestIndex];
    currentTestIndex = (currentTestIndex + 1) % 4;
    
    // Print test header
    Serial.println("\n-----------------------");
    Serial.println("Test case: " + currentTest);
    Serial.print("Temperature: ");
    Serial.print(temperature);
    Serial.println("°C");
    
    Serial.print("Humidity: ");
    Serial.print(humidity);
    Serial.println("%");
    
    // Run the fuzzy controller
    float inputs[2] = {temperature, humidity};
    float* outputs = fuzzy.evaluate(inputs);
    
    if (outputs != nullptr) {
      // Print results
      Serial.print("Fan speed: ");
      Serial.print(outputs[0]);
      Serial.println("%");
      
      // Simple interpretation
      if (outputs[0] < 25) {
        Serial.println("Fan is barely running");
      } else if (outputs[0] < 50) {
        Serial.println("Fan is running at low speed");
      } else if (outputs[0] < 75) {
        Serial.println("Fan is running at medium speed");
      } else {
        Serial.println("Fan is running at high speed");
      }
      
      delete[] outputs; // Free memory
    } else {
      Serial.print("Error: ");
      Serial.println(fuzzy.getErrorMessage());
    }
  }
}

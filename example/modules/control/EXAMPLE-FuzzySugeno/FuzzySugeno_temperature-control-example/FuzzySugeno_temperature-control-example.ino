/*
 * FuzzySugeno_Temperature_Control.ino
 * 
 * Thermostat controller example with FuzzySugeno
 * Created on: 2025. 3. 30
 */

#define ENABLE_MODULE_FUZZY_SUGENO
#include "Kinematrix.h"

// Create a FuzzySugeno instance
FuzzySugeno thermostat(2, 2, 8, 3);

// Pins for simulation
const int tempSensorPin = A0;
const int heaterPin = 9;
const int coolerPin = 10;

// Target temperature
const float targetTemp = 22.0;

void setup() {
  Serial.begin(115200);
  Serial.println("FuzzySugeno Temperature Controller");
  Serial.println("----------------------------------");
  
  // Setup pins
  pinMode(heaterPin, OUTPUT);
  pinMode(coolerPin, OUTPUT);
  
  // 1. Define input variables
  thermostat.addInputVariable("temperature", 0, 40);
  thermostat.addInputVariable("temp_error", -20, 20);  // Difference from target
  
  // 2. Define fuzzy sets for temperature
  float coldParams[] = {0, 0, 18};
  float comfortParams[] = {16, 22, 28};
  float hotParams[] = {26, 40, 40};
  
  thermostat.addFuzzySet(0, "cold", TRIANGULAR, coldParams);
  thermostat.addFuzzySet(0, "comfort", TRIANGULAR, comfortParams);
  thermostat.addFuzzySet(0, "hot", TRIANGULAR, hotParams);
  
  // 3. Define fuzzy sets for temperature error
  float negErrorParams[] = {-20, -20, -2};
  float zeroErrorParams[] = {-5, 0, 5};
  float posErrorParams[] = {2, 20, 20};
  
  thermostat.addFuzzySet(1, "negative", TRIANGULAR, negErrorParams);
  thermostat.addFuzzySet(1, "zero", TRIANGULAR, zeroErrorParams);
  thermostat.addFuzzySet(1, "positive", TRIANGULAR, posErrorParams);
  
  // 4. Define outputs
  thermostat.addConstantOutput("heater_off", 0.0);
  thermostat.addConstantOutput("heater_low", 0.3);
  thermostat.addConstantOutput("heater_medium", 0.6);
  thermostat.addConstantOutput("heater_high", 1.0);
  
  thermostat.addConstantOutput("cooler_off", 0.0);
  thermostat.addConstantOutput("cooler_low", 0.3);
  thermostat.addConstantOutput("cooler_medium", 0.6);
  thermostat.addConstantOutput("cooler_high", 1.0);
  
  // 5. Define rules for heater control
  // Rule 1: IF temp IS cold AND error IS negative THEN heater = high
  int rule1Vars[] = {0, 1};
  int rule1Sets[] = {0, 0}; // cold, negative
  thermostat.addRule(rule1Vars, rule1Sets, 2, 3, true); // heater_high
  
  // Rule 2: IF temp IS cold AND error IS zero THEN heater = medium
  int rule2Vars[] = {0, 1};
  int rule2Sets[] = {0, 1}; // cold, zero
  thermostat.addRule(rule2Vars, rule2Sets, 2, 2, true); // heater_medium
  
  // Rule 3: IF temp IS comfort AND error IS negative THEN heater = low
  int rule3Vars[] = {0, 1};
  int rule3Sets[] = {1, 0}; // comfort, negative
  thermostat.addRule(rule3Vars, rule3Sets, 2, 1, true); // heater_low
  
  // Rule 4: IF temp IS comfort AND error IS zero THEN heater = off
  int rule4Vars[] = {0, 1};
  int rule4Sets[] = {1, 1}; // comfort, zero
  thermostat.addRule(rule4Vars, rule4Sets, 2, 0, true); // heater_off
  
  // 6. Define rules for cooler control
  // Rule 5: IF temp IS hot AND error IS positive THEN cooler = high
  int rule5Vars[] = {0, 1};
  int rule5Sets[] = {2, 2}; // hot, positive
  thermostat.addRule(rule5Vars, rule5Sets, 2, 7, true); // cooler_high
  
  // Rule 6: IF temp IS hot AND error IS zero THEN cooler = medium
  int rule6Vars[] = {0, 1};
  int rule6Sets[] = {2, 1}; // hot, zero
  thermostat.addRule(rule6Vars, rule6Sets, 2, 6, true); // cooler_medium
  
  // Rule 7: IF temp IS comfort AND error IS positive THEN cooler = low
  int rule7Vars[] = {0, 1};
  int rule7Sets[] = {1, 2}; // comfort, positive
  thermostat.addRule(rule7Vars, rule7Sets, 2, 5, true); // cooler_low
  
  // Rule 8: IF temp IS cold AND error IS positive THEN cooler = off
  int rule8Vars[] = {0, 1};
  int rule8Sets[] = {0, 2}; // cold, positive
  thermostat.addRule(rule8Vars, rule8Sets, 2, 4, true); // cooler_off
  
  if (thermostat.hasError()) {
    Serial.print("Error: ");
    Serial.println(thermostat.getErrorMessage());
  } else {
    Serial.println("Thermostat initialized successfully!");
  }
}

void loop() {
  // Read temperature (simulated or from sensor)
  // For simulation, we'll use a pattern:
  static float currentTemp = 18.0;
  static int direction = 1;
  
  // Simulate temperature readings
  currentTemp += direction * 0.5;
  if (currentTemp > 30) direction = -1;
  if (currentTemp < 15) direction = 1;
  
  // Calculate error
  float tempError = targetTemp - currentTemp;
  
  // Input array for fuzzy system
  float inputs[] = {currentTemp, tempError};
  
  // Evaluate fuzzy system
  float* outputs = thermostat.evaluate(inputs);
  
  if (outputs != nullptr) {
    float heaterPower = outputs[0];
    float coolerPower = outputs[1];
    
    // Apply to outputs (PWM or relay control)
    analogWrite(heaterPin, (int)(heaterPower * 255));
    analogWrite(coolerPin, (int)(coolerPower * 255));
    
    // Display results
    Serial.print("Temperature: ");
    Serial.print(currentTemp);
    Serial.print(" °C, Error: ");
    Serial.print(tempError);
    Serial.print(" °C, Heater: ");
    Serial.print(int(heaterPower * 100));
    Serial.print("%, Cooler: ");
    Serial.print(int(coolerPower * 100));
    Serial.println("%");
    
    delete[] outputs;
  }
  
  delay(1000);
}

/*
 * FuzzySugeno_HVAC_Control.ino
 * 
 * HVAC system controller using FuzzySugeno
 * Created on: 2025. 3. 30
 */

#define ENABLE_MODULE_FUZZY_SUGENO
#include "Kinematrix.h"

// Create a FuzzySugeno instance
FuzzySugeno hvac(3, 2, 10, 3);

// Pins for simulation
const int tempSensorPin = 1;
const int humidSensorPin = 2;
const int occupancySensorPin = 3;

const int heatingPin = 9;
const int coolingPin = 10;
const int fanPin = 11;

// Target conditions
const float targetTemp = 22.0;
const float targetHumidity = 50.0;

// Simulation variables
float currentTemp = 24.0;
float currentHumidity = 60.0;
bool isOccupied = true;

// Last update time
unsigned long lastUpdateTime = 0;
const unsigned long updateInterval = 1000;

void setup() {
  Serial.begin(115200);
  Serial.println("FuzzySugeno HVAC Control System");
  Serial.println("-------------------------------");
  
  // Setup pins
  pinMode(occupancySensorPin, INPUT_PULLUP);
  pinMode(heatingPin, OUTPUT);
  pinMode(coolingPin, OUTPUT);
  pinMode(fanPin, OUTPUT);
  
  // 1. Define input variables
  hvac.addInputVariable("temperature", 10, 35);
  hvac.addInputVariable("temperature_error", -10, 10);  // Difference from target
  hvac.addInputVariable("humidity", 20, 80);
  
  // 2. Define fuzzy sets for temperature
  float coldParams[] = {10, 10, 19};
  float coolParams[] = {17, 20, 23};
  float comfortParams[] = {21, 23, 25};
  float warmParams[] = {23, 26, 29};
  float hotParams[] = {27, 35, 35};
  
  hvac.addFuzzySet(0, "cold", TRIANGULAR, coldParams);
  hvac.addFuzzySet(0, "cool", TRIANGULAR, coolParams);
  hvac.addFuzzySet(0, "comfort", TRIANGULAR, comfortParams);
  hvac.addFuzzySet(0, "warm", TRIANGULAR, warmParams);
  hvac.addFuzzySet(0, "hot", TRIANGULAR, hotParams);
  
  // 3. Define fuzzy sets for temperature error
  float negLargeParams[] = {-10, -10, -4};
  float negSmallParams[] = {-6, -2, 0};
  float zeroParams[] = {-1, 0, 1};
  float posSmallParams[] = {0, 2, 6};
  float posLargeParams[] = {4, 10, 10};
  
  hvac.addFuzzySet(1, "neg_large", TRIANGULAR, negLargeParams);
  hvac.addFuzzySet(1, "neg_small", TRIANGULAR, negSmallParams);
  hvac.addFuzzySet(1, "zero", TRIANGULAR, zeroParams);
  hvac.addFuzzySet(1, "pos_small", TRIANGULAR, posSmallParams);
  hvac.addFuzzySet(1, "pos_large", TRIANGULAR, posLargeParams);
  
  // 4. Define fuzzy sets for humidity
  float dryParams[] = {20, 20, 40};
  float normalParams[] = {35, 50, 65};
  float humidParams[] = {60, 80, 80};
  
  hvac.addFuzzySet(2, "dry", TRIANGULAR, dryParams);
  hvac.addFuzzySet(2, "normal", TRIANGULAR, normalParams);
  hvac.addFuzzySet(2, "humid", TRIANGULAR, humidParams);
  
  // 5. Define outputs
  // Linear outputs for heating and cooling
  // Heating = a₀ + a₁*temp + a₂*error + a₃*humidity
  float heatingCoeffs[] = {0.5, -0.02, -0.08, 0.005};  // Baseline - temp_effect - error_effect + humidity_effect
  hvac.addLinearOutput("heating_power", heatingCoeffs, 4);
  
  // Cooling = a₀ + a₁*temp + a₂*error + a₃*humidity
  float coolingCoeffs[] = {-0.3, 0.03, 0.07, 0.01};  // Baseline + temp_effect + error_effect + humidity_effect
  hvac.addLinearOutput("cooling_power", coolingCoeffs, 4);
  
  // Constant outputs for specific situations
  hvac.addConstantOutput("off", 0.0);
  hvac.addConstantOutput("low", 0.3);
  hvac.addConstantOutput("medium", 0.6);
  hvac.addConstantOutput("high", 0.9);
  hvac.addConstantOutput("max", 1.0);
  
  // 6. Define rules
  
  // Heating control rules
  // Rule 1: IF temp IS cold AND error IS neg_large THEN heating = max
  int rule1Vars[] = {0, 1};
  int rule1Sets[] = {0, 0}; // cold, neg_large
  hvac.addRule(rule1Vars, rule1Sets, 2, 6, true); // heating = max
  
  // Rule 2: IF temp IS cold AND error IS neg_small THEN heating = high
  int rule2Vars[] = {0, 1};
  int rule2Sets[] = {0, 1}; // cold, neg_small
  hvac.addRule(rule2Vars, rule2Sets, 2, 5, true); // heating = high
  
  // Rule 3: IF temp IS cool AND error IS neg_small THEN heating = medium
  int rule3Vars[] = {0, 1};
  int rule3Sets[] = {1, 1}; // cool, neg_small
  hvac.addRule(rule3Vars, rule3Sets, 2, 4, true); // heating = medium
  
  // Rule 4: IF temp IS comfort AND error IS zero THEN heating = off
  int rule4Vars[] = {0, 1};
  int rule4Sets[] = {2, 2}; // comfort, zero
  hvac.addRule(rule4Vars, rule4Sets, 2, 2, true); // heating = off
  
  // Cooling control rules
  // Rule 5: IF temp IS hot AND error IS pos_large THEN cooling = max
  int rule5Vars[] = {0, 1};
  int rule5Sets[] = {4, 4}; // hot, pos_large
  hvac.addRule(rule5Vars, rule5Sets, 2, 6, true); // cooling = max
  
  // Rule 6: IF temp IS hot AND error IS pos_small THEN cooling = high
  int rule6Vars[] = {0, 1};
  int rule6Sets[] = {4, 3}; // hot, pos_small
  hvac.addRule(rule6Vars, rule6Sets, 2, 5, true); // cooling = high
  
  // Rule 7: IF temp IS warm AND error IS pos_small THEN cooling = medium
  int rule7Vars[] = {0, 1};
  int rule7Sets[] = {3, 3}; // warm, pos_small
  hvac.addRule(rule7Vars, rule7Sets, 2, 4, true); // cooling = medium
  
  // Rule 8: IF temp IS comfort AND error IS zero THEN cooling = off
  int rule8Vars[] = {0, 1};
  int rule8Sets[] = {2, 2}; // comfort, zero
  hvac.addRule(rule8Vars, rule8Sets, 2, 2, true); // cooling = off
  
  // Dynamic control (linear equation) based on combined factors
  // Rule 9: Use linear output for general heating adjustment
  int rule9Vars[] = {0, 1, 2};
  int rule9Sets[] = {1, 1, 1}; // cool, neg_small, normal
  hvac.addRule(rule9Vars, rule9Sets, 3, 0, true); // heating_power (linear)
  
  // Rule 10: Use linear output for general cooling adjustment
  int rule10Vars[] = {0, 1, 2};
  int rule10Sets[] = {3, 3, 1}; // warm, pos_small, normal
  hvac.addRule(rule10Vars, rule10Sets, 3, 1, true); // cooling_power (linear)
  
  if (hvac.hasError()) {
    Serial.print("Error: ");
    Serial.println(hvac.getErrorMessage());
  } else {
    Serial.println("HVAC system initialized successfully!");
  }
}

void loop() {
  // Read sensor data
  // For simulation, we'll generate some dynamic values
  isOccupied = (digitalRead(occupancySensorPin) == LOW);
  
  // Simulate temperature and humidity changes
  // If heating or cooling is on, adjust accordingly
  static float heatingPower = 0.0;
  static float coolingPower = 0.0;
  
  // Simulate natural drift
  float tempDrift = random(-10, 10) / 100.0;
  float humidityDrift = random(-5, 5) / 10.0;
  
  // Apply HVAC effects
  currentTemp += tempDrift + (heatingPower * 0.1) - (coolingPower * 0.1);
  currentHumidity += humidityDrift - (coolingPower * 0.2);
  
  // Constrain to realistic values
  currentTemp = constrain(currentTemp, 15, 35);
  currentHumidity = constrain(currentHumidity, 20, 80);
  
  // Calculate temperature error (target - current)
  float tempError = targetTemp - currentTemp;
  
  // Input array for fuzzy system
  float inputs[] = {currentTemp, tempError, currentHumidity};
  
  // Update at regular intervals
  unsigned long currentTime = millis();
  if (currentTime - lastUpdateTime >= updateInterval) {
    lastUpdateTime = currentTime;
    
    // Evaluate fuzzy system
    float* outputs = hvac.evaluate(inputs);
    
    if (outputs != nullptr) {
      heatingPower = outputs[0];
      coolingPower = outputs[1];
      
      // Apply to HVAC outputs (PWM)
      if (isOccupied) {
        // Full control when occupied
        analogWrite(heatingPin, (int)(heatingPower * 255));
        analogWrite(coolingPin, (int)(coolingPower * 255));
        analogWrite(fanPin, (int)((heatingPower + coolingPower) * 127));
      } else {
        // Energy saving mode when unoccupied
        analogWrite(heatingPin, (int)(heatingPower * 127));
        analogWrite(coolingPin, (int)(coolingPower * 127));
        analogWrite(fanPin, (int)((heatingPower + coolingPower) * 64));
      }
      
      // Display results
      Serial.print("Temperature: ");
      Serial.print(currentTemp, 1);
      Serial.print("°C (Target: ");
      Serial.print(targetTemp);
      Serial.print("°C), Error: ");
      Serial.print(tempError, 1);
      Serial.print("°C, Humidity: ");
      Serial.print(currentHumidity, 1);
      Serial.print("%, Occupied: ");
      Serial.print(isOccupied ? "Yes" : "No");
      Serial.print(" | Heat: ");
      Serial.print(int(heatingPower * 100));
      Serial.print("%, Cool: ");
      Serial.print(int(coolingPower * 100));
      Serial.print("%, Fan: ");
      Serial.print(int((heatingPower + coolingPower) * 50));
      Serial.println("%");
      
      delete[] outputs;
    }
  }
  
  delay(100);
}

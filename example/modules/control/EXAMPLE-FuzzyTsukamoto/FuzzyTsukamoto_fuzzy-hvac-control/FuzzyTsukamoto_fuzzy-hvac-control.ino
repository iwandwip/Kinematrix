/*
 * FuzzyTsukamoto - HVAC Control System Example
 * 
 * This example demonstrates a complex HVAC control system that
 * manages temperature, humidity, and ventilation based on
 * occupancy and outdoor conditions
 */

#define ENABLE_MODULE_FUZZY_TSUKAMOTO
#include "Kinematrix.h"

// Pin definitions
#define TEMP_SENSOR_PIN A0
#define HUMID_SENSOR_PIN A1
#define OCCUPANCY_SENSOR_PIN 2
#define OUTDOOR_TEMP_PIN A2
#define HEATER_PIN 9
#define AC_PIN 10
#define FAN_PIN 11
#define HUMIDIFIER_PIN 12

// Global variables
FuzzyTsukamoto fuzzy(4, 3, 12, 3);
unsigned long lastPrintTime = 0;
bool occupancyState = false;

void setup() {
  Serial.begin(9600);
  pinMode(HEATER_PIN, OUTPUT);
  pinMode(AC_PIN, OUTPUT);
  pinMode(FAN_PIN, OUTPUT);
  pinMode(HUMIDIFIER_PIN, OUTPUT);
  pinMode(OCCUPANCY_SENSOR_PIN, INPUT);
  
  // Add inputs
  fuzzy.addInputVariable("IndoorTemp", 10.0, 35.0);  // Celsius
  fuzzy.addInputVariable("IndoorHumidity", 0.0, 100.0);  // Percent
  fuzzy.addInputVariable("OutdoorTemp", -10.0, 40.0);  // Celsius
  fuzzy.addInputVariable("Occupancy", 0.0, 1.0);  // Binary (0=unoccupied, 1=occupied)
  
  // Define indoor temperature sets
  float cold[] = {10.0, 10.0, 20.0};
  float comfortable[] = {18.0, 22.0, 26.0};
  float hot[] = {24.0, 35.0, 35.0};
  fuzzy.addFuzzySet(0, true, "Cold", TRIANGULAR, cold);
  fuzzy.addFuzzySet(0, true, "Comfortable", TRIANGULAR, comfortable);
  fuzzy.addFuzzySet(0, true, "Hot", TRIANGULAR, hot);
  
  // Define humidity sets
  float dry[] = {0.0, 0.0, 30.0};
  float normal[] = {25.0, 45.0, 65.0};
  float humid[] = {60.0, 100.0, 100.0};
  fuzzy.addFuzzySet(1, true, "Dry", TRIANGULAR, dry);
  fuzzy.addFuzzySet(1, true, "Normal", TRIANGULAR, normal);
  fuzzy.addFuzzySet(1, true, "Humid", TRIANGULAR, humid);
  
  // Define outdoor temperature sets
  float outdoor_cold[] = {-10.0, -10.0, 10.0};
  float outdoor_mild[] = {5.0, 20.0, 30.0};
  float outdoor_hot[] = {25.0, 40.0, 40.0};
  fuzzy.addFuzzySet(2, true, "Cold", TRIANGULAR, outdoor_cold);
  fuzzy.addFuzzySet(2, true, "Mild", TRIANGULAR, outdoor_mild);
  fuzzy.addFuzzySet(2, true, "Hot", TRIANGULAR, outdoor_hot);
  
  // Define occupancy sets (essentially binary but using fuzzy sets)
  float unoccupied[] = {0.0, 0.0, 0.5};
  float transitioning[] = {0.3, 0.5, 0.7};
  float occupied[] = {0.5, 1.0, 1.0};
  fuzzy.addFuzzySet(3, true, "Unoccupied", TRIANGULAR, unoccupied);
  fuzzy.addFuzzySet(3, true, "Transitioning", TRIANGULAR, transitioning);
  fuzzy.addFuzzySet(3, true, "Occupied", TRIANGULAR, occupied);
  
  // Add outputs
  fuzzy.addOutputVariable("HeatingCooling", -255.0, 255.0);  // Negative=AC, Positive=Heat
  fuzzy.addOutputVariable("FanSpeed", 0.0, 255.0);  // PWM value
  fuzzy.addOutputVariable("Humidifier", 0.0, 255.0);  // PWM value
  
  // Define heating/cooling output sets
  float cooling[] = {-255.0, 0.0};  // Monotonic increasing from -255 to 0
  float neutral[] = {-50.0, 50.0};  // Centered around 0
  float heating[] = {0.0, 255.0};  // Monotonic increasing from 0 to 255
  fuzzy.addFuzzySet(0, false, "Cooling", MONOTONIC_INCREASING, cooling);
  fuzzy.addFuzzySet(0, false, "Neutral", MONOTONIC_INCREASING, neutral);
  fuzzy.addFuzzySet(0, false, "Heating", MONOTONIC_INCREASING, heating);
  
  // Define fan speed output sets
  float low[] = {255.0, 0.0};  // Monotonic decreasing
  float medium[] = {0.0, 127.0};  // Monotonic increasing to middle
  float high[] = {127.0, 255.0};  // Monotonic increasing to max
  fuzzy.addFuzzySet(1, false, "Low", MONOTONIC_DECREASING, low);
  fuzzy.addFuzzySet(1, false, "Medium", MONOTONIC_INCREASING, medium);
  fuzzy.addFuzzySet(1, false, "High", MONOTONIC_INCREASING, high);
  
  // Define humidifier output sets
  float off[] = {255.0, 0.0};  // Monotonic decreasing
  float low_humid[] = {0.0, 127.0};  // Monotonic increasing to middle
  float high_humid[] = {127.0, 255.0};  // Monotonic increasing to max
  fuzzy.addFuzzySet(2, false, "Off", MONOTONIC_DECREASING, off);
  fuzzy.addFuzzySet(2, false, "Low", MONOTONIC_INCREASING, low_humid);
  fuzzy.addFuzzySet(2, false, "High", MONOTONIC_INCREASING, high_humid);
  
  // Add the rules
  
  // Temperature control rules
  int vars1[] = {0, 3};
  int sets1a[] = {0, 2}; // Cold, Occupied
  fuzzy.addRule(vars1, sets1a, 2, 0, 2, true); // Heating
  
  int sets1b[] = {2, 2}; // Hot, Occupied
  fuzzy.addRule(vars1, sets1b, 2, 0, 0, true); // Cooling
  
  int sets1c[] = {1, 2}; // Comfortable, Occupied
  fuzzy.addRule(vars1, sets1c, 2, 0, 1, true); // Neutral
  
  // Energy saving rules
  int vars2[] = {0, 3};
  int sets2a[] = {0, 0}; // Cold, Unoccupied
  fuzzy.addRule(vars2, sets2a, 2, 0, 1, true); // Neutral (minimal heating)
  
  int sets2b[] = {2, 0}; // Hot, Unoccupied
  fuzzy.addRule(vars2, sets2b, 2, 0, 1, true); // Neutral (minimal cooling)
  
  // Fan control rules
  int vars3[] = {0, 2, 3};
  int sets3a[] = {2, 2, 2}; // Hot indoor, Hot outdoor, Occupied
  fuzzy.addRule(vars3, sets3a, 3, 1, 2, true); // High fan
  
  int sets3b[] = {1, 1, 2}; // Comfortable indoor, Mild outdoor, Occupied
  fuzzy.addRule(vars3, sets3b, 3, 1, 1, true); // Medium fan
  
  int sets3c[] = {0, 0, 0}; // Cold indoor, Cold outdoor, Unoccupied
  fuzzy.addRule(vars3, sets3c, 3, 1, 0, true); // Low fan
  
  // Humidity control rules
  int vars4[] = {1, 3};
  int sets4a[] = {0, 2}; // Dry, Occupied
  fuzzy.addRule(vars4, sets4a, 2, 2, 2, true); // High humidifier
  
  int sets4b[] = {1, 2}; // Normal humidity, Occupied
  fuzzy.addRule(vars4, sets4b, 2, 2, 0, true); // Humidifier off
  
  int sets4c[] = {2, 2}; // Humid, Occupied
  fuzzy.addRule(vars4, sets4c, 2, 2, 0, true); // Humidifier off
  
  int sets4d[] = {0, 0}; // Dry, Unoccupied
  fuzzy.addRule(vars4, sets4d, 2, 2, 1, true); // Low humidifier
  
  Serial.println("HVAC control system initialized");
}

void loop() {
  // Simulated sensor readings
  float indoorTemp = 22.0 + 5.0 * sin(millis() / 20000.0);
  float indoorHumidity = 50.0 + 30.0 * sin(millis() / 30000.0);
  float outdoorTemp = 15.0 + 15.0 * sin(millis() / 50000.0);
  
  // Toggle occupancy every 30 seconds for simulation
  occupancyState = ((millis() / 30000) % 2) == 0;
  float occupancy = occupancyState ? 1.0 : 0.0;
  
  // Prepare input array
  float inputs[] = {indoorTemp, indoorHumidity, outdoorTemp, occupancy};
  
  // Evaluate the fuzzy system
  float* outputs = fuzzy.evaluate(inputs);
  
  if (outputs != nullptr) {
    // Process heating/cooling output (negative=cooling, positive=heating)
    if (outputs[0] < 0) {
      analogWrite(HEATER_PIN, 0);
      analogWrite(AC_PIN, (int)abs(outputs[0]));
    } else {
      analogWrite(AC_PIN, 0);
      analogWrite(HEATER_PIN, (int)outputs[0]);
    }
    
    // Apply other outputs
    analogWrite(FAN_PIN, (int)outputs[1]);
    analogWrite(HUMIDIFIER_PIN, (int)outputs[2]);
    
    // Print results every second
    if (millis() - lastPrintTime > 1000) {
      Serial.print("Indoor: ");
      Serial.print(indoorTemp, 1);
      Serial.print("°C, ");
      Serial.print(indoorHumidity, 1);
      Serial.print("% | Outdoor: ");
      Serial.print(outdoorTemp, 1);
      Serial.print("°C | ");
      Serial.print(occupancyState ? "Occupied" : "Unoccupied");
      Serial.println();
      
      Serial.print("Controls - Heat/Cool: ");
      Serial.print(outputs[0], 1);
      Serial.print(" | Fan: ");
      Serial.print(outputs[1], 1);
      Serial.print(" | Humidifier: ");
      Serial.println(outputs[2], 1);
      Serial.println("----------");
      
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

/*
 * FuzzyTsukamoto - Irrigation System Example
 * 
 * This example demonstrates an automated irrigation system that
 * controls watering based on soil moisture, temperature, time
 * since last watering, and recent rainfall
 */

#define ENABLE_MODULE_FUZZY_TSUKAMOTO
#include "Kinematrix.h"

// Pin definitions
#define MOISTURE_SENSOR_PIN A0
#define TEMP_SENSOR_PIN A1
#define RAIN_SENSOR_PIN A2
#define WATER_VALVE_PIN 9
#define PUMP_SPEED_PIN 10
#define STATUS_LED_PIN 13

// Global variables
FuzzyTsukamoto fuzzy(4, 2, 10, 3);
unsigned long lastPrintTime = 0;
unsigned long lastWateringTime = 0;
unsigned long simulationStartTime;
float rainAmount = 0.0;

void setup() {
  Serial.begin(9600);
  pinMode(WATER_VALVE_PIN, OUTPUT);
  pinMode(PUMP_SPEED_PIN, OUTPUT);
  pinMode(STATUS_LED_PIN, OUTPUT);
  
  simulationStartTime = millis();
  
  // Add inputs
  fuzzy.addInputVariable("SoilMoisture", 0.0, 100.0);  // Percent
  fuzzy.addInputVariable("Temperature", 0.0, 50.0);    // Celsius
  fuzzy.addInputVariable("HoursSinceWatering", 0.0, 72.0); // Hours
  fuzzy.addInputVariable("RecentRainfall", 0.0, 50.0); // mm
  
  // Define soil moisture sets
  float dry[] = {0.0, 0.0, 30.0};
  float moist[] = {20.0, 50.0, 80.0};
  float wet[] = {70.0, 100.0, 100.0};
  fuzzy.addFuzzySet(0, true, "Dry", TRIANGULAR, dry);
  fuzzy.addFuzzySet(0, true, "Moist", TRIANGULAR, moist);
  fuzzy.addFuzzySet(0, true, "Wet", TRIANGULAR, wet);
  
  // Define temperature sets
  float cool[] = {0.0, 0.0, 20.0};
  float moderate[] = {15.0, 25.0, 35.0};
  float hot[] = {30.0, 50.0, 50.0};
  fuzzy.addFuzzySet(1, true, "Cool", TRIANGULAR, cool);
  fuzzy.addFuzzySet(1, true, "Moderate", TRIANGULAR, moderate);
  fuzzy.addFuzzySet(1, true, "Hot", TRIANGULAR, hot);
  
  // Define time since watering sets
  float recent[] = {0.0, 0.0, 12.0};
  float medium[] = {8.0, 24.0, 40.0};
  float long_time[] = {36.0, 72.0, 72.0};
  fuzzy.addFuzzySet(2, true, "Recent", TRIANGULAR, recent);
  fuzzy.addFuzzySet(2, true, "Medium", TRIANGULAR, medium);
  fuzzy.addFuzzySet(2, true, "Long", TRIANGULAR, long_time);
  
  // Define rainfall sets
  float none[] = {0.0, 0.0, 5.0};
  float light[] = {2.0, 10.0, 20.0};
  float heavy[] = {15.0, 50.0, 50.0};
  fuzzy.addFuzzySet(3, true, "None", TRIANGULAR, none);
  fuzzy.addFuzzySet(3, true, "Light", TRIANGULAR, light);
  fuzzy.addFuzzySet(3, true, "Heavy", TRIANGULAR, heavy);
  
  // Add outputs
  fuzzy.addOutputVariable("WateringDuration", 0.0, 30.0);  // Minutes
  fuzzy.addOutputVariable("PumpSpeed", 0.0, 255.0);       // PWM value
  
  // Define watering duration output sets
  float none_duration[] = {30.0, 0.0};  // Monotonic decreasing
  float short_duration[] = {0.0, 10.0};  // Monotonic increasing
  float long_duration[] = {10.0, 30.0};  // Monotonic increasing
  fuzzy.addFuzzySet(0, false, "None", MONOTONIC_DECREASING, none_duration);
  fuzzy.addFuzzySet(0, false, "Short", MONOTONIC_INCREASING, short_duration);
  fuzzy.addFuzzySet(0, false, "Long", MONOTONIC_INCREASING, long_duration);
  
  // Define pump speed output sets
  float low_speed[] = {255.0, 0.0};  // Monotonic decreasing
  float medium_speed[] = {0.0, 127.0};  // Monotonic increasing
  float high_speed[] = {127.0, 255.0};  // Monotonic increasing
  fuzzy.addFuzzySet(1, false, "Low", MONOTONIC_DECREASING, low_speed);
  fuzzy.addFuzzySet(1, false, "Medium", MONOTONIC_INCREASING, medium_speed);
  fuzzy.addFuzzySet(1, false, "High", MONOTONIC_INCREASING, high_speed);
  
  // Add the rules
  
  // Rule 1: IF soil is dry AND temperature is hot AND watering not recent AND no rain THEN water long at high speed
  int vars1[] = {0, 1, 2, 3};
  int sets1[] = {0, 2, 2, 0}; // Dry, Hot, Long time, No rain
  fuzzy.addRule(vars1, sets1, 4, 0, 2, true); // Long watering
  fuzzy.addRule(vars1, sets1, 4, 1, 2, true); // High speed
  
  // Rule 2: IF soil is dry AND temperature is moderate AND medium time THEN water short at medium speed
  int vars2[] = {0, 1, 2};
  int sets2[] = {0, 1, 1}; // Dry, Moderate, Medium time
  fuzzy.addRule(vars2, sets2, 3, 0, 1, true); // Short watering
  fuzzy.addRule(vars2, sets2, 3, 1, 1, true); // Medium speed
  
  // Rule 3: IF soil is wet OR recent rain is heavy THEN don't water
  int vars3[] = {0, 3};
  int sets3[] = {2, 2}; // Wet, Heavy rain
  fuzzy.addRule(vars3, sets3, 2, 0, 0, false); // No watering (OR operator)
  fuzzy.addRule(vars3, sets3, 2, 1, 0, false); // Low speed (OR operator)
  
  // Rule 4: IF soil is moist AND temperature is cool THEN water short at low speed
  int vars4[] = {0, 1};
  int sets4[] = {1, 0}; // Moist, Cool
  fuzzy.addRule(vars4, sets4, 2, 0, 1, true); // Short watering
  fuzzy.addRule(vars4, sets4, 2, 1, 0, true); // Low speed
  
  // Rule 5: IF watering is recent THEN don't water
  int vars5[] = {2};
  int sets5[] = {0}; // Recent watering
  fuzzy.addRule(vars5, sets5, 1, 0, 0, true); // No watering
  
  Serial.println("Irrigation system initialized");
}

void loop() {
  // Simulate sensor readings based on time
  // In a real system, these would come from actual sensors
  float simulatedDays = (millis() - simulationStartTime) / 10000.0; // 1 simulated day every 10 seconds
  
  // Soil dries out over time, watering increases moisture
  float soilMoisture = 50.0 - 30.0 * sin(simulatedDays * 0.5);
  
  // Temperature varies with a daily cycle
  float temperature = 25.0 + 15.0 * sin(simulatedDays * 2.0 * PI);
  
  // Calculate hours since last watering
  float hoursSinceWatering = (millis() - lastWateringTime) / 1000.0 / 3600.0;
  // For simulation, scale time (1 hour = 1 second)
  hoursSinceWatering = (millis() - lastWateringTime) / 1000.0;
  if (hoursSinceWatering > 72.0) hoursSinceWatering = 72.0;
  
  // Simulate random rain events
  if (random(1000) < 5) { // 0.5% chance of rain each cycle
    rainAmount = random(50);
  } else {
    // Rain amount decreases over time
    rainAmount = max(0.0, rainAmount - 0.1);
  }
  
  // Prepare input array
  float inputs[] = {soilMoisture, temperature, hoursSinceWatering, rainAmount};
  
  // Evaluate the fuzzy system
  float* outputs = fuzzy.evaluate(inputs);
  
  if (outputs != nullptr) {
    float wateringDuration = outputs[0]; // minutes
    float pumpSpeed = outputs[1]; // PWM value
    
    // Apply the outputs
    if (wateringDuration > 1.0) {
      digitalWrite(WATER_VALVE_PIN, HIGH);
      analogWrite(PUMP_SPEED_PIN, (int)pumpSpeed);
      digitalWrite(STATUS_LED_PIN, HIGH);
      
      // In a real system, we would keep track of watering time
      // For simulation, reset the timer if we're actively watering
      if (lastWateringTime == 0 || hoursSinceWatering > 1.0) {
        lastWateringTime = millis();
      }
    } else {
      digitalWrite(WATER_VALVE_PIN, LOW);
      analogWrite(PUMP_SPEED_PIN, 0);
      digitalWrite(STATUS_LED_PIN, LOW);
    }
    
    // Print results every second
    if (millis() - lastPrintTime > 1000) {
      Serial.print("Day: ");
      Serial.print(simulatedDays, 1);
      Serial.print(" | Soil: ");
      Serial.print(soilMoisture, 1);
      Serial.print("% | Temp: ");
      Serial.print(temperature, 1);
      Serial.print("°C | Hours Since Watering: ");
      Serial.print(hoursSinceWatering, 1);
      Serial.print(" | Rain: ");
      Serial.print(rainAmount, 1);
      Serial.println("mm");
      
      Serial.print("Controls - Duration: ");
      Serial.print(wateringDuration, 1);
      Serial.print(" min | Pump Speed: ");
      Serial.println(pumpSpeed, 1);
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

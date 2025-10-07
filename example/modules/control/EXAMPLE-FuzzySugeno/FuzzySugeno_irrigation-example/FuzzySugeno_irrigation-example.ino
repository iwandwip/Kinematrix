/*
 * FuzzySugeno_Irrigation_System.ino
 * 
 * Smart irrigation controller using FuzzySugeno
 * Created on: 2025. 3. 30
 */

#define ENABLE_MODULE_FUZZY_SUGENO
#include "Kinematrix.h"

// Create a FuzzySugeno instance
FuzzySugeno irrigation(4, 2, 16, 3);

// Pins for sensors and actuators
const int soilMoisturePin = 1;
const int temperaturePin = 2;
const int lightSensorPin = 3;
const int rainSensorPin = 4;

const int waterValvePin = 9;
const int pumpPin = 10;

// Thresholds and parameters
const int dryThreshold = 300;   // Soil moisture raw ADC value when dry
const int wetThreshold = 700;   // Soil moisture raw ADC value when wet
const float tempThreshold = 30.0; // Temperature threshold in Celsius

// System state
bool valveOpen = false;
bool pumpActive = false;
unsigned long wateringStartTime = 0;
unsigned long wateringDuration = 0;

// Timer variables
unsigned long lastCheckTime = 0;
const unsigned long checkInterval = 2000;  // Check every 2 seconds
const unsigned long minWateringInterval = 3600000; // Minimum time between waterings (1 hour)
unsigned long lastWateringTime = 0;

void setup() {
  Serial.begin(115200);
  Serial.println("FuzzySugeno Irrigation System");
  Serial.println("-----------------------------");
  
  // Setup pins
  pinMode(waterValvePin, OUTPUT);
  pinMode(pumpPin, OUTPUT);
  
  // Initialize actuators (OFF)
  digitalWrite(waterValvePin, LOW);
  digitalWrite(pumpPin, LOW);
  
  // 1. Define input variables
  irrigation.addInputVariable("soil_moisture", 0, 1023);
  irrigation.addInputVariable("temperature", 0, 50);
  irrigation.addInputVariable("light_level", 0, 1023);
  irrigation.addInputVariable("rain_detection", 0, 1023);
  
  // 2. Define fuzzy sets for soil moisture
  float veryDryParams[] = {0, 0, 300};
  float dryParams[] = {200, 400, 600};
  float moistParams[] = {500, 700, 900};
  float wetParams[] = {800, 1023, 1023};
  
  irrigation.addFuzzySet(0, "very_dry", TRIANGULAR, veryDryParams);
  irrigation.addFuzzySet(0, "dry", TRIANGULAR, dryParams);
  irrigation.addFuzzySet(0, "moist", TRIANGULAR, moistParams);
  irrigation.addFuzzySet(0, "wet", TRIANGULAR, wetParams);
  
  // 3. Define fuzzy sets for temperature
  float coolParams[] = {0, 0, 20};
  float mildParams[] = {15, 25, 35};
  float hotParams[] = {30, 50, 50};
  
  irrigation.addFuzzySet(1, "cool", TRIANGULAR, coolParams);
  irrigation.addFuzzySet(1, "mild", TRIANGULAR, mildParams);
  irrigation.addFuzzySet(1, "hot", TRIANGULAR, hotParams);
  
  // 4. Define fuzzy sets for light level
  float darkParams[] = {0, 0, 300};
  float dimParams[] = {200, 500, 800};
  float brightParams[] = {700, 1023, 1023};
  
  irrigation.addFuzzySet(2, "dark", TRIANGULAR, darkParams);
  irrigation.addFuzzySet(2, "dim", TRIANGULAR, dimParams);
  irrigation.addFuzzySet(2, "bright", TRIANGULAR, brightParams);
  
  // 5. Define fuzzy sets for rain detection
  float dryWeatherParams[] = {0, 0, 300};
  float lightRainParams[] = {200, 500, 800};
  float heavyRainParams[] = {700, 1023, 1023};
  
  irrigation.addFuzzySet(3, "dry_weather", TRIANGULAR, dryWeatherParams);
  irrigation.addFuzzySet(3, "light_rain", TRIANGULAR, lightRainParams);
  irrigation.addFuzzySet(3, "heavy_rain", TRIANGULAR, heavyRainParams);
  
  // 6. Define outputs
  // Watering duration in seconds based on all input factors
  float wateringCoeffs[] = {300, -0.3, 1.5, -0.1, -0.2};
  irrigation.addLinearOutput("watering_duration", wateringCoeffs, 5);
  
  // Watering intensity (pump pressure %)
  float intensityCoeffs[] = {50, -0.02, 0.5, 0.0, -0.03};
  irrigation.addLinearOutput("watering_intensity", intensityCoeffs, 5);
  
  // Constant outputs for specific situations
  irrigation.addConstantOutput("no_watering", 0);
  irrigation.addConstantOutput("short_watering", 60);  // 1 minute
  irrigation.addConstantOutput("medium_watering", 300); // 5 minutes
  irrigation.addConstantOutput("long_watering", 600);  // 10 minutes
  irrigation.addConstantOutput("low_intensity", 30);   // 30% pump power
  irrigation.addConstantOutput("high_intensity", 80);  // 80% pump power
  
  // 7. Define rules
  
  // Very dry soil needs watering regardless of conditions
  int veryDryVars[] = {0};
  int veryDrySets[] = {0}; // very_dry
  irrigation.addRule(veryDryVars, veryDrySets, 1, 5, true); // long_watering
  irrigation.addRule(veryDryVars, veryDrySets, 1, 7, true); // high_intensity
  
  // Dry soil in hot weather
  int dryHotVars[] = {0, 1};
  int dryHotSets[] = {1, 2}; // dry, hot
  irrigation.addRule(dryHotVars, dryHotSets, 2, 5, true); // long_watering
  irrigation.addRule(dryHotVars, dryHotSets, 2, 7, true); // high_intensity
  
  // Dry soil in mild weather
  int dryMildVars[] = {0, 1};
  int dryMildSets[] = {1, 1}; // dry, mild
  irrigation.addRule(dryMildVars, dryMildSets, 2, 4, true); // medium_watering
  irrigation.addRule(dryMildVars, dryMildSets, 2, 6, true); // low_intensity
  
  // Moist soil in hot weather
  int moistHotVars[] = {0, 1};
  int moistHotSets[] = {2, 2}; // moist, hot
  irrigation.addRule(moistHotVars, moistHotSets, 2, 3, true); // short_watering
  irrigation.addRule(moistHotVars, moistHotSets, 2, 6, true); // low_intensity
  
  // Any soil condition during heavy rain = no watering
  int rainyVars[] = {3};
  int rainySets[] = {2}; // heavy_rain
  irrigation.addRule(rainyVars, rainySets, 1, 2, true); // no_watering
  
  // Dynamic watering control (using linear equations for normal conditions)
  int normalConditionVars[] = {0, 1, 2, 3};
  int normalConditionSets[] = {1, 1, 1, 0}; // dry, mild, dim, dry_weather
  irrigation.addRule(normalConditionVars, normalConditionSets, 4, 0, true); // linear watering_duration
  irrigation.addRule(normalConditionVars, normalConditionSets, 4, 1, true); // linear watering_intensity
  
  if (irrigation.hasError()) {
    Serial.print("Error: ");
    Serial.println(irrigation.getErrorMessage());
  } else {
    Serial.println("Irrigation controller initialized successfully!");
  }
}

void loop() {
  // Check if watering is in progress
  if (valveOpen) {
    // Check if watering should be stopped
    if (millis() - wateringStartTime >= wateringDuration * 1000) {
      // Stop watering
      digitalWrite(waterValvePin, LOW);
      digitalWrite(pumpPin, LOW);
      valveOpen = false;
      pumpActive = false;
      
      Serial.println("Watering completed");
      lastWateringTime = millis();
    }
    
    // Periodically update status during watering
    if (millis() - lastCheckTime >= checkInterval) {
      lastCheckTime = millis();
      
      unsigned long elapsedTime = (millis() - wateringStartTime) / 1000;
      unsigned long remainingTime = (wateringDuration > elapsedTime) ? 
                                    (wateringDuration - elapsedTime) : 0;
      
      Serial.print("Watering in progress - ");
      Serial.print(elapsedTime);
      Serial.print(" seconds elapsed, ");
      Serial.print(remainingTime);
      Serial.println(" seconds remaining");
    }
  }
  else {
    // Check if it's time to evaluate the system
    if (millis() - lastCheckTime >= checkInterval) {
      lastCheckTime = millis();
      
      // Read sensor values (or simulate them here)
      int soilMoisture = analogRead(soilMoisturePin);
      int temperature = analogRead(temperaturePin) * 50 / 1023; // Convert to Celsius
      int lightLevel = analogRead(lightSensorPin);
      int rainDetection = analogRead(rainSensorPin);
      
      // For simulation
      // soilMoisture = 350;  // Fairly dry
      // temperature = 28;    // Warm
      // lightLevel = 800;    // Bright
      // rainDetection = 100; // No rain
      
      // Input array for fuzzy system
      float inputs[] = {(float)soilMoisture, (float)temperature, 
                        (float)lightLevel, (float)rainDetection};
      
      // Display sensor readings
      Serial.print("Sensors - Soil: ");
      Serial.print(soilMoisture);
      Serial.print("/1023, Temp: ");
      Serial.print(temperature);
      Serial.print("°C, Light: ");
      Serial.print(lightLevel);
      Serial.print("/1023, Rain: ");
      Serial.print(rainDetection);
      Serial.println("/1023");
      
      // Check if enough time has passed since last watering
      if (millis() - lastWateringTime >= minWateringInterval) {
        // Evaluate irrigation needs
        float* outputs = irrigation.evaluate(inputs);
        
        if (outputs != nullptr) {
          wateringDuration = (unsigned long)outputs[0];
          float intensity = outputs[1] / 100.0f; // Convert to 0-1 range
          
          Serial.print("Irrigation decision - Duration: ");
          Serial.print(wateringDuration);
          Serial.print(" seconds, Intensity: ");
          Serial.print(int(intensity * 100));
          Serial.println("%");
          
          // Start watering if duration > 0
          if (wateringDuration > 0) {
            Serial.println("Starting irrigation cycle");
            
            // Open valve
            digitalWrite(waterValvePin, HIGH);
            valveOpen = true;
            
            // Set pump intensity (PWM)
            analogWrite(pumpPin, (int)(intensity * 255));
            pumpActive = true;
            
            wateringStartTime = millis();
          } else {
            Serial.println("No irrigation needed at this time");
          }
          
          delete[] outputs;
        }
      } else {
        // Too soon since last watering
        unsigned long timeToNextWatering = (lastWateringTime + minWateringInterval - millis()) / 60000;
        Serial.print("Minimum watering interval not reached. Next possible watering in ");
        Serial.print(timeToNextWatering);
        Serial.println(" minutes");
      }
    }
  }
  
  delay(100);
}

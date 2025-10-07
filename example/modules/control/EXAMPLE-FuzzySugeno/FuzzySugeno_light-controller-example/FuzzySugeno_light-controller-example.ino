/*
 * FuzzySugeno_Light_Controller.ino
 * 
 * Automatic light controller with FuzzySugeno
 * Created on: 2025. 3. 30
 */

#define ENABLE_MODULE_FUZZY_SUGENO
#include "Kinematrix.h"

// Create a FuzzySugeno instance
FuzzySugeno lightController(2, 1, 4, 3);

// Pins
const int lightSensorPin = A0;
const int ledPin = 9;

// Time parameters
unsigned long lastDisplayTime = 0;
const unsigned long displayInterval = 2000;

void setup() {
  Serial.begin(115200);
  Serial.println("FuzzySugeno Light Controller");
  Serial.println("----------------------------");
  
  // Setup pins
  pinMode(ledPin, OUTPUT);
  
  // 1. Define input variables
  lightController.addInputVariable("ambient_light", 0, 1023);
  lightController.addInputVariable("time_of_day", 0, 24);
  
  // 2. Define fuzzy sets for ambient light
  float darkParams[] = {0, 0, 300};
  float dimParams[] = {200, 500, 800};
  float brightParams[] = {700, 1023, 1023};
  
  lightController.addFuzzySet(0, "dark", TRIANGULAR, darkParams);
  lightController.addFuzzySet(0, "dim", TRIANGULAR, dimParams);
  lightController.addFuzzySet(0, "bright", TRIANGULAR, brightParams);
  
  // 3. Define fuzzy sets for time of day
  float nightParams[] = {0, 4, 8};
  float dayParams[] = {7, 12, 17};
  float eveningParams[] = {16, 20, 24};
  
  lightController.addFuzzySet(1, "night", TRIANGULAR, nightParams);
  lightController.addFuzzySet(1, "day", TRIANGULAR, dayParams);
  lightController.addFuzzySet(1, "evening", TRIANGULAR, eveningParams);
  
  // 4. Define linear outputs using Sugeno method
  // LED brightness = a + b*light + c*time
  
  // Rule 1: IF ambient_light IS dark THEN high brightness regardless of time
  // LED = 1.0
  float coeffsDark[] = {1.0};
  lightController.addConstantOutput("max_brightness", 1.0);
  
  // Rule 2: IF ambient_light IS dim AND time_of_day IS day THEN moderate brightness
  // LED = 0.5
  float coeffsDimDay[] = {0.5};
  lightController.addConstantOutput("medium_brightness", 0.5);
  
  // Rule 3: IF ambient_light IS dim AND time_of_day IS evening THEN higher brightness
  // LED = 0.8
  float coeffsDimEvening[] = {0.8};
  lightController.addConstantOutput("high_brightness", 0.8);
  
  // Rule 4: IF ambient_light IS bright THEN low brightness (inverse relation)
  // LED = 0.3 - 0.0003*light (linear equation)
  float coeffsBright[] = {0.3, -0.0003, 0.0};
  lightController.addLinearOutput("dynamic_brightness", coeffsBright, 3);
  
  // 5. Define rules
  // Rule 1: IF ambient_light IS dark THEN max_brightness
  int rule1Vars[] = {0};
  int rule1Sets[] = {0}; // dark
  lightController.addRule(rule1Vars, rule1Sets, 1, 0, true);
  
  // Rule 2: IF ambient_light IS dim AND time_of_day IS day THEN medium_brightness
  int rule2Vars[] = {0, 1};
  int rule2Sets[] = {1, 1}; // dim, day
  lightController.addRule(rule2Vars, rule2Sets, 2, 1, true);
  
  // Rule 3: IF ambient_light IS dim AND time_of_day IS evening THEN high_brightness
  int rule3Vars[] = {0, 1};
  int rule3Sets[] = {1, 2}; // dim, evening
  lightController.addRule(rule3Vars, rule3Sets, 2, 2, true);
  
  // Rule 4: IF ambient_light IS bright THEN dynamic_brightness (linear function)
  int rule4Vars[] = {0};
  int rule4Sets[] = {2}; // bright
  lightController.addRule(rule4Vars, rule4Sets, 1, 3, true);
  
  if (lightController.hasError()) {
    Serial.print("Error: ");
    Serial.println(lightController.getErrorMessage());
  } else {
    Serial.println("Light controller initialized successfully!");
  }
}

void loop() {
  // Read ambient light level (0-1023)
  // For simulation, we'll use a sine wave pattern
  int ambientLight = (int)(500 + 500 * sin(millis() / 10000.0));
  
  // Get time of day (0-24), simulated here
  float timeOfDay = (millis() % 86400000) / 3600000.0;
  
  // Input array for fuzzy system
  float inputs[] = {(float)ambientLight, timeOfDay};
  
  // Evaluate fuzzy system
  float* outputs = lightController.evaluate(inputs);
  
  if (outputs != nullptr) {
    float ledBrightness = outputs[0];
    
    // Apply to LED output (PWM)
    analogWrite(ledPin, (int)(ledBrightness * 255));
    
    // Display results every 2 seconds to avoid flooding the serial monitor
    unsigned long currentTime = millis();
    if (currentTime - lastDisplayTime >= displayInterval) {
      lastDisplayTime = currentTime;
      
      Serial.print("Ambient Light: ");
      Serial.print(ambientLight);
      Serial.print("/1023, Time: ");
      Serial.print(timeOfDay);
      Serial.print("h, LED Brightness: ");
      Serial.print(int(ledBrightness * 100));
      Serial.println("%");
    }
    
    delete[] outputs;
  }
  
  delay(100);
}

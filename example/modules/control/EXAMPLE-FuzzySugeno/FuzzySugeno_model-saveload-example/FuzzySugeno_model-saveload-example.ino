/*
 * FuzzySugeno_SaveLoad_Model.ino
 * 
 * Example demonstrating saving and loading models with ESP32
 * Created on: 2025. 3. 30
 */

#define ENABLE_MODULE_FUZZY_SUGENO
#include "Kinematrix.h"

#ifdef ESP32
#include "SPIFFS.h"
#endif

// Create a FuzzySugeno instance
FuzzySugeno fuzzy(2, 2, 4, 3);

// Model file path
const char* modelFile = "/fuzzy_model.dat";

// Button pin for triggering save/load
const int buttonPin = 2;

// LED indicator
const int ledPin = 13;

// Button state
bool lastButtonState = HIGH;
bool currentButtonState = HIGH;

// Operation mode
enum Mode {
  NORMAL,
  SAVE_COMPLETE,
  LOAD_COMPLETE,
  ERROR
};

Mode currentMode = NORMAL;
unsigned long modeChangeTime = 0;
const unsigned long modeDisplayTime = 3000; // 3 seconds

void initializeFuzzyModel();
void updateLED();
void testFuzzyModel();

void setup() {
  Serial.begin(115200);
  Serial.println("FuzzySugeno Save/Load Model Example");
  Serial.println("----------------------------------");
  
  // Setup pins
  pinMode(buttonPin, INPUT_PULLUP);
  pinMode(ledPin, OUTPUT);
  
#ifdef ESP32
  // Initialize SPIFFS
  if (!SPIFFS.begin(true)) {
    Serial.println("SPIFFS initialization failed!");
    currentMode = ERROR;
    return;
  }
  Serial.println("SPIFFS initialized successfully");
#else
  Serial.println("Warning: Save/Load only works on ESP32!");
#endif
  
  // Create a simple fuzzy model for demonstration
  initializeFuzzyModel();
  
  Serial.println("Press the button to toggle between save and load operations");
  Serial.println("LED blinking pattern indicates the operation result:");
  Serial.println("- Solid ON: Normal operation");
  Serial.println("- Fast blink: Save completed");
  Serial.println("- Slow blink: Load completed");
  Serial.println("- SOS pattern: Error occurred");
}

void loop() {
  // Read button state with debounce
  currentButtonState = digitalRead(buttonPin);
  
  // Detect button press (falling edge)
  if (lastButtonState == HIGH && currentButtonState == LOW) {
    Serial.println("Button pressed!");
    
#ifdef ESP32
    // Check if model exists in SPIFFS
    if (SPIFFS.exists(modelFile)) {
      // File exists, load it
      Serial.print("Loading model from ");
      Serial.println(modelFile);
      
      if (fuzzy.loadModel(modelFile)) {
        Serial.println("Model loaded successfully!");
        currentMode = LOAD_COMPLETE;
      } else {
        Serial.print("Error loading model: ");
        Serial.println(fuzzy.getErrorMessage());
        currentMode = ERROR;
      }
    } else {
      // File doesn't exist, save the model
      Serial.print("Saving model to ");
      Serial.println(modelFile);
      
      if (fuzzy.saveModel(modelFile)) {
        Serial.println("Model saved successfully!");
        currentMode = SAVE_COMPLETE;
      } else {
        Serial.print("Error saving model: ");
        Serial.println(fuzzy.getErrorMessage());
        currentMode = ERROR;
      }
    }
#else
    Serial.println("Save/Load only works on ESP32!");
    currentMode = ERROR;
#endif
    
    modeChangeTime = millis();
  }
  
  // Update button state
  lastButtonState = currentButtonState;
  
  // Update LED based on current mode
  updateLED();
  
  // Reset mode after display time
  if (currentMode != NORMAL && millis() - modeChangeTime > modeDisplayTime) {
    currentMode = NORMAL;
  }
  
  // Test the fuzzy model with sample inputs
  static unsigned long lastTestTime = 0;
  if (millis() - lastTestTime > 2000) {
    lastTestTime = millis();
    testFuzzyModel();
  }
  
  delay(50);
}

void initializeFuzzyModel() {
  // 1. Define input variables
  fuzzy.addInputVariable("temperature", 0, 40);
  fuzzy.addInputVariable("humidity", 0, 100);
  
  // 2. Define fuzzy sets for temperature
  float tempLowParams[] = {0, 0, 20};
  float tempMedParams[] = {15, 25, 35};
  float tempHighParams[] = {30, 40, 40};
  
  fuzzy.addFuzzySet(0, "low", TRIANGULAR, tempLowParams);
  fuzzy.addFuzzySet(0, "medium", TRIANGULAR, tempMedParams);
  fuzzy.addFuzzySet(0, "high", TRIANGULAR, tempHighParams);
  
  // 3. Define fuzzy sets for humidity
  float humLowParams[] = {0, 0, 40};
  float humMedParams[] = {30, 50, 70};
  float humHighParams[] = {60, 100, 100};
  
  fuzzy.addFuzzySet(1, "low", TRIANGULAR, humLowParams);
  fuzzy.addFuzzySet(1, "medium", TRIANGULAR, humMedParams);
  fuzzy.addFuzzySet(1, "high", TRIANGULAR, humHighParams);
  
  // 4. Define outputs
  fuzzy.addConstantOutput("comfort_low", 0.2);
  fuzzy.addConstantOutput("comfort_medium", 0.5);
  fuzzy.addConstantOutput("comfort_high", 0.8);
  
  // Add linear output for dynamic AC power calculation
  float acPowerCoeffs[] = {0.5, 0.01, -0.005}; // baseline + temp_effect - humidity_effect
  fuzzy.addLinearOutput("ac_power", acPowerCoeffs, 3);
  
  // 5. Define rules
  
  // Rule 1: IF temp IS high AND humidity IS high THEN comfort = low, ac_power = dynamic
  int rule1Vars[] = {0, 1};
  int rule1Sets[] = {2, 2}; // high, high
  fuzzy.addRule(rule1Vars, rule1Sets, 2, 0, true); // comfort_low
  fuzzy.addRule(rule1Vars, rule1Sets, 2, 3, true); // ac_power (linear)
  
  // Rule 2: IF temp IS medium AND humidity IS medium THEN comfort = high
  int rule2Vars[] = {0, 1};
  int rule2Sets[] = {1, 1}; // medium, medium
  fuzzy.addRule(rule2Vars, rule2Sets, 2, 2, true); // comfort_high
  
  // Rule 3: IF temp IS low AND humidity IS medium THEN comfort = medium
  int rule3Vars[] = {0, 1};
  int rule3Sets[] = {0, 1}; // low, medium
  fuzzy.addRule(rule3Vars, rule3Sets, 2, 1, true); // comfort_medium
  
  if (fuzzy.hasError()) {
    Serial.print("Error initializing model: ");
    Serial.println(fuzzy.getErrorMessage());
    currentMode = ERROR;
  } else {
    Serial.println("Fuzzy model initialized successfully!");
  }
}

void testFuzzyModel() {
  // Create some test inputs
  float temperature = random(0, 40);
  float humidity = random(30, 90);
  
  float inputs[] = {temperature, humidity};
  
  // Evaluate fuzzy system
  float* outputs = fuzzy.evaluate(inputs);
  
  if (outputs != nullptr) {
    float comfortLevel = outputs[0];
    float acPower = outputs[1];
    
    Serial.print("Temp: ");
    Serial.print(temperature);
    Serial.print("°C, Humidity: ");
    Serial.print(humidity);
    Serial.print("%, Comfort: ");
    Serial.print(int(comfortLevel * 100));
    Serial.print("%, AC Power: ");
    Serial.print(int(acPower * 100));
    Serial.println("%");
    
    delete[] outputs;
  } else if (fuzzy.hasError()) {
    Serial.print("Evaluation error: ");
    Serial.println(fuzzy.getErrorMessage());
  }
}

void updateLED() {
  unsigned long currentTime = millis();
  
  switch (currentMode) {
    case NORMAL:
      // Solid ON
      digitalWrite(ledPin, HIGH);
      break;
      
    case SAVE_COMPLETE:
      // Fast blink (every 200ms)
      digitalWrite(ledPin, (currentTime / 200) % 2);
      break;
      
    case LOAD_COMPLETE:
      // Slow blink (every 500ms)
      digitalWrite(ledPin, (currentTime / 500) % 2);
      break;
      
    case ERROR:
      // SOS pattern (... --- ...)
      // Repeating pattern every 5 seconds
      int patternPosition = (currentTime % 5000) / 250;
      
      if (patternPosition < 3) {
        // Short blinks (...)
        digitalWrite(ledPin, (currentTime / 250) % 2);
      } else if (patternPosition < 6) {
        // Long blinks (---)
        digitalWrite(ledPin, (currentTime / 750) % 2);
      } else if (patternPosition < 9) {
        // Short blinks (...)
        digitalWrite(ledPin, (currentTime / 250) % 2);
      } else {
        // Pause
        digitalWrite(ledPin, LOW);
      }
      break;
  }
}

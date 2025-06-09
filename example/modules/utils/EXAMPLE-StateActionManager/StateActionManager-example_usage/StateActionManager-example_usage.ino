#define ENABLE_MODULE_STATE_ACTION_MANAGER
#include "Kinematrix.h"

StateActionManager manager;

const int TEMP_SENSOR_PIN = A0;
const int RELAY_PIN = 7;
const int LED_PIN = 13;
const int BUTTON_PIN = 2;
const float TEMP_THRESHOLD = 28.0;

float readTemperature() {
  int sensorValue = analogRead(TEMP_SENSOR_PIN);
  return (sensorValue * 3.3 / 1023.0) * 100.0;
}

void triggerRelay() {
  digitalWrite(RELAY_PIN, HIGH);
  Serial.println("Relay ON");
  delay(200);
  digitalWrite(RELAY_PIN, LOW);
}

void blinkLed() {
  digitalWrite(LED_PIN, !digitalRead(LED_PIN));
}

float readLightLevel() {
  return analogRead(A1) / 10.23; // 0-100 scale
}

bool readButton() {
  return digitalRead(BUTTON_PIN) == HIGH;
}

void buttonPressed(void* param) {
  Serial.println("Button pressed!");
  
  int* ledBlinkCount = (int*)param;
  Serial.print("Will blink LED ");
  Serial.print(*ledBlinkCount);
  Serial.println(" times");
}

void setup() {
  Serial.begin(9600);
  pinMode(RELAY_PIN, OUTPUT);
  pinMode(LED_PIN, OUTPUT);
  pinMode(BUTTON_PIN, INPUT_PULLUP);
  
  static int ledBlinkCount = 5;
  
  int tempMonitor = manager.addThresholdMonitor(
    "Temperature",          
    readTemperature,        
    TEMP_THRESHOLD,         
    StateActionManager::TRIG_RISING, 
    triggerRelay,           
    3,                      
    1000                    
  );
  
  int lightMonitor = manager.addRangeMonitor(
    "LightLevel",           
    readLightLevel,         
    20, 80,                 
    StateActionManager::TRIG_BOTH_WAYS, 
    []() {                  
      Serial.print("Light level in normal range: ");
      Serial.println(manager.getCurrentValue(1)); 
    }
  );
  
  int buttonMonitor = manager.addThresholdMonitor(
    "Button",               
    []() { return digitalRead(BUTTON_PIN) == LOW ? 1.0f : 0.0f; },
    0.5,                    
    StateActionManager::TRIG_RISING, 
    buttonPressed,          
    &ledBlinkCount          
  );
  
  int ledBlinker = manager.addTimeBasedMonitor(
    "LedBlinker",           
    500,                    
    blinkLed,               
    0                       
  );
  
  int statusReporter = manager.addTimeBasedMonitor(
    "StatusReporter",       
    5000,                   
    []() {
      Serial.println("\n----- System Status -----");
      Serial.print("Temperature: ");
      Serial.print(readTemperature());
      Serial.println(" °C");
      Serial.print("Light level: ");
      Serial.print(readLightLevel());
      Serial.println("%");
      Serial.print("Button state: ");
      Serial.println(digitalRead(BUTTON_PIN) == LOW ? "PRESSED" : "RELEASED");
      Serial.println("------------------------\n");
    }
  );
  
  Serial.println("StateActionManager Demo Started");
  
  manager.setEnabled(ledBlinker, false);
}

void loop() {
  manager.update();
  
  static unsigned long lastCheck = 0;
  if (millis() - lastCheck > 2000) {
    if (manager.isActionRunning(0)) {
      Serial.print("Temperature monitor is running, execution count: ");
      Serial.println(manager.getExecutionCount(0));
    }
    
    lastCheck = millis();
  }
}
#include <Arduino.h>
#define ENABLE_MODULE_VARIABLE_WATCHER
#include "Kinematrix.h"

VariableWatcher watcher;

enum SystemState {
  IDLE,
  STARTUP,
  RUNNING,
  ERROR,
  SHUTDOWN
};

SystemState currentState = IDLE;
SystemState lastState = IDLE;
float temperature = 25.0;
int errorCode = 0;
String statusMessage = "System idle";
unsigned long operationTime = 0;

void onStateChange(void* value) {
  SystemState* state = (SystemState*)value;
  
  switch (*state) {
    case IDLE:
      digitalWrite(LED_BUILTIN, LOW);
      break;
    case STARTUP:
      digitalWrite(LED_BUILTIN, HIGH);
      break;
    case RUNNING:
      // Blink LED in running state
      digitalWrite(LED_BUILTIN, millis() % 1000 > 500);
      break;
    case ERROR:
      // Fast blink in error state
      digitalWrite(LED_BUILTIN, millis() % 200 > 100);
      break;
    case SHUTDOWN:
      digitalWrite(LED_BUILTIN, LOW);
      break;
  }
}

void setup() {
  Serial.begin(9600);
  pinMode(LED_BUILTIN, OUTPUT);
  
  watcher.addVariable(&currentState, "State", onStateChange);
  watcher.addVariable(&temperature, "Temperature");
  watcher.addVariable(&errorCode, "ErrorCode");
  watcher.addVariable(&statusMessage, "Status");
  watcher.addVariable(&operationTime, "OperationTime");
  
  Serial.println("State Machine Example Started");
}

void loop() {
  // Simulasi perubahan state
  unsigned long currentMillis = millis();
  static unsigned long lastStateChange = 0;
  
  // Update operationTime
  operationTime = currentMillis / 1000;
  
  // Simulasi pembacaan sensor
  temperature = 25.0 + sin(currentMillis / 10000.0) * 10.0;
  
  // State transitions logic
  switch (currentState) {
    case IDLE:
      if (currentMillis - lastStateChange > 5000) {
        currentState = STARTUP;
        statusMessage = "System starting up";
        lastStateChange = currentMillis;
      }
      break;
      
    case STARTUP:
      if (currentMillis - lastStateChange > 3000) {
        currentState = RUNNING;
        statusMessage = "System running normally";
        lastStateChange = currentMillis;
      }
      break;
      
    case RUNNING:
      // Simulasi error ketika suhu terlalu tinggi
      if (temperature > 30.0) {
        currentState = ERROR;
        errorCode = 1; // Overtemperature error
        statusMessage = "ERROR: Temperature too high";
        lastStateChange = currentMillis;
      } 
      // Random error
      else if (random(1000) == 0) {
        currentState = ERROR;
        errorCode = 2; // Random error
        statusMessage = "ERROR: System malfunction";
        lastStateChange = currentMillis;
      }
      // Simulasi shutdown setelah 30 detik
      else if (operationTime > 30) {
        currentState = SHUTDOWN;
        statusMessage = "System shutting down";
        lastStateChange = currentMillis;
      }
      break;
      
    case ERROR:
      if (currentMillis - lastStateChange > 5000) {
        // Auto-recovery dari error
        currentState = RUNNING;
        errorCode = 0;
        statusMessage = "System recovered, running normally";
        lastStateChange = currentMillis;
      }
      break;
      
    case SHUTDOWN:
      if (currentMillis - lastStateChange > 3000) {
        currentState = IDLE;
        statusMessage = "System idle";
        lastStateChange = currentMillis;
      }
      break;
  }
  
  // Check for variable changes
  watcher.checkAll();
  
  // Output state changes
  if (watcher.isChange("State")) {
    String stateNames[] = {"IDLE", "STARTUP", "RUNNING", "ERROR", "SHUTDOWN"};
    Serial.print("State changed: ");
    Serial.println(stateNames[watcher.getValueInt("State")]);
  }
  
  if (watcher.isChange("Temperature") && temperature > 28.0) {
    Serial.print("Temperature alert: ");
    Serial.print(watcher.getValueFloat("Temperature"));
    Serial.println(" °C");
  }
  
  if (watcher.isChange("ErrorCode") && errorCode > 0) {
    Serial.print("Error detected, code: ");
    Serial.println(watcher.getValueInt("ErrorCode"));
  }
  
  if (watcher.isChange("Status")) {
    Serial.print("Status: ");
    Serial.println(watcher.getValueString("Status"));
  }
  
  delay(100);
}
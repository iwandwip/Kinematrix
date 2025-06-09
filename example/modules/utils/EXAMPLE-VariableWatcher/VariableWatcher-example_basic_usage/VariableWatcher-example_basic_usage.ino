#include <Arduino.h>
#define ENABLE_MODULE_VARIABLE_WATCHER
#include "Kinematrix.h"

VariableWatcher watcher;

int counter = 0;
float sensorValue = 0.0;

void setup() {
  Serial.begin(9600);
  
  watcher.addVariable(&counter, "Counter");
  watcher.addVariable(&sensorValue, "Sensor");
  
  Serial.println("Basic Usage Example Started");
}

void loop() {
  counter++;
  sensorValue = analogRead(A0) * 0.01;
  
  watcher.checkAll();
  
  if (watcher.isChange("Counter")) {
    Serial.print("Counter value: ");
    Serial.println(watcher.getValueInt("Counter"));
  }
  
  if (watcher.isChange("Sensor")) {
    Serial.print("Sensor reading: ");
    Serial.println(watcher.getValueFloat("Sensor"));
  }
  
  delay(1000);
}
#include <Arduino.h>
#define ENABLE_MODULE_VARIABLE_WATCHER
#include "Kinematrix.h"

VariableWatcher watcher;

float temperature = 0.0;
float humidity = 0.0;
float pressure = 0.0;
float lightLevel = 0.0;
int batteryLevel = 100;

void setup() {
  Serial.begin(9600);
  
  watcher.addVariable(&temperature, "Temperature");
  watcher.addVariable(&humidity, "Humidity");
  watcher.addVariable(&pressure, "Pressure");
  watcher.addVariable(&lightLevel, "Light");
  watcher.addVariable(&batteryLevel, "Battery");
  
  Serial.println("Multiple Sensors Example Started");
}

void loop() {
  // Simulasi pembacaan sensor
  temperature = 20.0 + random(-10, 10) * 0.1;
  humidity = 60.0 + random(-5, 5);
  pressure = 1013.0 + random(-10, 10) * 0.1;
  lightLevel = analogRead(A0) / 10.0;
  batteryLevel = max(0, batteryLevel - random(0, 2)); // Simulasi baterai menurun
  
  watcher.checkAll();
  
  if (watcher.isChange("Temperature")) {
    Serial.print("Temperature: ");
    Serial.print(watcher.getValueFloat("Temperature"));
    Serial.println(" °C");
  }
  
  if (watcher.isChange("Humidity")) {
    Serial.print("Humidity: ");
    Serial.print(watcher.getValueFloat("Humidity"));
    Serial.println(" %");
  }
  
  if (watcher.isChange("Pressure")) {
    Serial.print("Pressure: ");
    Serial.print(watcher.getValueFloat("Pressure"));
    Serial.println(" hPa");
  }
  
  if (watcher.isChange("Light")) {
    Serial.print("Light level: ");
    Serial.print(watcher.getValueFloat("Light"));
    Serial.println(" lux");
  }
  
  if (watcher.isChange("Battery")) {
    Serial.print("Battery level: ");
    Serial.print(watcher.getValueInt("Battery"));
    Serial.println(" %");
    
    if (watcher.getValueInt("Battery") < 20) {
      Serial.println("WARNING: Low battery!");
    }
  }
  
  delay(1000);
}
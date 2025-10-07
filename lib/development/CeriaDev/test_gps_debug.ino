#define ENABLE_DEVELOPMENT_CERIA_DEV_OP
#include "Kinematrix.h"

using namespace CeriaDevOP;

CeriaSensorGPS gps;

void setup() {
  Serial.begin(115200);
  delay(2000);
  
  Serial.println("=== CeriaSensorGPS Debug Test ===");
  Serial.println("Initializing GPS on Serial3...");
  
  gps.begin(Serial3, 9600);
  Serial.println("GPS begin() called");
  
  Serial.println("Waiting for GPS data...");
}

void loop() {
  gps.update();
  
  static uint32_t lastCheck = 0;
  if (millis() - lastCheck > 2000) {
    lastCheck = millis();
    
    Serial.println("--- GPS Status Check ---");
    Serial.println("Connected: " + String(gps.isConnected() ? "YES" : "NO"));
    Serial.println("Chars processed: " + String(gps.charsProcessed()));
    Serial.println("Has fix: " + String(gps.hasFix() ? "YES" : "NO"));
    Serial.println("Satellites: " + String(gps.getSatellites()));
    
    if (gps.hasNewData()) {
      Serial.println("NEW DATA RECEIVED!");
      if (gps.hasFix()) {
        Serial.println("Location: " + gps.getLocation());
        Serial.println("Speed: " + String(gps.getSpeed()) + " km/h");
        Serial.println("Date: " + gps.getDate());
        Serial.println("Time: " + gps.getTime());
      }
    }
    Serial.println();
  }
  
  delay(100);
}
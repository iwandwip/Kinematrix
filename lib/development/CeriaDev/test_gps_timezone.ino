#define ENABLE_DEVELOPMENT_CERIA_DEV_OP
#include "Kinematrix.h"

using namespace CeriaDevOP;

CeriaSensorGPS gps;

void setup() {
  Serial.begin(115200);
  delay(2000);
  
  Serial.println("=== CeriaSensorGPS Timezone Test ===");
  Serial.println("Testing different timezone configurations");
  
  gps.begin(Serial3, 9600);
  
  Serial.println("\n1. Setting Indonesia timezone (GMT+7)...");
  gps.setTimezone(7);
  Serial.println("Timezone set to GMT+" + String(gps.getTimezoneHours()));
  
  Serial.println("\n2. Waiting for GPS fix...");
}

void loop() {
  gps.update();
  
  static uint32_t lastDisplay = 0;
  if (millis() - lastDisplay > 3000) {
    lastDisplay = millis();
    
    Serial.println("\n--- GPS Status ---");
    Serial.println("Connected: " + String(gps.isConnected() ? "YES" : "NO"));
    Serial.println("Has fix: " + String(gps.hasFix() ? "YES" : "NO"));
    Serial.println("Satellites: " + String(gps.getSatellites()));
    Serial.println("Chars processed: " + String(gps.charsProcessed()));
    
    if (gps.hasFix()) {
      Serial.println("\n=== Time Comparison ===");
      Serial.println("UTC Time: " + gps.getTime());
      Serial.println("Local Time (WIB): " + gps.getLocalTime());
      
      Serial.println("\n=== Location Data ===");
      Serial.println("Location: " + gps.getLocation());
      Serial.println("Speed: " + String(gps.getSpeed(), 1) + " km/h");
      Serial.println("Date: " + gps.getDate());
      
      Serial.println("\n=== Timezone Settings ===");
      Serial.println("Timezone Hours: " + String(gps.getTimezoneHours()));
      Serial.println("Timezone Minutes: " + String(gps.getTimezoneMinutes()));
      
      static bool testOtherTimezones = false;
      if (!testOtherTimezones) {
        testOtherTimezones = true;
        Serial.println("\n--- Testing Other Timezones ---");
        
        gps.setTimezone(0);
        Serial.println("GMT+0 (London): " + gps.getLocalTime());
        
        gps.setTimezone(8);
        Serial.println("GMT+8 (Singapore): " + gps.getLocalTime());
        
        gps.setTimezone(9);
        Serial.println("GMT+9 (Japan): " + gps.getLocalTime());
        
        gps.setTimezone(-5);
        Serial.println("GMT-5 (New York): " + gps.getLocalTime());
        
        gps.setTimezone(7, 30);
        Serial.println("GMT+7:30 (India): " + gps.getLocalTime());
        
        gps.setTimezone(7);
        Serial.println("Back to WIB (GMT+7): " + gps.getLocalTime());
      }
    }
  }
  
  delay(100);
}
#include <WiFi.h>
#include <ESPAsyncWebServer.h>
#include <LittleFS.h>
#include <ArduinoJson.h>
#include "WiFiManager.h"

WiFiManager wm;

void setup() {
    Serial.begin(115200);
    Serial.println();
    Serial.println("=== WiFiManager Simple Test ===");

    if (!wm.begin()) {
        Serial.println("ERROR: Failed to initialize WiFi Manager");
        return;
    }

    Serial.println("WiFi Manager initialized");
    Serial.println("Starting auto-connect...");

    if (!wm.autoConnect()) {
        Serial.println("No saved credentials - starting config mode");
    } else {
        Serial.println("Connecting with saved credentials...");
    }
}

void loop() {
    wm.loop();

    static unsigned long lastStatus = 0;
    if (millis() - lastStatus > 10000) {
        Serial.print("Status: ");
        if (wm.isConnected()) {
            Serial.print("Connected to ");
            Serial.print(wm.getSSID());
            Serial.print(" | IP: ");
            Serial.println(wm.getIP());
        } else if (wm.isConfigMode()) {
            Serial.println("Config Mode - Access Point Active");
            Serial.println("Connect to 'ESP32-WiFiManager' and go to 192.168.4.1");
        } else {
            Serial.println("Disconnected");
        }
        lastStatus = millis();
    }

    delay(100);
}
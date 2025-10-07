#define ENABLE_MODULE_WIFI_MANAGER
#include "Kinematrix.h"

WiFiManager wm;

void onWiFiConnected() {
    Serial.println("WiFi Connected!");
    Serial.print("IP Address: ");
    Serial.println(wm.getIP());
    Serial.print("SSID: ");
    Serial.println(wm.getSSID());
}

void onWiFiDisconnected() {
    Serial.println("WiFi Disconnected!");
}

void onConfigMode() {
    Serial.println("Entering Configuration Mode");
    Serial.println("Connect to ESP32-WiFiManager AP");
    Serial.println("Open browser to 192.168.4.1");
}

void setup() {
    Serial.begin(115200);
    Serial.println("WiFi Manager Example");
    
    wm.setAPCredentials("ESP32-WiFiManager", "12345678");
    wm.setTimeout(30000);
    wm.setConfigTimeout(300000);
    wm.enableAutoReconnect(true);
    
    wm.onConnected(onWiFiConnected);
    wm.onDisconnected(onWiFiDisconnected);
    wm.onConfigMode(onConfigMode);
    
    if (!wm.begin()) {
        Serial.println("Failed to initialize WiFi Manager");
        return;
    }
    
    wm.autoConnect();
}

void loop() {
    wm.loop();
    
    if (wm.isConnected()) {
        static unsigned long lastPrint = 0;
        if (millis() - lastPrint > 10000) {
            Serial.println("WiFi Status: Connected");
            Serial.print("Signal Quality: ");
            Serial.print(wm.getRSSI());
            Serial.println(" dBm");
            lastPrint = millis();
        }
    }
    
    delay(100);
}
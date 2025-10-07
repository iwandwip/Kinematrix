#define ENABLE_MODULE_WIFI_MANAGER
#include "Kinematrix.h"

WiFiManager wm;

void onWiFiConnected() {
    Serial.println("=== WiFi Connected ===");
    Serial.print("SSID: ");
    Serial.println(wm.getSSID());
    Serial.print("IP Address: ");
    Serial.println(wm.getIP());
    Serial.print("Signal Strength: ");
    Serial.print(wm.getRSSI());
    Serial.println(" dBm");
}

void onWiFiDisconnected() {
    Serial.println("=== WiFi Disconnected ===");
    Serial.println("Attempting to reconnect...");
}

void onConfigMode() {
    Serial.println("=== Configuration Mode Active ===");
    Serial.println("WiFi Manager is now in AP mode");
    Serial.print("AP Name: ");
    Serial.println("MyESP32-Config");
    Serial.println("AP Password: mypassword123");
    Serial.println("Web Interface: http://192.168.4.1");
    Serial.println("Configure your WiFi settings through the web interface");
}

void setup() {
    Serial.begin(115200);
    Serial.println("\n=== Advanced WiFi Manager Example ===");
    Serial.println("Kinematrix WiFi Manager v1.0");
    
    Serial.println("Setting up WiFi Manager...");
    wm.setAPCredentials("MyESP32-Config", "mypassword123");
    wm.setHostname("MyESP32Device");
    
    wm.setTimeout(45000);
    wm.setConfigTimeout(600000);
    wm.setRetryDelay(10000);
    wm.setReconnectInterval(30000);
    wm.setMinSignalQuality(-75);
    
    wm.enableAutoReconnect(true);
    
    wm.onConnected(onWiFiConnected);
    wm.onDisconnected(onWiFiDisconnected);
    wm.onConfigMode(onConfigMode);
    
    if (!wm.begin()) {
        Serial.println("ERROR: Failed to initialize WiFi Manager!");
        Serial.println("Check if LittleFS is properly configured");
        return;
    }
    
    Serial.println("WiFi Manager initialized successfully");
    Serial.println("Starting auto-connect process...");
    
    if (!wm.autoConnect()) {
        Serial.println("No saved WiFi credentials found");
        Serial.println("Starting configuration mode...");
    } else {
        Serial.println("Attempting to connect with saved credentials...");
    }
}

void loop() {
    wm.loop();
    
    static unsigned long lastStatus = 0;
    if (millis() - lastStatus > 30000) {
        Serial.println("\n--- Status Report ---");
        
        switch (wm.getState()) {
            case WM_IDLE:
                Serial.println("State: Idle");
                break;
            case WM_CONNECTING:
                Serial.println("State: Connecting to WiFi...");
                break;
            case WM_CONNECTED:
                Serial.println("State: Connected");
                Serial.print("SSID: ");
                Serial.println(wm.getSSID());
                Serial.print("IP: ");
                Serial.println(wm.getIP());
                Serial.print("Signal: ");
                Serial.print(wm.getRSSI());
                Serial.println(" dBm");
                break;
            case WM_CONFIG_MODE:
                Serial.println("State: Configuration Mode");
                Serial.println("Web interface available at: http://192.168.4.1");
                break;
            case WM_RETRY:
                Serial.println("State: Retrying connection...");
                break;
        }
        
        lastStatus = millis();
    }
    
    static unsigned long lastMemory = 0;
    if (millis() - lastMemory > 60000) {
        Serial.print("Free Heap: ");
        Serial.print(ESP.getFreeHeap());
        Serial.println(" bytes");
        lastMemory = millis();
    }
    
    delay(100);
}
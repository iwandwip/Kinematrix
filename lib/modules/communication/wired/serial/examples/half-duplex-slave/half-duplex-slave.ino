/*
 * Half-Duplex Communication - Slave Device
 * 
 * Hardware Setup (RS485 style or single-wire):
 * Slave (Arduino):         Master (ESP32):  
 * ├─ DATA (Pin 8) ←───────→ DATA (GPIO16)
 * ├─ GND ─────────────── GND
 * 
 * Slave Role: Response provider
 * - Never initiates communication (listen-only mode)
 * - Responds only when master requests
 * - Implements collision avoidance
 * - Provides sensor data and status
 */

#include "ceria-serial.h"
#include <SoftwareSerial.h>

using namespace CeriaDevOP;

SoftwareSerial swSerial(8, 9);
CeriaSerial link;

float temperature = 22.5;
float pressure = 1.013;
int batteryLevel = 85;
String deviceStatus = "operational";

unsigned long lastSensorUpdate = 0;
const unsigned long SENSOR_UPDATE_INTERVAL = 500;

void setup() {
    Serial.begin(115200);
    Serial.println("=== Half-Duplex Slave ===");
    
    swSerial.begin(9600);
    if (link.begin(swSerial, 9600, FRAME_64B)) {
        Serial.println("✅ Half-duplex slave initialized");
    } else {
        Serial.println("❌ Failed to initialize slave");
        return;
    }
    
    link.setMode(HALF_DUPLEX);
    link.setRetries(2);
    link.setTimeout(500);
    
    link.onReceive([](const String& key, const String& value) {
        Serial.println("📨 Master request: " + key + " = " + value);
        
        if (key == "request") {
            handleDataRequest(value);
        }
        else if (key == "command") {
            handleCommand(value);
        }
        else if (key == "ping") {
            delay(random(10, 50));
            link.send("pong", "slave_online");
            Serial.println("🏓 Responded to master ping");
        }
    });
    
    link.onError([](CeriaSerialError error) {
        Serial.println("⚠️ Communication error: " + String(error));
        
        if (error == CERIA_SERIAL_ERR_COLLISION) {
            Serial.println("🔄 Bus collision - backing off");
        }
    });
    
    randomSeed(analogRead(A0));
    
    Serial.println("🎯 Slave ready - waiting for master requests");
}

void loop() {
    link.tick();
    
    updateSensors();
    displayStatus();
    
    delay(50);
}

void handleDataRequest(const String& dataType) {
    Serial.println("🔍 Processing request for: " + dataType);
    
    delay(random(10, 100));
    
    if (dataType == "temperature") {
        if (link.send("temperature", temperature)) {
            Serial.println("📤 Temperature sent: " + String(temperature) + "°C");
        } else {
            Serial.println("❌ Failed to send temperature");
        }
    }
    else if (dataType == "pressure") {
        if (link.send("pressure", pressure)) {
            Serial.println("📤 Pressure sent: " + String(pressure) + " bar");
        } else {
            Serial.println("❌ Failed to send pressure");
        }
    }
    else if (dataType == "battery") {
        if (link.send("battery", batteryLevel)) {
            Serial.println("📤 Battery level sent: " + String(batteryLevel) + "%");
        } else {
            Serial.println("❌ Failed to send battery level");
        }
    }
    else if (dataType == "status") {
        if (link.send("status", deviceStatus)) {
            Serial.println("📤 Status sent: " + deviceStatus);
        } else {
            Serial.println("❌ Failed to send status");
        }
    }
    else if (dataType == "all") {
        sendAllData();
    }
    else {
        link.send("error", "unknown_request_" + dataType);
        Serial.println("❓ Unknown data request: " + dataType);
    }
}

void handleCommand(const String& command) {
    Serial.println("⚡ Processing command: " + command);
    
    delay(random(50, 150));
    
    if (command == "reset") {
        Serial.println("🔄 Reset command received - reinitializing...");
        temperature = 22.5;
        pressure = 1.013;
        batteryLevel = 85;
        deviceStatus = "operational";
        link.send("ack", "reset_complete");
    }
    else if (command == "calibrate") {
        Serial.println("🎯 Calibration command received");
        deviceStatus = "calibrating";
        link.send("ack", "calibration_started");
        
        delay(2000);
        
        deviceStatus = "operational";
        link.send("ack", "calibration_complete");
    }
    else if (command == "sleep") {
        Serial.println("😴 Sleep command received");
        deviceStatus = "sleeping";
        link.send("ack", "entering_sleep_mode");
    }
    else if (command == "wake") {
        Serial.println("⏰ Wake command received");
        deviceStatus = "operational";
        link.send("ack", "wake_up_complete");
    }
    else {
        link.send("error", "unknown_command_" + command);
        Serial.println("❓ Unknown command: " + command);
    }
}

void sendAllData() {
    Serial.println("📦 Sending all sensor data...");
    
    delay(random(20, 80));
    link.send("temperature", temperature);
    
    delay(random(20, 80));
    link.send("pressure", pressure);
    
    delay(random(20, 80));
    link.send("battery", batteryLevel);
    
    delay(random(20, 80));
    link.send("status", deviceStatus);
    
    Serial.println("✅ All data sent to master");
}

void updateSensors() {
    if (millis() - lastSensorUpdate > SENSOR_UPDATE_INTERVAL) {
        
        if (deviceStatus == "operational") {
            temperature += random(-30, 31) / 100.0;
            temperature = constrain(temperature, 18.0, 35.0);
            
            pressure += random(-20, 21) / 1000.0;
            pressure = constrain(pressure, 0.8, 1.2);
            
            batteryLevel += random(-2, 1);
            batteryLevel = constrain(batteryLevel, 0, 100);
            
            if (batteryLevel < 20) {
                deviceStatus = "low_battery";
            } else if (batteryLevel < 5) {
                deviceStatus = "critical_battery";
            } else if (temperature > 32.0) {
                deviceStatus = "overheating";
            } else if (deviceStatus != "operational" && 
                       deviceStatus != "calibrating" && 
                       deviceStatus != "sleeping") {
                deviceStatus = "operational";
            }
        }
        
        lastSensorUpdate = millis();
    }
}

void displayStatus() {
    static unsigned long lastDisplay = 0;
    
    if (millis() - lastDisplay > 15000) {
        Serial.println("\n🏭 === SLAVE STATUS ===");
        Serial.println("📊 Current Sensor Values:");
        Serial.println("  🌡️  Temperature: " + String(temperature, 1) + "°C");
        Serial.println("  🔺 Pressure: " + String(pressure, 3) + " bar");
        Serial.println("  🔋 Battery: " + String(batteryLevel) + "%");
        Serial.println("  📟 Status: " + deviceStatus);
        
        if (link.isConnected()) {
            Serial.println("🔗 Master Connection: ACTIVE");
        } else {
            Serial.println("🔗 Master Connection: LOST");
        }
        
        uint32_t sent, received, errors;
        link.getStats(sent, received, errors);
        Serial.println("📊 Communication Stats:");
        Serial.println("  📤 Responses sent: " + String(sent));
        Serial.println("  📥 Requests received: " + String(received));
        Serial.println("  ❌ Communication errors: " + String(errors));
        
        if (batteryLevel < 20) {
            Serial.println("⚠️  WARNING: Low battery level!");
        }
        if (temperature > 30.0) {
            Serial.println("🌡️  WARNING: High temperature!");
        }
        
        Serial.println("🎯 Waiting for master requests...");
        Serial.println("========================\n");
        
        lastDisplay = millis();
    }
}
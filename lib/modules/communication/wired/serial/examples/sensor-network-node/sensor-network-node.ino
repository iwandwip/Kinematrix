/*
 * Sensor Network - Node (Sensor Device)
 * 
 * Hardware Setup:
 * Node (Arduino):          Hub (ESP32):
 * ├─ TX (Pin 11) ─────────→ RX2 (GPIO16)
 * └─ RX (Pin 10) ←────── TX2 (GPIO17)
 * ├─ GND ─────────────── GND
 * 
 * Node Role: Autonomous sensor device
 * - Collects sensor data continuously
 * - Reports data to hub on request or schedule
 * - Manages power consumption and battery status
 * - Handles alerts and emergency conditions
 */

#include "ceria-serial.h"
#include <SoftwareSerial.h>

using namespace CeriaDevOP;

SoftwareSerial swSerial(10, 11);
CeriaSerial link;

const String NODE_ID = "garden_node_01";

float temperature = 25.0;
float humidity = 65.0;
int soilMoisture = 45;
int lightLevel = 600;
int batteryLevel = 85;
String nodeStatus = "operational";

unsigned long lastSensorUpdate = 0;
const unsigned long SENSOR_UPDATE_INTERVAL = 2000;

unsigned long lastHeartbeat = 0;
const unsigned long HEARTBEAT_INTERVAL = 30000;

unsigned long lastAutoReport = 0;
const unsigned long AUTO_REPORT_INTERVAL = 60000;

bool registeredWithHub = false;

void setup() {
    Serial.begin(115200);
    Serial.println("=== Sensor Network Node: " + NODE_ID + " ===");
    
    swSerial.begin(115200);
    if (link.begin(swSerial, 115200, FRAME_128B)) {
        Serial.println("✅ Node initialized - connecting to hub");
    } else {
        Serial.println("❌ Failed to initialize node");
        return;
    }
    
    link.setMode(FULL_DUPLEX);
    link.setRetries(3);
    link.setTimeout(2000);
    
    link.onReceive([](const String& key, const String& value) {
        Serial.println("📨 Hub message: " + key + " = " + value);
        
        if (key == "request_data") {
            if (value == NODE_ID || value == "all") {
                sendAllSensorData();
            }
        }
        else if (key == "hub_ack") {
            if (value == "node_registered") {
                registeredWithHub = true;
                Serial.println("✅ Successfully registered with hub");
            }
        }
        else if (key == "ping") {
            if (value == NODE_ID || value == "all") {
                link.send("pong", NODE_ID);
                Serial.println("🏓 Responded to hub ping");
            }
        }
        else if (key == "command") {
            handleCommand(value);
        }
        else if (key == "alert_ack") {
            Serial.println("✅ Hub acknowledged alert");
        }
    });
    
    link.onError([](CeriaSerialError error) {
        Serial.println("⚠️ Node communication error: " + String(error));
    });
    
    randomSeed(analogRead(A0));
    
    registerWithHub();
    
    Serial.println("🎯 Node ready - collecting sensor data");
}

void loop() {
    link.tick();
    
    if (millis() - lastSensorUpdate > SENSOR_UPDATE_INTERVAL) {
        updateSensors();
        checkAlertConditions();
        lastSensorUpdate = millis();
    }
    
    if (millis() - lastHeartbeat > HEARTBEAT_INTERVAL) {
        sendHeartbeat();
        lastHeartbeat = millis();
    }
    
    if (millis() - lastAutoReport > AUTO_REPORT_INTERVAL) {
        if (registeredWithHub) {
            sendAllSensorData();
            lastAutoReport = millis();
        }
    }
    
    if (!registeredWithHub && millis() > 10000) {
        registerWithHub();
    }
    
    displayNodeStatus();
    
    delay(200);
}

void registerWithHub() {
    Serial.println("📡 Registering with hub...");
    link.send("node_id", NODE_ID);
    link.send("node_" + NODE_ID + "_status", "initializing");
    
    delay(1000);
    sendAllSensorData();
}

void sendHeartbeat() {
    if (registeredWithHub) {
        link.send("heartbeat", NODE_ID);
        Serial.println("💓 Heartbeat sent to hub");
    }
}

void sendAllSensorData() {
    Serial.println("📊 Sending all sensor data to hub...");
    
    link.send("node_" + NODE_ID + "_temp", temperature);
    delay(50);
    
    link.send("node_" + NODE_ID + "_humid", humidity);
    delay(50);
    
    link.send("node_" + NODE_ID + "_soil", soilMoisture);
    delay(50);
    
    link.send("node_" + NODE_ID + "_light", lightLevel);
    delay(50);
    
    link.send("node_" + NODE_ID + "_battery", batteryLevel);
    delay(50);
    
    link.send("node_" + NODE_ID + "_status", nodeStatus);
    
    Serial.println("📤 All sensor data sent");
}

void updateSensors() {
    if (nodeStatus == "operational" || nodeStatus == "low_battery") {
        
        temperature += random(-20, 21) / 100.0;
        temperature = constrain(temperature, 15.0, 35.0);
        
        humidity += random(-30, 31) / 100.0;
        humidity = constrain(humidity, 30.0, 90.0);
        
        soilMoisture += random(-5, 6);
        soilMoisture = constrain(soilMoisture, 0, 100);
        
        lightLevel += random(-50, 51);
        lightLevel = constrain(lightLevel, 0, 1023);
        
        batteryLevel += random(-1, 0);
        batteryLevel = constrain(batteryLevel, 0, 100);
        
        updateNodeStatus();
    }
}

void updateNodeStatus() {
    if (batteryLevel < 10) {
        nodeStatus = "critical_battery";
    } else if (batteryLevel < 25) {
        nodeStatus = "low_battery";
    } else if (temperature > 32.0) {
        nodeStatus = "overheating";
    } else if (soilMoisture < 20) {
        nodeStatus = "dry_soil";
    } else if (soilMoisture > 80) {
        nodeStatus = "wet_soil";
    } else {
        nodeStatus = "operational";
    }
}

void checkAlertConditions() {
    static String lastAlertSent = "";
    static unsigned long lastAlertTime = 0;
    
    String alertMessage = "";
    
    if (batteryLevel < 10) {
        alertMessage = NODE_ID + "_critical_battery_" + String(batteryLevel);
    }
    else if (temperature > 35.0) {
        alertMessage = NODE_ID + "_overheating_" + String(temperature);
    }
    else if (soilMoisture < 10) {
        alertMessage = NODE_ID + "_soil_critical_" + String(soilMoisture);
    }
    
    if (alertMessage.length() > 0 && 
        alertMessage != lastAlertSent && 
        (millis() - lastAlertTime > 60000)) {
        
        Serial.println("🚨 Sending alert: " + alertMessage);
        link.send("alert", alertMessage);
        
        lastAlertSent = alertMessage;
        lastAlertTime = millis();
    }
}

void handleCommand(const String& command) {
    Serial.println("⚡ Processing command: " + command);
    
    if (command == "reset_" + NODE_ID || command == "reset_all") {
        Serial.println("🔄 Reset command received");
        
        temperature = 25.0;
        humidity = 65.0;
        soilMoisture = 45;
        lightLevel = 600;
        batteryLevel = 85;
        nodeStatus = "operational";
        
        link.send("node_" + NODE_ID + "_ack", "reset_complete");
    }
    else if (command == "sleep_" + NODE_ID || command == "sleep_all") {
        Serial.println("😴 Sleep command received");
        nodeStatus = "sleeping";
        link.send("node_" + NODE_ID + "_ack", "entering_sleep");
        
        delay(5000);
        
        nodeStatus = "operational";
        link.send("node_" + NODE_ID + "_ack", "wake_from_sleep");
    }
    else if (command == "calibrate_" + NODE_ID) {
        Serial.println("🎯 Calibration command received");
        nodeStatus = "calibrating";
        link.send("node_" + NODE_ID + "_ack", "calibration_started");
        
        delay(3000);
        
        nodeStatus = "operational";
        link.send("node_" + NODE_ID + "_ack", "calibration_complete");
    }
}

void displayNodeStatus() {
    static unsigned long lastDisplay = 0;
    
    if (millis() - lastDisplay > 15000) {
        Serial.println("\n🏠 === NODE STATUS: " + NODE_ID + " ===");
        Serial.println("📊 Current Sensor Readings:");
        Serial.println("  🌡️  Temperature: " + String(temperature, 1) + "°C");
        Serial.println("  💧 Humidity: " + String(humidity, 1) + "%");
        Serial.println("  🌱 Soil Moisture: " + String(soilMoisture) + "%");
        Serial.println("  💡 Light Level: " + String(lightLevel));
        Serial.println("  🔋 Battery: " + String(batteryLevel) + "%");
        Serial.println("  📟 Status: " + nodeStatus);
        
        Serial.println("🔗 Hub Registration: " + String(registeredWithHub ? "ACTIVE" : "PENDING"));
        
        if (link.isConnected()) {
            Serial.println("📡 Hub Connection: ACTIVE");
        } else {
            Serial.println("📡 Hub Connection: LOST");
        }
        
        uint32_t sent, received, errors;
        link.getStats(sent, received, errors);
        Serial.println("📊 Comm Stats - Sent:" + String(sent) + " Recv:" + String(received) + " Err:" + String(errors));
        
        if (batteryLevel < 25) {
            Serial.println("⚠️  WARNING: Low battery - " + String(batteryLevel) + "%");
        }
        if (soilMoisture < 20) {
            Serial.println("🌱 WARNING: Soil moisture low - " + String(soilMoisture) + "%");
        }
        
        Serial.println("=====================================\n");
        
        lastDisplay = millis();
    }
}
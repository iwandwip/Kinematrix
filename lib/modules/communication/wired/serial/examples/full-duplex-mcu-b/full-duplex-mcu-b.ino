/*
 * Full-Duplex Communication - MCU B (Control Center)
 * 
 * Hardware Setup:
 * MCU B (ESP32):           MCU A (ESP32):
 * ├─ TX2 (GPIO17) ────────→ RX2 (GPIO16)
 * └─ RX2 (GPIO16) ←────── TX2 (GPIO17)
 * ├─ GND ─────────────── GND
 * 
 * MCU B Role: Control center + data logger
 * - Receives sensor data from MCU A
 * - Makes control decisions based on sensor data
 * - Sends control commands to MCU A
 * - Logs data and provides user interface
 */

#include "ceria-serial.h"

using namespace CeriaDevOP;

CeriaSerial link;

struct SensorData {
    float temperature = 0.0;
    float humidity = 0.0;
    int lightLevel = 0;
    unsigned long uptime = 0;
    unsigned long lastUpdate = 0;
} sensors;

struct ActuatorStatus {
    bool ledOn = false;
    int fanSpeed = 0;
    unsigned long lastUpdate = 0;
} actuators;

unsigned long lastControlCheck = 0;
const unsigned long CONTROL_INTERVAL = 3000;

unsigned long lastPing = 0;
const unsigned long PING_INTERVAL = 10000;

void setup() {
    Serial.begin(115200);
    Serial.println("=== MCU B - Control Center ===");
    
    if (link.begin(Serial2, 115200, FRAME_256B)) {
        Serial.println("✅ Full-duplex link established with MCU A");
    } else {
        Serial.println("❌ Failed to establish link");
        return;
    }
    
    link.setMode(FULL_DUPLEX);
    link.setRetries(3);
    link.setTimeout(2000);
    
    link.onReceive([](const String& key, const String& value) {
        Serial.println("📈 Data from MCU A: " + key + " = " + value);
        
        if (key == "temperature") {
            sensors.temperature = value.toFloat();
            sensors.lastUpdate = millis();
        }
        else if (key == "humidity") {
            sensors.humidity = value.toFloat();
            sensors.lastUpdate = millis();
        }
        else if (key == "light") {
            sensors.lightLevel = value.toInt();
            sensors.lastUpdate = millis();
        }
        else if (key == "uptime") {
            sensors.uptime = value.toInt();
            sensors.lastUpdate = millis();
        }
        else if (key == "led_status") {
            actuators.ledOn = (value == "on" || value == "true");
            actuators.lastUpdate = millis();
        }
        else if (key == "fan_status") {
            actuators.fanSpeed = value.toInt();
            actuators.lastUpdate = millis();
        }
        else if (key == "pong") {
            Serial.println("🏓 MCU A responded: " + value);
        }
    });
    
    link.onError([](CeriaSerialError error) {
        Serial.println("⚠️ Communication error: " + String(error));
        
        if (error == CERIA_SERIAL_ERR_NO_CONNECTION) {
            Serial.println("🔄 Attempting to reconnect to MCU A...");
        }
    });
    
    Serial.println("🎯 MCU B ready - monitoring MCU A and making control decisions");
}

void loop() {
    link.tick();
    
    if (millis() - lastControlCheck > CONTROL_INTERVAL) {
        makeControlDecisions();
        lastControlCheck = millis();
    }
    
    if (millis() - lastPing > PING_INTERVAL) {
        link.send("ping", "health_check");
        lastPing = millis();
    }
    
    processUserCommands();
    displayDashboard();
    
    delay(100);
}

void makeControlDecisions() {
    Serial.println("🤖 Making control decisions based on sensor data...");
    
    if (sensors.lastUpdate > 0 && (millis() - sensors.lastUpdate < 5000)) {
        
        if (sensors.temperature > 30.0) {
            if (actuators.fanSpeed < 80) {
                link.send("fan_speed", 80);
                Serial.println("🌡️  High temp detected - increasing fan to 80%");
            }
        } else if (sensors.temperature < 23.0) {
            if (actuators.fanSpeed > 20) {
                link.send("fan_speed", 20);
                Serial.println("🌡️  Low temp detected - reducing fan to 20%");
            }
        }
        
        if (sensors.lightLevel < 200) {
            if (!actuators.ledOn) {
                link.send("led", "on");
                Serial.println("💡 Dark environment - turning LED on");
            }
        } else if (sensors.lightLevel > 800) {
            if (actuators.ledOn) {
                link.send("led", "off");
                Serial.println("💡 Bright environment - turning LED off");
            }
        }
        
        if (sensors.humidity > 75.0) {
            Serial.println("💧 High humidity warning! Consider ventilation.");
        }
        
    } else {
        Serial.println("⚠️  No recent sensor data - requesting update");
        link.send("request_sensors", "all");
    }
}

void processUserCommands() {
    if (Serial.available()) {
        String command = Serial.readStringUntil('\n');
        command.trim();
        
        if (command.startsWith("led ")) {
            String state = command.substring(4);
            link.send("led", state);
            Serial.println("👤 User command: LED " + state);
        }
        else if (command.startsWith("fan ")) {
            String speed = command.substring(4);
            link.send("fan_speed", speed);
            Serial.println("👤 User command: Fan " + speed + "%");
        }
        else if (command == "status") {
            link.send("request_sensors", "all");
            Serial.println("👤 User requested status update");
        }
        else if (command == "ping") {
            link.send("ping", "user_ping");
            Serial.println("👤 User ping sent");
        }
        else if (command == "help") {
            Serial.println("📖 Available commands:");
            Serial.println("  led on/off    - Control LED");
            Serial.println("  fan 0-100     - Set fan speed");
            Serial.println("  status        - Request sensor update");
            Serial.println("  ping          - Test connection");
            Serial.println("  help          - Show this help");
        }
        else if (command.length() > 0) {
            Serial.println("❓ Unknown command: " + command + " (type 'help' for commands)");
        }
    }
}

void displayDashboard() {
    static unsigned long lastDisplay = 0;
    
    if (millis() - lastDisplay > 5000) {
        Serial.println("\n🖥️  === CONTROL CENTER DASHBOARD ===");
        
        unsigned long dataAge = sensors.lastUpdate > 0 ? (millis() - sensors.lastUpdate) / 1000 : 999;
        Serial.println("📊 Sensor Data (age: " + String(dataAge) + "s):");
        Serial.println("  🌡️  Temperature: " + String(sensors.temperature, 1) + "°C");
        Serial.println("  💧 Humidity: " + String(sensors.humidity, 1) + "%");
        Serial.println("  💡 Light: " + String(sensors.lightLevel));
        Serial.println("  ⏰ MCU A Uptime: " + String(sensors.uptime) + "s");
        
        unsigned long actuatorAge = actuators.lastUpdate > 0 ? (millis() - actuators.lastUpdate) / 1000 : 999;
        Serial.println("🎛️  Actuator Status (age: " + String(actuatorAge) + "s):");
        Serial.println("  💡 LED: " + String(actuators.ledOn ? "ON" : "OFF"));
        Serial.println("  🌀 Fan: " + String(actuators.fanSpeed) + "%");
        
        if (link.isConnected()) {
            Serial.println("🔗 MCU A Connection: ACTIVE");
        } else {
            Serial.println("🔗 MCU A Connection: LOST");
        }
        
        uint32_t sent, received, errors;
        link.getStats(sent, received, errors);
        float successRate = sent > 0 ? ((float)(sent - errors) / sent) * 100.0 : 0.0;
        Serial.println("📈 Communication Stats:");
        Serial.println("  📤 Sent: " + String(sent) + " | 📥 Received: " + String(received));
        Serial.println("  ❌ Errors: " + String(errors) + " | ✅ Success: " + String(successRate, 1) + "%");
        
        Serial.println("💬 Type 'help' for available commands");
        Serial.println("======================================\n");
        
        lastDisplay = millis();
    }
}
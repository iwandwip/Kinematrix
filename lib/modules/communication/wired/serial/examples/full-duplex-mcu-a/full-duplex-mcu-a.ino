/*
 * Full-Duplex Communication - MCU A (Sensor Controller)
 * 
 * Hardware Setup:
 * MCU A (ESP32):           MCU B (ESP32):
 * ├─ TX2 (GPIO17) ────────→ RX2 (GPIO16)
 * └─ RX2 (GPIO16) ←────── TX2 (GPIO17)
 * ├─ GND ─────────────── GND
 * 
 * MCU A Role: Sensor data collector + actuator controller
 * - Continuously sends sensor readings to MCU B
 * - Receives control commands from MCU B
 * - Executes commands (LED, fan, etc.)
 */

#include "ceria-serial.h"

using namespace CeriaDevOP;

CeriaSerial link;

float temperature = 25.0;
float humidity = 60.0;
int lightLevel = 512;
bool ledState = false;
int fanSpeed = 0;

unsigned long lastSensorSend = 0;
const unsigned long SENSOR_INTERVAL = 1000;

void setup() {
    Serial.begin(115200);
    Serial.println("=== MCU A - Sensor Controller ===");
    
    if (link.begin(Serial2, 115200, FRAME_256B)) {
        Serial.println("✅ Full-duplex link established with MCU B");
    } else {
        Serial.println("❌ Failed to establish link");
        return;
    }
    
    link.setMode(FULL_DUPLEX);
    link.setRetries(3);
    link.setTimeout(2000);
    
    link.onReceive([](const String& key, const String& value) {
        Serial.println("📨 Command from MCU B: " + key + " = " + value);
        
        if (key == "led") {
            ledState = (value == "on" || value == "true" || value == "1");
            digitalWrite(2, ledState ? HIGH : LOW);
            Serial.println("💡 LED: " + String(ledState ? "ON" : "OFF"));
            
            link.send("led_status", ledState ? "on" : "off");
        }
        else if (key == "fan_speed") {
            fanSpeed = value.toInt();
            fanSpeed = constrain(fanSpeed, 0, 100);
            analogWrite(5, map(fanSpeed, 0, 100, 0, 255));
            Serial.println("🌀 Fan speed: " + String(fanSpeed) + "%");
            
            link.send("fan_status", fanSpeed);
        }
        else if (key == "request_sensors") {
            sendAllSensorData();
        }
        else if (key == "ping") {
            link.send("pong", "mcu_a_online");
        }
    });
    
    link.onError([](CeriaSerialError error) {
        Serial.println("⚠️ Communication error: " + String(error));
    });
    
    pinMode(2, OUTPUT);
    pinMode(5, OUTPUT);
    
    randomSeed(analogRead(A0));
    
    Serial.println("🎯 MCU A ready - sending sensor data to MCU B");
}

void loop() {
    simulateSensors();
    
    if (millis() - lastSensorSend > SENSOR_INTERVAL) {
        sendSensorData();
        lastSensorSend = millis();
    }
    
    link.tick();
    
    displayStatus();
    
    delay(100);
}

void simulateSensors() {
    temperature += random(-20, 21) / 100.0;
    temperature = constrain(temperature, 20.0, 35.0);
    
    humidity += random(-30, 31) / 100.0;
    humidity = constrain(humidity, 40.0, 80.0);
    
    lightLevel += random(-20, 21);
    lightLevel = constrain(lightLevel, 0, 1023);
}

void sendSensorData() {
    link.send("temperature", temperature);
    link.send("humidity", humidity);
    link.send("light", lightLevel);
    link.send("uptime", millis() / 1000);
    
    Serial.println("📊 Sensor data sent to MCU B");
}

void sendAllSensorData() {
    sendSensorData();
    link.send("led_status", ledState ? "on" : "off");
    link.send("fan_status", fanSpeed);
    Serial.println("📦 All status sent to MCU B");
}

void displayStatus() {
    static unsigned long lastDisplay = 0;
    
    if (millis() - lastDisplay > 5000) {
        Serial.println("\n🏭 === MCU A STATUS ===");
        Serial.println("🌡️  Temperature: " + String(temperature, 1) + "°C");
        Serial.println("💧 Humidity: " + String(humidity, 1) + "%");
        Serial.println("💡 Light: " + String(lightLevel));
        Serial.println("🔗 LED: " + String(ledState ? "ON" : "OFF"));
        Serial.println("🌀 Fan: " + String(fanSpeed) + "%");
        
        if (link.isConnected()) {
            Serial.println("📡 MCU B connection: ACTIVE");
        } else {
            Serial.println("📡 MCU B connection: LOST");
        }
        
        uint32_t sent, received, errors;
        link.getStats(sent, received, errors);
        Serial.println("📊 Stats - Sent:" + String(sent) + " Recv:" + String(received) + " Err:" + String(errors));
        Serial.println("========================\n");
        
        lastDisplay = millis();
    }
}
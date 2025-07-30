/*
 * Half-Duplex Communication - Master Device
 * 
 * Hardware Setup (RS485 style or single-wire):
 * Master (ESP32):          Slave (Arduino):
 * ├─ DATA (GPIO16) ←──────→ DATA (Pin 8)
 * ├─ GND ──────────────── GND
 * 
 * Master Role: Request coordinator
 * - Initiates all communication (no simultaneous TX)
 * - Requests data from slave devices
 * - Handles collision detection and retry
 * - Maintains timing to avoid bus conflicts
 */

#include "ceria-serial.h"

using namespace CeriaDevOP;

CeriaSerial link;

struct SlaveData {
    float temperature = 0.0;
    float pressure = 0.0;
    int batteryLevel = 0;
    String status = "unknown";
    unsigned long lastUpdate = 0;
    bool responseReceived = false;
} slave;

unsigned long lastRequest = 0;
const unsigned long REQUEST_INTERVAL = 3000;
const unsigned long RESPONSE_TIMEOUT = 1000;

enum RequestState {
    REQ_IDLE,
    REQ_TEMPERATURE,
    REQ_PRESSURE,
    REQ_BATTERY,
    REQ_STATUS,
    REQ_WAITING
} currentRequest = REQ_IDLE;

unsigned long requestStartTime = 0;

void setup() {
    Serial.begin(115200);
    Serial.println("=== Half-Duplex Master ===");
    
    if (link.begin(Serial1, 9600, FRAME_128B)) {
        Serial.println("✅ Half-duplex master initialized");
    } else {
        Serial.println("❌ Failed to initialize master");
        return;
    }
    
    link.setMode(HALF_DUPLEX);
    link.setRetries(3);
    link.setTimeout(1000);
    
    link.onReceive([](const String& key, const String& value) {
        Serial.println("📨 Slave response: " + key + " = " + value);
        
        if (key == "temperature") {
            slave.temperature = value.toFloat();
            slave.responseReceived = true;
        }
        else if (key == "pressure") {
            slave.pressure = value.toFloat();
            slave.responseReceived = true;
        }
        else if (key == "battery") {
            slave.batteryLevel = value.toInt();
            slave.responseReceived = true;
        }
        else if (key == "status") {
            slave.status = value;
            slave.responseReceived = true;
        }
        else if (key == "ack") {
            Serial.println("✅ Slave acknowledged: " + value);
            slave.responseReceived = true;
        }
        else if (key == "error") {
            Serial.println("❌ Slave reported error: " + value);
            slave.responseReceived = true;
        }
        
        if (slave.responseReceived) {
            slave.lastUpdate = millis();
            currentRequest = REQ_IDLE;
        }
    });
    
    link.onError([](CeriaSerialError error) {
        Serial.println("⚠️ Communication error: " + String(error));
        
        if (error == CERIA_SERIAL_ERR_COLLISION) {
            Serial.println("🔄 Bus collision detected - retrying with backoff");
        }
        else if (error == CERIA_SERIAL_ERR_TIMEOUT) {
            Serial.println("⏰ Response timeout - slave may be busy");
            currentRequest = REQ_IDLE;
        }
    });
    
    Serial.println("🎯 Master ready - will request data from slave every " + String(REQUEST_INTERVAL/1000) + "s");
    Serial.println("💬 Type commands: 'temp', 'pressure', 'battery', 'status', 'reset', 'ping'");
}

void loop() {
    link.tick();
    
    handleRequestSequence();
    processUserCommands();
    displayStatus();
    
    delay(50);
}

void handleRequestSequence() {
    unsigned long now = millis();
    
    switch (currentRequest) {
        case REQ_IDLE:
            if (now - lastRequest > REQUEST_INTERVAL) {
                startRequestSequence();
            }
            break;
            
        case REQ_TEMPERATURE:
            if (sendRequest("request", "temperature")) {
                currentRequest = REQ_WAITING;
                requestStartTime = now;
            }
            break;
            
        case REQ_PRESSURE:
            if (sendRequest("request", "pressure")) {
                currentRequest = REQ_WAITING;
                requestStartTime = now;
            }
            break;
            
        case REQ_BATTERY:
            if (sendRequest("request", "battery")) {
                currentRequest = REQ_WAITING;
                requestStartTime = now;
            }
            break;
            
        case REQ_STATUS:
            if (sendRequest("request", "status")) {
                currentRequest = REQ_WAITING;
                requestStartTime = now;
            }
            break;
            
        case REQ_WAITING:
            if (slave.responseReceived) {
                slave.responseReceived = false;
                proceedToNextRequest();
            } else if (now - requestStartTime > RESPONSE_TIMEOUT) {
                Serial.println("⏰ Request timeout - proceeding to next");
                proceedToNextRequest();
            }
            break;
    }
}

void startRequestSequence() {
    Serial.println("🔄 Starting request sequence...");
    currentRequest = REQ_TEMPERATURE;
    lastRequest = millis();
}

void proceedToNextRequest() {
    delay(100);
    
    switch (currentRequest) {
        case REQ_WAITING:
            if (slave.responseReceived) {
                switch (lastRequestType()) {
                    case REQ_TEMPERATURE:
                        currentRequest = REQ_PRESSURE;
                        break;
                    case REQ_PRESSURE:
                        currentRequest = REQ_BATTERY;
                        break;
                    case REQ_BATTERY:
                        currentRequest = REQ_STATUS;
                        break;
                    case REQ_STATUS:
                    default:
                        currentRequest = REQ_IDLE;
                        Serial.println("✅ Request sequence completed");
                        break;
                }
            } else {
                currentRequest = REQ_IDLE;
            }
            break;
        default:
            currentRequest = REQ_IDLE;
            break;
    }
}

RequestState lastRequestType() {
    return REQ_TEMPERATURE;
}

bool sendRequest(const String& key, const String& value) {
    Serial.println("📤 Requesting: " + value);
    return link.send(key, value);
}

void processUserCommands() {
    if (Serial.available()) {
        String command = Serial.readStringUntil('\n');
        command.trim();
        
        if (currentRequest != REQ_IDLE) {
            Serial.println("⏳ Busy with request sequence - please wait");
            return;
        }
        
        if (command == "temp") {
            if (link.send("request", "temperature")) {
                Serial.println("👤 Manual temperature request sent");
            }
        }
        else if (command == "pressure") {
            if (link.send("request", "pressure")) {
                Serial.println("👤 Manual pressure request sent");
            }
        }
        else if (command == "battery") {
            if (link.send("request", "battery")) {
                Serial.println("👤 Manual battery request sent");
            }
        }
        else if (command == "status") {
            if (link.send("request", "status")) {
                Serial.println("👤 Manual status request sent");
            }
        }
        else if (command == "reset") {
            if (link.send("command", "reset")) {
                Serial.println("👤 Reset command sent to slave");
            }
        }
        else if (command == "ping") {
            if (link.send("ping", "master_ping")) {
                Serial.println("👤 Ping sent to slave");
            }
        }
        else if (command == "help") {
            Serial.println("📖 Available commands:");
            Serial.println("  temp     - Request temperature");
            Serial.println("  pressure - Request pressure");
            Serial.println("  battery  - Request battery level");
            Serial.println("  status   - Request status");
            Serial.println("  reset    - Send reset command");
            Serial.println("  ping     - Test connection");
            Serial.println("  help     - Show this help");
        }
        else if (command.length() > 0) {
            Serial.println("❓ Unknown command: " + command + " (type 'help')");
        }
    }
}

void displayStatus() {
    static unsigned long lastDisplay = 0;
    
    if (millis() - lastDisplay > 10000) {
        Serial.println("\n📊 === MASTER STATUS ===");
        
        unsigned long dataAge = slave.lastUpdate > 0 ? (millis() - slave.lastUpdate) / 1000 : 999;
        Serial.println("📈 Slave Data (age: " + String(dataAge) + "s):");
        Serial.println("  🌡️  Temperature: " + String(slave.temperature, 1) + "°C");
        Serial.println("  🔺 Pressure: " + String(slave.pressure, 1) + " bar");
        Serial.println("  🔋 Battery: " + String(slave.batteryLevel) + "%");
        Serial.println("  📟 Status: " + slave.status);
        
        Serial.println("🔄 Request State: " + getRequestStateName(currentRequest));
        
        if (link.isConnected()) {
            Serial.println("🔗 Slave Connection: ACTIVE");
        } else {
            Serial.println("🔗 Slave Connection: LOST");
        }
        
        uint32_t sent, received, errors;
        link.getStats(sent, received, errors);
        Serial.println("📊 Bus Stats - Sent:" + String(sent) + " Recv:" + String(received) + " Err:" + String(errors));
        Serial.println("========================\n");
        
        lastDisplay = millis();
    }
}

String getRequestStateName(RequestState state) {
    switch (state) {
        case REQ_IDLE: return "IDLE";
        case REQ_TEMPERATURE: return "REQUESTING_TEMP";
        case REQ_PRESSURE: return "REQUESTING_PRESSURE";
        case REQ_BATTERY: return "REQUESTING_BATTERY";
        case REQ_STATUS: return "REQUESTING_STATUS";
        case REQ_WAITING: return "WAITING_RESPONSE";
        default: return "UNKNOWN";
    }
}
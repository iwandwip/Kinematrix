#include "../CeriaDevOP.h"

using namespace CeriaDevOP;

CeriaSerialI2C i2cMaster;

const uint8_t SLAVE_NODE_1 = 0x08;
const uint8_t SLAVE_NODE_2 = 0x09;

void setup() {
    Serial.begin(115200);
    Serial.println("=== CeriaSerialI2C Master Example ===");
    
    if (!i2cMaster.begin(I2C_MASTER)) {
        Serial.println("Failed to initialize I2C Master");
        while(1) delay(1000);
    }
    
    i2cMaster.setTimeout(2000);
    i2cMaster.setRetries(3);
    
    i2cMaster.onError([](CeriaI2CError error) {
        Serial.println("I2C Error: " + String(error));
    });
    
    Serial.println("I2C Master initialized successfully");
    Serial.println("Scanning for slave devices...");
    
    for (uint8_t addr = 0x08; addr <= 0x77; addr++) {
        if (i2cMaster.isConnected(addr)) {
            Serial.println("Slave found at address: 0x" + String(addr, HEX));
        }
    }
}

void loop() {
    i2cMaster.update();
    
    static unsigned long lastSend = 0;
    if (millis() - lastSend > 2000) {
        
        Serial.println("\n=== Sending Commands to Slaves ===");
        
        if (i2cMaster.send(SLAVE_NODE_1, 255)) {
            Serial.println("✓ Sent LED command to Node 1");
        } else {
            Serial.println("✗ Failed to send to Node 1");
        }
        
        float temperature = 25.6;
        if (i2cMaster.send(SLAVE_NODE_2, temperature)) {
            Serial.println("✓ Sent temperature to Node 2");
        } else {
            Serial.println("✗ Failed to send to Node 2");
        }
        
        String message = "Hello Slave";
        if (i2cMaster.send(SLAVE_NODE_1, message)) {
            Serial.println("✓ Sent message to Node 1");
        } else {
            Serial.println("✗ Failed to send message to Node 1");
        }
        
        lastSend = millis();
    }
    
    static unsigned long lastRequest = 0;
    if (millis() - lastRequest > 3000) {
        
        Serial.println("\n=== Requesting Data from Slaves ===");
        
        uint16_t sensorValue;
        if (i2cMaster.request(SLAVE_NODE_1, sensorValue)) {
            Serial.println("Sensor value from Node 1: " + String(sensorValue));
        } else {
            Serial.println("Failed to get sensor value from Node 1");
        }
        
        String status;
        if (i2cMaster.request(SLAVE_NODE_2, status)) {
            Serial.println("Status from Node 2: " + status);
        } else {
            Serial.println("Failed to get status from Node 2");
        }
        
        float voltage;
        if (i2cMaster.request(SLAVE_NODE_1, voltage)) {
            Serial.println("Voltage from Node 1: " + String(voltage, 2) + "V");
        } else {
            Serial.println("Failed to get voltage from Node 1");
        }
        
        lastRequest = millis();
    }
    
    static unsigned long lastStats = 0;
    if (millis() - lastStats > 10000) {
        
        Serial.println("\n=== Communication Statistics ===");
        uint32_t sent, received, errors;
        i2cMaster.getStats(sent, received, errors);
        
        Serial.println("Frames sent: " + String(sent));
        Serial.println("Frames received: " + String(received));
        Serial.println("Total errors: " + String(errors));
        
        if (sent > 0) {
            float successRate = (float)(sent - errors) / sent * 100;
            Serial.println("Success rate: " + String(successRate, 1) + "%");
        }
        
        lastStats = millis();
    }
    
    delay(100);
}
#include "CeriaDevOP.h"

CeriaSerialI2C i2c;

void setup() {
    Serial.begin(115200);
    delay(2000);
    
    Serial.println("=== CeriaSerialI2C Enhanced Test ===");
    
    i2c.begin(I2C_MASTER, 0, 5000);
    
    Serial.println("Master initialized");
}

void loop() {
    testMasterFunctions();
    delay(2000);
}

void testMasterFunctions() {
    uint8_t slaveAddr = 0x08;
    
    if (i2c.isConnected(slaveAddr)) {
        Serial.println("Slave connected at 0x" + String(slaveAddr, HEX));
        
        float tempValue = 25.6;
        if (i2c.send(slaveAddr, tempValue)) {
            Serial.println("✓ Sent temperature: " + String(tempValue));
        } else {
            Serial.println("✗ Failed to send temperature");
        }
        
        uint16_t sensorValue = 1023;
        if (i2c.send(slaveAddr, sensorValue)) {
            Serial.println("✓ Sent sensor value: " + String(sensorValue));
        } else {
            Serial.println("✗ Failed to send sensor value");
        }
        
        String message = "Hello I2C!";
        if (i2c.send(slaveAddr, message)) {
            Serial.println("✓ Sent message: " + message);
        } else {
            Serial.println("✗ Failed to send message");
        }
        
        float receivedTemp;
        if (i2c.request(slaveAddr, receivedTemp)) {
            Serial.println("✓ Received temperature: " + String(receivedTemp));
        } else {
            Serial.println("✗ Failed to receive temperature");
        }
        
        uint32_t sent, received, errors;
        i2c.getStats(sent, received, errors);
        Serial.println("Stats - Sent: " + String(sent) + ", Received: " + String(received) + ", Errors: " + String(errors));
        
    } else {
        Serial.println("No slave found at 0x" + String(slaveAddr, HEX));
    }
}
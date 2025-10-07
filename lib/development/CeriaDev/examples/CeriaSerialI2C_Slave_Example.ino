#include "../CeriaDevOP.h"

using namespace CeriaDevOP;

CeriaSerialI2C i2cSlave;

const uint8_t MY_SLAVE_ADDRESS = 0x08;
const uint8_t LED_PIN = 2;
const uint8_t SENSOR_PIN = A0;

uint16_t sensorValue = 0;
float batteryVoltage = 3.7;
String deviceStatus = "online";
bool ledState = false;
uint8_t responseMode = 0;

void setup() {
    Serial.begin(115200);
    delay(2000);
    
    Serial.println("=== CeriaSerialI2C Enhanced Slave Example ===");
    Serial.println("Slave Address: 0x" + String(MY_SLAVE_ADDRESS, HEX));
    
    pinMode(LED_PIN, OUTPUT);
    pinMode(SENSOR_PIN, INPUT);
    
    if (!i2cSlave.begin(I2C_SLAVE, MY_SLAVE_ADDRESS, 5000)) {
        Serial.println("Failed to initialize I2C Slave");
        while(1) delay(1000);
    }
    
    i2cSlave.setResponseFloat(batteryVoltage);
    
    Serial.println("I2C Slave initialized successfully");
    Serial.println("Ready to receive commands...");
}

void loop() {
    i2cSlave.update();
    
    if (i2cSlave.hasNewData()) {
        Serial.println("✓ New data received from master!");
        
        I2CDataType dataType = i2cSlave.getReceivedType();
        Serial.println("Data type: " + String(dataType));
        
        switch (dataType) {
            case I2C_TYPE_UINT16: {
                uint16_t receivedValue;
                if (i2cSlave.getReceivedData(receivedValue)) {
                    Serial.println("Received uint16: " + String(receivedValue));
                    sensorValue = receivedValue;
                    
                    if (receivedValue == 255) {
                        ledState = !ledState;
                        digitalWrite(LED_PIN, ledState);
                        Serial.println("LED toggled: " + String(ledState ? "ON" : "OFF"));
                    }
                    
                    i2cSlave.setResponseUint16(sensorValue + 100);
                }
                break;
            }
            
            case I2C_TYPE_FLOAT: {
                float receivedValue;
                if (i2cSlave.getReceivedData(receivedValue)) {
                    Serial.println("Received float: " + String(receivedValue, 2));
                    batteryVoltage = receivedValue;
                    i2cSlave.setResponseFloat(batteryVoltage + 0.5);
                }
                break;
            }
            
            case I2C_TYPE_STRING: {
                String receivedValue;
                if (i2cSlave.getReceivedData(receivedValue)) {
                    Serial.println("Received string: '" + receivedValue + "'");
                    deviceStatus = receivedValue;
                    i2cSlave.setResponseString("ACK:" + receivedValue);
                }
                break;
            }
        }
        
        responseMode = (responseMode + 1) % 3;
        switch (responseMode) {
            case 0:
                i2cSlave.setResponseUint16(sensorValue);
                break;
            case 1:
                i2cSlave.setResponseFloat(batteryVoltage);
                break;
            case 2:
                i2cSlave.setResponseString(deviceStatus);
                break;
        }
    }
    
    static unsigned long lastSensorRead = 0;
    if (millis() - lastSensorRead > 1000) {
        sensorValue = analogRead(SENSOR_PIN);
        batteryVoltage = 3.3 + (random(-100, 100) / 1000.0);
        
        if (sensorValue > 2000) {
            deviceStatus = "high";
        } else if (sensorValue > 1000) {
            deviceStatus = "medium";
        } else {
            deviceStatus = "low";
        }
        
        lastSensorRead = millis();
    }
    
    static unsigned long lastHeartbeat = 0;
    if (millis() - lastHeartbeat > 5000) {
        Serial.println("Heartbeat - Sensor: " + String(sensorValue) + 
                      ", Voltage: " + String(batteryVoltage, 2) + "V" +
                      ", Status: " + deviceStatus);
        
        uint32_t sent, received, errors;
        i2cSlave.getStats(sent, received, errors);
        Serial.println("Stats - Sent: " + String(sent) + 
                      ", Received: " + String(received) + 
                      ", Errors: " + String(errors));
        
        lastHeartbeat = millis();
    }
    
    delay(50);
}
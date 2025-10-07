#include "CeriaDevOP.h"

using namespace CeriaDevOP;

CeriaSerialI2C i2cTest;

void setup() {
    Serial.begin(115200);
    delay(2000);
    
    Serial.println("=== Testing CeriaSerialI2C Enhanced Compilation ===");
    
    if (i2cTest.begin(I2C_MASTER, 0, 5000)) {
        Serial.println("✓ Master initialization OK");
    }
    
    if (i2cTest.begin(I2C_SLAVE, 0x08, 5000)) {
        Serial.println("✓ Slave initialization OK");
    }
    
    i2cTest.setTimeout(1000);
    
    uint16_t testValue16 = 1234;
    float testFloat = 3.14159;
    String testString = "Hello Test";
    
    bool result = true;
    
    result &= i2cTest.send(0x08, testValue16);
    Serial.println(result ? "✓ send(uint16) OK" : "✗ send(uint16) Failed");
    
    result &= i2cTest.send(0x08, testFloat);
    Serial.println(result ? "✓ send(float) OK" : "✗ send(float) Failed");
    
    result &= i2cTest.send(0x08, testString);
    Serial.println(result ? "✓ send(String) OK" : "✗ send(String) Failed");
    
    uint16_t readValue16;
    float readFloat;
    String readString;
    
    result &= i2cTest.request(0x08, readValue16);
    Serial.println(result ? "✓ request(uint16) OK" : "✗ request(uint16) Failed");
    
    result &= i2cTest.request(0x08, readFloat);
    Serial.println(result ? "✓ request(float) OK" : "✗ request(float) Failed");
    
    result &= i2cTest.request(0x08, readString);
    Serial.println(result ? "✓ request(String) OK" : "✗ request(String) Failed");
    
    i2cTest.setResponseUint16(42);
    Serial.println("✓ setResponseUint16 OK");
    
    i2cTest.setResponseFloat(3.14f);
    Serial.println("✓ setResponseFloat OK");
    
    i2cTest.setResponseString("Response");
    Serial.println("✓ setResponseString OK");
    
    bool connected = i2cTest.isConnected(0x08);
    Serial.println("✓ isConnected() compiled OK");
    
    bool hasData = i2cTest.hasNewData();
    Serial.println("✓ hasNewData() compiled OK");
    
    I2CDataType dataType = i2cTest.getReceivedType();
    Serial.println("✓ getReceivedType() compiled OK");
    
    I2CError lastError = i2cTest.getLastError();
    Serial.println("✓ getLastError() compiled OK");
    
    uint32_t sent, received, errors;
    i2cTest.getStats(sent, received, errors);
    Serial.println("✓ getStats() compiled OK");
    
    i2cTest.reset();
    Serial.println("✓ reset() compiled OK");
    
    Serial.println("✓ Enhanced frame protocol compiled successfully");
    Serial.println("✓ Error handling enums compiled successfully");
    Serial.println("✓ Data type enums compiled successfully");
    Serial.println("✓ CeriaSerialI2C Enhanced compilation test PASSED");
}

void loop() {
    i2cTest.update();
    delay(100);
}
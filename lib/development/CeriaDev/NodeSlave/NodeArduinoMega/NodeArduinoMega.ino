#include "Header.h"

void setup() {
  Serial.begin(115200);
  usbSerial.begin(&Serial);
  gpsSensor.begin(Serial3, 9600);
  gpsSensor.setTimezone(7);
  
  if (!i2cSlave.begin(I2C_SLAVE, MY_SLAVE_ADDRESS)) {
    Serial.println("❌ CeriaSerialI2C Slave initialization failed");
  } else {
    Serial.println("✅ I2C Slave initialized at address 0x" + String(MY_SLAVE_ADDRESS, HEX));
    
    // Set static responses (Simple API)
    i2cSlave.setResponseUint16(5678);
    i2cSlave.setResponseFloat(99.42);
    i2cSlave.setResponseString("World");
    
    Serial.println("Ready with simple cross-platform responses...");
  }
  
  Serial.println("✅ NodeArduinoMega I2C Slave ready");
}

void loop() {
  i2cSlave.update();
  
  // Debug: Always print status - less frequent now
  static unsigned long lastDebug = 0;
  if (millis() - lastDebug > 3000) {
    Serial.println("🔍 Slave alive...");
    
    // Check stats
    uint32_t sent, received, errors;
    i2cSlave.getStats(sent, received, errors);
    Serial.println("Stats - Sent:" + String(sent) + " Received:" + String(received) + " Errors:" + String(errors));
    
    lastDebug = millis();
  }
  
  // Check for received data
  if (i2cSlave.hasNewData()) {
    Serial.println("📥 NEW DATA DETECTED!");
    I2CDataType dataType = i2cSlave.getReceivedType();
    Serial.println("Data type: " + String(dataType));
    
    if (dataType == I2C_TYPE_UINT16) {
      uint16_t intData;
      if (i2cSlave.getReceivedData(intData)) {
        Serial.println("✅ Received int: " + String(intData));
      } else {
        Serial.println("❌ Failed to get int data");
      }
    }
    else if (dataType == I2C_TYPE_FLOAT) {
      float floatData;
      if (i2cSlave.getReceivedData(floatData)) {
        Serial.println("✅ Received float: " + String(floatData));
      } else {
        Serial.println("❌ Failed to get float data");
      }
    }
    else if (dataType == I2C_TYPE_STRING) {
      String stringData;
      if (i2cSlave.getReceivedData(stringData)) {
        Serial.println("✅ Received string: " + stringData);
      } else {
        Serial.println("❌ Failed to get string data");
      }
    }
  }

  gpsSensor.update();
  if (debugEnable) usbDebugSerial();
  usbSerial.receiveString(usbOnReceiveCallback);
  delay(50);
}
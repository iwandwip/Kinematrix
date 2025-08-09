#include "Header.h"

void setup() {
  Serial.begin(115200);
  usbSerial.begin(&Serial);

  if (!sdCard.beginSPI()) {
    Serial.println("❌ CeriaModuleESP32SDCard initialization failed");
  }

  for (uint8_t i = 0; i < 8; i++) {
    buttonPCF.setupButton(i, 20, IO_INPUT);
  }
  if (!buttonPCF.begin()) {
    Serial.println("❌ CeriaModuleIO initialization failed");
  }

  if (!rtcSensor.begin()) {
    Serial.println("❌ CeriaSensorDS3231 not found!");
  } else {
    if (rtcSensor.isPowerLost()) {
      Serial.println("RTC lost power, setting compile time...");
      rtcSensor.setToCompileTime();
    }
  }

  if (!powerSensor.begin(&Wire, INA219_CAL_32V_2A)) {
    Serial.println("❌ CeriaSensorINA219 power initialization failed");
  } else {
    powerSensor.setDefaultUnits(INA219_UNIT_BASE, INA219_UNIT_BASE, INA219_UNIT_BASE);
    powerSensor.setAllowNegativeReadings(false);
  }

  if (!batterySensor.begin(&Wire, INA219_CAL_32V_2A)) {
    Serial.println("❌ CeriaSensorINA219 baterry initialization failed");
  } else {
    batterySensor.setDefaultUnits(INA219_UNIT_BASE, INA219_UNIT_BASE, INA219_UNIT_BASE);
    batterySensor.setAllowNegativeReadings(false);
  }

  if (!i2cMaster.begin(I2C_MASTER)) {
    Serial.println("❌ CeriaSerialI2C Master initialization failed");
  } else {
    Serial.println("✅ I2C Master initialized (Simple & Cross-Platform)");
    
    delay(1000);
    if (i2cMaster.isConnected(ARDUINO_MEGA_SLAVE_ADDR)) {
      Serial.println("✅ Arduino Mega slave detected at 0x" + String(ARDUINO_MEGA_SLAVE_ADDR, HEX));
    } else {
      Serial.println("⚠️ Arduino Mega slave not detected - will retry in loop");
    }
  }

  Serial.println("✅ NodeESP32 I2C Master init success");
}

void loop() {
  i2cMaster.update();
  
  buttonPCF.updateAll();
  powerSensor.update();
  batterySensor.update();
  rtcSensor.update();
  sdCard.update();

  if (powerSensor.hasNewData() && powerSensor.isConnected()) {
    power.voltage = powerSensor.getVoltage();
    power.current = powerSensor.getCurrent();
    power.power = power.voltage * power.current;
  }

  if (batterySensor.hasNewData() && batterySensor.isConnected()) {
    battery.voltage = batterySensor.getVoltage();
    battery.current = batterySensor.getCurrent();
    battery.power = battery.voltage * battery.current;
  }

  static unsigned long lastI2CTest = 0;
  if (millis() - lastI2CTest > 5000) {
    testI2CEnhancedCommunication();
    lastI2CTest = millis();
  }

  if (debugEnable) usbDebugSerial();
  
  usbSerial.receiveString(usbOnReceiveCallback);
  delay(50);
}

void testI2CEnhancedCommunication() {
  if (!i2cMaster.isConnected(ARDUINO_MEGA_SLAVE_ADDR)) {
    Serial.println("❌ Arduino Mega not connected");
    return;
  }
  
  Serial.println("=== Simple I2C Test ===");
  
  // Test 1: Send int with better error handling
  uint16_t intData = 1234;
  if (i2cMaster.send(ARDUINO_MEGA_SLAVE_ADDR, intData)) {
    Serial.println("✅ Sent int: " + String(intData));
  } else {
    Serial.println("❌ Failed to send int");
  }
  delay(300); // Longer delay for Arduino Mega
  
  // Test 2: Send float with better error handling
  float floatData = 25.67;
  if (i2cMaster.send(ARDUINO_MEGA_SLAVE_ADDR, floatData)) {
    Serial.println("✅ Sent float: " + String(floatData));
  } else {
    Serial.println("❌ Failed to send float");
  }
  delay(300); // Longer delay for Arduino Mega
  
  // Test 3: Send string with better error handling
  String stringData = "Hello";
  if (i2cMaster.send(ARDUINO_MEGA_SLAVE_ADDR, stringData)) {
    Serial.println("✅ Sent string: " + stringData);
  } else {
    Serial.println("❌ Failed to send string");
  }
  delay(500); // Even longer delay before requesting
  
  // Request responses with error handling
  uint16_t respInt;
  if (i2cMaster.request(ARDUINO_MEGA_SLAVE_ADDR, respInt)) {
    Serial.println("✅ Got int: " + String(respInt));
  } else {
    Serial.println("❌ Failed to get int response");
  }
  delay(200);
  
  float respFloat;
  if (i2cMaster.request(ARDUINO_MEGA_SLAVE_ADDR, respFloat)) {
    Serial.println("✅ Got float: " + String(respFloat));
  } else {
    Serial.println("❌ Failed to get float response");
  }
  delay(200);
  
  String respString;
  if (i2cMaster.request(ARDUINO_MEGA_SLAVE_ADDR, respString)) {
    Serial.println("✅ Got string: " + respString);
  } else {
    Serial.println("❌ Failed to get string response");
  }
  
  // Show stats
  uint32_t sent, received, errors;
  i2cMaster.getStats(sent, received, errors);
  if (errors > 0) {
    Serial.println("⚠️ Errors: " + String(errors) + "/" + String(sent + received));
  }
  
  Serial.println("========================");
}
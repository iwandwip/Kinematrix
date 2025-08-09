#include <Wire.h>

const uint8_t SLAVE_ADDRESS = 0x08;

void setup() {
  Serial.begin(115200);
  delay(2000);
  
  Serial.println("=== Arduino Mega I2C Slave Simple Test ===");
  
  Wire.begin(SLAVE_ADDRESS);
  Wire.onReceive(receiveEvent);
  Wire.onRequest(requestEvent);
  
  Serial.println("✅ Arduino Mega I2C Slave ready at 0x" + String(SLAVE_ADDRESS, HEX));
}

void receiveEvent(int bytes) {
  Serial.println("📥 Received " + String(bytes) + " bytes:");
  Serial.print("Data: ");
  
  while (Wire.available()) {
    uint8_t data = Wire.read();
    Serial.print("0x" + String(data, HEX) + " ");
  }
  Serial.println();
}

void requestEvent() {
  uint16_t response = 5678;
  Wire.write((uint8_t*)&response, 2);
  Serial.println("📤 Sent response: " + String(response));
}

void loop() {
  static unsigned long lastHeartbeat = 0;
  if (millis() - lastHeartbeat > 5000) {
    Serial.println("💓 Arduino Mega I2C slave alive");
    lastHeartbeat = millis();
  }
  
  delay(100);
}
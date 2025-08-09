#include <Wire.h>

const uint8_t SLAVE_ADDRESS = 0x08;

void setup() {
  Serial.begin(115200);
  delay(2000);
  
  Serial.println("=== ESP32 I2C Master Simple Test ===");
  
  Wire.begin();
  Wire.setClock(100000); // 100kHz
  
  Serial.println("✅ ESP32 I2C Master ready");
}

void loop() {
  Serial.println("=== Testing I2C Communication ===");
  
  // Test 1: Send int (2 bytes)
  uint16_t intData = 1234;
  Wire.beginTransmission(SLAVE_ADDRESS);
  Wire.write((uint8_t*)&intData, 2);
  uint8_t result1 = Wire.endTransmission();
  Serial.println("Send int 1234, result: " + String(result1));
  delay(500);
  
  // Test 2: Send float (4 bytes) 
  float floatData = 25.67;
  Wire.beginTransmission(SLAVE_ADDRESS);
  Wire.write((uint8_t*)&floatData, 4);
  uint8_t result2 = Wire.endTransmission();
  Serial.println("Send float 25.67, result: " + String(result2));
  delay(500);
  
  // Test 3: Send string
  String stringData = "Hello";
  Wire.beginTransmission(SLAVE_ADDRESS);
  Wire.print(stringData);
  uint8_t result3 = Wire.endTransmission();
  Serial.println("Send string Hello, result: " + String(result3));
  delay(500);
  
  // Test 4: Request data
  uint8_t received = Wire.requestFrom(SLAVE_ADDRESS, (uint8_t)2);
  if (received == 2) {
    uint16_t response = 0;
    response = Wire.read();
    response |= (Wire.read() << 8);
    Serial.println("✅ Got response: " + String(response));
  } else {
    Serial.println("❌ Failed to get response, got " + String(received) + " bytes");
  }
  
  Serial.println("========================");
  delay(3000);
}
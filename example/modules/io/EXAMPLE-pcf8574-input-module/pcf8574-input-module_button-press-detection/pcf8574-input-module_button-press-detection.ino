/**
 * PCF8574 Digital Input Press Detection Example
 * 
 * Contoh untuk mendeteksi saat tombol ditekan dan dilepas (edge detection)
 * 
 * Koneksi Hardware:
 * - PCF8574 terhubung ke Arduino via I2C (SDA, SCL)
 * - Push button terhubung ke pin P0 pada PCF8574
 * - LED terhubung ke pin 13 Arduino (LED bawaan)
 */

#include "pcf8574-module.h"
#include "pcf8574-input-module.h"

#define PCF_ADDRESS 0x20
#define BTN_PIN PCF_PIN0
#define LED_PIN 13

PCF8574Module pcfModule(PCF_ADDRESS);
PCF8574DigitalIn button(BTN_PIN);

void setup() {
  Serial.begin(115200);
  Serial.println("PCF8574 Digital Input - Press Detection Example");
  
  // Setup LED untuk output
  pinMode(LED_PIN, OUTPUT);
  digitalWrite(LED_PIN, LOW);
  
  // Inisialisasi PCF8574
  if (pcfModule.begin()) {
    Serial.println("PCF8574 berhasil diinisialisasi");
  } else {
    Serial.println("PCF8574 gagal diinisialisasi. Cek koneksi!");
    while(1);
  }
  
  // Inisialisasi button
  button.init(&pcfModule);
}

void loop() {
  // Update status button
  button.update();
  
  // Cek apakah button baru saja ditekan (falling edge)
  if (button.isPressed()) {
    Serial.println("Tombol ditekan!");
    digitalWrite(LED_PIN, HIGH);
  }
  
  // Cek apakah button baru saja dilepas (rising edge)
  if (button.isReleased()) {
    Serial.println("Tombol dilepas!");
    digitalWrite(LED_PIN, LOW);
  }
  
  delay(10); // Delay kecil untuk stabilitas
}

/**
 * PCF8574 Digital Input untuk ESP32 dengan Custom I2C Pins
 * 
 * Contoh penggunaan PCF8574 dengan ESP32 dan pin I2C kustom
 * 
 * Koneksi Hardware:
 * - PCF8574 terhubung ke ESP32 via I2C dengan pin kustom:
 *   - SDA: GPIO21
 *   - SCL: GPIO22
 * - Push button terhubung ke pin P0 pada PCF8574
 */

#include "pcf8574-module.h"
#include "pcf8574-input-module.h"
#include <Wire.h>

#define PCF_ADDRESS 0x20
#define BTN_PIN PCF_PIN0

// Pin I2C kustom untuk ESP32
#define SDA_PIN 21
#define SCL_PIN 22

// Buat objek Wire tambahan untuk ESP32
TwoWire customI2C = TwoWire(0);

// Gunakan konstruktor khusus ESP32 dengan pin SDA dan SCL kustom
PCF8574Module pcfModule(&customI2C, PCF_ADDRESS, SDA_PIN, SCL_PIN);
PCF8574DigitalIn button(BTN_PIN);

void setup() {
  Serial.begin(115200);
  Serial.println("PCF8574 Digital Input - ESP32 Custom I2C Pins Example");
  
  // Inisialisasi I2C dengan pin kustom
  customI2C.begin(SDA_PIN, SCL_PIN);
  
  // Inisialisasi PCF8574
  if (pcfModule.begin()) {
    Serial.println("PCF8574 berhasil diinisialisasi");
  } else {
    Serial.println("PCF8574 gagal diinisialisasi. Cek koneksi!");
    while(1);
  }
  
  // Inisialisasi button
  button.init(&pcfModule);
  
  Serial.println("Sistem siap!");
}

void loop() {
  // Update status button
  button.update();
  
  // Tampilkan status tombol saat ditekan
  if (button.isPressed()) {
    Serial.println("Tombol ditekan!");
  }
  
  delay(10); // Delay kecil untuk stabilitas
}

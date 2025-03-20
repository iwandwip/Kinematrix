/**
 * PCF8574 Digital Input Basic Example
 * 
 * Contoh dasar untuk membaca status tombol yang terhubung ke PCF8574
 * 
 * Koneksi Hardware:
 * - PCF8574 terhubung ke Arduino via I2C (SDA, SCL)
 * - Push button terhubung ke pin P0 pada PCF8574
 */

#include "pcf8574-module.h"
#include "pcf8574-input-module.h"

// Alamat I2C untuk PCF8574 (biasanya 0x20-0x27)
#define PCF_ADDRESS 0x20

// Pin yang terhubung ke button pada PCF8574
#define BTN_PIN PCF_PIN0

// Buat objek PCF8574Module
PCF8574Module pcfModule(PCF_ADDRESS);

// Buat objek digital input
PCF8574DigitalIn button(BTN_PIN);

void setup() {
  Serial.begin(115200);
  Serial.println("PCF8574 Digital Input - Basic Example");
  
  // Inisialisasi module PCF8574
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
  // Update status button (penting untuk debouncing)
  button.update();
  
  // Baca status button saat ini
  int state = button.getState();
  
  // Pada PCF8574DigitalIn, tombol menggunakan pull-up resistor internal,
  // sehingga LOW berarti tombol ditekan
  if (state == LOW) {
    Serial.println("Tombol sedang ditekan!");
  }
  
  delay(100); // Delay kecil untuk stabilitas
}

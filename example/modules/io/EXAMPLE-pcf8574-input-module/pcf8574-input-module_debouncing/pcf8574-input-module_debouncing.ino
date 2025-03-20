/**
 * PCF8574 Digital Input Debouncing Example
 * 
 * Contoh penggunaan fitur debouncing untuk mencegah false triggering
 * 
 * Koneksi Hardware:
 * - PCF8574 terhubung ke Arduino via I2C (SDA, SCL)
 * - Push button terhubung ke pin P0 pada PCF8574
 */

#include "pcf8574-module.h"
#include "pcf8574-input-module.h"

#define PCF_ADDRESS 0x20
#define BTN_PIN PCF_PIN0

PCF8574Module pcfModule(PCF_ADDRESS);
PCF8574DigitalIn button(BTN_PIN);

int pressCount = 0;

void setup() {
  Serial.begin(115200);
  Serial.println("PCF8574 Digital Input - Debouncing Example");
  
  // Inisialisasi PCF8574
  if (pcfModule.begin()) {
    Serial.println("PCF8574 berhasil diinisialisasi");
  } else {
    Serial.println("PCF8574 gagal diinisialisasi. Cek koneksi!");
    while(1);
  }
  
  // Inisialisasi button
  button.init(&pcfModule);
  
  // Atur waktu debounce (default: 50ms)
  // Nilai lebih tinggi = lebih stabil tapi lebih lambat
  // Nilai lebih rendah = lebih responsif tapi bisa ada noise
  button.setDebounceTime(50);
  
  Serial.println("Tekan tombol untuk melihat efek debouncing");
}

void loop() {
  // Update status button (penting untuk debouncing)
  button.update();
  
  // Cek apakah button baru saja ditekan
  if (button.isPressed()) {
    pressCount++;
    Serial.print("Tombol ditekan sebanyak: ");
    Serial.println(pressCount);
  }
  
  delay(10); // Delay kecil untuk stabilitas
}

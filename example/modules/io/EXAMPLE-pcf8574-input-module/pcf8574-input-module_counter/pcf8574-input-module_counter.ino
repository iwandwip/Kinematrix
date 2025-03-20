/**
 * PCF8574 Digital Input Button Counter Example
 * 
 * Contoh untuk menghitung jumlah penekanan tombol menggunakan fitur counter bawaan
 * 
 * Koneksi Hardware:
 * - PCF8574 terhubung ke Arduino via I2C (SDA, SCL)
 * - Push button terhubung ke pin P0 pada PCF8574
 * - Push button reset terhubung ke pin P1 pada PCF8574
 */

#include "pcf8574-module.h"
#include "pcf8574-input-module.h"

#define PCF_ADDRESS 0x20
#define BTN_PIN PCF_PIN0
#define RESET_PIN PCF_PIN1

PCF8574Module pcfModule(PCF_ADDRESS);
PCF8574DigitalIn button(BTN_PIN);
PCF8574DigitalIn resetButton(RESET_PIN);

void setup() {
  Serial.begin(115200);
  Serial.println("PCF8574 Digital Input - Button Counter Example");
  
  // Inisialisasi PCF8574
  if (pcfModule.begin()) {
    Serial.println("PCF8574 berhasil diinisialisasi");
  } else {
    Serial.println("PCF8574 gagal diinisialisasi. Cek koneksi!");
    while(1);
  }
  
  // Inisialisasi tombol utama
  button.init(&pcfModule);
  
  // Atur mode penghitungan: COUNT_FALLING (default), COUNT_RISING, atau COUNT_BOTH
  // COUNT_FALLING: Hitung saat tombol ditekan (high ke low)
  // COUNT_RISING: Hitung saat tombol dilepas (low ke high)
  // COUNT_BOTH: Hitung saat tombol ditekan dan dilepas
  button.setCountMode(COUNT_FALLING);
  
  // Inisialisasi tombol reset
  resetButton.init(&pcfModule);
  
  Serial.println("Tekan tombol untuk menambah counter");
  Serial.println("Tekan tombol reset untuk mengatur ulang counter");
}

void loop() {
  // Update status tombol
  button.update();
  resetButton.update();
  
  // Tampilkan nilai counter saat tombol ditekan
  if (button.isPressed()) {
    unsigned long count = button.getCount();
    Serial.print("Tombol telah ditekan sebanyak: ");
    Serial.println(count);
  }
  
  // Reset counter jika tombol reset ditekan
  if (resetButton.isPressed()) {
    button.resetCount();
    Serial.println("Counter direset ke 0");
  }
  
  delay(10); // Delay kecil untuk stabilitas
}

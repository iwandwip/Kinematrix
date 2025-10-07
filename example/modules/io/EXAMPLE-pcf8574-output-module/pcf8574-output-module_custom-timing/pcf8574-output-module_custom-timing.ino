/**
 * PCF8574 Digital Input dengan Custom Timing Detection
 * 
 * Contoh penggunaan fungsi isPressed dengan parameter waktu untuk
 * membatasi frekuensi deteksi penekanan tombol
 * 
 * Koneksi Hardware:
 * - PCF8574 terhubung ke Arduino via I2C (SDA, SCL)
 * - Push button terhubung ke pin P0 pada PCF8574
 */

#include "pcf8574-module.h"
#include "pcf8574-input-module.h"

#define PCF_ADDRESS 0x20
#define BTN_PIN PCF_PIN0

// Interval waktu antara deteksi dalam milidetik
#define DETECTION_INTERVAL 1000

PCF8574Module pcfModule(PCF_ADDRESS);
PCF8574DigitalIn button(BTN_PIN);

int counter = 0;

void setup() {
  Serial.begin(115200);
  Serial.println("PCF8574 Digital Input - Custom Timing Example");
  
  // Inisialisasi PCF8574
  if (pcfModule.begin()) {
    Serial.println("PCF8574 berhasil diinisialisasi");
  } else {
    Serial.println("PCF8574 gagal diinisialisasi. Cek koneksi!");
    while(1);
  }
  
  // Inisialisasi button
  button.init(&pcfModule);
  
  Serial.println("Tekan tombol untuk menambah counter (maksimal 1 kali per detik)");
}

void loop() {
  // Update status button
  button.update();
  
  // Cek apakah tombol ditekan dan apakah sudah waktunya mendeteksi
  // Fungsi isPressed dengan parameter waktu akan mengembalikan true
  // hanya jika sudah melewati interval waktu yang ditentukan
  if (button.getState() == LOW && button.isPressed(DETECTION_INTERVAL)) {
    counter++;
    Serial.print("Counter: ");
    Serial.println(counter);
    Serial.println("Tunggu 1 detik untuk deteksi berikutnya...");
  }
}

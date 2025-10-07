/**
 * PCF8574 Digital Input Multiple Buttons Example
 * 
 * Contoh penggunaan beberapa tombol sekaligus dengan PCF8574
 * 
 * Koneksi Hardware:
 * - PCF8574 terhubung ke Arduino via I2C (SDA, SCL)
 * - Push button 1 terhubung ke pin P0 pada PCF8574
 * - Push button 2 terhubung ke pin P1 pada PCF8574
 * - Push button 3 terhubung ke pin P2 pada PCF8574
 * - Push button 4 terhubung ke pin P3 pada PCF8574
 */

#include "pcf8574-module.h"
#include "pcf8574-input-module.h"

#define PCF_ADDRESS 0x20

// Definisikan pin untuk tombol
#define BTN1_PIN PCF_PIN0
#define BTN2_PIN PCF_PIN1
#define BTN3_PIN PCF_PIN2
#define BTN4_PIN PCF_PIN3

PCF8574Module pcfModule(PCF_ADDRESS);

// Buat objek untuk setiap tombol
PCF8574DigitalIn button1(BTN1_PIN);
PCF8574DigitalIn button2(BTN2_PIN);
PCF8574DigitalIn button3(BTN3_PIN);
PCF8574DigitalIn button4(BTN4_PIN);

void setup() {
  Serial.begin(115200);
  Serial.println("PCF8574 Digital Input - Multiple Buttons Example");
  
  // Inisialisasi PCF8574
  if (pcfModule.begin()) {
    Serial.println("PCF8574 berhasil diinisialisasi");
  } else {
    Serial.println("PCF8574 gagal diinisialisasi. Cek koneksi!");
    while(1);
  }
  
  // Inisialisasi semua tombol
  button1.init(&pcfModule);
  button2.init(&pcfModule);
  button3.init(&pcfModule);
  button4.init(&pcfModule);
  
  Serial.println("Siap menerima input dari 4 tombol");
}

void loop() {
  // Update semua tombol sekaligus menggunakan fungsi updateAll()
  // Parameter terakhir PCF8574DigitalIn::stop() menandakan akhir dari daftar
  PCF8574DigitalIn::updateAll(&button1, &button2, &button3, &button4, PCF8574DigitalIn::stop());
  
  // Cek status masing-masing tombol
  if (button1.isPressed()) {
    Serial.println("Tombol 1 ditekan!");
  }
  
  if (button2.isPressed()) {
    Serial.println("Tombol 2 ditekan!");
  }
  
  if (button3.isPressed()) {
    Serial.println("Tombol 3 ditekan!");
  }
  
  if (button4.isPressed()) {
    Serial.println("Tombol 4 ditekan!");
  }
  
  delay(10); // Delay kecil untuk stabilitas
}

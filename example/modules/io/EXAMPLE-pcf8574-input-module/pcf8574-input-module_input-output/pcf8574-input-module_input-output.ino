/**
 * PCF8574 Kombinasi Digital Input dan Output
 * 
 * Contoh penggunaan input dan output secara bersamaan dengan PCF8574
 * 
 * Koneksi Hardware:
 * - PCF8574 terhubung ke Arduino via I2C (SDA, SCL)
 * - Push button terhubung ke pin P0 pada PCF8574
 * - LED terhubung ke pin P4 pada PCF8574 (dengan resistor yang sesuai)
 */

#include "pcf8574-module.h"
#include "pcf8574-input-module.h"
#include "pcf8574-output-module.h"

#define PCF_ADDRESS 0x20
#define BTN_PIN PCF_PIN0
#define LED_PIN PCF_PIN4

PCF8574Module pcfModule(PCF_ADDRESS);
PCF8574DigitalIn button(BTN_PIN);
PCF8574DigitalOut led(LED_PIN, &pcfModule);

void setup() {
  Serial.begin(115200);
  Serial.println("PCF8574 - Kombinasi Input Output Example");
  
  // Inisialisasi PCF8574
  if (pcfModule.begin()) {
    Serial.println("PCF8574 berhasil diinisialisasi");
  } else {
    Serial.println("PCF8574 gagal diinisialisasi. Cek koneksi!");
    while(1);
  }
  
  // Inisialisasi button
  button.init(&pcfModule);
  
  // LED sudah diinisialisasi di constructor, tapi kita bisa melakukan inizialisasi
  // tambahan di sini jika diperlukan
  
  Serial.println("Tekan tombol untuk menghidupkan/mematikan LED");
}

void loop() {
  // Update status button
  button.update();
  
  // Update status LED (diperlukan jika menggunakan fungsi delay seperti onDelay/offDelay)
  led.update();
  
  // Toggle LED saat tombol ditekan
  if (button.isPressed()) {
    Serial.println("Tombol ditekan - Toggle LED");
    led.toggle();
  }
  
  delay(10); // Delay kecil untuk stabilitas
}

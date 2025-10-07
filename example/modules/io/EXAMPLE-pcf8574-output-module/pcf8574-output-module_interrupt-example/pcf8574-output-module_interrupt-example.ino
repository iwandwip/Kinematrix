/**
 * PCF8574 Digital Input dengan Interrupt
 * 
 * Contoh penggunaan PCF8574 dengan pin interrupt untuk deteksi perubahan status
 * 
 * Koneksi Hardware:
 * - PCF8574 terhubung ke Arduino via I2C (SDA, SCL)
 * - Pin INT dari PCF8574 terhubung ke pin 2 Arduino (interrupt)
 * - Push button terhubung ke pin P0 pada PCF8574
 */

#include "pcf8574-module.h"
#include "pcf8574-input-module.h"

#define PCF_ADDRESS 0x20
#define BTN_PIN PCF_PIN0
#define INT_PIN 2

// Flag untuk interrupt
volatile bool interruptOccurred = false;

// Interrupt Service Routine
void pcfInterrupt() {
  interruptOccurred = true;
}

// Objek PCF8574 dengan interrupt
PCF8574Module pcfModule(PCF_ADDRESS, INT_PIN, pcfInterrupt);
PCF8574DigitalIn button(BTN_PIN);

void setup() {
  Serial.begin(115200);
  Serial.println("PCF8574 Digital Input - Interrupt Example");
  
  // Inisialisasi PCF8574
  if (pcfModule.begin()) {
    Serial.println("PCF8574 berhasil diinisialisasi");
  } else {
    Serial.println("PCF8574 gagal diinisialisasi. Cek koneksi!");
    while(1);
  }
  
  // Inisialisasi button
  button.init(&pcfModule);
  
  Serial.println("Siap menerima interrupt dari PCF8574");
}

void loop() {
  // Cek apakah interrupt terjadi
  if (interruptOccurred) {
    // Reset flag
    interruptOccurred = false;
    
    // Update status tombol
    button.update();
    
    // Baca status tombol
    int state = button.getState();
    
    if (state == LOW) {
      Serial.println("Tombol ditekan! (via interrupt)");
    } else {
      Serial.println("Tombol dilepas! (via interrupt)");
    }
  }
  
  // Anda masih bisa melakukan operasi lain di sini tanpa polling terus-menerus
  delay(10);
}

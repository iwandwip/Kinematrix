/**
 * PCF8574 Digital Input Long Press Detection Example
 * 
 * Contoh untuk mendeteksi long press pada tombol
 * 
 * Koneksi Hardware:
 * - PCF8574 terhubung ke Arduino via I2C (SDA, SCL)
 * - Push button terhubung ke pin P0 pada PCF8574
 * - LED merah terhubung ke pin 12 Arduino (short press)
 * - LED hijau terhubung ke pin 13 Arduino (long press)
 */

#include "pcf8574-module.h"
#include "pcf8574-input-module.h"

#define PCF_ADDRESS 0x20
#define BTN_PIN PCF_PIN0

#define LED_SHORT 12
#define LED_LONG 13

// Waktu untuk long press dalam milidetik
#define LONG_PRESS_TIME 1000

PCF8574Module pcfModule(PCF_ADDRESS);
PCF8574DigitalIn button(BTN_PIN);

void setup() {
  Serial.begin(115200);
  Serial.println("PCF8574 Digital Input - Long Press Detection Example");
  
  // Setup LED untuk output
  pinMode(LED_SHORT, OUTPUT);
  pinMode(LED_LONG, OUTPUT);
  digitalWrite(LED_SHORT, LOW);
  digitalWrite(LED_LONG, LOW);
  
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
  
  // Cek kondisi long press
  if (button.isLongPressed(LONG_PRESS_TIME)) {
    Serial.println("Tombol ditekan lama!");
    digitalWrite(LED_LONG, HIGH);
  } else {
    digitalWrite(LED_LONG, LOW);
  }
  
  // Deteksi short press
  if (button.isPressed()) {
    Serial.println("Tombol ditekan!");
    digitalWrite(LED_SHORT, HIGH);
  }
  
  if (button.isReleased()) {
    Serial.println("Tombol dilepas!");
    digitalWrite(LED_SHORT, LOW);
  }
  
  delay(10); // Delay kecil untuk stabilitas
}

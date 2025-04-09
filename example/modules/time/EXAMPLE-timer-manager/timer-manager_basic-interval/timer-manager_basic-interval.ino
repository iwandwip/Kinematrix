/*
 * Basic_Interval.ino
 * 
 * Contoh penggunaan dasar TimerManager dengan tipe TIMER_INTERVAL.
 * Timer interval akan otomatis reset dan berjalan kembali saat waktu habis.
 */

#include "timer-manager.h"

#define LED_PIN 13

// Buat timer interval dengan durasi 1000ms (1 detik)
TimerManager blinkTimer(1000, TIMER_INTERVAL);

// Variabel untuk status LED
bool ledState = false;

void setup() {
  Serial.begin(115200);
  pinMode(LED_PIN, OUTPUT);
  
  Serial.println("Contoh Timer Interval");
  Serial.println("LED akan berkedip setiap 1 detik");
  
  // Mulai timer
  blinkTimer.start();
}

void loop() {
  // Periksa apakah timer sudah selesai
  if (blinkTimer.isExpired()) {
    // Toggle LED ketika interval selesai
    ledState = !ledState;
    digitalWrite(LED_PIN, ledState ? HIGH : LOW);
    
    Serial.print("Kedip! Status LED: ");
    Serial.println(ledState ? "ON" : "OFF");
    
    // Timer interval akan otomatis reset, 
    // tapi kita perlu memanggil update() untuk menjalankan callback
    // jika ada (dalam contoh ini tidak menggunakan callback)
    blinkTimer.update();
  }
  
  // Kode lain dapat berjalan di sini - program tetap responsif
  // Misalnya, baca sensor, komunikasi, dll.
}

/*
 * Basic_OneShot.ino
 * 
 * Contoh penggunaan dasar TimerManager dengan tipe TIMER_ONESHOT.
 * Timer one-shot akan berjalan sekali dan berhenti setelah waktu habis.
 */

#include "timer-manager.h"

#define LED_PIN 13

// Buat timer one-shot dengan durasi 5000ms (5 detik)
TimerManager timer(5000, TIMER_ONESHOT);

void setup() {
  Serial.begin(115200);
  pinMode(LED_PIN, OUTPUT);
  
  Serial.println("Contoh Timer One-Shot");
  Serial.println("LED akan menyala selama 5 detik");
  
  // Nyalakan LED
  digitalWrite(LED_PIN, HIGH);
  
  // Mulai timer
  timer.start();
}

void loop() {
  // Periksa apakah timer sudah selesai
  if (timer.isExpired() && timer.isRunning()) {
    // Matikan LED saat timer habis
    digitalWrite(LED_PIN, LOW);
    Serial.println("Timer selesai - LED dimatikan");
    
    // Hentikan timer
    timer.stop();
  }
  
  // Tampilkan waktu tersisa jika timer masih berjalan
  if (timer.isRunning() && !timer.isExpired()) {
    Serial.print("Waktu tersisa: ");
    Serial.print(timer.getRemainingSeconds());
    Serial.println(" detik");
    delay(1000); // Update display setiap 1 detik
  }
  
  // Kode lain bisa berjalan di sini - program tetap responsif
}

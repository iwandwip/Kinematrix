/*
 * Long_Duration_Timer.ino
 * 
 * Contoh penggunaan TimerManager untuk timer dengan durasi panjang
 * dan menampilkan waktu dalam format yang mudah dibaca.
 */

#include "timer-manager.h"

#define LED_PIN 13

// Timer 1 jam
TimerManager longTimer;

void setup() {
  Serial.begin(115200);
  pinMode(LED_PIN, OUTPUT);
  
  Serial.println("Contoh Long Duration Timer");
  Serial.println("Timer 1 jam dengan display waktu tersisa");

  // Set durasi 1 jam (bisa juga dengan setDurationMinutes atau setDurationSeconds)
  longTimer.setDurationHours(1);
  
  // Untuk demo, kita bisa mempercepat dengan durasi lebih pendek
  // Uncomment baris berikut untuk menggunakan timer 1 menit saja
  // longTimer.setDurationMinutes(1);
  
  Serial.print("Durasi timer: ");
  Serial.print(longTimer.getDurationMinutes());
  Serial.println(" menit");
  
  // Nyalakan LED
  digitalWrite(LED_PIN, HIGH);
  
  // Mulai timer
  longTimer.start();
  
  Serial.println("Timer dimulai!");
}

void loop() {
  // Periksa status timer
  if (longTimer.isRunning() && !longTimer.isExpired()) {
    // Dapatkan waktu yang tersisa dalam berbagai format
    unsigned long remainingMillis = longTimer.getRemainingMillis();
    unsigned long remainingSeconds = longTimer.getRemainingSeconds();
    unsigned long remainingMinutes = longTimer.getRemainingMinutes();
    unsigned long remainingHours = longTimer.getRemainingHours();
    
    // Dapatkan waktu dalam komponen terpisah
    unsigned long days, hours, minutes, seconds, millis;
    longTimer.getRemainingTime(days, hours, minutes, seconds, millis);
    
    // Tampilkan waktu tersisa setiap 5 detik
    static unsigned long lastUpdate = 0;
    if (millis() - lastUpdate >= 5000 || lastUpdate == 0) {
      // Gunakan format string
      String formattedTime = longTimer.getRemainingTimeString();
      
      Serial.println("\nWaktu tersisa:");
      Serial.print("Format string: ");
      Serial.println(formattedTime);
      
      Serial.print("Manual format: ");
      if (hours > 0) {
        Serial.print(hours);
        Serial.print(" jam, ");
      }
      Serial.print(minutes);
      Serial.print(" menit, ");
      Serial.print(seconds);
      Serial.println(" detik");
      
      // Tampilkan progress
      Serial.print("Progress: ");
      Serial.print(longTimer.getProgressPercent());
      Serial.println("%");
      
      lastUpdate = millis();
    }
    
    // Kedipkan LED saat mendekati akhir timer (< 10%)
    float progress = longTimer.getProgress();
    if (progress > 0.9) {
      // Kedipkan LED cepat pada 10% terakhir
      static unsigned long lastBlink = 0;
      if (millis() - lastBlink >= 100) {
        digitalWrite(LED_PIN, !digitalRead(LED_PIN));
        lastBlink = millis();
        
        // Peringatan 10% terakhir (tampilkan hanya sekali)
        static bool warningShown = false;
        if (!warningShown) {
          Serial.println("\n*** PERINGATAN: 10% waktu tersisa! ***");
          warningShown = true;
        }
      }
    }
  } 
  else if (longTimer.isExpired()) {
    // Timer telah selesai
    if (longTimer.isRunning()) {
      Serial.println("\n*** TIMER SELESAI! ***");
      digitalWrite(LED_PIN, LOW);
      longTimer.stop();
      
      // Tampilkan statistik
      unsigned long totalDuration = longTimer.getDuration();
      Serial.print("Total durasi: ");
      Serial.print(totalDuration / 1000);
      Serial.println(" detik");
      
      // Gunakan fungsi konversi waktu statis
      Serial.print("Dalam menit: ");
      Serial.print(TimerManager::millisToMinutes(totalDuration));
      Serial.println(" menit");
    }
  }
  
  // Kode lain dapat ditambahkan di sini
  // Sistem tetap responsif selama timer berjalan
}

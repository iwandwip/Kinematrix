/*
 * Pause_Resume.ino
 * 
 * Contoh penggunaan fitur pause dan resume pada TimerManager.
 * Fitur ini berguna untuk aplikasi yang memerlukan pembekuan sementara timer.
 */

#include "timer-manager.h"

#define LED_PIN 13
#define BUTTON_PIN 2

// Buat timer dengan durasi 10 detik
TimerManager countdownTimer(10000, TIMER_ONESHOT);

// Tombol untuk pause/resume
bool lastButtonState = HIGH;
bool timerPaused = false;

void setup() {
  Serial.begin(115200);
  pinMode(LED_PIN, OUTPUT);
  pinMode(BUTTON_PIN, INPUT_PULLUP);
  
  Serial.println("Contoh Timer Pause/Resume");
  Serial.println("- Timer berjalan selama 10 detik total");
  Serial.println("- LED menyala selama timer berjalan");
  Serial.println("- Tekan tombol untuk pause/resume timer");
  
  // Nyalakan LED
  digitalWrite(LED_PIN, HIGH);
  
  // Mulai timer
  countdownTimer.start();
  
  Serial.println("Timer dimulai!");
}

void loop() {
  // Baca tombol untuk kontrol pause/resume
  bool currentButtonState = digitalRead(BUTTON_PIN);
  
  // Deteksi falling edge (ditekan)
  if (currentButtonState == LOW && lastButtonState == HIGH) {
    if (countdownTimer.isPaused()) {
      // Resume timer
      countdownTimer.resume();
      timerPaused = false;
      
      // Nyalakan LED kembali
      digitalWrite(LED_PIN, HIGH);
      
      Serial.print("Timer dilanjutkan. Waktu tersisa: ");
      Serial.print(countdownTimer.getRemainingSeconds());
      Serial.println(" detik");
    } else if (countdownTimer.isRunning()) {
      // Pause timer
      countdownTimer.pause();
      timerPaused = true;
      
      // Matikan LED selama pause
      digitalWrite(LED_PIN, LOW);
      
      Serial.print("Timer dijeda. Waktu berlalu: ");
      Serial.print(countdownTimer.getElapsedSeconds());
      Serial.println(" detik");
    }
    
    // Debounce delay
    delay(50);
  }
  
  // Update status tombol
  lastButtonState = currentButtonState;
  
  // Periksa apakah timer sudah selesai
  if (countdownTimer.isExpired() && countdownTimer.isRunning()) {
    Serial.println("Timer selesai!");
    digitalWrite(LED_PIN, LOW);
    countdownTimer.stop();
  }
  
  // Tampilkan status timer setiap detik
  static unsigned long lastUpdate = 0;
  if (millis() - lastUpdate >= 1000) {
    if (countdownTimer.isRunning() && !countdownTimer.isExpired()) {
      if (!timerPaused) {
        Serial.print("Berjalan: ");
        Serial.print(countdownTimer.getElapsedSeconds());
        Serial.print("s / Tersisa: ");
        Serial.print(countdownTimer.getRemainingSeconds());
        Serial.println("s");
      } else {
        Serial.print("PAUSED pada ");
        Serial.print(countdownTimer.getElapsedSeconds());
        Serial.println("s");
      }
    }
    lastUpdate = millis();
  }
  
  // Blink LED saat paused untuk indikasi visual
  if (timerPaused) {
    static unsigned long lastBlink = 0;
    if (millis() - lastBlink >= 300) {
      digitalWrite(LED_PIN, !digitalRead(LED_PIN));
      lastBlink = millis();
    }
  }
  
  // Kode lain bisa ditambahkan di sini
}

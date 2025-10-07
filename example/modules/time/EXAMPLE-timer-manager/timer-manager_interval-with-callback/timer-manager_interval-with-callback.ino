/*
 * Interval_With_Callback.ino
 * 
 * Contoh penggunaan TimerManager dengan tipe TIMER_INTERVAL
 * dan menggunakan callback function.
 */

#include "timer-manager.h"

#define LED_PIN 13

// Buat timer interval dengan durasi 1000ms (1 detik)
TimerManager blinkTimer(1000, TIMER_INTERVAL);

// Variabel untuk status LED
bool ledState = false;
int blinkCount = 0;
const int MAX_BLINKS = 10;

// Callback function untuk timer
void blinkCallback(TimerManager* timer) {
  // Toggle LED
  ledState = !ledState;
  digitalWrite(LED_PIN, ledState ? HIGH : LOW);
  
  blinkCount++;
  Serial.print("Blink #");
  Serial.print(blinkCount);
  Serial.print(" LED: ");
  Serial.println(ledState ? "ON" : "OFF");
  
  // Hentikan timer setelah 10 kali kedip
  if (blinkCount >= MAX_BLINKS) {
    Serial.println("10 kedipan selesai, timer berhenti");
    timer->stop();
    digitalWrite(LED_PIN, LOW);
  }
}

void setup() {
  Serial.begin(115200);
  pinMode(LED_PIN, OUTPUT);
  
  Serial.println("Contoh Timer Interval dengan Callback");
  Serial.println("LED akan berkedip 10 kali, sekali per detik");
  
  // Set callback function
  blinkTimer.setCallback(blinkCallback);
  
  // Aktifkan auto-execute callback
  blinkTimer.enableAutoExecuteCallback(true);
  
  // Mulai timer
  blinkTimer.start();
}

void loop() {
  // Update timer untuk mengeksekusi callback saat interval selesai
  blinkTimer.update();
  
  // Kode lain dapat ditambahkan di sini tanpa mengganggu timer
  
  // Contoh: tampilkan kemajuan jika timer masih berjalan
  if (blinkTimer.isRunning()) {
    float progress = (float)blinkCount / MAX_BLINKS * 100.0;
    
    // Tampilkan progress setiap 50ms, tapi hanya jika berubah
    static int lastProgress = -1;
    if ((millis() % 50) == 0) {
      int currentProgress = (int)progress;
      if (currentProgress != lastProgress) {
        Serial.print("Kemajuan: ");
        Serial.print(currentProgress);
        Serial.println("%");
        lastProgress = currentProgress;
      }
    }
  }
}

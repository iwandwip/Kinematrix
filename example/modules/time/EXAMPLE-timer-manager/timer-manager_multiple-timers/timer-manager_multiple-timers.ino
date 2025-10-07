/*
 * Multiple_Timers.ino
 * 
 * Contoh penggunaan beberapa TimerManager secara bersamaan
 * untuk menangani tugas-tugas yang berbeda.
 */

#include "timer-manager.h"

#define LED_PIN 13
#define BUTTON_PIN 2

// Buat beberapa timer untuk berbagai keperluan
TimerManager ledTimer(500, TIMER_INTERVAL);     // Timer untuk kontrol LED
TimerManager buttonTimer(50, TIMER_INTERVAL);   // Timer untuk debounce tombol
TimerManager displayTimer(1000, TIMER_INTERVAL); // Timer untuk update display

// Variabel status
bool ledState = false;
bool buttonPressed = false;
unsigned long loopCount = 0;

void setup() {
  Serial.begin(115200);
  pinMode(LED_PIN, OUTPUT);
  pinMode(BUTTON_PIN, INPUT_PULLUP);
  
  Serial.println("Contoh Multiple Timers");
  Serial.println("- LED berkedip setiap 0.5 detik");
  Serial.println("- Tombol dicek setiap 50ms");
  Serial.println("- Display diupdate setiap 1 detik");
  
  // Mulai semua timer
  ledTimer.start();
  buttonTimer.start();
  displayTimer.start();
}

void loop() {
  // Increment counter untuk menghitung jumlah loop per detik
  loopCount++;
  
  // Timer 1: Kontrol LED
  if (ledTimer.isExpired()) {
    ledState = !ledState;
    digitalWrite(LED_PIN, ledState ? HIGH : LOW);
    ledTimer.reset(); // Reset timer interval
  }
  
  // Timer 2: Baca tombol
  if (buttonTimer.isExpired()) {
    // Baca status tombol (active low dengan pull-up internal)
    bool currentButtonState = (digitalRead(BUTTON_PIN) == LOW);
    
    // Deteksi perubahan status tombol (tekan)
    if (currentButtonState && !buttonPressed) {
      Serial.println("Tombol ditekan");
      
      // Ubah kecepatan kedipan LED
      unsigned long currentInterval = ledTimer.getDuration();
      if (currentInterval > 100) {
        ledTimer.setDuration(currentInterval - 100);
        Serial.print("Kecepatan kedipan dipercepat: ");
        Serial.print(ledTimer.getDuration());
        Serial.println("ms");
      } else {
        // Reset ke nilai awal jika terlalu cepat
        ledTimer.setDuration(500);
        Serial.println("Kecepatan kedipan direset ke 500ms");
      }
    }
    
    buttonPressed = currentButtonState;
    buttonTimer.reset(); // Reset timer untuk pembacaan berikutnya
  }
  
  // Timer 3: Update display status
  if (displayTimer.isExpired()) {
    Serial.println("\n----- STATUS SISTEM -----");
    Serial.print("LED blink interval: ");
    Serial.print(ledTimer.getDuration());
    Serial.println("ms");
    
    Serial.print("Tombol: ");
    Serial.println(buttonPressed ? "DITEKAN" : "DILEPAS");
    
    Serial.print("Loop per detik: ");
    Serial.println(loopCount);
    loopCount = 0; // Reset counter
    
    Serial.println("-----------------------\n");
    
    displayTimer.reset(); // Reset timer untuk update berikutnya
  }
  
  // Kode lain dapat ditambahkan di sini
}

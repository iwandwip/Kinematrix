/*
 * Button_Debounce.ino
 * 
 * Contoh penggunaan TimerManager untuk mengatasi bouncing pada tombol
 * dan mendeteksi berbagai jenis penekanan (short press, long press).
 */

#include "timer-manager.h"

#define BUTTON_PIN 2
#define LED_PIN 13

// Definisi durasi
#define DEBOUNCE_TIME 50       // 50ms untuk debounce
#define LONG_PRESS_TIME 1000   // 1000ms untuk long press

// Timer untuk debounce dan deteksi tipe press
TimerManager debounceTimer(DEBOUNCE_TIME);
TimerManager longPressTimer(LONG_PRESS_TIME);

// Status tombol
bool buttonState = HIGH;       // Status saat ini (HIGH = released)
bool lastButtonState = HIGH;   // Status terakhir
bool buttonPressed = false;    // Flag untuk tracking button down
bool longPressDetected = false;// Flag untuk track jika sudah terdeteksi long press

// Counter button
int shortPressCount = 0;
int longPressCount = 0;

void setup() {
  Serial.begin(115200);
  pinMode(BUTTON_PIN, INPUT_PULLUP);
  pinMode(LED_PIN, OUTPUT);
  
  Serial.println("Contoh Button Debounce dengan TimerManager");
  Serial.println("- Short press: Nyalakan LED selama 1 detik");
  Serial.println("- Long press: Toggle status LED");
}

void loop() {
  // Baca status tombol (dengan pull-up, LOW berarti ditekan)
  buttonState = digitalRead(BUTTON_PIN);
  
  // ==== DETECT BUTTON PRESS (Falling Edge) ====
  if (buttonState == LOW && lastButtonState == HIGH) {
    // Tombol baru saja ditekan, mulai debounce timer
    debounceTimer.start();
  }
  
  // ==== HANDLE DEBOUNCE COMPLETION ====
  if (debounceTimer.isExpired() && debounceTimer.isRunning()) {
    // Debounce timer habis, baca tombol lagi untuk konfirmasi
    if (digitalRead(BUTTON_PIN) == LOW) {
      // Konfirmasi tombol masih ditekan setelah debounce
      buttonPressed = true;
      
      // Mulai timer untuk deteksi long press
      longPressTimer.start();
      
      Serial.println("Tombol ditekan");
    }
    
    // Stop timer debounce
    debounceTimer.stop();
  }
  
  // ==== DETECT LONG PRESS ====
  if (buttonPressed && !longPressDetected && longPressTimer.isExpired()) {
    // Terdeteksi long press
    longPressDetected = true;
    longPressCount++;
    
    // Toggle LED
    static bool ledStatus = false;
    ledStatus = !ledStatus;
    digitalWrite(LED_PIN, ledStatus);
    
    Serial.print("LONG PRESS #");
    Serial.print(longPressCount);
    Serial.print(" - LED toggled to: ");
    Serial.println(ledStatus ? "ON" : "OFF");
    
    longPressTimer.stop();
  }
  
  // ==== DETECT BUTTON RELEASE (Rising Edge) ====
  if (buttonState == HIGH && lastButtonState == LOW) {
    // Tombol baru dilepas
    if (buttonPressed) {
      // Cek apakah ini short press (belum mencapai long press threshold)
      if (!longPressDetected) {
        // Short press dideteksi
        shortPressCount++;
        
        Serial.print("SHORT PRESS #");
        Serial.println(shortPressCount);
        
        // Nyalakan LED selama 1 detik
        digitalWrite(LED_PIN, HIGH);
        
        // Gunakan timer one-shot untuk mematikan LED setelah 1 detik
        static TimerManager ledTimer(1000, TIMER_ONESHOT);
        ledTimer.setCallback([](TimerManager* timer) {
          digitalWrite(LED_PIN, LOW);
        });
        ledTimer.enableAutoExecuteCallback(true);
        ledTimer.restart();
      }
      
      // Reset semua state tracking
      buttonPressed = false;
      longPressDetected = false;
      longPressTimer.stop();
    }
  }
  
  // Update button state
  lastButtonState = buttonState;
  
  // Update timers
  longPressTimer.update();
  
  // Delay kecil untuk stabilitas
  delay(10);
}

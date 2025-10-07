/*
 * Complex_Application.ino
 * 
 * Contoh aplikasi kompleks menggunakan TimerManager untuk mengelola
 * sistem otomasi sederhana dengan sensor dan aktuator.
 */

#include "timer-manager.h"

// Pin definitions
#define SENSOR_PIN A0
#define RELAY_PIN 7
#define LED_STATUS_PIN 13
#define BUTTON_PIN 2

// Threshold sensor
#define SENSOR_THRESHOLD 500

// Timer untuk berbagai tugas
TimerManager sensorTimer(1000, TIMER_INTERVAL);     // Baca sensor setiap 1 detik
TimerManager displayTimer(5000, TIMER_INTERVAL);    // Update display setiap 5 detik
TimerManager relayTimer(10000, TIMER_ONESHOT);      // Timer untuk kontrol relay
TimerManager buttonTimer(50, TIMER_INTERVAL);       // Debounce tombol
TimerManager ledBlinkTimer(200, TIMER_INTERVAL);    // Kedipan LED

// Status sistem
bool systemActive = true;
bool relayActive = false;
bool ledBlinkEnabled = false;
bool ledState = false;
int sensorValue = 0;
int alarmCount = 0;
unsigned long systemStartTime = 0;

// Callback untuk sensor timer
void readSensorCallback(TimerManager* timer) {
  // Baca sensor
  sensorValue = analogRead(SENSOR_PIN);
  
  // Cek threshold
  if (sensorValue > SENSOR_THRESHOLD && systemActive) {
    // Aktifkan alarm jika nilai sensor di atas threshold
    if (!ledBlinkEnabled) {
      Serial.println("ALARM: Sensor melebihi threshold!");
      ledBlinkEnabled = true;
      ledBlinkTimer.start();
      alarmCount++;
    }
    
    // Aktifkan relay jika belum aktif dan tidak ada timer aktif
    if (!relayActive && !relayTimer.isRunning()) {
      digitalWrite(RELAY_PIN, HIGH);
      relayActive = true;
      
      // Set timer untuk mematikan relay setelah 10 detik
      relayTimer.start();
      
      Serial.println("Relay diaktifkan, akan mati dalam 10 detik");
    }
  }
}

// Callback untuk relay timer
void relayTimerCallback(TimerManager* timer) {
  // Matikan relay saat timer habis
  digitalWrite(RELAY_PIN, LOW);
  relayActive = false;
  
  Serial.println("Relay dimatikan oleh timer");
}

// Callback untuk display timer
void displayStatusCallback(TimerManager* timer) {
  // Hitung uptime (waktu sistem berjalan)
  unsigned long uptimeSeconds = (millis() - systemStartTime) / 1000;
  
  // Tampilkan status sistem
  Serial.println("\n----- STATUS SISTEM -----");
  Serial.print("Uptime: ");
  Serial.print(uptimeSeconds / 3600); // jam
  Serial.print("h ");
  Serial.print((uptimeSeconds % 3600) / 60); // menit
  Serial.print("m ");
  Serial.print(uptimeSeconds % 60); // detik
  Serial.println("s");
  
  Serial.print("Status: ");
  Serial.println(systemActive ? "AKTIF" : "PAUSED");
  
  Serial.print("Nilai Sensor: ");
  Serial.println(sensorValue);
  
  Serial.print("Relay: ");
  Serial.println(relayActive ? "ON" : "OFF");
  
  if (relayActive && relayTimer.isRunning()) {
    Serial.print("Relay akan mati dalam: ");
    Serial.print(relayTimer.getRemainingSeconds());
    Serial.println(" detik");
  }
  
  Serial.print("Jumlah Alarm: ");
  Serial.println(alarmCount);
  
  Serial.println("--------------------------\n");
}

void setup() {
  Serial.begin(115200);
  
  // Setup pin
  pinMode(SENSOR_PIN, INPUT);
  pinMode(RELAY_PIN, OUTPUT);
  pinMode(LED_STATUS_PIN, OUTPUT);
  pinMode(BUTTON_PIN, INPUT_PULLUP);
  
  // Inisialisasi relay ke OFF
  digitalWrite(RELAY_PIN, LOW);
  
  // Set callbacks
  sensorTimer.setCallback(readSensorCallback);
  sensorTimer.enableAutoExecuteCallback(true);
  
  displayTimer.setCallback(displayStatusCallback);
  displayTimer.enableAutoExecuteCallback(true);
  
  relayTimer.setCallback(relayTimerCallback);
  relayTimer.enableAutoExecuteCallback(true);
  
  // Start timers
  sensorTimer.start();
  displayTimer.start();
  buttonTimer.start();
  
  // Catat waktu mulai sistem
  systemStartTime = millis();
  
  Serial.println("Sistem Kontrol Otomasi");
  Serial.println("- Membaca sensor setiap 1 detik");
  Serial.println("- Update display setiap 5 detik");
  Serial.println("- Relay aktif selama 10 detik saat alarm");
  Serial.println("- Tekan tombol untuk pause/resume sistem");
  Serial.println("\nSistem dimulai!");
}

void loop() {
  // Update semua timer
  sensorTimer.update();
  displayTimer.update();
  relayTimer.update();
  
  // Cek tombol setiap 50ms
  if (buttonTimer.isExpired()) {
    static bool lastButtonState = HIGH;
    bool buttonState = digitalRead(BUTTON_PIN);
    
    // Deteksi tekan tombol (falling edge)
    if (buttonState == LOW && lastButtonState == HIGH) {
      // Toggle status sistem
      systemActive = !systemActive;
      
      if (systemActive) {
        Serial.println("Sistem diaktifkan kembali");
        sensorTimer.resume();
        displayTimer.resume();
      } else {
        Serial.println("Sistem di-pause");
        sensorTimer.pause();
        displayTimer.pause();
        
        // Matikan blink LED dan relay saat paused
        ledBlinkEnabled = false;
        if (relayActive) {
          digitalWrite(RELAY_PIN, LOW);
          relayActive = false;
        }
      }
    }
    
    lastButtonState = buttonState;
    buttonTimer.reset();
  }
  
  // Kontrol LED berkedip untuk alarm
  if (ledBlinkEnabled) {
    if (ledBlinkTimer.isExpired()) {
      ledState = !ledState;
      digitalWrite(LED_STATUS_PIN, ledState ? HIGH : LOW);
      ledBlinkTimer.reset();
      
      // Matikan blink jika sensor kembali normal
      if (sensorValue <= SENSOR_THRESHOLD) {
        ledBlinkEnabled = false;
        digitalWrite(LED_STATUS_PIN, LOW);
        ledState = false;
      }
    }
  } else {
    // Jika tidak berkedip, gunakan LED sebagai indikator sistem
    digitalWrite(LED_STATUS_PIN, systemActive ? HIGH : LOW);
  }
  
  // Kode lain yang perlu dijalankan loop dapat ditambahkan di sini
}

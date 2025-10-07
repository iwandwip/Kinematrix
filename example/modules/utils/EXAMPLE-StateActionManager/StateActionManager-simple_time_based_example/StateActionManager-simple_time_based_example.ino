#define ENABLE_MODULE_STATE_ACTION_MANAGER
#include "Kinematrix.h"

StateActionManager manager;

const int LED_PIN = 13;
const int PUMP_RELAY_PIN = 7;

void blinkLed() {
  static bool ledState = false;
  ledState = !ledState;
  digitalWrite(LED_PIN, ledState);
}

void waterPumpOn() {
  digitalWrite(PUMP_RELAY_PIN, HIGH);
  Serial.println("Pompa air aktif");
}

void waterPumpOff() {
  digitalWrite(PUMP_RELAY_PIN, LOW);
  Serial.println("Pompa air mati");
}

void showStatus() {
  Serial.println("=== Status Sistem ===");
  Serial.print("Waktu berjalan: ");
  Serial.print(millis() / 1000);
  Serial.println(" detik");
  Serial.println("===================");
}

void setup() {
  Serial.begin(9600);
  pinMode(LED_PIN, OUTPUT);
  pinMode(PUMP_RELAY_PIN, OUTPUT);
  
  // LED berkedip setiap 500ms
  manager.addTimeBasedMonitor(
    "LedBlinker",                // Nama monitor
    500,                         // Interval 500ms
    blinkLed,                    // Fungsi yang dijalankan
    0                            // 0 = tanpa batas
  );
  
  // Jadwal pompa air - nyala setiap 10 detik
  manager.addTimeBasedMonitor(
    "PumpOn",
    10000,                       // Interval 10 detik
    waterPumpOn,                 // Fungsi yang dijalankan
    0                            // Tanpa batas
  );
  
  // Matikan pompa air 2 detik setelah menyala
  manager.addTimeBasedMonitor(
    "PumpOff",
    12000,                       // Interval 12 detik (10+2)
    waterPumpOff,                // Fungsi yang dijalankan
    0                            // Tanpa batas
  );
  
  // Tampilkan status setiap 5 detik
  manager.addTimeBasedMonitor(
    "StatusDisplay",
    5000,                        // Interval 5 detik
    showStatus,                  // Fungsi yang dijalankan
    0                            // Tanpa batas
  );
  
  Serial.println("Sistem penjadwalan dimulai");
}

void loop() {
  manager.update();
  delay(50);
}
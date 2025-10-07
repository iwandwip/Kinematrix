#define ENABLE_MODULE_STATE_ACTION_MANAGER
#include "Kinematrix.h"

StateActionManager manager;

const int LIGHT_SENSOR_PIN = A0;
const int BUZZER_PIN = 8;

float readLightLevel() {
  int sensorValue = analogRead(LIGHT_SENSOR_PIN);
  return map(sensorValue, 0, 1023, 0, 100); // 0-100%
}

void lightChangeDetected() {
  digitalWrite(BUZZER_PIN, HIGH);
  Serial.print("Perubahan cahaya terdeteksi! Nilai baru: ");
  Serial.print(manager.getCurrentValue(0));
  Serial.println("%");
  delay(200);
  digitalWrite(BUZZER_PIN, LOW);
}

void setup() {
  Serial.begin(9600);
  pinMode(BUZZER_PIN, OUTPUT);
  
  // Monitor perubahan cahaya yang signifikan (>20%)
  manager.addChangeMonitor(
    "LightChange",               // Nama monitor
    readLightLevel,              // Fungsi pembacaan
    20.0,                        // Minimal perubahan 20%
    lightChangeDetected,         // Fungsi yang dijalankan
    1                            // Jalankan 1 kali
  );
  
  Serial.println("Monitoring perubahan cahaya dimulai");
  Serial.println("Akan mendeteksi perubahan >20%");
}

void loop() {
  manager.update();
  
  // Tampilkan level cahaya setiap 1 detik
  static unsigned long lastPrintTime = 0;
  if (millis() - lastPrintTime > 1000) {
    float light = readLightLevel();
    Serial.print("Level cahaya saat ini: ");
    Serial.print(light);
    Serial.println("%");
    lastPrintTime = millis();
  }
  
  delay(100);
}
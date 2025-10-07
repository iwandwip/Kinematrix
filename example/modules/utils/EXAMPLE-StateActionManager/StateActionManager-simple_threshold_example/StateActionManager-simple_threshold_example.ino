#define ENABLE_MODULE_STATE_ACTION_MANAGER
#include "Kinematrix.h"

StateActionManager manager;

const int TEMP_SENSOR_PIN = A0;
const int LED_PIN = 13;
const float TEMP_THRESHOLD = 28.0;

float readTemperature() {
  int sensorValue = analogRead(TEMP_SENSOR_PIN);
  float voltage = sensorValue * (5.0 / 1023.0);
  float temperature = voltage * 100.0; // Asumsi sensor yang memberikan 10mV per derajat
  return temperature;
}

void temperatureAlert() {
  digitalWrite(LED_PIN, HIGH);
  Serial.print("Suhu di atas threshold: ");
  Serial.print(manager.getCurrentValue(0));
  Serial.println(" °C");
}

void temperatureNormal() {
  digitalWrite(LED_PIN, LOW);
  Serial.println("Suhu kembali normal");
}

void setup() {
  Serial.begin(9600);
  pinMode(LED_PIN, OUTPUT);
  
  // Tambahkan monitor suhu
  manager.addThresholdMonitor(
    "TempMonitor",                // Nama monitor
    readTemperature,              // Fungsi pembacaan
    TEMP_THRESHOLD,               // Nilai threshold (28°C)
    StateActionManager::TRIG_RISING,      // Trigger ketika naik di atas threshold
    temperatureAlert,             // Fungsi yang dijalankan
    1                             // Jalankan 1 kali
  );
  
  // Tambahkan monitor untuk mendeteksi suhu kembali normal
  manager.addThresholdMonitor(
    "TempNormal",
    readTemperature,
    TEMP_THRESHOLD - 2.0,         // Threshold lebih rendah (hysteresis)
    StateActionManager::TRIG_FALLING,     // Trigger ketika turun di bawah threshold
    temperatureNormal,            // Fungsi yang dijalankan
    1                             // Jalankan 1 kali
  );
  
  Serial.println("Monitoring suhu dimulai");
  Serial.print("Threshold: ");
  Serial.print(TEMP_THRESHOLD);
  Serial.println(" °C");
}

void loop() {
  manager.update();
  
  // Tampilkan suhu setiap 2 detik
  static unsigned long lastPrintTime = 0;
  unsigned long currentTime = millis();
  if (currentTime - lastPrintTime > 2000) {
    float temp = readTemperature();
    Serial.print("Suhu saat ini: ");
    Serial.print(temp);
    Serial.println(" °C");
    lastPrintTime = currentTime;
  }
  
  delay(100);
}
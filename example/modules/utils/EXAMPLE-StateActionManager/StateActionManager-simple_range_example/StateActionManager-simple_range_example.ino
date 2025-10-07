#include <Arduino.h>
#include "StateActionManager.h"

StateActionManager manager;

const int HUMID_SENSOR_PIN = A0;
const int RED_LED_PIN = 5;
const int GREEN_LED_PIN = 6;

float readHumidity() {
  int sensorValue = analogRead(HUMID_SENSOR_PIN);
  return map(sensorValue, 0, 1023, 0, 100); // 0-100%
}

void humidityOptimal() {
  digitalWrite(RED_LED_PIN, LOW);
  digitalWrite(GREEN_LED_PIN, HIGH);
  Serial.println("Kelembaban optimal (40-60%)");
}

void humidityOutOfRange() {
  digitalWrite(RED_LED_PIN, HIGH);
  digitalWrite(GREEN_LED_PIN, LOW);
  Serial.print("Kelembaban di luar range: ");
  Serial.print(manager.getCurrentValue(0));
  Serial.println("%");
}

void setup() {
  Serial.begin(9600);
  pinMode(RED_LED_PIN, OUTPUT);
  pinMode(GREEN_LED_PIN, OUTPUT);
  
  // Monitor kelembaban dalam range optimal 40-60%
  manager.addRangeMonitor(
    "HumidityRange",              // Nama monitor
    readHumidity,                 // Fungsi pembacaan
    40.0, 60.0,                   // Range optimal (40-60%)
    StateActionManager::TRIG_RISING,     // Trigger ketika masuk ke dalam range
    humidityOptimal,              // Fungsi yang dijalankan
    1                             // Jalankan 1 kali
  );
  
  // Monitor kelembaban di luar range optimal
  manager.addRangeMonitor(
    "HumidityOutOfRange",
    readHumidity,
    40.0, 60.0,                   // Range yang sama (40-60%)
    StateActionManager::TRIG_FALLING,    // Trigger ketika keluar dari range
    humidityOutOfRange,           // Fungsi yang dijalankan
    1                             // Jalankan 1 kali
  );
  
  Serial.println("Monitoring kelembaban dimulai");
  Serial.println("Range optimal: 40-60%");
}

void loop() {
  manager.update();
  
  // Tampilkan kelembaban setiap 2 detik
  static unsigned long lastPrintTime = 0;
  if (millis() - lastPrintTime > 2000) {
    float humidity = readHumidity();
    Serial.print("Kelembaban saat ini: ");
    Serial.print(humidity);
    Serial.println("%");
    lastPrintTime = millis();
  }
  
  delay(100);
}
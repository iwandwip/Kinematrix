#define ENABLE_MODULE_STATE_ACTION_MANAGER
#include "Kinematrix.h"

StateActionManager manager;

const int TEMP_SENSOR_PIN = A0;
const int FAN_RELAY_PIN = 7;
const int HEATER_RELAY_PIN = 8;
const int LED_ALERT_PIN = 13;

float readTemperature() {
  int sensorValue = analogRead(TEMP_SENSOR_PIN);
  float voltage = sensorValue * (5.0 / 1023.0);
  float temperature = (voltage - 0.5) * 100.0; // LM35 sensor
  return temperature;
}

void fanOn() {
  digitalWrite(FAN_RELAY_PIN, HIGH);
  Serial.println("Fan turned ON");
}

void fanOff() {
  digitalWrite(FAN_RELAY_PIN, LOW);
  Serial.println("Fan turned OFF");
}

void heaterOn() {
  digitalWrite(HEATER_RELAY_PIN, HIGH);
  Serial.println("Heater turned ON");
}

void heaterOff() {
  digitalWrite(HEATER_RELAY_PIN, LOW);
  Serial.println("Heater turned OFF");
}

void blinkAlert(void* param) {
  static bool ledState = false;
  ledState = !ledState;
  digitalWrite(LED_ALERT_PIN, ledState);
  
  // Parameter digunakan untuk jumlah derajat melewati batas
  float* excessTemp = (float*)param;
  Serial.print("Temperature exceeded by ");
  Serial.print(*excessTemp);
  Serial.println(" degrees!");
}

void setup() {
  Serial.begin(9600);
  pinMode(FAN_RELAY_PIN, OUTPUT);
  pinMode(HEATER_RELAY_PIN, OUTPUT);
  pinMode(LED_ALERT_PIN, OUTPUT);
  
  static float excessTempValue = 0.0;
  
  // 1. Monitor suhu tinggi - nyalakan kipas saat suhu di atas 28°C
  int highTempMonitorId = manager.addThresholdMonitor(
    "HighTemp",                // Nama monitor
    readTemperature,           // Fungsi pembacaan 
    28.0,                      // Nilai threshold
    StateActionManager::TRIG_RISING,  // Trigger ketika naik melewati threshold
    fanOn,                     // Fungsi yang dijalankan
    1                          // Dijalankan 1 kali
  );
  
  // 2. Monitor suhu kembali normal - matikan kipas saat suhu di bawah 26°C
  manager.addThresholdMonitor(
    "HighTempOff", 
    readTemperature,
    26.0,                      // Threshold sedikit lebih rendah (hysteresis)
    StateActionManager::TRIG_FALLING,
    fanOff,
    1
  );
  
  // 3. Monitor suhu rendah - nyalakan pemanas saat suhu di bawah 18°C
  int lowTempMonitorId = manager.addThresholdMonitor(
    "LowTemp",
    readTemperature,
    18.0,
    StateActionManager::TRIG_FALLING,
    heaterOn,
    1
  );
  
  // 4. Monitor suhu kembali normal - matikan pemanas saat suhu di atas 20°C
  manager.addThresholdMonitor(
    "LowTempOff",
    readTemperature,
    20.0,                      // Threshold sedikit lebih tinggi (hysteresis)
    StateActionManager::TRIG_RISING,
    heaterOff,
    1
  );
  
  // 5. Monitor suhu kritis dengan parameter - lebih dari 35°C
  manager.addThresholdMonitor(
    "CriticalTemp",
    []() {
      float temp = readTemperature();
      if (temp > 35.0) {
        excessTempValue = temp - 35.0;
      }
      return temp;
    },
    35.0,
    StateActionManager::TRIG_RISING,
    blinkAlert,               // Fungsi dengan parameter
    &excessTempValue,         // Parameter untuk fungsi
    10,                       // Jalankan 10 kali
    500                       // Interval 500ms (alarm cepat)
  );
  
  // Contoh pengaturan tambahan
  manager.setThreshold(highTempMonitorId, 29.0);    // Ubah threshold
  manager.setEnabled(lowTempMonitorId, false);      // Matikan sementara
  
  Serial.println("Temperature monitoring system started");
  Serial.println("High threshold: 28°C (fan), Low threshold: 18°C (heater)");
}

void loop() {
  manager.update();
  
  static unsigned long lastPrintTime = 0;
  unsigned long currentTime = millis();
  
  // Tampilkan suhu setiap 2 detik
  if (currentTime - lastPrintTime > 2000) {
    float temp = readTemperature();
    Serial.print("Current temperature: ");
    Serial.print(temp);
    Serial.println(" °C");
    
    lastPrintTime = currentTime;
  }
  
  delay(100);
}
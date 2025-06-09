#define ENABLE_MODULE_STATE_ACTION_MANAGER
#include "Kinematrix.h"

StateActionManager manager;

const int LIGHT_SENSOR_PIN = A0;
const int MOTION_SENSOR_PIN = A1;
const int SOUND_SENSOR_PIN = A2;
const int BATTERY_VOLTAGE_PIN = A3;

const int ALERT_LED_PIN = 13;
const int SECURITY_RELAY_PIN = 7;
const int BUZZER_PIN = 8;
const int STATUS_LED_R_PIN = 9;
const int STATUS_LED_G_PIN = 10;
const int STATUS_LED_B_PIN = 11;

float readLightLevel() {
  int sensorValue = analogRead(LIGHT_SENSOR_PIN);
  return map(sensorValue, 0, 1023, 0, 100); // 0-100%
}

float readMotionLevel() {
  int sensorValue = analogRead(MOTION_SENSOR_PIN);
  return map(sensorValue, 0, 1023, 0, 100); // 0-100%
}

float readSoundLevel() {
  int sensorValue = analogRead(SOUND_SENSOR_PIN);
  return map(sensorValue, 0, 1023, 30, 120); // 30-120 dB
}

float readBatteryVoltage() {
  int sensorValue = analogRead(BATTERY_VOLTAGE_PIN);
  return sensorValue * (5.0 / 1023.0) * 3.0; // Voltage divider (3x)
}

void lightChangeDetected() {
  digitalWrite(ALERT_LED_PIN, HIGH);
  Serial.println("Significant light change detected!");
  delay(100);
  digitalWrite(ALERT_LED_PIN, LOW);
}

void motionAlert() {
  digitalWrite(SECURITY_RELAY_PIN, HIGH);
  Serial.println("Motion detected in secured area!");
  delay(3000); // Keep alarm on for 3 seconds
  digitalWrite(SECURITY_RELAY_PIN, LOW);
}

void soundAlert() {
  digitalWrite(BUZZER_PIN, HIGH);
  Serial.println("Unusual sound level detected!");
  delay(500);
  digitalWrite(BUZZER_PIN, LOW);
}

void batteryAlert(void* param) {
  float* voltagePtr = (float*)param;
  
  Serial.print("Battery voltage changed by ");
  Serial.print(*voltagePtr);
  Serial.println("V!");
  
  if (*voltagePtr < 0) {
    // Voltage dropping
    analogWrite(STATUS_LED_R_PIN, 255);
    analogWrite(STATUS_LED_G_PIN, 0);
    analogWrite(STATUS_LED_B_PIN, 0);
    Serial.println("WARNING: Battery voltage dropping!");
  } else {
    // Voltage increasing
    analogWrite(STATUS_LED_R_PIN, 0);
    analogWrite(STATUS_LED_G_PIN, 255);
    analogWrite(STATUS_LED_B_PIN, 0);
    Serial.println("Battery voltage increasing (charging)");
  }
}

void setup() {
  Serial.begin(9600);
  
  pinMode(ALERT_LED_PIN, OUTPUT);
  pinMode(SECURITY_RELAY_PIN, OUTPUT);
  pinMode(BUZZER_PIN, OUTPUT);
  pinMode(STATUS_LED_R_PIN, OUTPUT);
  pinMode(STATUS_LED_G_PIN, OUTPUT);
  pinMode(STATUS_LED_B_PIN, OUTPUT);
  
  static float voltageDelta = 0.0;
  
  // 1. Monitor perubahan cahaya mendadak (>20%)
  int lightChangeId = manager.addChangeMonitor(
    "LightChange",                // Nama monitor
    readLightLevel,               // Fungsi pembacaan
    20.0,                         // Minimal perubahan 20%
    lightChangeDetected,          // Fungsi yang dijalankan
    1                             // Jalankan 1 kali
  );
  
  // 2. Monitor perubahan gerakan (>15%)
  int motionChangeId = manager.addChangeMonitor(
    "MotionChange",
    readMotionLevel,
    15.0,                         // Minimal perubahan 15%
    motionAlert,
    1
  );
  
  // 3. Monitor perubahan level suara (>10dB)
  int soundChangeId = manager.addChangeMonitor(
    "SoundChange",
    readSoundLevel,
    10.0,                         // Minimal perubahan 10dB
    soundAlert,
    3,                            // Jalankan 3 kali
    1000                          // Interval 1 detik
  );
  
  // 4. Monitor perubahan tegangan baterai dengan parameter
  int batteryChangeId = manager.addChangeMonitor(
    "BatteryChange",
    []() {
      static float lastVoltage = 0;
      float currentVoltage = readBatteryVoltage();
      voltageDelta = currentVoltage - lastVoltage;
      lastVoltage = currentVoltage;
      return currentVoltage;
    },
    0.2,                          // Minimal perubahan 0.2V
    batteryAlert,                 // Fungsi dengan parameter
    &voltageDelta,                // Parameter (delta tegangan)
    1
  );
  
  // Pengaturan tambahan
  manager.setRepeatCount(soundChangeId, 5); // Ubah jumlah eksekusi
  
  Serial.println("Change monitoring system started");
  Serial.println("Monitoring: Light (20%), Motion (15%), Sound (10dB), Battery (0.2V)");
}

void loop() {
  manager.update();
  
  static unsigned long lastPrintTime = 0;
  unsigned long currentTime = millis();
  
  // Tampilkan pembacaan sensor setiap 2 detik
  if (currentTime - lastPrintTime > 2000) {
    Serial.print("Light: ");
    Serial.print(readLightLevel());
    Serial.print("%, Motion: ");
    Serial.print(readMotionLevel());
    Serial.print("%, Sound: ");
    Serial.print(readSoundLevel());
    Serial.print("dB, Battery: ");
    Serial.print(readBatteryVoltage());
    Serial.println("V");
    
    lastPrintTime = currentTime;
  }
  
  // Simulasi perubahan sensor untuk demo
  static unsigned long lastSimTime = 0;
  if (currentTime - lastSimTime > 10000) {
    Serial.println("Simulating sensor change...");
    
    // Pilih sensor acak untuk disimulasikan
    int sensor = random(4);
    switch (sensor) {
      case 0:
        // Simulate light change
        analogWrite(A0, random(0, 1023));
        break;
      case 1:
        // Simulate motion
        analogWrite(A1, random(800, 1023));
        break;
      case 2:
        // Simulate sound
        analogWrite(A2, random(800, 1023));
        break;
      case 3:
        // Simulate battery change
        analogWrite(A3, random(0, 1023));
        break;
    }
    
    lastSimTime = currentTime;
  }
  
  delay(50);
}
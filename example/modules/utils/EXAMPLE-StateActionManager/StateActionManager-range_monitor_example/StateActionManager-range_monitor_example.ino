#define ENABLE_MODULE_STATE_ACTION_MANAGER
#include "Kinematrix.h"

StateActionManager manager;

const int HUMID_SENSOR_PIN = A0;
const int LIGHT_SENSOR_PIN = A1;
const int PRESSURE_SENSOR_PIN = A2;

const int HUMIDIFIER_PIN = 5;
const int DEHUMIDIFIER_PIN = 6;
const int LIGHT_RELAY_PIN = 7;
const int RGB_RED_PIN = 9;
const int RGB_GREEN_PIN = 10;
const int RGB_BLUE_PIN = 11;
const int BUZZER_PIN = 12;

float readHumidity() {
  int sensorValue = analogRead(HUMID_SENSOR_PIN);
  return map(sensorValue, 0, 1023, 0, 100); // 0-100%
}

float readLightLevel() {
  int sensorValue = analogRead(LIGHT_SENSOR_PIN);
  return map(sensorValue, 0, 1023, 0, 1000); // 0-1000 lux
}

float readPressure() {
  int sensorValue = analogRead(PRESSURE_SENSOR_PIN);
  return map(sensorValue, 0, 1023, 900, 1100); // 900-1100 hPa
}

void humidifierOn() {
  digitalWrite(HUMIDIFIER_PIN, HIGH);
  Serial.println("Humidifier ON");
}

void humidifierOff() {
  digitalWrite(HUMIDIFIER_PIN, LOW);
  Serial.println("Humidifier OFF");
}

void dehumidifierOn() {
  digitalWrite(DEHUMIDIFIER_PIN, HIGH);
  Serial.println("Dehumidifier ON");
}

void dehumidifierOff() {
  digitalWrite(DEHUMIDIFIER_PIN, LOW);
  Serial.println("Dehumidifier OFF");
}

void lightingAdjust() {
  digitalWrite(LIGHT_RELAY_PIN, HIGH);
  Serial.println("Lights adjusted for optimal range");
}

void setRgbLed(byte r, byte g, byte b) {
  analogWrite(RGB_RED_PIN, r);
  analogWrite(RGB_GREEN_PIN, g);
  analogWrite(RGB_BLUE_PIN, b);
}

void alertNormalRange() {
  setRgbLed(0, 255, 0); // Green - normal
  Serial.println("Environment parameters in normal range");
}

void alertOutOfRange() {
  setRgbLed(255, 0, 0); // Red - warning
  Serial.println("WARNING: Environment parameters out of acceptable range");
}

void soundPressureAlert(void* param) {
  static bool buzzerState = false;
  buzzerState = !buzzerState;
  digitalWrite(BUZZER_PIN, buzzerState);
  
  bool* isHigh = (bool*)param;
  if (*isHigh) {
    Serial.println("High pressure alert!");
  } else {
    Serial.println("Low pressure alert!");
  }
}

void setup() {
  Serial.begin(9600);
  
  pinMode(HUMIDIFIER_PIN, OUTPUT);
  pinMode(DEHUMIDIFIER_PIN, OUTPUT);
  pinMode(LIGHT_RELAY_PIN, OUTPUT);
  pinMode(RGB_RED_PIN, OUTPUT);
  pinMode(RGB_GREEN_PIN, OUTPUT);
  pinMode(RGB_BLUE_PIN, OUTPUT);
  pinMode(BUZZER_PIN, OUTPUT);
  
  static bool isHighPressure = true;
  
  // 1. Range monitor untuk kelembaban optimal (40-60%)
  int humidityRangeId = manager.addRangeMonitor(
    "HumidityRange",               // Nama monitor
    readHumidity,                  // Fungsi pembacaan
    40.0, 60.0,                    // Range optimal: 40-60%
    StateActionManager::TRIG_BOTH_WAYS,  // Trigger ketika masuk/keluar range
    []() {
      if (manager.getCurrentValue(0) < 40.0) {
        humidifierOn();            // Terlalu kering
        dehumidifierOff();
      } else if (manager.getCurrentValue(0) > 60.0) {
        dehumidifierOn();          // Terlalu lembab
        humidifierOff();
      } else {
        humidifierOff();           // Kelembaban optimal
        dehumidifierOff();
      }
    },
    1                              // Jalankan 1 kali
  );
  
  // 2. Range monitor untuk pencahayaan optimal (300-700 lux)
  int lightRangeId = manager.addRangeMonitor(
    "LightRange",
    readLightLevel,
    300.0, 700.0,                  // Range optimal: 300-700 lux
    StateActionManager::TRIG_RISING,     // Trigger hanya ketika masuk ke range
    lightingAdjust,
    1
  );
  
  // 3. Range monitor untuk tekanan udara dengan callback kondisional
  int pressureRangeId = manager.addRangeMonitor(
    "PressureRange",
    readPressure,
    980.0, 1030.0,                 // Range optimal: 980-1030 hPa
    StateActionManager::TRIG_BOTH_WAYS,
    []() {
      float pressure = manager.getCurrentValue(2);
      if (pressure >= 980.0 && pressure <= 1030.0) {
        alertNormalRange();
      } else {
        alertOutOfRange();
      }
    },
    1
  );
  
  // 4. Range monitor dengan parameter untuk tekanan ekstrem
  manager.addRangeMonitor(
    "PressureExtreme",
    readPressure,
    950.0, 1050.0,                // Range yang lebih luas
    StateActionManager::TRIG_FALLING,    // Trigger ketika keluar range
    [](void* param) {
      float pressure = manager.getCurrentValue(3);
      bool* isHighPtr = (bool*)param;
      
      if (pressure > 1050.0) {
        *isHighPtr = true;         // Tekanan sangat tinggi
      } else if (pressure < 950.0) {
        *isHighPtr = false;        // Tekanan sangat rendah
      }
      
      soundPressureAlert(param);
    },
    &isHighPressure,               // Parameter untuk callback
    5,                             // Jalankan 5 kali
    500                            // Interval 500ms
  );
  
  // Contoh pengaturan tambahan
  manager.setRange(humidityRangeId, 35.0, 65.0);  // Perluas range yang diterima
  
  Serial.println("Environment monitoring system started");
  Serial.println("Optimal Humidity: 40-60%, Light: 300-700 lux, Pressure: 980-1030 hPa");
}

void loop() {
  manager.update();
  
  static unsigned long lastPrintTime = 0;
  unsigned long currentTime = millis();
  
  // Tampilkan pembacaan sensor setiap 3 detik
  if (currentTime - lastPrintTime > 3000) {
    Serial.print("Humidity: ");
    Serial.print(readHumidity());
    Serial.print("%, Light: ");
    Serial.print(readLightLevel());
    Serial.print(" lux, Pressure: ");
    Serial.print(readPressure());
    Serial.println(" hPa");
    
    lastPrintTime = currentTime;
  }
  
  delay(100);
}
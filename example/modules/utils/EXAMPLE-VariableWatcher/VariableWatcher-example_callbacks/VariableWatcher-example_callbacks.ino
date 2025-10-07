#include <Arduino.h>
#define ENABLE_MODULE_VARIABLE_WATCHER
#include "Kinematrix.h"

VariableWatcher watcher;

float temperature = 25.0;
int motionDetected = 0;
bool lightOn = false;
int fanSpeed = 0;
float humidity = 50.0;

// Callback untuk suhu
void onTemperatureChange(void* value) {
  float temp = *(float*)value;
  
  // Kontrol fan berdasarkan suhu
  if (temp > 28.0) {
    fanSpeed = map(constrain(temp, 28.0, 35.0), 28.0, 35.0, 1, 5);
  } else {
    fanSpeed = 0;
  }
  
  // Pesan peringatan untuk suhu ekstrim
  if (temp > 30.0) {
    Serial.println("WARNING: High temperature detected!");
  } else if (temp < 10.0) {
    Serial.println("WARNING: Low temperature detected!");
  }
}

// Callback untuk deteksi gerakan
void onMotionChange(void* value) {
  int motion = *(int*)value;
  
  if (motion > 0) {
    // Nyalakan lampu ketika gerakan terdeteksi
    lightOn = true;
    
    // Perbarui timestamp deteksi terakhir
    Serial.println("Motion detected! Light turned ON");
  }
}

// Callback untuk status lampu
void onLightChange(void* value) {
  bool light = *(bool*)value;
  
  // Aktifkan pin LED sesuai status lampu
  digitalWrite(LED_BUILTIN, light ? HIGH : LOW);
  
  Serial.print("Light is now ");
  Serial.println(light ? "ON" : "OFF");
}

// Callback untuk kecepatan fan
void onFanSpeedChange(void* value) {
  int speed = *(int*)value;
  
  if (speed > 0) {
    Serial.print("Fan activated at speed ");
    Serial.println(speed);
  } else {
    Serial.println("Fan turned OFF");
  }
  
  // Simulasi kontrol fan dengan nilai PWM
  // analogWrite(FAN_PIN, speed * 51); // 0-5 => 0-255
}

// Callback untuk humidity
void onHumidityChange(void* value) {
  float hum = *(float*)value;
  
  if (hum < 30.0) {
    Serial.println("WARNING: Humidity too low!");
  } else if (hum > 70.0) {
    Serial.println("WARNING: Humidity too high!");
  }
}

void setup() {
  Serial.begin(9600);
  pinMode(LED_BUILTIN, OUTPUT);
  
  // Tambahkan variabel beserta callback functions
  watcher.addVariable(&temperature, "Temperature", onTemperatureChange);
  watcher.addVariable(&motionDetected, "Motion", onMotionChange);
  watcher.addVariable(&lightOn, "Light", onLightChange);
  watcher.addVariable(&fanSpeed, "FanSpeed", onFanSpeedChange);
  watcher.addVariable(&humidity, "Humidity", onHumidityChange);
  
  Serial.println("Callbacks Example Started");
}

void loop() {
  static unsigned long lastMotionReset = 0;
  unsigned long currentMillis = millis();
  
  // Simulasi pembacaan suhu & kelembaban
  temperature = 25.0 + sin(currentMillis / 10000.0) * 8.0;
  humidity = 50.0 + cos(currentMillis / 8000.0) * 30.0;
  
  // Simulasi deteksi gerakan acak
  if (random(100) < 10) {
    motionDetected = random(1, 10); // Kekuatan deteksi (1-10)
    lastMotionReset = currentMillis;
  }
  
  // Reset deteksi gerakan dan matikan lampu setelah 5 detik
  if (motionDetected > 0 && currentMillis - lastMotionReset > 5000) {
    motionDetected = 0;
    
    if (lightOn) {
      lightOn = false;
      Serial.println("Light turned OFF (timeout)");
    }
  }
  
  // Periksa perubahan pada semua variabel
  watcher.checkAll();
  
  // Semuanya ditangani oleh callbacks, tidak perlu kode tambahan di sini
  
  delay(100);
}
#define ENABLE_MODULE_STATE_ACTION_MANAGER
#include "Kinematrix.h"

StateActionManager manager;

const int TEMP_SENSOR_PIN = A0;
const int BUTTON_PIN = 2;
const int LED_PIN = 13;
const int RELAY_PIN = 7;

// ID monitor
int tempMonitorId = -1;
int blinkerId = -1;

float readTemperature() {
  int sensorValue = analogRead(TEMP_SENSOR_PIN);
  float voltage = sensorValue * (5.0 / 1023.0);
  float temperature = voltage * 100.0;
  return temperature;
}

void temperatureAlert() {
  digitalWrite(RELAY_PIN, HIGH);
  Serial.print("Suhu tinggi: ");
  Serial.print(manager.getCurrentValue(tempMonitorId));
  Serial.println(" °C");
}

void blinkLed() {
  static bool ledState = false;
  ledState = !ledState;
  digitalWrite(LED_PIN, ledState);
}

void setup() {
  Serial.begin(9600);
  pinMode(LED_PIN, OUTPUT);
  pinMode(RELAY_PIN, OUTPUT);
  pinMode(BUTTON_PIN, INPUT_PULLUP);
  
  // Buat monitor suhu
  tempMonitorId = manager.addThresholdMonitor(
    "Temperature",
    readTemperature,
    30.0,
    StateActionManager::TRIG_RISING,
    temperatureAlert,
    1
  );
  
  // Buat LED blinker
  blinkerId = manager.addTimeBasedMonitor(
    "LedBlinker",
    500,
    blinkLed,
    0
  );
  
  Serial.println("Contoh metode kontrol dimulai");
  Serial.println("Tekan tombol untuk mengontrol monitor");
}

void loop() {
  manager.update();
  
  // Contoh penggunaan tombol untuk mengontrol monitor
  static bool lastButtonState = true;
  bool buttonState = digitalRead(BUTTON_PIN);
  
  // Deteksi tepi turun (tombol ditekan)
  if (buttonState == LOW && lastButtonState == HIGH) {
    static int controlMode = 0;
    controlMode = (controlMode + 1) % 6;
    
    switch (controlMode) {
      case 0:
        // 1. resetMonitor - Reset monitor suhu
        Serial.println("Mereset monitor suhu");
        manager.resetMonitor(tempMonitorId);
        break;
        
      case 1:
        // 2. setEnabled - Menonaktifkan monitor suhu
        Serial.println("Menonaktifkan monitor suhu");
        manager.setEnabled(tempMonitorId, false);
        break;
        
      case 2:
        // 3. setEnabled - Mengaktifkan monitor suhu
        Serial.println("Mengaktifkan monitor suhu");
        manager.setEnabled(tempMonitorId, true);
        break;
        
      case 3:
        // 4. setThreshold - Mengubah threshold suhu
        Serial.println("Mengubah threshold suhu ke 25°C");
        manager.setThreshold(tempMonitorId, 25.0);
        break;
        
      case 4:
        // 5. pauseAction - Menjeda LED blinker
        Serial.println("Menjeda LED blinker");
        manager.pauseAction(blinkerId);
        break;
        
      case 5:
        // 6. resumeAction - Melanjutkan LED blinker
        Serial.println("Melanjutkan LED blinker");
        manager.resumeAction(blinkerId);
        break;
    }
    
    // Tampilkan status monitor
    Serial.println("\n=== Status Monitor ===");
    manager.printStatus(tempMonitorId);
    manager.printStatus(blinkerId);
    Serial.println("====================\n");
  }
  
  lastButtonState = buttonState;
  
  // Tampilkan suhu setiap 3 detik
  static unsigned long lastPrintTime = 0;
  if (millis() - lastPrintTime > 3000) {
    float temp = readTemperature();
    Serial.print("Suhu saat ini: ");
    Serial.print(temp);
    Serial.println(" °C");
    lastPrintTime = millis();
  }
  
  // Reset relay
  static unsigned long relayOffTime = 0;
  if (digitalRead(RELAY_PIN) == HIGH) {
    if (millis() - relayOffTime > 1000) {
      digitalWrite(RELAY_PIN, LOW);
      Serial.println("Relay dimatikan");
    }
  } else {
    relayOffTime = millis();
  }
  
  delay(50);
}
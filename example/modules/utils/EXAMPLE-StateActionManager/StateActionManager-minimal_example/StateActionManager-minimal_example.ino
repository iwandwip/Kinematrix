#define ENABLE_MODULE_STATE_ACTION_MANAGER
#include "Kinematrix.h"

StateActionManager manager;

// Nilai default
float currentTemp = 25.0;
float threshold = 28.0;

// Monitor ID
int tempMonitorId = -1;

// Fungsi untuk membaca nilai suhu simulasi
float readTemperature() {
  return currentTemp;
}

// Callback saat threshold terlewati
void thresholdCrossed() {
  Serial.println(">> Suhu di atas threshold!");
}

void thresholdNormal() {
  Serial.println(">> Suhu kembali normal (di bawah threshold)");
}

void setup() {
  Serial.begin(9600);
  
  // Siapkan monitor threshold
  tempMonitorId = manager.addThresholdMonitor(
    "TempMonitor",
    readTemperature,
    threshold,
    StateActionManager::TRIG_RISING,
    thresholdCrossed,
    1
  );
  
  // Monitor untuk saat suhu kembali normal
  manager.addThresholdMonitor(
    "TempNormal",
    readTemperature,
    threshold,
    StateActionManager::TRIG_FALLING,
    thresholdNormal,
    1
  );
  
  Serial.println("Monitor Threshold Suhu");
  Serial.println("Masukkan nilai suhu (misal: 30)");
  Serial.println("Threshold saat ini: " + String(threshold) + " C");
}

void loop() {
  // Update StateActionManager
  manager.update();
  
  // Baca input dari Serial
  if (Serial.available()) {
    String input = Serial.readStringUntil('\n');
    input.trim();
    
    // Coba parse nilai
    float value = input.toFloat();
    
    // Update suhu
    currentTemp = value;
    
    // Tampilkan informasi
    Serial.print("Suhu: ");
    Serial.print(currentTemp);
    Serial.println(" C");
  }
  
  // Jeda kecil untuk stabilitas
  delay(50);
}
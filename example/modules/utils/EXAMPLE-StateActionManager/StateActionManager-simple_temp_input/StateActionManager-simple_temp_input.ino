#define ENABLE_MODULE_STATE_ACTION_MANAGER
#include "Kinematrix.h"

StateActionManager manager;

// Pin output
const int LED_PIN = 13;
const int FAN_RELAY_PIN = 8;

// Nilai threshold
const float DEFAULT_THRESHOLD = 28.0;

// Variabel simulasi sensor
float simulatedTemp = 25.0;  // Suhu awal dalam °C

// ID monitor
int tempHighMonitorId = -1;

// Fungsi untuk membaca suhu simulasi
float readTemperature() {
  return simulatedTemp;
}

// Fungsi yang dijalankan ketika suhu terlalu tinggi
void tempHighAlert() {
  digitalWrite(LED_PIN, HIGH);
  digitalWrite(FAN_RELAY_PIN, HIGH);
  Serial.print("*** ALERT: Suhu tinggi (");
  Serial.print(simulatedTemp);
  Serial.println("°C)! Kipas dinyalakan ***");
}

// Fungsi yang dijalankan ketika suhu kembali normal
void tempNormal() {
  digitalWrite(LED_PIN, LOW);
  digitalWrite(FAN_RELAY_PIN, LOW);
  Serial.print("Suhu normal (");
  Serial.print(simulatedTemp);
  Serial.println("°C). Kipas dimatikan");
}

void setup() {
  Serial.begin(9600);
  pinMode(LED_PIN, OUTPUT);
  pinMode(FAN_RELAY_PIN, OUTPUT);
  
  // Siapkan monitor suhu
  tempHighMonitorId = manager.addThresholdMonitor(
    "Temperature",            // Nama
    readTemperature,          // Fungsi pembacaan
    DEFAULT_THRESHOLD,        // Threshold
    StateActionManager::TRIG_RISING,  // Trigger saat melewati ke atas
    tempHighAlert,            // Fungsi saat suhu tinggi
    1                         // Jalankan 1 kali
  );
  
  // Monitor untuk deteksi suhu kembali normal
  manager.addThresholdMonitor(
    "TempNormal",
    readTemperature,
    DEFAULT_THRESHOLD - 2.0,  // Sedikit lebih rendah (hysteresis)
    StateActionManager::TRIG_FALLING, // Trigger saat turun ke bawah
    tempNormal,
    1
  );
  
  Serial.println("\n=== MONITOR SUHU SEDERHANA ===");
  Serial.println("Masukkan nilai suhu langsung di Serial Monitor.");
  Serial.println("Contoh: ketik '30' untuk mengatur suhu ke 30°C");
  Serial.println();
  Serial.println("Perintah khusus:");
  Serial.println("  'threshold XX' - Mengubah threshold (contoh: threshold 30)");
  Serial.println("  'status' - Menampilkan status monitor");
  Serial.println("===================================");
  
  Serial.print("Suhu awal: ");
  Serial.print(simulatedTemp);
  Serial.println("°C");
  
  Serial.print("Threshold: ");
  Serial.print(DEFAULT_THRESHOLD);
  Serial.println("°C");
  
  Serial.println("\nSilakan masukkan nilai suhu:");
}

void loop() {
  // Update manager
  manager.update();
  
  // Proses input dari Serial
  if (Serial.available()) {
    String input = Serial.readStringUntil('\n');
    input.trim();
    
    // Periksa apakah input adalah perintah khusus
    if (input.startsWith("threshold ")) {
      // Parse nilai threshold baru
      float newThreshold = input.substring(10).toFloat();
      if (newThreshold > 0) {
        manager.setThreshold(tempHighMonitorId, newThreshold);
        // Update juga threshold untuk monitor normal
        for (int i = 0; i < 10; i++) { // Cek beberapa monitor pertama
          if (manager.getMonitorName(i) == "TempNormal") {
            manager.setThreshold(i, newThreshold - 2.0);
            break;
          }
        }
        Serial.print("Threshold diubah ke ");
        Serial.print(newThreshold);
        Serial.println("°C");
      } else {
        Serial.println("Nilai threshold tidak valid.");
      }
    }
    else if (input == "status") {
      Serial.println("\n--- Status Monitor ---");
      Serial.print("Suhu saat ini: ");
      Serial.print(simulatedTemp);
      Serial.println("°C");
      
      // Tampilkan status threshold
      float currentThreshold = 0;
      for (int i = 0; i < 10; i++) {
        if (manager.getMonitorName(i) == "Temperature") {
          // Coba dapatkan threshold secara tidak langsung
          float currentValue = manager.getCurrentValue(i);
          bool isAbove = currentValue > manager.getCurrentValue(i);
          Serial.print("Threshold: sekitar ");
          Serial.print(DEFAULT_THRESHOLD); // Ini perkiraan, tidak selalu akurat
          Serial.println("°C");
          Serial.print("Status: ");
          Serial.println(isAbove ? "DI ATAS THRESHOLD" : "NORMAL");
          break;
        }
      }
      
      Serial.print("Kipas: ");
      Serial.println(digitalRead(FAN_RELAY_PIN) == HIGH ? "AKTIF" : "MATI");
      Serial.println("--------------------");
    }
    else {
      // Coba parse input sebagai nilai suhu
      float newTemp = input.toFloat();
      if (newTemp != 0 || input == "0") { // Check if conversion was successful
        simulatedTemp = newTemp;
        Serial.print("> Suhu diubah ke ");
        Serial.print(simulatedTemp);
        Serial.println("°C");
      } else {
        Serial.println("Input tidak valid. Masukkan nilai suhu numerik atau perintah khusus.");
      }
    }
  }
  
  // Jeda kecil untuk stabilitas
  delay(50);
}
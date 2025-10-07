#define ENABLE_MODULE_STATE_ACTION_MANAGER
#include "Kinematrix.h"

StateActionManager manager;

// Pin output
const int LED_PIN = 13;

// Variabel simulasi sensor
float simulatedTemp = 25.0;  // Suhu awal dalam °C

// ID monitor
int tempMonitorId = -1;

// Fungsi untuk membaca suhu simulasi
float readTemperature() {
  return simulatedTemp;
}

// Fungsi yang dijalankan ketika suhu terlalu tinggi
void tempHighAlert() {
  digitalWrite(LED_PIN, HIGH);
  Serial.print("ALERT: Suhu tinggi (");
  Serial.print(simulatedTemp);
  Serial.println("°C)!");
}

// Fungsi yang dijalankan ketika suhu kembali normal
void tempNormal() {
  digitalWrite(LED_PIN, LOW);
  Serial.println("INFO: Suhu kembali normal");
}

// Memproses perintah dari Serial
void processSerialCommand(String command) {
  command.trim();
  
  // Bantuan
  if (command == "help" || command == "?") {
    Serial.println("\n--- Perintah yang tersedia ---");
    Serial.println("T:25.5    - Set suhu ke 25.5°C");
    Serial.println("S:30      - Set threshold suhu ke 30°C");
    Serial.println("status    - Tampilkan status monitor");
    Serial.println("reset     - Reset monitor suhu");
    Serial.println("help/?    - Tampilkan bantuan");
    Serial.println("---------------------------");
    return;
  }
  
  // Status
  if (command == "status") {
    Serial.print("Suhu saat ini: ");
    Serial.print(simulatedTemp);
    Serial.println("°C");
    
    Serial.print("Threshold: ");
    Serial.print(manager.getCurrentValue(tempMonitorId) > 28.0 ? "TERLEWATI" : "NORMAL");
    Serial.println();
    
    manager.printStatus(tempMonitorId);
    return;
  }
  
  // Reset
  if (command == "reset") {
    manager.resetMonitor(tempMonitorId);
    Serial.println("Monitor suhu direset");
    return;
  }
  
  // Set suhu
  if (command.startsWith("T:")) {
    String valueStr = command.substring(2);
    simulatedTemp = valueStr.toFloat();
    
    Serial.print("Suhu diubah ke ");
    Serial.print(simulatedTemp);
    Serial.println("°C");
    return;
  }
  
  // Set threshold
  if (command.startsWith("S:")) {
    String valueStr = command.substring(2);
    float newThreshold = valueStr.toFloat();
    
    manager.setThreshold(tempMonitorId, newThreshold);
    Serial.print("Threshold suhu diubah ke ");
    Serial.print(newThreshold);
    Serial.println("°C");
    return;
  }
  
  // Perintah tidak dikenal
  Serial.println("Perintah tidak dikenal. Ketik 'help' untuk bantuan.");
}

void setup() {
  Serial.begin(9600);
  pinMode(LED_PIN, OUTPUT);
  
  // Siapkan monitor suhu (threshold = 28°C)
  tempMonitorId = manager.addThresholdMonitor(
    "Temperature",            // Nama
    readTemperature,          // Fungsi pembacaan
    28.0,                     // Threshold
    StateActionManager::TRIG_RISING,  // Trigger saat melewati ke atas
    tempHighAlert,            // Fungsi saat suhu tinggi
    1                         // Jalankan 1 kali
  );
  
  // Monitor untuk deteksi suhu kembali normal
  manager.addThresholdMonitor(
    "TempNormal",
    readTemperature,
    26.0,                     // Sedikit lebih rendah (hysteresis)
    StateActionManager::TRIG_FALLING, // Trigger saat turun ke bawah
    tempNormal,
    1
  );
  
  Serial.println("\n=== Monitor Suhu Sederhana ===");
  Serial.println("Masukkan perintah melalui Serial.");
  Serial.println("Contoh: T:30 (set suhu ke 30°C)");
  Serial.println("Ketik 'help' untuk bantuan");
  Serial.println("=============================\n");
  
  Serial.print("Suhu awal: ");
  Serial.print(simulatedTemp);
  Serial.println("°C");
  
  Serial.print("Threshold: ");
  Serial.print(28.0);
  Serial.println("°C");
}

void loop() {
  // Update manager
  manager.update();
  
  // Proses perintah dari Serial
  if (Serial.available()) {
    String command = Serial.readStringUntil('\n');
    processSerialCommand(command);
  }
  
  // Jeda kecil untuk stabilitas
  delay(50);
}
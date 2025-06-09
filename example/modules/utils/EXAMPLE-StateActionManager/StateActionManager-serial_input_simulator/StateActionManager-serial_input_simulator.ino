#define ENABLE_MODULE_STATE_ACTION_MANAGER
#include "Kinematrix.h"

StateActionManager manager;

// Pin output
const int TEMP_LED_PIN = 5;
const int HUMID_LED_PIN = 6;
const int LIGHT_LED_PIN = 7;
const int MOTION_RELAY_PIN = 8;
const int BUZZER_PIN = 9;

// Variabel untuk nilai sensor simulasi
float simulatedTemp = 25.0;      // Suhu awal (°C)
float simulatedHumidity = 50.0;  // Kelembaban awal (%)
float simulatedLight = 60.0;     // Level cahaya awal (%)
float simulatedMotion = 0.0;     // Tidak ada gerakan awal

// ID monitor
int tempMonitorId = -1;
int humidityRangeId = -1;
int lightChangeId = -1;
int motionMonitorId = -1;
int statusReportId = -1;

// Fungsi pembacaan simulasi
float readTemperature() {
  return simulatedTemp;
}

float readHumidity() {
  return simulatedHumidity;
}

float readLightLevel() {
  return simulatedLight;
}

float checkMotion() {
  return simulatedMotion;
}

// Fungsi aksi
void tempAlert() {
  digitalWrite(TEMP_LED_PIN, HIGH);
  tone(BUZZER_PIN, 1000, 200);
  Serial.print("ALERT: Suhu tinggi ");
  Serial.print(simulatedTemp);
  Serial.println("°C");
}

void tempNormal() {
  digitalWrite(TEMP_LED_PIN, LOW);
  Serial.println("INFO: Suhu kembali normal");
}

void humidityInRange() {
  digitalWrite(HUMID_LED_PIN, HIGH);
  Serial.println("INFO: Kelembaban dalam range optimal (40-60%)");
}

void humidityOutOfRange() {
  digitalWrite(HUMID_LED_PIN, LOW);
  tone(BUZZER_PIN, 800, 100);
  Serial.print("ALERT: Kelembaban di luar range optimal: ");
  Serial.print(simulatedHumidity);
  Serial.println("%");
}

void significantLightChange() {
  digitalWrite(LIGHT_LED_PIN, HIGH);
  tone(BUZZER_PIN, 1200, 100);
  Serial.print("ALERT: Perubahan cahaya signifikan terdeteksi: ");
  Serial.print(simulatedLight);
  Serial.println("%");
  delay(500);
  digitalWrite(LIGHT_LED_PIN, LOW);
}

void motionDetected() {
  digitalWrite(MOTION_RELAY_PIN, HIGH);
  tone(BUZZER_PIN, 2000, 500);
  Serial.println("ALERT: Gerakan terdeteksi!");
}

void motionStopped() {
  digitalWrite(MOTION_RELAY_PIN, LOW);
  Serial.println("INFO: Gerakan berhenti");
}

void printSystemStatus() {
  Serial.println("\n=== STATUS SISTEM ===");
  Serial.print("Suhu: ");
  Serial.print(simulatedTemp);
  Serial.println("°C");
  
  Serial.print("Kelembaban: ");
  Serial.print(simulatedHumidity);
  Serial.println("%");
  
  Serial.print("Level Cahaya: ");
  Serial.print(simulatedLight);
  Serial.println("%");
  
  Serial.print("Gerakan: ");
  Serial.println(simulatedMotion > 0.5 ? "TERDETEKSI" : "TIDAK ADA");
  
  Serial.println("===================\n");
}

void processSerialCommand(String command) {
  command.trim();
  
  if (command.length() < 2) {
    if (command == "S" || command == "s") {
      // Tampilkan status semua monitor
      manager.printAllStatus();
      return;
    }
    
    if (command == "H" || command == "h") {
      // Tampilkan bantuan
      printHelp();
      return;
    }
    
    Serial.println("Perintah tidak valid. Ketik 'H' untuk bantuan.");
    return;
  }
  
  char cmd = command.charAt(0);
  String value = command.substring(2);
  
  switch (cmd) {
    case 'T': case 't':  // Set suhu
      simulatedTemp = value.toFloat();
      Serial.print("Suhu diatur ke ");
      Serial.print(simulatedTemp);
      Serial.println("°C");
      break;
      
    case 'H': case 'h':  // Set kelembaban
      simulatedHumidity = value.toFloat();
      Serial.print("Kelembaban diatur ke ");
      Serial.print(simulatedHumidity);
      Serial.println("%");
      break;
      
    case 'L': case 'l':  // Set level cahaya
      simulatedLight = value.toFloat();
      Serial.print("Level cahaya diatur ke ");
      Serial.print(simulatedLight);
      Serial.println("%");
      break;
      
    case 'M': case 'm':  // Set gerakan
      simulatedMotion = value.toFloat();
      Serial.println(simulatedMotion > 0.5 ? "Gerakan terdeteksi" : "Tidak ada gerakan");
      break;
      
    case 'S': case 's':  // Pengaturan tambahan
      if (command.startsWith("ST:")) {  // Set threshold suhu
        float newThreshold = value.toFloat();
        manager.setThreshold(tempMonitorId, newThreshold);
        Serial.print("Threshold suhu diubah ke ");
        Serial.print(newThreshold);
        Serial.println("°C");
      }
      else if (command.startsWith("SL:")) {  // Set min perubahan cahaya
        lightChangeId = manager.addChangeMonitor(
          "LightChange",
          readLightLevel,
          value.toFloat(),
          significantLightChange,
          1
        );
        Serial.print("Min perubahan cahaya diubah ke ");
        Serial.print(value.toFloat());
        Serial.println("%");
      }
      break;
      
    case 'E': case 'e':  // Enable/disable monitor
      {
        int separator = value.indexOf(':');
        if (separator > 0) {
          int monitorId = value.substring(0, separator).toInt();
          bool enabled = value.substring(separator + 1).toInt() > 0;
          
          manager.setEnabled(monitorId, enabled);
          Serial.print("Monitor #");
          Serial.print(monitorId);
          Serial.println(enabled ? " diaktifkan" : " dinonaktifkan");
        }
      }
      break;
      
    case 'R': case 'r':  // Reset monitor
      {
        int monitorId = value.toInt();
        manager.resetMonitor(monitorId);
        Serial.print("Monitor #");
        Serial.print(monitorId);
        Serial.println(" direset");
      }
      break;
      
    default:
      Serial.println("Perintah tidak dikenal. Ketik 'H' untuk bantuan.");
      break;
  }
}

void printHelp() {
  Serial.println("\n=== BANTUAN SIMULATOR SENSOR ===");
  Serial.println("Format perintah: X:nilai");
  Serial.println("  T:25.5    - Set suhu ke 25.5°C");
  Serial.println("  H:45      - Set kelembaban ke 45%");
  Serial.println("  L:75      - Set level cahaya ke 75%");
  Serial.println("  M:1       - Simulasi gerakan terdeteksi");
  Serial.println("  M:0       - Simulasi tidak ada gerakan");
  Serial.println("  ST:30     - Set threshold suhu ke 30°C");
  Serial.println("  SL:15     - Set min perubahan cahaya ke 15%");
  Serial.println("  E:1:0     - Nonaktifkan monitor dengan ID 1");
  Serial.println("  E:1:1     - Aktifkan monitor dengan ID 1");
  Serial.println("  R:1       - Reset monitor dengan ID 1");
  Serial.println("  S         - Tampilkan status semua monitor");
  Serial.println("  H         - Tampilkan pesan bantuan ini");
  Serial.println("\nID Monitor:");
  Serial.print("  Suhu: ");
  Serial.println(tempMonitorId);
  Serial.print("  Kelembaban: ");
  Serial.println(humidityRangeId);
  Serial.print("  Cahaya: ");
  Serial.println(lightChangeId);
  Serial.print("  Gerakan: ");
  Serial.println(motionMonitorId);
  Serial.println("==============================\n");
}

void setup() {
  Serial.begin(9600);
  
  pinMode(TEMP_LED_PIN, OUTPUT);
  pinMode(HUMID_LED_PIN, OUTPUT);
  pinMode(LIGHT_LED_PIN, OUTPUT);
  pinMode(MOTION_RELAY_PIN, OUTPUT);
  pinMode(BUZZER_PIN, OUTPUT);
  
  // Siapkan monitor suhu
  tempMonitorId = manager.addThresholdMonitor(
    "Temperature",
    readTemperature,
    28.0,
    StateActionManager::TRIG_RISING,
    tempAlert,
    1
  );
  
  manager.addThresholdMonitor(
    "TempNormal",
    readTemperature,
    26.0,
    StateActionManager::TRIG_FALLING,
    tempNormal,
    1
  );
  
  // Siapkan monitor kelembaban
  humidityRangeId = manager.addRangeMonitor(
    "HumidityRange",
    readHumidity,
    40.0, 60.0,
    StateActionManager::TRIG_BOTH_WAYS,
    []() {
      if (simulatedHumidity >= 40.0 && simulatedHumidity <= 60.0) {
        humidityInRange();
      } else {
        humidityOutOfRange();
      }
    },
    1
  );
  
  // Siapkan monitor perubahan cahaya
  lightChangeId = manager.addChangeMonitor(
    "LightChange",
    readLightLevel,
    20.0,
    significantLightChange,
    1
  );
  
  // Siapkan monitor gerakan
  motionMonitorId = manager.addThresholdMonitor(
    "Motion",
    checkMotion,
    0.5,
    StateActionManager::TRIG_RISING,
    motionDetected,
    1
  );
  
  manager.addThresholdMonitor(
    "MotionStopped",
    checkMotion,
    0.5,
    StateActionManager::TRIG_FALLING,
    motionStopped,
    1
  );
  
  // Status reporter setiap 10 detik
  statusReportId = manager.addTimeBasedMonitor(
    "StatusReport",
    10000,
    printSystemStatus,
    0
  );
  
  Serial.println("=== SIMULATOR SENSOR MELALUI SERIAL ===");
  Serial.println("Sistem monitoring dimulai");
  Serial.println("Kirim 'H' untuk melihat perintah yang tersedia.");
  Serial.println("Nilai awal: T=25°C, H=50%, L=60%, M=0");
  Serial.println("=========================================");
}

void loop() {
  manager.update();
  
  // Menangani input dari Serial
  if (Serial.available()) {
    String command = Serial.readStringUntil('\n');
    processSerialCommand(command);
  }
  
  // Reset output yang perlu disetel ulang
  static unsigned long lastResetTime = 0;
  if (millis() - lastResetTime > 1000) {
    if (digitalRead(LIGHT_LED_PIN) == HIGH) {
      digitalWrite(LIGHT_LED_PIN, LOW);
    }
    lastResetTime = millis();
  }
  
  delay(50);
}
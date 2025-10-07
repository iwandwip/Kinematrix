#define ENABLE_MODULE_STATE_ACTION_MANAGER
#include "Kinematrix.h"

StateActionManager manager;

// Definisi pin
const int RELAY1_PIN = 5;   // Relay untuk kipas/pendingin
const int RELAY2_PIN = 6;   // Relay untuk pemanas
const int RELAY3_PIN = 7;   // Relay untuk humidifier
const int LED_PIN = 13;     // LED status
const int BUZZER_PIN = 9;   // Buzzer untuk alert

// Mode sistem
enum SystemMode {
  MODE_AUTO,
  MODE_MANUAL,
  MODE_ECO,
  MODE_AWAY
};

// Variabel global
SystemMode currentMode = MODE_AUTO;
float currentTemperature = 25.0;
float currentHumidity = 50.0;
float currentPressure = 1013.0;
float currentLight = 50.0;
unsigned long lastReportTime = 0;

// ID Monitor
int tempHighMonitorId = -1;
int tempLowMonitorId = -1;
int humidityMonitorId = -1;
int pressureMonitorId = -1;
int lightChangeMonitorId = -1;
int modeSwitcherId = -1;

// Fungsi pembacaan sensor (simulasi)
float readTemperature() {
  return currentTemperature;
}

float readHumidity() {
  return currentHumidity;
}

float readPressure() {
  return currentPressure;
}

float readLight() {
  return currentLight;
}

// Fungsi aksi
void coolingOn() {
  digitalWrite(RELAY1_PIN, HIGH);
  digitalWrite(RELAY2_PIN, LOW);
  Serial.println(">> Pendingin aktif");
}

void coolingOff() {
  digitalWrite(RELAY1_PIN, LOW);
  Serial.println(">> Pendingin mati");
}

void heatingOn() {
  digitalWrite(RELAY2_PIN, HIGH);
  digitalWrite(RELAY1_PIN, LOW);
  Serial.println(">> Pemanas aktif");
}

void heatingOff() {
  digitalWrite(RELAY2_PIN, LOW);
  Serial.println(">> Pemanas mati");
}

void humidifierControl() {
  if (currentHumidity < 40.0) {
    digitalWrite(RELAY3_PIN, HIGH);
    Serial.println(">> Humidifier aktif");
  } else if (currentHumidity > 60.0) {
    digitalWrite(RELAY3_PIN, LOW);
    Serial.println(">> Humidifier mati");
  }
}

void alertPressureChange() {
  tone(BUZZER_PIN, 2000, 200);
  Serial.println(">> ALERT: Perubahan tekanan signifikan!");
}

void alertLightChange() {
  digitalWrite(LED_PIN, HIGH);
  tone(BUZZER_PIN, 1500, 200);
  Serial.println(">> ALERT: Perubahan cahaya signifikan!");
  delay(200);
  digitalWrite(LED_PIN, LOW);
}

void reportStatus() {
  Serial.println("\n=== STATUS SISTEM ===");
  Serial.print("Mode: ");
  switch (currentMode) {
    case MODE_AUTO: Serial.println("AUTO"); break;
    case MODE_MANUAL: Serial.println("MANUAL"); break;
    case MODE_ECO: Serial.println("ECO"); break;
    case MODE_AWAY: Serial.println("AWAY"); break;
  }
  Serial.print("Suhu: ");
  Serial.print(currentTemperature);
  Serial.println(" °C");
  Serial.print("Kelembaban: ");
  Serial.print(currentHumidity);
  Serial.println(" %");
  Serial.print("Tekanan: ");
  Serial.print(currentPressure);
  Serial.println(" hPa");
  Serial.print("Cahaya: ");
  Serial.print(currentLight);
  Serial.println(" %");
  
  Serial.print("Pendingin: ");
  Serial.println(digitalRead(RELAY1_PIN) == HIGH ? "AKTIF" : "MATI");
  Serial.print("Pemanas: ");
  Serial.println(digitalRead(RELAY2_PIN) == HIGH ? "AKTIF" : "MATI");
  Serial.print("Humidifier: ");
  Serial.println(digitalRead(RELAY3_PIN) == HIGH ? "AKTIF" : "MATI");
  Serial.println("===================\n");
}

void switchToMode(SystemMode newMode) {
  currentMode = newMode;
  
  switch (newMode) {
    case MODE_AUTO:
      Serial.println("\n>> Beralih ke mode AUTO");
      manager.setThreshold(tempHighMonitorId, 28.0);
      manager.setThreshold(tempLowMonitorId, 18.0);
      manager.setEnabled(tempHighMonitorId, true);
      manager.setEnabled(tempLowMonitorId, true);
      manager.setEnabled(humidityMonitorId, true);
      break;
      
    case MODE_ECO:
      Serial.println("\n>> Beralih ke mode ECO");
      manager.setThreshold(tempHighMonitorId, 30.0);  // Lebih toleran terhadap panas
      manager.setThreshold(tempLowMonitorId, 16.0);   // Lebih toleran terhadap dingin
      manager.setEnabled(tempHighMonitorId, true);
      manager.setEnabled(tempLowMonitorId, true);
      manager.setEnabled(humidityMonitorId, true);
      break;
      
    case MODE_AWAY:
      Serial.println("\n>> Beralih ke mode AWAY");
      // Matikan semua kecuali monitoring cahaya untuk keamanan
      manager.setEnabled(tempHighMonitorId, false);
      manager.setEnabled(tempLowMonitorId, false);
      manager.setEnabled(humidityMonitorId, false);
      manager.setEnabled(lightChangeMonitorId, true);
      coolingOff();
      heatingOff();
      digitalWrite(RELAY3_PIN, LOW);
      break;
      
    case MODE_MANUAL:
      Serial.println("\n>> Beralih ke mode MANUAL");
      // Matikan otomatisasi, kontrol melalui perintah serial
      manager.setEnabled(tempHighMonitorId, false);
      manager.setEnabled(tempLowMonitorId, false);
      manager.setEnabled(humidityMonitorId, false);
      break;
  }
  
  reportStatus();
}

void processCommand(String command) {
  command.trim();
  
  // Cek perintah bantuan
  if (command == "?") {
    printHelp();
    return;
  }
  
  // Cek perintah report
  if (command == "STATUS" || command == "status") {
    reportStatus();
    return;
  }
  
  // Cek perintah monitor
  if (command == "MONITOR" || command == "monitor") {
    manager.printAllStatus();
    return;
  }
  
  // Cek perintah mode
  if (command == "AUTO" || command == "auto") {
    switchToMode(MODE_AUTO);
    return;
  }
  
  if (command == "ECO" || command == "eco") {
    switchToMode(MODE_ECO);
    return;
  }
  
  if (command == "AWAY" || command == "away") {
    switchToMode(MODE_AWAY);
    return;
  }
  
  if (command == "MANUAL" || command == "manual") {
    switchToMode(MODE_MANUAL);
    return;
  }
  
  // Cek perintah kontrol perangkat
  if (command == "COOL ON" || command == "cool on") {
    if (currentMode == MODE_MANUAL) {
      coolingOn();
    } else {
      Serial.println(">> GAGAL: Hanya tersedia dalam mode MANUAL");
    }
    return;
  }
  
  if (command == "COOL OFF" || command == "cool off") {
    if (currentMode == MODE_MANUAL) {
      coolingOff();
    } else {
      Serial.println(">> GAGAL: Hanya tersedia dalam mode MANUAL");
    }
    return;
  }
  
  if (command == "HEAT ON" || command == "heat on") {
    if (currentMode == MODE_MANUAL) {
      heatingOn();
    } else {
      Serial.println(">> GAGAL: Hanya tersedia dalam mode MANUAL");
    }
    return;
  }
  
  if (command == "HEAT OFF" || command == "heat off") {
    if (currentMode == MODE_MANUAL) {
      heatingOff();
    } else {
      Serial.println(">> GAGAL: Hanya tersedia dalam mode MANUAL");
    }
    return;
  }
  
  // Cek perintah input sensor
  if (command.startsWith("TEMP ") || command.startsWith("temp ")) {
    float newTemp = command.substring(5).toFloat();
    currentTemperature = newTemp;
    Serial.print(">> Suhu diatur ke ");
    Serial.print(newTemp);
    Serial.println(" °C");
    return;
  }
  
  if (command.startsWith("HUMID ") || command.startsWith("humid ")) {
    float newHumid = command.substring(6).toFloat();
    currentHumidity = newHumid;
    Serial.print(">> Kelembaban diatur ke ");
    Serial.print(newHumid);
    Serial.println(" %");
    return;
  }
  
  if (command.startsWith("PRESS ") || command.startsWith("press ")) {
    float newPress = command.substring(6).toFloat();
    currentPressure = newPress;
    Serial.print(">> Tekanan diatur ke ");
    Serial.print(newPress);
    Serial.println(" hPa");
    return;
  }
  
  if (command.startsWith("LIGHT ") || command.startsWith("light ")) {
    float newLight = command.substring(6).toFloat();
    currentLight = newLight;
    Serial.print(">> Level cahaya diatur ke ");
    Serial.print(newLight);
    Serial.println(" %");
    return;
  }
  
  // Cek perintah threshold
  if (command.startsWith("SET HIGH ") || command.startsWith("set high ")) {
    float newThreshold = command.substring(9).toFloat();
    manager.setThreshold(tempHighMonitorId, newThreshold);
    Serial.print(">> Threshold suhu tinggi diatur ke ");
    Serial.print(newThreshold);
    Serial.println(" °C");
    return;
  }
  
  if (command.startsWith("SET LOW ") || command.startsWith("set low ")) {
    float newThreshold = command.substring(8).toFloat();
    manager.setThreshold(tempLowMonitorId, newThreshold);
    Serial.print(">> Threshold suhu rendah diatur ke ");
    Serial.print(newThreshold);
    Serial.println(" °C");
    return;
  }
  
  // Jika tidak ada yang cocok
  Serial.println(">> Perintah tidak dikenal. Ketik '?' untuk bantuan.");
}

void printHelp() {
  Serial.println("\n=== BANTUAN PERINTAH SERIAL ===");
  Serial.println("Perintah Mode Sistem:");
  Serial.println("  AUTO     - Beralih ke mode otomatis");
  Serial.println("  ECO      - Beralih ke mode ekonomis");
  Serial.println("  AWAY     - Beralih ke mode tidak di rumah");
  Serial.println("  MANUAL   - Beralih ke mode manual");
  
  Serial.println("\nPerintah Kontrol (hanya mode MANUAL):");
  Serial.println("  COOL ON  - Aktifkan pendingin");
  Serial.println("  COOL OFF - Matikan pendingin");
  Serial.println("  HEAT ON  - Aktifkan pemanas");
  Serial.println("  HEAT OFF - Matikan pemanas");
  
  Serial.println("\nInput Sensor Simulasi:");
  Serial.println("  TEMP 25.5  - Set suhu ke 25.5°C");
  Serial.println("  HUMID 45   - Set kelembaban ke 45%");
  Serial.println("  PRESS 1013 - Set tekanan ke 1013 hPa");
  Serial.println("  LIGHT 75   - Set level cahaya ke 75%");
  
  Serial.println("\nPeintah Pengaturan:");
  Serial.println("  SET HIGH 28 - Atur threshold suhu tinggi");
  Serial.println("  SET LOW 18  - Atur threshold suhu rendah");
  
  Serial.println("\nPerintah Lainnya:");
  Serial.println("  STATUS  - Tampilkan status sistem");
  Serial.println("  MONITOR - Tampilkan status monitor");
  Serial.println("  ?       - Tampilkan bantuan ini");
  Serial.println("===============================\n");
}

void setup() {
  Serial.begin(9600);
  
  pinMode(RELAY1_PIN, OUTPUT);
  pinMode(RELAY2_PIN, OUTPUT);
  pinMode(RELAY3_PIN, OUTPUT);
  pinMode(LED_PIN, OUTPUT);
  pinMode(BUZZER_PIN, OUTPUT);
  
  // Inisialisasi monitor suhu tinggi
  tempHighMonitorId = manager.addThresholdMonitor(
    "TempHigh",
    readTemperature,
    28.0,
    StateActionManager::TRIG_RISING,
    coolingOn,
    1
  );
  
  manager.addThresholdMonitor(
    "TempHighOff",
    readTemperature,
    26.0,
    StateActionManager::TRIG_FALLING,
    coolingOff,
    1
  );
  
  // Inisialisasi monitor suhu rendah
  tempLowMonitorId = manager.addThresholdMonitor(
    "TempLow",
    readTemperature,
    18.0,
    StateActionManager::TRIG_FALLING,
    heatingOn,
    1
  );
  
  manager.addThresholdMonitor(
    "TempLowOff",
    readTemperature,
    20.0,
    StateActionManager::TRIG_RISING,
    heatingOff,
    1
  );
  
  // Inisialisasi monitor kelembaban
  humidityMonitorId = manager.addRangeMonitor(
    "HumidityRange",
    readHumidity,
    40.0, 60.0,
    StateActionManager::TRIG_BOTH_WAYS,
    humidifierControl,
    1
  );
  
  // Inisialisasi monitor perubahan tekanan
  pressureMonitorId = manager.addChangeMonitor(
    "PressureChange",
    readPressure,
    5.0,
    alertPressureChange,
    1
  );
  
  // Inisialisasi monitor perubahan cahaya
  lightChangeMonitorId = manager.addChangeMonitor(
    "LightChange",
    readLight,
    25.0,
    alertLightChange,
    1
  );
  
  // Monitor pelaporan status
  manager.addTimeBasedMonitor(
    "StatusReport",
    30000,
    reportStatus,
    0
  );
  
  Serial.println("\n========================================");
  Serial.println("SISTEM KONTROL MULTI-SENSOR");
  Serial.println("Ketik '?' untuk melihat daftar perintah");
  Serial.println("========================================\n");
  
  reportStatus();
}

void loop() {
  manager.update();
  
  // Tangani input serial
  if (Serial.available()) {
    String command = Serial.readStringUntil('\n');
    processCommand(command);
  }
  
  // Laporkan status setiap 30 detik
  unsigned long currentTime = millis();
  if (currentTime - lastReportTime > 30000) {
    reportStatus();
    lastReportTime = currentTime;
  }
  
  delay(50);
}
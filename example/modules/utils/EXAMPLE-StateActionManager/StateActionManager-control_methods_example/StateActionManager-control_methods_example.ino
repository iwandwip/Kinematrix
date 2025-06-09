#define ENABLE_MODULE_STATE_ACTION_MANAGER
#include "Kinematrix.h"

StateActionManager manager;

const int LED_R_PIN = 9;
const int LED_G_PIN = 10;
const int LED_B_PIN = 11;
const int RELAY_1_PIN = 5;
const int RELAY_2_PIN = 6;
const int BUTTON_PIN = 2;
const int BUZZER_PIN = 8;

// Monitor IDs
int temperatureMonitorId = -1;
int humidityMonitorId = -1;
int motionMonitorId = -1;
int ledBlinkerId = -1;
int maintenanceId = -1;

// Simulasi fungsi pembacaan sensor
float readTemperature() {
  return 20.0 + (millis() % 3000) / 100.0; // 20-50°C simulasi
}

float readHumidity() {
  return 40.0 + (millis() % 2000) / 100.0; // 40-60% simulasi
}

float readMotion() {
  return random(100) < 10 ? 1.0 : 0.0; // 10% kemungkinan mendeteksi gerakan
}

// Fungsi aksi
void tempAlarm() {
  digitalWrite(RELAY_1_PIN, HIGH);
  tone(BUZZER_PIN, 1000, 200);
  Serial.println("Temperature alarm triggered!");
}

void humidityAlert() {
  digitalWrite(LED_R_PIN, 255);
  digitalWrite(LED_G_PIN, 0);
  digitalWrite(LED_B_PIN, 0);
  Serial.println("Humidity outside optimal range!");
}

void motionDetected() {
  digitalWrite(RELAY_2_PIN, HIGH);
  Serial.println("Motion detected!");
}

void blinkLed() {
  static bool ledState = false;
  ledState = !ledState;
  digitalWrite(LED_G_PIN, ledState ? 255 : 0);
}

void maintenanceTask() {
  Serial.println("Performing maintenance task...");
  digitalWrite(LED_B_PIN, 255);
  delay(1000);
  digitalWrite(LED_B_PIN, 0);
}

void setup() {
  Serial.begin(9600);
  
  pinMode(LED_R_PIN, OUTPUT);
  pinMode(LED_G_PIN, OUTPUT);
  pinMode(LED_B_PIN, OUTPUT);
  pinMode(RELAY_1_PIN, OUTPUT);
  pinMode(RELAY_2_PIN, OUTPUT);
  pinMode(BUTTON_PIN, INPUT_PULLUP);
  pinMode(BUZZER_PIN, OUTPUT);
  
  Serial.println("Setting up monitors...");
  
  // Pengaturan monitor dasar
  temperatureMonitorId = manager.addThresholdMonitor("Temperature", readTemperature, 
                                                   30.0, StateActionManager::TRIG_RISING, 
                                                   tempAlarm, 3, 1000);
  
  humidityMonitorId = manager.addRangeMonitor("Humidity", readHumidity, 
                                            45.0, 55.0, StateActionManager::TRIG_FALLING, 
                                            humidityAlert);
  
  motionMonitorId = manager.addThresholdMonitor("Motion", readMotion, 
                                              0.5, StateActionManager::TRIG_RISING, 
                                              motionDetected);
  
  ledBlinkerId = manager.addTimeBasedMonitor("LedBlinker", 500, blinkLed, 0);
  
  maintenanceId = manager.addTimeBasedMonitor("Maintenance", 60000, maintenanceTask, 0);
  
  Serial.println("Control Methods Demo Started");
  printCommands();
}

void printCommands() {
  Serial.println("\n--- Available Commands ---");
  Serial.println("1: Reset temperature monitor");
  Serial.println("2: Disable humidity monitor");
  Serial.println("3: Enable humidity monitor");
  Serial.println("4: Set temperature threshold to 35°C");
  Serial.println("5: Set humidity range to 40-60%");
  Serial.println("6: Pause LED blinker");
  Serial.println("7: Resume LED blinker");
  Serial.println("8: Stop maintenance task");
  Serial.println("9: Print status of all monitors");
  Serial.println("0: Print status of temperature monitor");
  Serial.println("------------------------\n");
}

void loop() {
  manager.update();
  
  // Menangani input seri untuk demo metode kontrol
  if (Serial.available()) {
    char command = Serial.read();
    
    switch (command) {
      case '1':
        // 1. resetMonitor() - Reset monitor ke kondisi awal
        Serial.println("Resetting temperature monitor");
        manager.resetMonitor(temperatureMonitorId);
        break;
        
      case '2':
        // 2. setEnabled() - Menonaktifkan monitor
        Serial.println("Disabling humidity monitor");
        manager.setEnabled(humidityMonitorId, false);
        digitalWrite(LED_R_PIN, 0); // Matikan LED
        break;
        
      case '3':
        // 3. setEnabled() - Mengaktifkan monitor
        Serial.println("Enabling humidity monitor");
        manager.setEnabled(humidityMonitorId, true);
        break;
        
      case '4':
        // 4. setThreshold() - Mengubah nilai threshold
        Serial.println("Setting temperature threshold to 35°C");
        manager.setThreshold(temperatureMonitorId, 35.0);
        break;
        
      case '5':
        // 5. setRange() - Mengubah range monitor
        Serial.println("Setting humidity range to 40-60%");
        manager.setRange(humidityMonitorId, 40.0, 60.0);
        break;
        
      case '6':
        // 6. pauseAction() - Menjeda aksi yang sedang berjalan
        Serial.println("Pausing LED blinker");
        manager.pauseAction(ledBlinkerId);
        break;
        
      case '7':
        // 7. resumeAction() - Melanjutkan aksi yang dijeda
        Serial.println("Resuming LED blinker");
        manager.resumeAction(ledBlinkerId);
        break;
        
      case '8':
        // 8. stopAction() - Menghentikan aksi yang sedang berjalan
        Serial.println("Stopping maintenance task");
        manager.stopAction(maintenanceId);
        break;
        
      case '9':
        // 9. printAllStatus() - Mencetak status semua monitor
        manager.printAllStatus();
        break;
        
      case '0':
        // 10. printStatus() - Mencetak status monitor tertentu
        manager.printStatus(temperatureMonitorId);
        break;
        
      case 'h':
        printCommands();
        break;
    }
  }
  
  // Simulasi berbagai pembacaan sensor untuk demo
  static unsigned long lastSimulation = 0;
  if (millis() - lastSimulation > 5000) {
    // Simulasikan suhu tinggi setiap 5 detik
    Serial.println("Simulating high temperature event...");
    // Di-comment, karena readTemperature() sudah melakukan simulasi berbasis waktu
    
    lastSimulation = millis();
  }
  
  // Pemeriksaan status monitor
  static unsigned long lastStatusCheck = 0;
  if (millis() - lastStatusCheck > 3000) {
    Serial.print("Temperature: ");
    Serial.print(readTemperature());
    Serial.print("°C, Humidity: ");
    Serial.print(readHumidity());
    Serial.println("%");
    
    Serial.print("Temperature monitor state: ");
    switch (manager.getMonitorState(temperatureMonitorId)) {
      case StateActionManager::IDLE: Serial.println("IDLE"); break;
      case StateActionManager::TRIGGERED: Serial.println("TRIGGERED"); break;
      case StateActionManager::EXECUTING: Serial.println("EXECUTING"); break;
      case StateActionManager::COMPLETED: Serial.println("COMPLETED"); break;
      case StateActionManager::PAUSED: Serial.println("PAUSED"); break;
    }
    
    if (manager.isActionRunning(temperatureMonitorId)) {
      Serial.print("Temperature action is running, execution count: ");
      Serial.println(manager.getExecutionCount(temperatureMonitorId));
    }
    
    lastStatusCheck = millis();
  }
  
  // Reset relay setelah 2 detik
  static unsigned long lastRelayReset = 0;
  if (millis() - lastRelayReset > 2000) {
    digitalWrite(RELAY_1_PIN, LOW);
    digitalWrite(RELAY_2_PIN, LOW);
    lastRelayReset = millis();
  }
  
  delay(50);
}
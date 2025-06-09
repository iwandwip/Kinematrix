#define ENABLE_MODULE_STATE_ACTION_MANAGER
#include "Kinematrix.h"

StateActionManager manager;

const int LED_PIN = 13;
const int PUMP_RELAY_PIN = 7;
const int HEATER_RELAY_PIN = 8;
const int DISPLAY_CS_PIN = 9;
const int BUZZER_PIN = 10;

unsigned long startTime = 0;
int runningHours = 0;
int runningMinutes = 0;
int runningSeconds = 0;

void blinkLed() {
  static bool ledState = false;
  ledState = !ledState;
  digitalWrite(LED_PIN, ledState);
}

void waterPumpCycle() {
  Serial.println("Starting water pump cycle");
  digitalWrite(PUMP_RELAY_PIN, HIGH);
  delay(3000);  // Pump active for 3 seconds
  digitalWrite(PUMP_RELAY_PIN, LOW);
  Serial.println("Water pump cycle completed");
}

void heaterMaintenance() {
  Serial.println("Heater maintenance cycle");
  digitalWrite(HEATER_RELAY_PIN, HIGH);
  delay(2000);
  digitalWrite(HEATER_RELAY_PIN, LOW);
  Serial.println("Heater maintenance completed");
}

void updateRuntime() {
  runningSeconds++;
  
  if (runningSeconds >= 60) {
    runningSeconds = 0;
    runningMinutes++;
    
    if (runningMinutes >= 60) {
      runningMinutes = 0;
      runningHours++;
    }
  }
  
  Serial.print("System runtime: ");
  Serial.print(runningHours);
  Serial.print(":");
  if (runningMinutes < 10) Serial.print("0");
  Serial.print(runningMinutes);
  Serial.print(":");
  if (runningSeconds < 10) Serial.print("0");
  Serial.println(runningSeconds);
}

void displayStatus() {
  Serial.println("\n----- System Status -----");
  Serial.print("Runtime: ");
  Serial.print(runningHours);
  Serial.print(":");
  if (runningMinutes < 10) Serial.print("0");
  Serial.print(runningMinutes);
  Serial.print(":");
  if (runningSeconds < 10) Serial.print("0");
  Serial.println(runningSeconds);
  
  Serial.print("Next pump cycle in: ");
  int pumpMonitorId = 1; // Assuming pump monitor is ID 1
  unsigned long timeSinceLastRun = millis() - manager.getLastTriggerTime(pumpMonitorId);
  unsigned long timeToNextRun = (manager.getLastTriggerTime(pumpMonitorId) > 0) ? 
                              (3600000 - timeSinceLastRun) : 0; // 1 hour in milliseconds
  Serial.print(timeToNextRun / 60000); // Convert to minutes
  Serial.println(" minutes");
  
  Serial.println("-------------------------\n");
}

void dailyMaintenance() {
  Serial.println("Performing daily maintenance routines");
  
  // Simulate a series of maintenance tasks
  Serial.println("1. System diagnostics");
  delay(1000);
  Serial.println("2. Cleaning cycle");
  delay(1000);
  Serial.println("3. Calibration check");
  delay(1000);
  
  Serial.println("Daily maintenance completed");
  
  // Sound buzzer to indicate completion
  digitalWrite(BUZZER_PIN, HIGH);
  delay(500);
  digitalWrite(BUZZER_PIN, LOW);
}

void setup() {
  Serial.begin(9600);
  
  pinMode(LED_PIN, OUTPUT);
  pinMode(PUMP_RELAY_PIN, OUTPUT);
  pinMode(HEATER_RELAY_PIN, OUTPUT);
  pinMode(DISPLAY_CS_PIN, OUTPUT);
  pinMode(BUZZER_PIN, OUTPUT);
  
  startTime = millis();
  
  // 1. LED blink setiap 500ms (tanpa batas)
  int ledBlinkId = manager.addTimeBasedMonitor(
    "LedBlink",                  // Nama monitor
    500,                         // Interval 500ms
    blinkLed,                    // Fungsi yang dijalankan
    0                            // 0 = tanpa batas pengulangan
  );
  
  // 2. Siklus pompa air setiap 1 jam
  int pumpCycleId = manager.addTimeBasedMonitor(
    "WaterPumpCycle",
    3600000,                     // 1 jam = 3600000ms
    waterPumpCycle,
    0                            // Jalankan tanpa batas
  );
  
  // 3. Pemeliharaan pemanas setiap 2 jam
  int heaterMaintenanceId = manager.addTimeBasedMonitor(
    "HeaterMaintenance",
    7200000,                     // 2 jam = 7200000ms
    heaterMaintenance,
    0                            // Jalankan tanpa batas
  );
  
  // 4. Runtime counter setiap detik
  int runtimeCounterId = manager.addTimeBasedMonitor(
    "RuntimeCounter",
    1000,                        // Setiap 1 detik
    updateRuntime,
    0                            // Jalankan tanpa batas
  );
  
  // 5. Tampilkan status setiap 10 detik
  int statusDisplayId = manager.addTimeBasedMonitor(
    "StatusDisplay",
    10000,                       // Setiap 10 detik
    displayStatus,
    0                            // Jalankan tanpa batas
  );
  
  // 6. Pemeliharaan harian (akan dijalankan setiap 24 jam)
  int dailyMaintenanceId = manager.addTimeBasedMonitor(
    "DailyMaintenance",
    86400000,                    // 24 jam = 86400000ms
    dailyMaintenance,
    0                            // Jalankan tanpa batas
  );
  
  // Pengaturan tambahan
  manager.setInterval(pumpCycleId, 1800000);      // Ubah interval menjadi 30 menit
  manager.setEnabled(dailyMaintenanceId, false);  // Matikan sementara
  
  // Untuk keperluan demo, kita akan mengaktifkan pemeliharaan harian setelah 60 detik
  manager.addTimeBasedMonitor(
    "EnableMaintenance",
    60000,                       // 60 detik
    [dailyMaintenanceId]() {
      Serial.println("Activating daily maintenance schedule");
      manager.setEnabled(dailyMaintenanceId, true);
    },
    1                            // Jalankan 1 kali
  );
  
  Serial.println("Time-based monitoring system started");
  Serial.println("LED Blink: 500ms, Pump: 30min, Heater: 2h, Maintenance: 24h");
}

void loop() {
  manager.update();
  
  // Menangani input seri untuk kontrol manual
  if (Serial.available()) {
    char command = Serial.read();
    
    switch (command) {
      case 'p':
        Serial.println("Manual pump cycle triggered");
        waterPumpCycle();
        break;
      case 'h':
        Serial.println("Manual heater maintenance triggered");
        heaterMaintenance();
        break;
      case 'm':
        Serial.println("Manual daily maintenance triggered");
        dailyMaintenance();
        break;
      case 's':
        manager.printAllStatus();
        break;
    }
  }
  
  delay(50);
}
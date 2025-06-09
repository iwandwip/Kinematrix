#include <Arduino.h>
#define ENABLE_MODULE_VARIABLE_WATCHER
#include "Kinematrix.h"

// Simulasikan SD Card Library
// #include <SD.h>
// #include <SPI.h>

VariableWatcher watcher;

// Variabel sensor
float temperature = 22.0;
float humidity = 55.0;
float pressure = 1013.25;
float batteryVoltage = 3.7;
int lightLevel = 0;
int motionCount = 0;
float soilMoisture = 0.0;

// Variabel status
bool sdCardMounted = false;
String logFileName = "";
unsigned long totalLogEntries = 0;
unsigned long logInterval = 10; // detik
unsigned long lastLogTime = 0;

// Simulasikan mounting SD Card
void mountSDCard() {
  Serial.println("Mounting SD card...");
  delay(500);
  sdCardMounted = (random(10) > 1); // 90% berhasil
  
  if (sdCardMounted) {
    Serial.println("SD card mounted successfully");
    
    // Buat nama file log dengan timestamp
    char buffer[32];
    sprintf(buffer, "LOG_%lu.CSV", millis());
    logFileName = String(buffer);
    
    // Simulasi menulis header ke file
    Serial.print("Created log file: ");
    Serial.println(logFileName);
    Serial.println("Writing CSV header...");
    // File logFile = SD.open(logFileName, FILE_WRITE);
    // logFile.println("timestamp,temperature,humidity,pressure,light,soil,battery,motion");
    // logFile.close();
  } else {
    Serial.println("ERROR: Failed to mount SD card");
  }
}

// Simulasikan logging data ke SD Card
void logDataToSD() {
  if (!sdCardMounted) return;
  
  totalLogEntries++;
  
  // Timestamp
  unsigned long timestamp = millis() / 1000;
  
  // Format entri log sebagai string CSV
  String logEntry = String(timestamp) + "," +
                    String(temperature) + "," +
                    String(humidity) + "," +
                    String(pressure) + "," +
                    String(lightLevel) + "," +
                    String(soilMoisture) + "," +
                    String(batteryVoltage) + "," +
                    String(motionCount);
  
  // Simulasi menulis ke file
  Serial.print("Logging data: ");
  Serial.println(logEntry);
  
  // File logFile = SD.open(logFileName, FILE_APPEND);
  // logFile.println(logEntry);
  // logFile.close();
  
  lastLogTime = timestamp;
}

// Callback untuk suhu
void onTemperatureChange(void* value) {
  float temp = *(float*)value;
  
  if (temp < 0 || temp > 50) {
    Serial.println("WARNING: Temperature out of valid range");
    
    // Log anomali khusus
    if (sdCardMounted) {
      Serial.println("Logging temperature anomaly");
      // File anomalyFile = SD.open("ANOMALY.TXT", FILE_APPEND);
      // anomalyFile.print("Temperature anomaly: ");
      // anomalyFile.print(temp);
      // anomalyFile.print(" at ");
      // anomalyFile.println(millis() / 1000);
      // anomalyFile.close();
    }
  }
}

// Callback untuk kelembaban
void onHumidityChange(void* value) {
  float hum = *(float*)value;
  
  if (hum < 10 || hum > 95) {
    Serial.println("WARNING: Humidity out of valid range");
  }
}

// Callback untuk tingkat cahaya
void onLightChange(void* value) {
  int light = *(int*)value;
  
  static int lastLightState = 0; // 0=dark, 1=light
  int currentLightState = (light > 500) ? 1 : 0;
  
  if (currentLightState != lastLightState) {
    if (currentLightState == 1) {
      Serial.println("Light level increased (day/light detected)");
    } else {
      Serial.println("Light level decreased (night/dark detected)");
    }
    lastLightState = currentLightState;
  }
}

// Callback untuk deteksi gerakan
void onMotionChange(void* value) {
  int motion = *(int*)value;
  
  // Hanya merespon ketika nilai bertambah
  static int lastMotion = 0;
  if (motion > lastMotion) {
    Serial.print("Motion detected! Total count: ");
    Serial.println(motion);
    lastMotion = motion;
  }
}

void setup() {
  Serial.begin(9600);
  
  // Daftarkan variabel ke watcher
  watcher.addVariable(&temperature, "Temperature", onTemperatureChange);
  watcher.addVariable(&humidity, "Humidity", onHumidityChange);
  watcher.addVariable(&pressure, "Pressure");
  watcher.addVariable(&batteryVoltage, "Battery");
  watcher.addVariable(&lightLevel, "Light", onLightChange);
  watcher.addVariable(&motionCount, "Motion", onMotionChange);
  watcher.addVariable(&soilMoisture, "Soil");
  
  // Status variabel
  watcher.addVariable(&sdCardMounted, "SDCard");
  watcher.addVariable(&totalLogEntries, "LogEntries");
  
  Serial.println("Data Logger Example Started");
  
  // Inisialisasi SD Card
  mountSDCard();
  
  // Log data awal
  if (sdCardMounted) {
    logDataToSD();
  }
}

void loop() {
  unsigned long currentTime = millis() / 1000; // Waktu dalam detik
  
  // Simulasikan pembacaan sensor
  temperature = 22.0 + sin(millis() / 30000.0) * 5.0 + random(-10, 10) * 0.1;
  humidity = 55.0 + sin(millis() / 25000.0) * 15.0 + random(-5, 5);
  pressure = 1013.25 + sin(millis() / 60000.0) * 5.0;
  lightLevel = analogRead(A0);
  soilMoisture = max(0, min(100, 60.0 + sin(millis() / 20000.0) * 25.0));
  
  // Simulasikan battery drain
  batteryVoltage = 3.7 - (currentTime % 3600) / 36000.0;
  
  // Simulasikan deteksi gerakan acak
  if (random(100) < 5) {
    motionCount++;
  }
  
  // Periksa perubahan pada semua variabel
  watcher.checkAll();
  
  // Logging data berkala
  if (sdCardMounted && (currentTime - lastLogTime >= logInterval)) {
    logDataToSD();
    
    // Cek jika SD card penuh (simulasi)
    if (totalLogEntries % 1000 == 0) {
      Serial.println("Checking SD card space...");
      // Simulasi SD card hampir penuh
      if (totalLogEntries >= 5000) {
        Serial.println("WARNING: SD card space low");
      }
    }
  }
  
  // Simulasi error SD card sewaktu-waktu
  if (sdCardMounted && random(2000) == 0) {
    sdCardMounted = false;
    Serial.println("ERROR: SD card disconnected");
  }
  
  // Coba pasang ulang jika SD card tidak terpasang
  if (!sdCardMounted && random(100) == 0) {
    mountSDCard();
  }
  
  // Tampilkan status berkala
  static unsigned long lastStatusDisplay = 0;
  if (currentTime - lastStatusDisplay >= 5) {
    Serial.println("\n--- Data Logger Status ---");
    Serial.print("Time: ");
    Serial.print(currentTime);
    Serial.println(" seconds");
    Serial.print("Log file: ");
    Serial.println(sdCardMounted ? logFileName : "No SD Card");
    Serial.print("Entries: ");
    Serial.println(totalLogEntries);
    Serial.println("--- Current Readings ---");
    Serial.print("Temp: ");
    Serial.print(temperature);
    Serial.println(" °C");
    Serial.print("Humidity: ");
    Serial.print(humidity);
    Serial.println(" %");
    Serial.print("Pressure: ");
    Serial.print(pressure);
    Serial.println(" hPa");
    Serial.print("Light: ");
    Serial.println(lightLevel);
    Serial.print("Soil: ");
    Serial.print(soilMoisture);
    Serial.println(" %");
    Serial.print("Battery: ");
    Serial.print(batteryVoltage);
    Serial.println(" V");
    Serial.print("Motion events: ");
    Serial.println(motionCount);
    Serial.println("-------------------------");
    
    lastStatusDisplay = currentTime;
  }
  
  delay(100);
}
#define ENABLE_MODULE_SD_CARD_MODULE_ESP32
#include "Kinematrix.h"
#include <ArduinoJson.h>

SDCardModuleESP32 sdCard(5); // CS pin pada GPIO5

// Struktur data untuk menyimpan konfigurasi
struct DeviceConfig {
  char deviceName[32];
  char wifiSSID[32];
  char wifiPassword[64];
  bool enableWifi;
  int sensorInterval;
  float calibrationFactor;
  char serverAddress[64];
  int serverPort;
};

// Struktur data untuk menyimpan data sensor
struct SensorReading {
  unsigned long timestamp;
  float temperature;
  float humidity;
  float pressure;
  int batteryLevel;
};

// Nama file untuk penyimpanan
const char* configFileName = "/config.json";
const char* dataLogFileName = "/data_log.json";

// Variabel konfigurasi default
DeviceConfig config = {
  "KinematrixDevice1",  // deviceName
  "YourWiFiSSID",       // wifiSSID
  "YourWiFiPassword",   // wifiPassword
  true,                 // enableWifi
  60,                   // sensorInterval (seconds)
  1.0,                  // calibrationFactor
  "example.com",        // serverAddress
  8080                  // serverPort
};

// Array untuk merekam beberapa pembacaan sensor
const int maxReadings = 10;
SensorReading readings[maxReadings];
int readingCount = 0;

// Simulasi pembacaan sensor
void simulateSensorReading(SensorReading* reading) {
  reading->timestamp = millis();
  reading->temperature = 22.0 + random(0, 100) / 10.0;
  reading->humidity = 40.0 + random(0, 200) / 10.0;
  reading->pressure = 1000.0 + random(0, 50);
  reading->batteryLevel = 70 + random(0, 30);
}

// Fungsi untuk menyimpan konfigurasi ke file JSON
bool saveConfig() {
  // Alokasi buffer JSON
  StaticJsonDocument<512> doc;
  
  // Isi dokumen JSON
  doc["deviceName"] = config.deviceName;
  doc["wifiSSID"] = config.wifiSSID;
  doc["wifiPassword"] = config.wifiPassword;
  doc["enableWifi"] = config.enableWifi;
  doc["sensorInterval"] = config.sensorInterval;
  doc["calibrationFactor"] = config.calibrationFactor;
  doc["serverAddress"] = config.serverAddress;
  doc["serverPort"] = config.serverPort;
  
  // Serializasi JSON ke string
  String jsonString;
  serializeJson(doc, jsonString);
  
  // Tulis ke file
  return sdCard.writeFile(configFileName, jsonString.c_str());
}

// Fungsi untuk memuat konfigurasi dari file JSON
bool loadConfig() {
  // Periksa apakah file ada
  if (!sdCard.exists(configFileName)) {
    Serial.println("Config file not found, using defaults");
    return false;
  }
  
  // Baca file
  String jsonString = sdCard.readFileString(configFileName);
  if (jsonString.length() == 0) {
    Serial.println("Empty config file, using defaults");
    return false;
  }
  
  // Parse JSON
  StaticJsonDocument<512> doc;
  DeserializationError error = deserializeJson(doc, jsonString);
  
  if (error) {
    Serial.print("Failed to parse config file: ");
    Serial.println(error.c_str());
    return false;
  }
  
  // Salin nilai ke config
  strlcpy(config.deviceName, doc["deviceName"] | "KinematrixDevice1", sizeof(config.deviceName));
  strlcpy(config.wifiSSID, doc["wifiSSID"] | "YourWiFiSSID", sizeof(config.wifiSSID));
  strlcpy(config.wifiPassword, doc["wifiPassword"] | "YourWiFiPassword", sizeof(config.wifiPassword));
  config.enableWifi = doc["enableWifi"] | true;
  config.sensorInterval = doc["sensorInterval"] | 60;
  config.calibrationFactor = doc["calibrationFactor"] | 1.0;
  strlcpy(config.serverAddress, doc["serverAddress"] | "example.com", sizeof(config.serverAddress));
  config.serverPort = doc["serverPort"] | 8080;
  
  return true;
}

// Fungsi untuk menyimpan data sensor ke file JSON
bool saveSensorData() {
  if (readingCount == 0) {
    return true; // Tidak ada data untuk disimpan
  }
  
  // Alokasi buffer JSON
  DynamicJsonDocument doc(2048);
  
  // Periksa apakah file sudah ada
  bool fileExists = sdCard.exists(dataLogFileName);
  
  // Jika file sudah ada, muat data yang ada
  if (fileExists) {
    String jsonString = sdCard.readFileString(dataLogFileName);
    if (jsonString.length() > 0) {
      DeserializationError error = deserializeJson(doc, jsonString);
      if (error) {
        Serial.print("Failed to parse existing data file: ");
        Serial.println(error.c_str());
        return false;
      }
    }
  } else {
    // Buat array kosong jika file belum ada
    doc.add(JsonArray());
  }
  
  // Tambahkan pembacaan baru
  JsonArray data = doc.as<JsonArray>();
  for (int i = 0; i < readingCount; i++) {
    JsonObject reading = data.createNestedObject();
    reading["timestamp"] = readings[i].timestamp;
    reading["temperature"] = readings[i].temperature;
    reading["humidity"] = readings[i].humidity;
    reading["pressure"] = readings[i].pressure;
    reading["batteryLevel"] = readings[i].batteryLevel;
  }
  
  // Serializasi JSON ke string
  String jsonString;
  serializeJson(doc, jsonString);
  
  // Tulis ke file
  bool success = sdCard.writeFile(dataLogFileName, jsonString.c_str());
  if (success) {
    readingCount = 0; // Reset counter jika berhasil disimpan
  }
  
  return success;
}

// Fungsi untuk memuat data sensor dari file JSON
bool loadSensorData() {
  // Periksa apakah file ada
  if (!sdCard.exists(dataLogFileName)) {
    Serial.println("Data log file not found");
    return false;
  }
  
  // Baca file
  String jsonString = sdCard.readFileString(dataLogFileName);
  if (jsonString.length() == 0) {
    Serial.println("Empty data log file");
    return false;
  }
  
  // Parse JSON
  DynamicJsonDocument doc(16384); // Ukuran besar untuk file log panjang
  DeserializationError error = deserializeJson(doc, jsonString);
  
  if (error) {
    Serial.print("Failed to parse data log file: ");
    Serial.println(error.c_str());
    return false;
  }
  
  // Tampilkan ringkasan data
  JsonArray data = doc.as<JsonArray>();
  int count = data.size();
  
  Serial.print("Loaded ");
  Serial.print(count);
  Serial.println(" sensor readings from file");
  
  if (count > 0) {
    Serial.println("First reading:");
    JsonObject first = data[0];
    Serial.print("  Timestamp: ");
    Serial.println(first["timestamp"].as<unsigned long>());
    Serial.print("  Temperature: ");
    Serial.println(first["temperature"].as<float>());
    
    Serial.println("Last reading:");
    JsonObject last = data[count - 1];
    Serial.print("  Timestamp: ");
    Serial.println(last["timestamp"].as<unsigned long>());
    Serial.print("  Temperature: ");
    Serial.println(last["temperature"].as<float>());
  }
  
  return true;
}

// Fungsi untuk menambahkan pembacaan sensor ke array
void addSensorReading() {
  if (readingCount < maxReadings) {
    simulateSensorReading(&readings[readingCount]);
    readingCount++;
    
    Serial.print("Added sensor reading: Temperature ");
    Serial.print(readings[readingCount-1].temperature);
    Serial.print("°C, Humidity ");
    Serial.print(readings[readingCount-1].humidity);
    Serial.println("%");
    
    // Simpan ke SD card jika array penuh
    if (readingCount >= maxReadings) {
      Serial.println("Reading buffer full, saving to SD card...");
      if (saveSensorData()) {
        Serial.println("Data saved successfully");
      } else {
        Serial.println("Error saving data");
      }
    }
  }
}

void printConfig() {
  Serial.println("\n----- Current Configuration -----");
  Serial.print("Device Name: ");
  Serial.println(config.deviceName);
  Serial.print("WiFi SSID: ");
  Serial.println(config.wifiSSID);
  Serial.print("WiFi Password: ");
  Serial.println(config.wifiPassword);
  Serial.print("WiFi Enabled: ");
  Serial.println(config.enableWifi ? "Yes" : "No");
  Serial.print("Sensor Interval: ");
  Serial.print(config.sensorInterval);
  Serial.println(" seconds");
  Serial.print("Calibration Factor: ");
  Serial.println(config.calibrationFactor);
  Serial.print("Server Address: ");
  Serial.println(config.serverAddress);
  Serial.print("Server Port: ");
  Serial.println(config.serverPort);
  Serial.println("-------------------------------");
}

void setup() {
  Serial.begin(115200);
  delay(1000);
  
  Serial.println("SD Card Module Structured Data Example");
  
  // Initialize random seed
  randomSeed(analogRead(0));
  
  // Inisialisasi kartu SD
  if (!sdCard.begin()) {
    Serial.println("SD Card initialization failed!");
    return;
  }
  
  Serial.println("SD Card initialized successfully!");
  
  // Coba muat konfigurasi
  if (loadConfig()) {
    Serial.println("Configuration loaded successfully from SD card");
  } else {
    Serial.println("Using default configuration");
    
    // Simpan konfigurasi default ke SD card
    if (saveConfig()) {
      Serial.println("Default configuration saved to SD card");
    } else {
      Serial.println("Error saving default configuration");
    }
  }
  
  // Tampilkan konfigurasi saat ini
  printConfig();
  
  // Contoh modifikasi konfigurasi
  Serial.println("\nModifying configuration...");
  strlcpy(config.deviceName, "KinematrixDevice2", sizeof(config.deviceName));
  config.sensorInterval = 30;
  
  // Simpan konfigurasi yang diubah
  if (saveConfig()) {
    Serial.println("Modified configuration saved successfully");
  } else {
    Serial.println("Error saving modified configuration");
  }
  
  // Coba muat data sensor
  loadSensorData();
  
  Serial.println("\nSimulating sensor readings...");
  
  // Simulasi beberapa pembacaan sensor
  for (int i = 0; i < 5; i++) {
    addSensorReading();
    delay(1000);
  }
  
  // Simpan data sensor secara manual jika belum penuh
  if (readingCount > 0) {
    Serial.println("Saving sensor data to SD card...");
    if (saveSensorData()) {
      Serial.println("Data saved successfully");
    } else {
      Serial.println("Error saving data");
    }
  }
  
  Serial.println("\nStructured data example completed");
}

void loop() {
  // Simulasi pembacaan sensor periodik
  static unsigned long lastReadingTime = 0;
  unsigned long currentTime = millis();
  
  if (currentTime - lastReadingTime > (config.sensorInterval * 1000)) {
    lastReadingTime = currentTime;
    
    Serial.println("\nPeriodic sensor reading...");
    addSensorReading();
  }
  
  delay(1000);
}

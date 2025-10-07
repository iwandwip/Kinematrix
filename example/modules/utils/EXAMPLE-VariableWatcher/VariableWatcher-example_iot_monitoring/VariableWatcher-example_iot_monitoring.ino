#include <Arduino.h>
#define ENABLE_MODULE_VARIABLE_WATCHER
#include "Kinematrix.h"

// Simulasi library WiFi dan MQTT
// #include <WiFi.h>
// #include <PubSubClient.h>

VariableWatcher watcher;

// Variabel sensor
float temperature = 0.0;
float humidity = 0.0;
float pressure = 0.0;
float voltage = 3.3;
int lightLevel = 0;
float waterLevel = 0.0;
bool motionDetected = false;

// Variabel status
bool wifiConnected = false;
bool mqttConnected = false;
int rssi = -70;
int errorCount = 0;
String deviceStatus = "Initializing";
unsigned long uptime = 0;
unsigned long lastReportTime = 0;

// Pengaturan
bool alarmEnabled = true;
float tempThreshold = 30.0;
int reportInterval = 60; // detik

// Simulasi koneksi WiFi
void connectWifi() {
  Serial.println("Connecting to WiFi...");
  delay(2000);
  wifiConnected = true;
  rssi = -60 + random(-20, 10);
  Serial.println("WiFi connected!");
}

// Simulasi koneksi MQTT
void connectMQTT() {
  if (!wifiConnected) return;
  
  Serial.println("Connecting to MQTT broker...");
  delay(1000);
  mqttConnected = (random(10) > 1); // 90% kemungkinan berhasil
  
  if (mqttConnected) {
    Serial.println("MQTT connected!");
    deviceStatus = "Online";
  } else {
    Serial.println("MQTT connection failed!");
    errorCount++;
    deviceStatus = "MQTT Error";
  }
}

// Simulasi kirim data ke cloud
void sendDataToCloud() {
  if (!mqttConnected) return;
  
  String payload = "{\"temp\":";
  payload += temperature;
  payload += ",\"hum\":";
  payload += humidity;
  payload += ",\"press\":";
  payload += pressure;
  payload += ",\"light\":";
  payload += lightLevel;
  payload += ",\"water\":";
  payload += waterLevel;
  payload += ",\"motion\":";
  payload += motionDetected ? "true" : "false";
  payload += ",\"batt\":";
  payload += voltage;
  payload += ",\"rssi\":";
  payload += rssi;
  payload += ",\"uptime\":";
  payload += uptime;
  payload += "}";
  
  Serial.print("Publishing data: ");
  Serial.println(payload);
  
  lastReportTime = millis() / 1000;
}

// Callbacks
void onTemperatureChange(void* value) {
  float temp = *(float*)value;
  
  if (alarmEnabled && temp > tempThreshold) {
    Serial.println("ALERT: Temperature exceeded threshold!");
    // Simulasi mengirim notifikasi alarm
    if (mqttConnected) {
      Serial.println("Sending temperature alert notification!");
    }
  }
}

void onMotionChange(void* value) {
  bool motion = *(bool*)value;
  
  if (motion) {
    Serial.println("Motion detected!");
    // Kirim notifikasi hanya jika terjadi gerakan
    if (mqttConnected) {
      Serial.println("Sending motion alert notification!");
    }
  }
}

void onWifiStatusChange(void* value) {
  bool connected = *(bool*)value;
  
  if (connected) {
    Serial.println("WiFi connection established");
    // Connect MQTT when WiFi connects
    connectMQTT();
  } else {
    Serial.println("WiFi connection lost");
    mqttConnected = false;
    deviceStatus = "WiFi Disconnected";
  }
}

void onWaterLevelChange(void* value) {
  float level = *(float*)value;
  
  if (level > 80.0) {
    Serial.println("ALERT: Water level high!");
    // Simulasi mengirim notifikasi alarm
    if (mqttConnected) {
      Serial.println("Sending water level alert notification!");
    }
  }
}

void setup() {
  Serial.begin(9600);
  pinMode(LED_BUILTIN, OUTPUT);
  
  // Daftarkan variabel dengan watcher dan callback functions
  watcher.addVariable(&temperature, "Temperature", onTemperatureChange);
  watcher.addVariable(&humidity, "Humidity");
  watcher.addVariable(&pressure, "Pressure");
  watcher.addVariable(&voltage, "Voltage");
  watcher.addVariable(&lightLevel, "Light");
  watcher.addVariable(&waterLevel, "Water", onWaterLevelChange);
  watcher.addVariable(&motionDetected, "Motion", onMotionChange);
  watcher.addVariable(&wifiConnected, "WiFi", onWifiStatusChange);
  watcher.addVariable(&mqttConnected, "MQTT");
  watcher.addVariable(&rssi, "RSSI");
  watcher.addVariable(&deviceStatus, "Status");
  watcher.addVariable(&uptime, "Uptime");
  
  Serial.println("IoT Monitoring System Starting...");
  
  // Koneksi jaringan
  connectWifi();
  if (wifiConnected) {
    connectMQTT();
  }
}

void loop() {
  unsigned long currentMillis = millis();
  
  // Update uptime (dalam detik)
  uptime = currentMillis / 1000;
  
  // Simulasi pembacaan sensor
  temperature = 22.0 + sin(currentMillis / 20000.0) * 8.0;
  humidity = 50.0 + sin(currentMillis / 30000.0) * 20.0;
  pressure = 1000.0 + sin(currentMillis / 40000.0) * 15.0;
  lightLevel = analogRead(A0);
  waterLevel = 50.0 + sin(currentMillis / 10000.0) * 40.0;
  
  // Simulasi deteksi gerakan random
  motionDetected = (random(100) < 5);
  
  // Simulasi fluktuasi tegangan baterai
  voltage = 3.3 - (uptime % 3600) / 36000.0;
  
  // Simulasi fluktuasi kekuatan sinyal
  if (wifiConnected && currentMillis % 10000 < 100) {
    rssi = -60 + random(-20, 10);
  }
  
  // Simulasi disconnection acak
  if (wifiConnected && random(1000) == 0) {
    wifiConnected = false;
  } else if (!wifiConnected && random(100) == 0) {
    connectWifi();
  }
  
  if (mqttConnected && random(500) == 0) {
    mqttConnected = false;
    deviceStatus = "MQTT Disconnected";
  } else if (!mqttConnected && wifiConnected && random(50) == 0) {
    connectMQTT();
  }
  
  // Periksa perubahan pada semua variabel
  watcher.checkAll();
  
  // Pembacaan dan logging berkala
  if (uptime - lastReportTime >= reportInterval) {
    if (wifiConnected && mqttConnected) {
      sendDataToCloud();
    } else {
      Serial.println("Cannot send data: not connected");
      if (!wifiConnected) {
        connectWifi();
      } else if (!mqttConnected) {
        connectMQTT();
      }
    }
  }
  
  // Serial output untuk debugging
  static unsigned long lastDisplayTime = 0;
  if (currentMillis - lastDisplayTime > 5000) {
    Serial.println("\n--- System Status ---");
    Serial.print("Uptime: ");
    Serial.print(uptime);
    Serial.println(" seconds");
    Serial.print("Status: ");
    Serial.println(deviceStatus);
    Serial.print("WiFi: ");
    Serial.print(wifiConnected ? "Connected" : "Disconnected");
    Serial.print(", RSSI: ");
    Serial.println(rssi);
    Serial.print("MQTT: ");
    Serial.println(mqttConnected ? "Connected" : "Disconnected");
    Serial.println("--- Sensor Data ---");
    Serial.print("Temperature: ");
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
    Serial.print("Water Level: ");
    Serial.print(waterLevel);
    Serial.println(" %");
    Serial.print("Battery: ");
    Serial.print(voltage);
    Serial.println(" V");
    Serial.print("Motion: ");
    Serial.println(motionDetected ? "Detected" : "None");
    Serial.println("------------------");
    
    lastDisplayTime = currentMillis;
  }
  
  delay(100);
}
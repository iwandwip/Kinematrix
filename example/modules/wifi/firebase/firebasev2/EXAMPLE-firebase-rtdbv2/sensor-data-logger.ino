#include "firebase-rtdbv2.h"
#include <WiFi.h>
#include <WiFiClientSecure.h>
#include <time.h>

// WiFi credentials
#define WIFI_SSID "YourWiFiSSID"
#define WIFI_PASSWORD "YourWiFiPassword"

// Firebase credentials
#define API_KEY "YourFirebaseAPIKey"
#define USER_EMAIL "YourEmail@example.com"
#define USER_PASSWORD "YourPassword"
#define DATABASE_URL "https://your-project-id-default-rtdb.firebaseio.com/"

// NTP Server settings
#define NTP_SERVER "pool.ntp.org"
#define GMT_OFFSET_SEC 0       // Change this based on your timezone (in seconds)
#define DAYLIGHT_OFFSET_SEC 0  // Daylight saving time offset (in seconds)

// Pin definitions for sensors (simulated in this example)
#define TEMP_SENSOR_PIN 36
#define HUMIDITY_SENSOR_PIN 39
#define LIGHT_SENSOR_PIN 34

// Timing definitions
#define UPLOAD_INTERVAL 60000  // Upload data every 60 seconds

WiFiClientSecure client;
FirebaseV2RTDB firebase;

unsigned long lastUploadTime = 0;
String deviceId = "ESP32_"; // Will be appended with MAC address

// Simulated sensor data (replace with actual sensor readings in your implementation)
float readTemperature() {
  // Simulate temperature between 20-30°C
  return 20.0 + (random(0, 100) / 10.0);
}

float readHumidity() {
  // Simulate humidity between 40-80%
  return 40.0 + (random(0, 400) / 10.0);
}

int readLightLevel() {
  // Simulate light level between 0-1000 lux
  return random(0, 1000);
}

// Get a timestamp string in ISO 8601 format
String getISOTimestamp() {
  struct tm timeinfo;
  if (!getLocalTime(&timeinfo)) {
    Serial.println("Failed to obtain time");
    return "1970-01-01T00:00:00Z";
  }
  
  char timeStringBuff[30];
  strftime(timeStringBuff, sizeof(timeStringBuff), "%Y-%m-%dT%H:%M:%SZ", &timeinfo);
  return String(timeStringBuff);
}

void setupWiFi() {
  Serial.print("Connecting to WiFi");
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  
  unsigned long startAttemptTime = millis();
  
  while (WiFi.status() != WL_CONNECTED && 
         millis() - startAttemptTime < 15000) { // 15 second timeout
    Serial.print(".");
    delay(500);
  }
  
  if (WiFi.status() != WL_CONNECTED) {
    Serial.println("\nFailed to connect to WiFi! Restarting...");
    ESP.restart();
  }
  
  Serial.println();
  Serial.println("Connected with IP: " + WiFi.localIP().toString());
  
  // Set device ID based on MAC address
  uint8_t mac[6];
  WiFi.macAddress(mac);
  char macStr[13];
  sprintf(macStr, "%02X%02X%02X%02X%02X%02X", mac[0], mac[1], mac[2], mac[3], mac[4], mac[5]);
  deviceId += macStr;
  Serial.println("Device ID: " + deviceId);
}

void setupTime() {
  configTime(GMT_OFFSET_SEC, DAYLIGHT_OFFSET_SEC, NTP_SERVER);
  Serial.println("Syncing time with NTP server...");
  
  unsigned long startAttemptTime = millis();
  struct tm timeinfo;
  
  while (!getLocalTime(&timeinfo) && 
         millis() - startAttemptTime < 10000) { // 10 second timeout
    Serial.print(".");
    delay(500);
  }
  
  Serial.println();
  if (!getLocalTime(&timeinfo)) {
    Serial.println("Failed to obtain time! Continuing without accurate timestamps.");
  } else {
    Serial.println("Time synchronized.");
    Serial.print("Current time: ");
    Serial.println(getISOTimestamp());
  }
}

void setupFirebase() {
  client.setInsecure();
  
  Serial.println("Connecting to Firebase...");
  if (!firebase.begin(client, DATABASE_URL, API_KEY, USER_EMAIL, USER_PASSWORD)) {
    Serial.println("Firebase initialization failed: " + firebase.getError());
    Serial.println("Will retry in loop...");
    return;
  }
  
  Serial.println("Firebase connected successfully!");
  
  // Wait for connection to be ready
  Serial.print("Waiting for Firebase to be ready");
  unsigned long startAttemptTime = millis();
  
  while (!firebase.ready() && 
         millis() - startAttemptTime < 10000) { // 10 second timeout
    Serial.print(".");
    delay(500);
  }
  
  Serial.println();
  if (!firebase.ready()) {
    Serial.println("Firebase not ready after timeout! Will retry in loop...");
  } else {
    Serial.println("Firebase ready!");
  }
}

void collectAndUploadData() {
  // Only upload if we're ready and it's time to upload
  if (!firebase.ready()) {
    Serial.println("Firebase not ready, skipping data upload");
    return;
  }
  
  // Collect sensor data
  float temperature = readTemperature();
  float humidity = readHumidity();
  int lightLevel = readLightLevel();
  String timestamp = getISOTimestamp();
  
  Serial.println("Collected sensor data:");
  Serial.println("Temperature: " + String(temperature) + "°C");
  Serial.println("Humidity: " + String(humidity) + "%");
  Serial.println("Light: " + String(lightLevel) + " lux");
  Serial.println("Timestamp: " + timestamp);
  
  // Create JSON object for data
  char jsonBuffer[256];
  snprintf(jsonBuffer, sizeof(jsonBuffer), 
    "{"
      "\"temperature\":%.2f,"
      "\"humidity\":%.2f,"
      "\"light\":%d,"
      "\"timestamp\":\"%s\","
      "\"deviceId\":\"%s\""
    "}",
    temperature, humidity, lightLevel, 
    timestamp.c_str(), deviceId.c_str()
  );
  
  // Push data to timestamped node
  String basePath = "/sensor_data";
  String pushKey = firebase.push(basePath, jsonBuffer);
  
  if (pushKey.length() > 0) {
    Serial.println("Data uploaded successfully with key: " + pushKey);
  } else {
    Serial.println("Failed to upload data: " + firebase.getError());
  }
  
  // Also update the "latest" node for quick access to most recent values
  String latestPath = "/latest/" + deviceId;
  if (firebase.set(latestPath, jsonBuffer)) {
    Serial.println("Latest data updated successfully");
  } else {
    Serial.println("Failed to update latest data: " + firebase.getError());
  }
}

void setup() {
  Serial.begin(115200);
  delay(1000);
  Serial.println("\n=== ESP32 Firebase Sensor Logger ===");
  
  // Initialize WiFi
  setupWiFi();
  
  // Initialize time
  setupTime();
  
  // Initialize Firebase
  setupFirebase();
  
  // Force first reading
  lastUploadTime = 0;
  Serial.println("Setup complete!");
}

void loop() {
  firebase.loop(); // Always keep this in your loop
  
  // Check WiFi and reconnect if needed
  if (WiFi.status() != WL_CONNECTED) {
    Serial.println("WiFi connection lost! Reconnecting...");
    setupWiFi();
  }
  
  // Upload data at the specified interval
  if (millis() - lastUploadTime >= UPLOAD_INTERVAL || lastUploadTime == 0) {
    lastUploadTime = millis();
    collectAndUploadData();
  }
  
  // Process other tasks here
  
  delay(1000);
}

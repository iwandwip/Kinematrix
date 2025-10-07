#include "firebase-firestorev2.h"
#include <WiFi.h>
#include <time.h>

// WiFi credentials
#define WIFI_SSID "YourWiFiSSID"
#define WIFI_PASSWORD "YourWiFiPassword"

// Firebase Credentials
#define API_KEY "YourFirebaseAPIKey"
#define USER_EMAIL "YourEmail@example.com" 
#define USER_PASSWORD "YourPassword"
#define PROJECT_ID "your-project-id"

// NTP Server settings for time sync
#define NTP_SERVER "pool.ntp.org"
#define GMT_OFFSET_SEC 0
#define DAYLIGHT_OFFSET_SEC 0

// Pin definitions for sensors (can be actual sensors or simulated)
#define TEMP_SENSOR_PIN 36
#define HUMIDITY_SENSOR_PIN 39
#define LIGHT_SENSOR_PIN 34

// Configuration
#define UPLOAD_INTERVAL 60000  // Upload data every 60 seconds

FirebaseV2Firestore firestore;
unsigned long lastUploadTime = 0;
String deviceId = "ESP32_";  // Will be appended with MAC address
bool firestoreInitialized = false;

// Sample readings (replace with actual sensor readings in your implementation)
float readTemperature() {
  // For a real sensor, you would return actual sensor reading
  // This is a simulation that returns a value between 20-30°C
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
    char timeString[30];
    strftime(timeString, sizeof(timeString), "%Y-%m-%d %H:%M:%S", &timeinfo);
    Serial.print("Current time: ");
    Serial.println(timeString);
  }
}

bool setupFirestore() {
  Serial.println("Initializing Firestore...");
  firestoreInitialized = firestore.begin(API_KEY, USER_EMAIL, USER_PASSWORD, PROJECT_ID);
  
  if (!firestoreInitialized) {
    Serial.println("Failed to initialize Firestore: " + firestore.getLastError());
    return false;
  }
  
  Serial.println("Firestore initialized successfully!");
  return true;
}

bool waitForFirestoreReady(int timeoutSeconds = 10) {
  if (!firestoreInitialized) return false;
  
  Serial.print("Waiting for Firestore to be ready");
  unsigned long startTime = millis();
  
  while (!firestore.isReady() && (millis() - startTime < timeoutSeconds * 1000)) {
    firestore.loop();
    Serial.print(".");
    delay(500);
  }
  Serial.println();
  
  if (!firestore.isReady()) {
    Serial.println("Firestore not ready after timeout!");
    return false;
  }
  
  Serial.println("Firestore ready!");
  return true;
}

String createSensorDocumentJson(float temperature, float humidity, int lightLevel, time_t timestamp, const String &deviceId) {
  // Create JSON document with proper Firestore format
  char jsonBuffer[512];
  snprintf(jsonBuffer, sizeof(jsonBuffer), 
    "{"
      "\"fields\": {"
        "\"temperature\": {\"doubleValue\": %.2f},"
        "\"humidity\": {\"doubleValue\": %.2f},"
        "\"light\": {\"integerValue\": %d},"
        "\"timestamp\": {\"integerValue\": %ld},"
        "\"deviceId\": {\"stringValue\": \"%s\"}"
      "}"
    "}",
    temperature, humidity, lightLevel, timestamp, deviceId.c_str()
  );
  
  return String(jsonBuffer);
}

void collectAndUploadData() {
  if (!firestoreInitialized || !firestore.isReady()) {
    Serial.println("Firestore not ready, skipping data upload");
    return;
  }
  
  // Collect sensor data
  float temperature = readTemperature();
  float humidity = readHumidity();
  int lightLevel = readLightLevel();
  time_t timestamp = time(nullptr);
  
  Serial.println("Collected sensor data:");
  Serial.println("Temperature: " + String(temperature) + "°C");
  Serial.println("Humidity: " + String(humidity) + "%");
  Serial.println("Light: " + String(lightLevel) + " lux");
  Serial.println("Timestamp: " + String(timestamp));
  
  // Create JSON document with proper Firestore format
  String sensorData = createSensorDocumentJson(temperature, humidity, lightLevel, timestamp, deviceId);
  
  // Create a unique document ID based on timestamp
  String docId = deviceId + "_" + String(timestamp);
  String docPath = "sensor_data/" + docId;
  
  // Upload data to Firestore
  Serial.println("Uploading data to Firestore...");
  firestore.createDocument(docPath, sensorData, false);
  
  // Keep processing Firestore events until we get a result
  unsigned long startTime = millis();
  bool gotResult = false;
  
  while (!gotResult && (millis() - startTime < 5000)) {
    firestore.loop();
    
    if (firestore.isResultReady()) {
      gotResult = true;
      String result = firestore.getLastPayload();
      
      if (result.indexOf("\"error\":") >= 0) {
        Serial.println("Error uploading data: " + result);
      } else {
        Serial.println("Data uploaded successfully!");
        
        // Now update the "latest" document with the same data
        String latestDocPath = "latest/" + deviceId;
        firestore.createDocument(latestDocPath, sensorData, false);
      }
    }
    
    delay(100);
  }
  
  if (!gotResult) {
    Serial.println("No response from Firestore within timeout!");
  }
}

void setup() {
  Serial.begin(115200);
  delay(1000);
  Serial.println("\n=== ESP32 Firestore Sensor Data Logger (Correct Format) ===");
  
  // Initialize WiFi
  setupWiFi();
  
  // Initialize time
  setupTime();
  
  // Initialize Firestore
  if (!setupFirestore()) {
    Serial.println("Failed to set up Firestore. Restarting in 5 seconds...");
    delay(5000);
    ESP.restart();
  }
  
  // Wait for Firestore to be ready
  if (!waitForFirestoreReady(30)) {
    Serial.println("Firestore not ready. Restarting in 5 seconds...");
    delay(5000);
    ESP.restart();
  }
  
  // Force first reading
  lastUploadTime = 0;
  Serial.println("Setup complete!");
}

void loop() {
  // Process Firestore events
  if (firestoreInitialized) {
    firestore.loop();
  }
  
  // Check WiFi and reconnect if needed
  if (WiFi.status() != WL_CONNECTED) {
    Serial.println("WiFi connection lost! Reconnecting...");
    setupWiFi();
  }
  
  // Check if Firestore is still ready, reconnect if needed
  if (!firestoreInitialized || !firestore.isReady()) {
    Serial.println("Firestore connection lost! Reconnecting...");
    if (!setupFirestore() || !waitForFirestoreReady()) {
      Serial.println("Failed to reconnect to Firestore. Will retry later.");
      delay(5000);
      return;
    }
  }
  
  // Upload data at the specified interval
  if (millis() - lastUploadTime >= UPLOAD_INTERVAL || lastUploadTime == 0) {
    lastUploadTime = millis();
    collectAndUploadData();
  }
  
  // Small delay to prevent CPU hogging
  delay(100);
}

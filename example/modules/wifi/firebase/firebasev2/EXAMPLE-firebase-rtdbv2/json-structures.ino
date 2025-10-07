#include "firebase-rtdbv2.h"
#include <WiFi.h>
#include <WiFiClientSecure.h>

#define WIFI_SSID "YourWiFiSSID"
#define WIFI_PASSWORD "YourWiFiPassword"

#define API_KEY "YourFirebaseAPIKey"
#define USER_EMAIL "YourEmail@example.com"
#define USER_PASSWORD "YourPassword"
#define DATABASE_URL "https://your-project-id-default-rtdb.firebaseio.com/"

WiFiClientSecure client;
FirebaseV2RTDB firebase;

void setup() {
  Serial.begin(115200);

  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  Serial.print("Connecting to WiFi");
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print(".");
    delay(300);
  }
  Serial.println();
  Serial.println("Connected with IP: " + WiFi.localIP().toString());

  client.setInsecure();
  
  if (!firebase.begin(client, DATABASE_URL, API_KEY, USER_EMAIL, USER_PASSWORD)) {
    Serial.println("Firebase initialization failed: " + firebase.getError());
    return;
  }
  
  Serial.println("Firebase connected successfully!");
  
  // Wait for connection to be ready
  while (!firebase.ready()) {
    Serial.print(".");
    delay(100);
  }
  Serial.println("\nFirebase ready!");
  
  // Examples with JSON structures
  Serial.println("\n--- Working with JSON ---");
  
  // Create a simple JSON object
  const char* simpleJson = "{\"temperature\":25.5,\"humidity\":60,\"status\":\"normal\"}";
  
  // Set JSON object
  if (firebase.set("/json/simple", simpleJson)) {
    Serial.println("Simple JSON set successfully");
  } else {
    Serial.println("Failed to set simple JSON: " + firebase.getError());
  }
  
  // Create a complex JSON object
  const char* complexJson = "{"
    "\"device\":{"
      "\"name\":\"ESP32\","
      "\"id\":\"ABC123\","
      "\"firmware\":\"1.0.0\""
    "},"
    "\"sensors\":["
      "{\"type\":\"temperature\",\"value\":25.5,\"unit\":\"C\"},"
      "{\"type\":\"humidity\",\"value\":60,\"unit\":\"%\"},"
      "{\"type\":\"pressure\",\"value\":1013,\"unit\":\"hPa\"}"
    "],"
    "\"status\":{"
      "\"online\":true,"
      "\"lastUpdate\":\"2023-01-01 12:00:00\","
      "\"battery\":85"
    "}"
  "}";
  
  // Set complex JSON object
  if (firebase.set("/json/complex", complexJson)) {
    Serial.println("Complex JSON set successfully");
  } else {
    Serial.println("Failed to set complex JSON: " + firebase.getError());
  }
  
  // Read JSON data
  String jsonData = firebase.getJSON("/json/complex");
  Serial.println("Retrieved JSON data:");
  Serial.println(jsonData);
  
  // Update only specific fields using update() method
  const char* updateJson = "{\"status/online\":false,\"status/battery\":75}";
  
  if (firebase.update("/json/complex", updateJson)) {
    Serial.println("JSON partially updated successfully");
  } else {
    Serial.println("Failed to update JSON: " + firebase.getError());
  }
  
  // Push a new JSON object to a list
  const char* newSensorJson = "{\"type\":\"light\",\"value\":500,\"unit\":\"lux\"}";
  
  String newKey = firebase.push("/json/complex/sensors", newSensorJson);
  if (newKey.length() > 0) {
    Serial.println("New sensor pushed with key: " + newKey);
  } else {
    Serial.println("Failed to push new sensor: " + firebase.getError());
  }
  
  // Read the updated data
  jsonData = firebase.getJSON("/json/complex");
  Serial.println("Updated JSON data:");
  Serial.println(jsonData);
}

void loop() {
  firebase.loop(); // Always keep this in your loop
  
  // Your code here
  
  delay(1000);
}

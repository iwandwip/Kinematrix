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

  client.setInsecure(); // Disable SSL certificate verification
  
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
  
  // Basic write operations
  Serial.println("\n--- Basic Write Operations ---");
  
  // Set string value
  if (firebase.set("/test/string", "Hello World")) {
    Serial.println("String write successful");
  } else {
    Serial.println("String write failed: " + firebase.getError());
  }
  
  // Set integer value
  if (firebase.set("/test/integer", 42)) {
    Serial.println("Integer write successful");
  } else {
    Serial.println("Integer write failed: " + firebase.getError());
  }
  
  // Set boolean value
  if (firebase.set("/test/boolean", true)) {
    Serial.println("Boolean write successful");
  } else {
    Serial.println("Boolean write failed: " + firebase.getError());
  }
  
  // Set float value with 2 decimal places
  if (firebase.set("/test/float", 3.14159, 2)) {
    Serial.println("Float write successful");
  } else {
    Serial.println("Float write failed: " + firebase.getError());
  }
  
  // Basic read operations
  Serial.println("\n--- Basic Read Operations ---");
  
  // Read string
  String stringValue = firebase.getString("/test/string");
  Serial.println("String value: " + stringValue);
  
  // Read integer
  int intValue = firebase.getInt("/test/integer");
  Serial.println("Integer value: " + String(intValue));
  
  // Read boolean
  bool boolValue = firebase.getBool("/test/boolean");
  Serial.println("Boolean value: " + String(boolValue ? "true" : "false"));
  
  // Read float
  float floatValue = firebase.getFloat("/test/float");
  Serial.println("Float value: " + String(floatValue));
  
  // Check if node exists
  if (firebase.exists("/test")) {
    Serial.println("Test node exists!");
  } else {
    Serial.println("Test node doesn't exist!");
  }
  
  // Remove a node
  if (firebase.remove("/test/toBeRemoved")) {
    Serial.println("Node removed successfully");
  }
}

void loop() {
  firebase.loop(); // Always keep this in your loop
  
  // Your code here
  
  delay(1000);
}

#include "firebase-rtdbv2.h"
#include <WiFi.h>
#include <WiFiClientSecure.h>

#define WIFI_SSID "YourWiFiSSID"
#define WIFI_PASSWORD "YourWiFiPassword"

#define API_KEY "YourFirebaseAPIKey"
#define USER_EMAIL "YourEmail@example.com"
#define USER_PASSWORD "YourPassword"
#define DATABASE_URL "https://your-project-id-default-rtdb.firebaseio.com/"

WiFiClientSecure authClient;
WiFiClientSecure streamClient;
FirebaseV2RTDB firebase;

unsigned long sendDataPrevMillis = 0;
int count = 0;

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

  authClient.setInsecure();
  streamClient.setInsecure();
  
  if (!firebase.begin(authClient, DATABASE_URL, API_KEY, USER_EMAIL, USER_PASSWORD)) {
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

  // Setup streaming on a specific path
  if (!firebase.beginStream(streamClient, "/stream/data")) {
    Serial.println("Could not begin stream: " + firebase.getError());
    return;
  }
  
  Serial.println("Stream setup successful. Waiting for updates...");
  
  // Initialize data for the first time
  firebase.set("/stream/data/sensor1", 0);
  firebase.set("/stream/data/sensor2", 0);
  firebase.set("/stream/data/message", "Initializing...");
}

void loop() {
  firebase.loop(); // Always keep this in your loop
  
  // Check if any new data is available from the stream
  if (firebase.available()) {
    if (firebase.hasStreamData()) {
      // We have new data!
      Serial.println("=== New Stream Data ===");
      Serial.println("Event: " + firebase.getStreamEvent());
      Serial.println("Path: " + firebase.getStreamPath());
      Serial.println("Data: " + firebase.getStreamData());
      Serial.println("Type: " + String(firebase.getStreamDataType()));
      Serial.println("=======================");
    }
    
    if (firebase.hasError()) {
      Serial.println("Stream error: " + firebase.getError());
    }
  }
  
  // Update data periodically (every 5 seconds)
  if (millis() - sendDataPrevMillis > 5000 || sendDataPrevMillis == 0) {
    sendDataPrevMillis = millis();
    count++;
    
    // Update sensor values (simulated)
    int randomTemp = random(20, 30);
    int randomHumidity = random(40, 90);
    
    firebase.set("/stream/data/sensor1", randomTemp);
    firebase.set("/stream/data/sensor2", randomHumidity);
    firebase.set("/stream/data/message", "Update #" + String(count));
    
    Serial.println("Data updated - count: " + String(count));
  }
}

/**
 * Basic Firebase Cloud Messaging Example
 * 
 * This example shows how to initialize the Firebase Messaging Wrapper
 * and send a simple message to a topic.
 */

#define ENABLE_MODULE_FIREBASE_MESSAGING_V2
#include "Kinematrix.h"

#include <Arduino.h>
#include <WiFi.h>

// WiFi credentials
#define WIFI_SSID "YOUR_WIFI_SSID"
#define WIFI_PASSWORD "YOUR_WIFI_PASSWORD"

// Firebase project details
#define FIREBASE_PROJECT_ID "your-project-id"  // without .firebaseio.com
#define FIREBASE_CLIENT_EMAIL "your-service-account@your-project-id.iam.gserviceaccount.com"
const char PRIVATE_KEY[] PROGMEM = "-----BEGIN PRIVATE KEY-----\nYOUR_PRIVATE_KEY_HERE\n-----END PRIVATE KEY-----\n";

// Create messaging wrapper instance
FirebaseV2Messaging fcm;
bool messageSent = false;

void setup() {
  Serial.begin(115200);
  Serial.println();
  Serial.println("Firebase Cloud Messaging - Basic Example");
  Serial.println("---------------------------------------");
  
  // Connect to WiFi
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  Serial.print("Connecting to WiFi");
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print(".");
    delay(300);
  }
  Serial.println();
  Serial.print("Connected with IP: ");
  Serial.println(WiFi.localIP());
  Serial.println();
  
  // Set the time (required for authentication)
  configTime(0, 0, "pool.ntp.org", "time.nist.gov");
  Serial.println("Waiting for NTP time sync...");
  time_t now = time(nullptr);
  while (now < 1000000000) {
    delay(500);
    now = time(nullptr);
  }
  Serial.println("Time synchronized");
  
  // Initialize the Firebase Messaging Wrapper with service account credentials
  Serial.println("Initializing Firebase...");
  if (fcm.begin(FIREBASE_CLIENT_EMAIL, FIREBASE_PROJECT_ID, PRIVATE_KEY)) {
    Serial.println("Firebase initialized successfully!");
  } else {
    Serial.println("Failed to initialize Firebase");
    Serial.println("Error: " + fcm.getLastError());
  }
}

void loop() {
  // Process Firebase tasks
  fcm.loop();
  
  // Send a message once when Firebase is ready
  if (fcm.isReady() && !messageSent) {
    Serial.println("Firebase is ready. Sending message...");
    
    // Set message target (topic)
    fcm.setTopic("news");
    
    // Set notification content
    fcm.setNotification("Hello from ESP32", "This is a basic FCM message");
    
    // Send the message asynchronously
    fcm.sendMessage(false);
    
    messageSent = true;
    Serial.println("Message sent. Check for result in subsequent loops.");
  }
  
  // Check if we have a result from the async operation
  if (messageSent && fcm.isResultReady()) {
    Serial.println("FCM Result: " + fcm.getLastPayload());
    
    // Clear the message to prepare for the next one
    fcm.clearMessage();
    
    // In a real application, you might want to:
    // - Wait some time before sending another message
    // - Enter deep sleep
    // - etc.
    Serial.println("Message sending complete");
  }
  
  delay(1000);
}
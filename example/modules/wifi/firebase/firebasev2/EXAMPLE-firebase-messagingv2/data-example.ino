/**
 * Firebase Cloud Messaging Example with Custom Data
 * 
 * This example shows how to send FCM messages with custom data payload.
 * Custom data allows your app to process the message differently
 * or display custom content.
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
int messageCount = 0;

// Sensor simulation values
float temperature = 25.5;
float humidity = 58.2;

void setup() {
  Serial.begin(115200);
  Serial.println();
  Serial.println("Firebase Cloud Messaging - Custom Data Example");
  Serial.println("--------------------------------------------");
  
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
  
  // Simulate sensor reading
  temperature = 25.0 + random(-20, 20) / 10.0;
  humidity = 60.0 + random(-50, 50) / 10.0;
  
  // Send a message every 60 seconds when Firebase is ready
  static unsigned long lastSendTime = 0;
  if (fcm.isReady() && (millis() - lastSendTime > 60000 || lastSendTime == 0)) {
    if (!messageSent) {
      Serial.println("Firebase is ready. Sending message with sensor data...");
      
      // Set message target (topic)
      fcm.setTopic("sensors");
      
      // Set notification content
      String title = "Sensor Update #" + String(++messageCount);
      String body = "New sensor readings available";
      fcm.setNotification(title, body);
      
      // Add custom data - Add individual key-value pairs
      fcm.addData("temp", String(temperature, 1));
      fcm.addData("humidity", String(humidity, 1));
      fcm.addData("timestamp", String(time(nullptr)));
      fcm.addData("device_id", "ESP32_" + String((uint32_t)ESP.getEfuseMac(), HEX));
      
      // Alternative: Set all data at once with a JSON string
      /*
      String jsonData = "{";
      jsonData += "\"temp\":" + String(temperature, 1) + ",";
      jsonData += "\"humidity\":" + String(humidity, 1) + ",";
      jsonData += "\"timestamp\":" + String(time(nullptr)) + ",";
      jsonData += "\"device_id\":\"ESP32_" + String((uint32_t)ESP.getEfuseMac(), HEX) + "\"";
      jsonData += "}";
      fcm.setData(jsonData);
      */
      
      // Send the message asynchronously
      fcm.sendMessage(false);
      
      messageSent = true;
      lastSendTime = millis();
      Serial.println("Message sent. Check for result in subsequent loops.");
    }
  }
  
  // Check if we have a result from the async operation
  if (messageSent && fcm.isResultReady()) {
    Serial.println("FCM Result: " + fcm.getLastPayload());
    
    // Clear the message to prepare for the next one
    fcm.clearMessage();
    messageSent = false;
    
    Serial.println("Ready to send next message after delay");
  }
  
  delay(1000);
}
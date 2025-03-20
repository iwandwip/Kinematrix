/**
 * Firebase Cloud Messaging Example - Send to Specific Device
 * 
 * This example shows how to send FCM messages to a specific device
 * using a device registration token instead of a topic.
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

// Device registration tokens
// You would typically get these from a database or other source
// These are just example formats, replace with actual tokens from your app
const char* DEVICE_TOKENS[] = {
  "fQPxlLXsQGGg5XAcVzU2Lh:APA91bEJ8RPTFvRs6KJxo-LqFfHHWvSOGcGfPl7...", // Example Android token
  "dTEBw7Qcs0f9ZBCcnAdI6l:APA91bH2C3O7v1_i5-9vTXgczIhfQFhzBSyOGo8..."  // Example iOS token
};
const int NUM_DEVICES = sizeof(DEVICE_TOKENS) / sizeof(DEVICE_TOKENS[0]);

// Create messaging wrapper instance
FirebaseV2Messaging fcm;
int currentDeviceIndex = 0;
bool messageSent = false;

void setup() {
  Serial.begin(115200);
  Serial.println();
  Serial.println("Firebase Cloud Messaging - Send to Specific Device Example");
  Serial.println("--------------------------------------------------------");
  
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
  
  // Send a message to each device in sequence when Firebase is ready
  if (fcm.isReady() && currentDeviceIndex < NUM_DEVICES && !messageSent) {
    Serial.print("Firebase is ready. Sending message to device #");
    Serial.print(currentDeviceIndex + 1);
    Serial.println("...");
    
    // Target the specific device using its token
    fcm.setToken(DEVICE_TOKENS[currentDeviceIndex]);
    
    // Set notification content
    String title = "Direct message to Device #" + String(currentDeviceIndex + 1);
    String body = "This message was sent directly to your device";
    fcm.setNotification(title, body);
    
    // Add some custom data
    fcm.addData("type", "direct_message");
    fcm.addData("message_id", String(random(10000, 99999)));
    fcm.addData("timestamp", String(time(nullptr)));
    
    // For Android devices, set high priority
    fcm.setAndroidPriority(true);  // HIGH priority
    fcm.setAndroidNotificationPriority(2);  // HIGH notification priority
    
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
    messageSent = false;
    
    // Move to the next device
    currentDeviceIndex++;
    
    if (currentDeviceIndex < NUM_DEVICES) {
      Serial.println("Waiting 5 seconds before sending to next device...");
    } else {
      Serial.println("All messages sent. Demo complete.");
    }
    
    delay(5000);  // Wait 5 seconds between messages
  }
  
  delay(1000);
}
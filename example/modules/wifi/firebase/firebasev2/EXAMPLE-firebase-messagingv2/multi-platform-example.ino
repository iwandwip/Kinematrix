/**
 * Firebase Cloud Messaging Example - Platform-Specific Messages
 * 
 * This example shows how to send FCM messages with platform-specific
 * configurations for Android, iOS (APNS), and Web.
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
int messageType = 0;  // 0: Basic, 1: Android specific, 2: iOS specific, 3: Web specific
bool messageSent = false;

void setup() {
  Serial.begin(115200);
  Serial.println();
  Serial.println("Firebase Cloud Messaging - Multi-Platform Example");
  Serial.println("-----------------------------------------------");
  
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
  
  // Send different types of messages in sequence
  if (fcm.isReady() && messageType < 4 && !messageSent) {
    Serial.print("Firebase is ready. Preparing message type #");
    Serial.print(messageType);
    Serial.println("...");
    
    // Common message setup - target a topic
    fcm.setTopic("all_devices");
    
    // Basic notification that will appear on all platforms
    fcm.setNotification("Multi-platform message", 
                       "This shows different configs for each platform");
    
    // Add some common data
    fcm.addData("message_type", String(messageType));
    fcm.addData("timestamp", String(time(nullptr)));
    
    // Apply platform-specific configurations based on message type
    switch (messageType) {
      case 0:  // Basic message (no platform-specific settings)
        Serial.println("Sending basic message (no platform-specific settings)");
        break;
        
      case 1:  // Android-specific
        Serial.println("Sending with Android-specific configuration");
        
        // Set Android priority
        fcm.setAndroidPriority(true);  // HIGH priority
        fcm.setAndroidNotificationPriority(2);  // HIGH notification priority
        break;
        
      case 2:  // iOS-specific
        Serial.println("Sending with iOS-specific configuration");
        
        // Set APNS headers for iOS
        // These affect how notifications behave on iOS devices
        fcm.addApnsHeader("apns-priority", "10");  // High priority (10) vs normal (5)
        fcm.addApnsHeader("apns-push-type", "alert");  // alert, background, etc.
        break;
        
      case 3:  // Web-specific
        Serial.println("Sending with Web-specific configuration");
        
        // Set webpush headers for browsers
        fcm.addWebpushHeader("Urgency", "high");
        fcm.addWebpushHeader("TTL", "86400");  // Time-to-live in seconds (1 day)
        break;
    }
    
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
    
    // Move to the next message type
    messageType++;
    
    if (messageType < 4) {
      Serial.println("Waiting 10 seconds before sending next message type...");
    } else {
      Serial.println("All message types sent. Demo complete.");
    }
    
    delay(10000);  // Wait 10 seconds between messages
  }
  
  delay(1000);
}
/**
 * Firebase Cloud Messaging Example - Conditional Message Targeting
 * 
 * This example shows how to use FCM condition expressions to target
 * devices that are subscribed to specific combinations of topics.
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
int conditionIndex = 0;
bool messageSent = false;

// Example condition expressions to demonstrate targeting options
const char* CONDITIONS[] = {
  // Basic conditions
  "'premium' in topics",  // Devices subscribed to 'premium' topic
  
  // AND condition
  "'android' in topics && 'premium' in topics",  // Android premium users
  
  // OR condition
  "'ios' in topics || 'android' in topics",  // All mobile users
  
  // Complex condition
  "('ios' in topics || 'android' in topics) && 'high_value' in topics",  // High-value mobile users
  
  // More complex condition (up to 5 total operators allowed)
  "('ios' in topics || 'android' in topics) && ('premium' in topics || 'trial' in topics) && 'active' in topics"  // Active users on premium/trial
};
const int NUM_CONDITIONS = sizeof(CONDITIONS) / sizeof(CONDITIONS[0]);

void setup() {
  Serial.begin(115200);
  Serial.println();
  Serial.println("Firebase Cloud Messaging - Conditional Targeting Example");
  Serial.println("-----------------------------------------------------");
  
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
  
  // Send a message with each condition in sequence
  if (fcm.isReady() && conditionIndex < NUM_CONDITIONS && !messageSent) {
    Serial.print("Firebase is ready. Sending message with condition #");
    Serial.print(conditionIndex + 1);
    Serial.println("...");
    
    // Current condition to use
    const char* currentCondition = CONDITIONS[conditionIndex];
    Serial.print("Condition: ");
    Serial.println(currentCondition);
    
    // Set the condition
    fcm.setCondition(currentCondition);
    
    // Set notification content
    String title = "Conditional Message #" + String(conditionIndex + 1);
    String body = "You received this because you match the condition";
    fcm.setNotification(title, body);
    
    // Add condition information to the data
    fcm.addData("condition_used", String(currentCondition));
    fcm.addData("condition_index", String(conditionIndex + 1));
    fcm.addData("timestamp", String(time(nullptr)));
    
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
    
    // Move to the next condition
    conditionIndex++;
    
    if (conditionIndex < NUM_CONDITIONS) {
      Serial.println("Waiting 8 seconds before testing next condition...");
    } else {
      Serial.println("All conditions tested. Demo complete.");
    }
    
    delay(8000);  // Wait 8 seconds between messages
  }
  
  delay(1000);
}
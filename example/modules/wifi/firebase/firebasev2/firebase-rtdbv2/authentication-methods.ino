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

// Change this to test different authentication methods
#define AUTH_METHOD 1  // 1: Email/Password, 2: Anonymous

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
  
  bool firebaseInitialized = false;
  
  #if AUTH_METHOD == 1
    // Method 1: Email and Password Authentication
    Serial.println("Authenticating with email and password...");
    firebaseInitialized = firebase.begin(client, DATABASE_URL, API_KEY, USER_EMAIL, USER_PASSWORD);
  #elif AUTH_METHOD == 2
    // Method 2: Anonymous Authentication (no credentials)
    Serial.println("Authenticating anonymously...");
    firebaseInitialized = firebase.beginAnonymous(client, DATABASE_URL);
  #endif
  
  if (!firebaseInitialized) {
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
  
  // Test access by writing a timestamp
  String timestamp = String(millis());
  bool writeSuccess = false;
  
  #if AUTH_METHOD == 1
    // With Email/Password auth, we can write to protected paths
    writeSuccess = firebase.set("/users/authenticated/lastAccess", timestamp);
  #elif AUTH_METHOD == 2
    // With Anonymous auth, we can only write to public paths
    writeSuccess = firebase.set("/public/anonymous/lastAccess", timestamp);
  #endif
  
  if (writeSuccess) {
    Serial.println("Data written successfully at " + timestamp);
  } else {
    Serial.println("Failed to write data: " + firebase.getError());
  }
  
  // Display authentication method info
  #if AUTH_METHOD == 1
    Serial.println("\nUsing Email/Password Authentication");
    Serial.println("- Full read/write access based on Firebase rules");
    Serial.println("- User identified by email: " + String(USER_EMAIL));
  #elif AUTH_METHOD == 2
    Serial.println("\nUsing Anonymous Authentication");
    Serial.println("- Limited access based on Firebase rules");
    Serial.println("- No user identification");
  #endif
}

void loop() {
  firebase.loop(); // Always keep this in your loop
  
  // Periodically check if we're still authenticated
  static unsigned long lastAuthCheck = 0;
  if (millis() - lastAuthCheck > 10000) { // Every 10 seconds
    lastAuthCheck = millis();
    
    if (firebase.ready()) {
      Serial.println("Firebase connection still active");
    } else {
      Serial.println("Firebase connection lost, reconnecting...");
      // Here you could implement reconnection logic if needed
    }
  }
  
  delay(1000);
}

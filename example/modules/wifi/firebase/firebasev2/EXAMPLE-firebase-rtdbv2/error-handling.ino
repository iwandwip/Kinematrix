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

void logError(const String &operation, const String &path) {
  if (firebase.hasError()) {
    Serial.print("[ERROR] ");
    Serial.print(operation);
    Serial.print(" at path '");
    Serial.print(path);
    Serial.print("' failed: ");
    Serial.println(firebase.getError());
  }
}

void printOperationResult(bool success, const String &operation, const String &path) {
  if (success) {
    Serial.print("[SUCCESS] ");
    Serial.print(operation);
    Serial.print(" at path '");
    Serial.print(path);
    Serial.println("' successful.");
  } else {
    logError(operation, path);
  }
}

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
  
  Serial.println("\n=== Testing Firebase Connection ===");
  // Test initialization with error handling
  bool initSuccess = firebase.begin(client, DATABASE_URL, API_KEY, USER_EMAIL, USER_PASSWORD);
  if (!initSuccess) {
    Serial.println("Firebase initialization failed!");
    logError("Initialization", "");
    
    // Try with incorrect credentials to see error
    Serial.println("\nTrying with incorrect credentials on purpose...");
    initSuccess = firebase.begin(client, DATABASE_URL, API_KEY, "wrong@email.com", "wrongpassword");
    logError("Initialization with wrong credentials", "");
    
    // After seeing the error, try again with correct credentials
    Serial.println("\nRetrying with correct credentials...");
    initSuccess = firebase.begin(client, DATABASE_URL, API_KEY, USER_EMAIL, USER_PASSWORD);
    if (initSuccess) {
      Serial.println("Firebase initialization successful on retry!");
    } else {
      Serial.println("Firebase initialization failed again! Check your credentials.");
      return;
    }
  } else {
    Serial.println("Firebase initialization successful!");
  }
  
  // Wait for connection to be ready
  Serial.print("Waiting for Firebase to be ready");
  unsigned long timeout = millis() + 10000; // 10 second timeout
  while (!firebase.ready() && millis() < timeout) {
    Serial.print(".");
    delay(100);
  }
  
  if (!firebase.ready()) {
    Serial.println("\nTimeout waiting for Firebase to be ready!");
    return;
  }
  
  Serial.println("\nFirebase ready!");
  
  // === Testing various error scenarios ===
  Serial.println("\n=== Testing Error Scenarios ===");
  
  // 1. Test writing to an invalid path (containing forbidden characters)
  String invalidPath = "/test/#invalid/path";
  bool result = firebase.set(invalidPath, "This shouldn't work");
  printOperationResult(result, "Write to invalid path", invalidPath);
  
  // 2. Test reading from a non-existent path
  String nonExistentPath = "/this/path/does/not/exist";
  String value = firebase.getString(nonExistentPath);
  if (value.length() == 0 && firebase.hasError()) {
    Serial.print("[EXPECTED] Reading from non-existent path returned empty value. Error: ");
    Serial.println(firebase.getError());
  } else {
    Serial.println("[UNEXPECTED] Reading non-existent path didn't error as expected.");
  }
  
  // 3. Test writing a valid value
  String testPath = "/errorTests/validWrite";
  result = firebase.set(testPath, "This should work fine");
  printOperationResult(result, "Write valid data", testPath);
  
  // 4. Test reading a valid value
  value = firebase.getString(testPath);
  if (value.length() > 0 && !firebase.hasError()) {
    Serial.print("[SUCCESS] Read value from '");
    Serial.print(testPath);
    Serial.print("': ");
    Serial.println(value);
  } else {
    logError("Read", testPath);
  }
  
  // 5. Test checking if a path exists
  bool exists = firebase.exists(testPath);
  if (exists) {
    Serial.println("[SUCCESS] Path exists check passed for existing path");
  } else {
    logError("Exists check", testPath);
  }
  
  exists = firebase.exists(nonExistentPath);
  if (!exists) {
    Serial.println("[SUCCESS] Path exists check passed for non-existent path");
  } else {
    Serial.println("[UNEXPECTED] Path exists check failed for non-existent path");
  }
  
  // 6. Clean up test data
  result = firebase.remove("/errorTests");
  printOperationResult(result, "Clean up test data", "/errorTests");
  
  Serial.println("\n=== Error Testing Complete ===");
}

void loop() {
  firebase.loop(); // Always keep this in your loop
  
  // Periodically check for any errors in the connection
  static unsigned long lastErrorCheck = 0;
  if (millis() - lastErrorCheck > 5000) { // Every 5 seconds
    lastErrorCheck = millis();
    
    if (firebase.hasError()) {
      Serial.print("Firebase error detected: ");
      Serial.println(firebase.getError());
    }
  }
  
  delay(1000);
}

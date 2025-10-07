#include "firebase-firestorev2.h"
#include <WiFi.h>
#include <time.h>

#define WIFI_SSID "YourWiFiSSID"
#define WIFI_PASSWORD "YourWiFiPassword"

#define API_KEY "YourFirebaseAPIKey"
#define USER_EMAIL "YourEmail@example.com" 
#define USER_PASSWORD "YourPassword"
#define PROJECT_ID "your-project-id"

#define NTP_SERVER "pool.ntp.org"
#define GMT_OFFSET_SEC 0
#define DAYLIGHT_OFFSET_SEC 0

FirebaseV2Firestore firestore;
unsigned long lastOperationTime = 0;
int operationStep = 0;

void setup() {
  Serial.begin(115200);
  delay(1000);
  Serial.println("\n=== ESP32 Firestore Special Fields Example (Correct Format) ===");
  
  // Connect to WiFi
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  Serial.print("Connecting to WiFi");
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print(".");
    delay(300);
  }
  Serial.println();
  Serial.println("Connected with IP: " + WiFi.localIP().toString());
  
  // Initialize time (needed for auth)
  configTime(GMT_OFFSET_SEC, DAYLIGHT_OFFSET_SEC, NTP_SERVER);
  Serial.print("Waiting for NTP time sync: ");
  time_t now = time(nullptr);
  while (now < 8 * 3600 * 2) {
    delay(100);
    Serial.print(".");
    now = time(nullptr);
  }
  Serial.println();
  
  // Initialize Firestore
  if (firestore.begin(API_KEY, USER_EMAIL, USER_PASSWORD, PROJECT_ID)) {
    Serial.println("Firestore initialized successfully!");
  } else {
    Serial.println("Failed to initialize Firestore: " + firestore.getLastError());
    return;
  }
  
  // Reset operation sequence
  operationStep = 0;
  lastOperationTime = 0;
}

void loop() {
  // Process Firestore events
  firestore.loop();
  
  // Check if Firestore is ready for operations
  if (!firestore.isReady()) {
    Serial.println("Waiting for Firestore to be ready...");
    delay(1000);
    return;
  }
  
  // Perform operations with a delay between them
  unsigned long currentTime = millis();
  if (currentTime - lastOperationTime < 2000 && lastOperationTime != 0) {
    // Check if we have a result to process
    if (firestore.isResultReady()) {
      Serial.println("Got result: " + firestore.getLastPayload());
    }
    return;
  }
  
  lastOperationTime = currentTime;
  
  // Execute steps in sequence
  switch (operationStep) {
    case 0:
      Serial.println("\n1. Creating initial document with counter and array...");
      {
        // Initial document with counter, array, and nested map
        String initialData = "{"
          "\"fields\": {"
            "\"counter\": {\"integerValue\": 0},"
            "\"visits\": {\"integerValue\": 0},"
            "\"tags\": {"
              "\"arrayValue\": {"
                "\"values\": ["
                  "{\"stringValue\": \"iot\"},"
                  "{\"stringValue\": \"esp32\"}"
                "]"
              "}"
            "},"
            "\"settings\": {"
              "\"mapValue\": {"
                "\"fields\": {"
                  "\"notificationsEnabled\": {\"booleanValue\": false},"
                  "\"displayMode\": {\"stringValue\": \"dark\"}"
                "}"
              "}"
            "}"
          "}"
        "}";
        
        firestore.createDocument("demo/special_fields", initialData, false);
      }
      operationStep++;
      break;
      
    case 1:
      Serial.println("\n2. Reading initial document...");
      firestore.getDocument("demo/special_fields", "", false);
      operationStep++;
      break;
      
    case 2:
      Serial.println("\n3. Setting server timestamp field...");
      firestore.setServerTimestamp("demo/special_fields", "lastUpdated", false);
      operationStep++;
      break;
      
    case 3:
      Serial.println("\n4. Reading document with timestamp...");
      firestore.getDocument("demo/special_fields", "", false);
      operationStep++;
      break;
      
    case 4:
      Serial.println("\n5. Incrementing counter field by 5...");
      firestore.incrementFieldValue("demo/special_fields", "counter", 5, false);
      operationStep++;
      break;
      
    case 5:
      Serial.println("\n6. Reading document with incremented field...");
      firestore.getDocument("demo/special_fields", "", false);
      operationStep++;
      break;
      
    case 6:
      Serial.println("\n7. Appending items to array...");
      {
        // Create array value to append
        Values::StringValue val1("firebase");
        Values::StringValue val2("firestore");
        
        Values::ArrayValue arrayValue;
        arrayValue.values(Values::Value(val1));
        arrayValue.values(Values::Value(val2));
        
        firestore.appendArray("demo/special_fields", "tags", arrayValue, false);
      }
      operationStep++;
      break;
      
    case 7:
      Serial.println("\n8. Reading document with updated array...");
      firestore.getDocument("demo/special_fields", "", false);
      operationStep++;
      break;
      
    case 8:
      Serial.println("\n9. Adding field to nested map...");
      {
        // Add a field to the settings map
        Values::BooleanValue boolValue(true);
        Values::Value value(boolValue);
        
        firestore.appendMapValue("demo/special_fields", "settings", "autoSync", value, false);
      }
      operationStep++;
      break;
      
    case 9:
      Serial.println("\n10. Reading document with updated map...");
      firestore.getDocument("demo/special_fields", "", false);
      operationStep++;
      break;
      
    case 10:
      Serial.println("\n11. Cleaning up - deleting test document...");
      firestore.deleteDocument("demo/special_fields", false);
      operationStep++;
      break;
      
    default:
      // End of sequence, restart after a longer delay
      Serial.println("\nCompleted all special field operations. Restarting sequence in 20 seconds...");
      delay(20000);
      operationStep = 0;
      break;
  }
}

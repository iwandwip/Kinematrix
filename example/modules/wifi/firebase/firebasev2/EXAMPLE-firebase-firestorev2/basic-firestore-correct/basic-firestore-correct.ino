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
  Serial.println("\n=== ESP32 Firestore Basic Example (Correct Format) ===");
  
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
      Serial.println("\n1. Creating a document with correct format...");
      {
        // Create document with proper Firestore JSON format
        String documentData = "{"
          "\"fields\": {"
            "\"name\": {\"stringValue\": \"John Doe\"},"
            "\"age\": {\"integerValue\": 30},"
            "\"active\": {\"booleanValue\": true}"
          "}"
        "}";
        
        firestore.createDocument("users/user1", documentData, false);
      }
      operationStep++;
      break;
      
    case 1:
      Serial.println("\n2. Reading the document...");
      firestore.getDocument("users/user1", "", false);
      operationStep++;
      break;
      
    case 2:
      Serial.println("\n3. Updating the document with correct format...");
      {
        // Update document with proper Firestore JSON format
        String updateData = "{"
          "\"fields\": {"
            "\"age\": {\"integerValue\": 31},"
            "\"city\": {\"stringValue\": \"New York\"}"
          "}"
        "}";
        
        firestore.updateDocument("users/user1", updateData, "age,city", false);
      }
      operationStep++;
      break;
      
    case 3:
      Serial.println("\n4. Reading the updated document...");
      firestore.getDocument("users/user1", "", false);
      operationStep++;
      break;
      
    case 4:
      Serial.println("\n5. Creating a document with nested data...");
      {
        // Document with nested fields
        String documentData = "{"
          "\"fields\": {"
            "\"name\": {\"stringValue\": \"Jane Smith\"},"
            "\"age\": {\"integerValue\": 28},"
            "\"active\": {\"booleanValue\": true},"
            "\"address\": {"
              "\"mapValue\": {"
                "\"fields\": {"
                  "\"street\": {\"stringValue\": \"123 Main St\"},"
                  "\"city\": {\"stringValue\": \"Boston\"},"
                  "\"zipCode\": {\"stringValue\": \"02101\"}"
                "}"
              "}"
            "},"
            "\"scores\": {"
              "\"arrayValue\": {"
                "\"values\": ["
                  "{\"integerValue\": 95},"
                  "{\"integerValue\": 88},"
                  "{\"integerValue\": 92}"
                "]"
              "}"
            "}"
          "}"
        "}";
        
        firestore.createDocument("users/user2", documentData, false);
      }
      operationStep++;
      break;
      
    case 5:
      Serial.println("\n6. Reading document with nested data...");
      firestore.getDocument("users/user2", "", false);
      operationStep++;
      break;
      
    case 6:
      Serial.println("\n7. Deleting a document...");
      firestore.deleteDocument("users/user1", false);
      operationStep++;
      break;
      
    case 7:
      Serial.println("\n8. Deleting second document...");
      firestore.deleteDocument("users/user2", false);
      operationStep++;
      break;
      
    default:
      // End of sequence, restart after a longer delay
      Serial.println("\nCompleted all operations. Restarting sequence in 10 seconds...");
      delay(10000);
      operationStep = 0;
      break;
  }
}

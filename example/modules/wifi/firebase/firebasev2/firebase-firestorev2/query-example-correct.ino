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
  Serial.println("\n=== ESP32 Firestore Query Example (Correct Format) ===");
  
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
      Serial.println("\n1. Creating sample products for querying...");
      {
        // Laptop - Electronics - $1200 - In Stock
        String product1 = "{"
          "\"fields\": {"
            "\"name\": {\"stringValue\": \"Laptop\"},"
            "\"price\": {\"integerValue\": 1200},"
            "\"category\": {\"stringValue\": \"Electronics\"},"
            "\"inStock\": {\"booleanValue\": true}"
          "}"
        "}";
        firestore.createDocument("products/prod1", product1, false);
      }
      operationStep++;
      break;
      
    case 1:
      {
        // Smartphone - Electronics - $800 - In Stock
        String product2 = "{"
          "\"fields\": {"
            "\"name\": {\"stringValue\": \"Smartphone\"},"
            "\"price\": {\"integerValue\": 800},"
            "\"category\": {\"stringValue\": \"Electronics\"},"
            "\"inStock\": {\"booleanValue\": true}"
          "}"
        "}";
        firestore.createDocument("products/prod2", product2, false);
      }
      operationStep++;
      break;
      
    case 2:
      {
        // Headphones - Electronics - $150 - Not In Stock
        String product3 = "{"
          "\"fields\": {"
            "\"name\": {\"stringValue\": \"Headphones\"},"
            "\"price\": {\"integerValue\": 150},"
            "\"category\": {\"stringValue\": \"Electronics\"},"
            "\"inStock\": {\"booleanValue\": false}"
          "}"
        "}";
        firestore.createDocument("products/prod3", product3, false);
      }
      operationStep++;
      break;
      
    case 3:
      {
        // Book - Media - $25 - In Stock
        String product4 = "{"
          "\"fields\": {"
            "\"name\": {\"stringValue\": \"Book\"},"
            "\"price\": {\"integerValue\": 25},"
            "\"category\": {\"stringValue\": \"Media\"},"
            "\"inStock\": {\"booleanValue\": true}"
          "}"
        "}";
        firestore.createDocument("products/prod4", product4, false);
      }
      operationStep++;
      break;
      
    case 4:
      Serial.println("\n2. Listing all documents in collection...");
      firestore.listDocuments("products", 10, "", false);
      operationStep++;
      break;
      
    case 5:
      Serial.println("\n3. Query - Filter by category (Electronics)...");
      {
        // Query for all electronics products
        String query = "{"
          "\"structuredQuery\": {"
            "\"where\": {"
              "\"fieldFilter\": {"
                "\"field\": {\"fieldPath\": \"category\"},"
                "\"op\": \"EQUAL\","
                "\"value\": {\"stringValue\": \"Electronics\"}"
              "}"
            "}"
          "}"
        "}";
        
        firestore.runQuery("products", query, false);
      }
      operationStep++;
      break;
      
    case 6:
      Serial.println("\n4. Query - Filter by price (> 100)...");
      {
        // Query for products with price > 100
        String query = "{"
          "\"structuredQuery\": {"
            "\"where\": {"
              "\"fieldFilter\": {"
                "\"field\": {\"fieldPath\": \"price\"},"
                "\"op\": \"GREATER_THAN\","
                "\"value\": {\"integerValue\": 100}"
              "}"
            "}"
          "}"
        "}";
        
        firestore.runQuery("products", query, false);
      }
      operationStep++;
      break;
      
    case 7:
      Serial.println("\n5. Query - Filter with ordering by price...");
      {
        // Query with ordering
        String query = "{"
          "\"structuredQuery\": {"
            "\"orderBy\": [{"
              "\"field\": {\"fieldPath\": \"price\"},"
              "\"direction\": \"ASCENDING\""
            "}]"
          "}"
        "}";
        
        firestore.runQuery("products", query, false);
      }
      operationStep++;
      break;
      
    case 8:
      Serial.println("\n6. Query - Composite filter (Electronics AND inStock)...");
      {
        // Query for in-stock electronics products
        String query = "{"
          "\"structuredQuery\": {"
            "\"where\": {"
              "\"compositeFilter\": {"
                "\"op\": \"AND\","
                "\"filters\": ["
                  "{"
                    "\"fieldFilter\": {"
                      "\"field\": {\"fieldPath\": \"category\"},"
                      "\"op\": \"EQUAL\","
                      "\"value\": {\"stringValue\": \"Electronics\"}"
                    "}"
                  "},"
                  "{"
                    "\"fieldFilter\": {"
                      "\"field\": {\"fieldPath\": \"inStock\"},"
                      "\"op\": \"EQUAL\","
                      "\"value\": {\"booleanValue\": true}"
                    "}"
                  "}"
                "]"
              "}"
            "}"
          "}"
        "}";
        
        firestore.runQuery("products", query, false);
      }
      operationStep++;
      break;
      
    case 9:
      Serial.println("\n7. Query - With limit (2 products)...");
      {
        // Query with limit
        String query = "{"
          "\"structuredQuery\": {"
            "\"orderBy\": [{"
              "\"field\": {\"fieldPath\": \"price\"},"
              "\"direction\": \"DESCENDING\""
            "}],"
            "\"limit\": 2"
          "}"
        "}";
        
        firestore.runQuery("products", query, false);
      }
      operationStep++;
      break;
      
    case 10:
      Serial.println("\n8. Cleaning up - Delete products...");
      firestore.deleteDocument("products/prod1", false);
      operationStep++;
      break;
      
    case 11:
      firestore.deleteDocument("products/prod2", false);
      operationStep++;
      break;
      
    case 12:
      firestore.deleteDocument("products/prod3", false);
      operationStep++;
      break;
      
    case 13:
      firestore.deleteDocument("products/prod4", false);
      operationStep++;
      break;
      
    default:
      // End of sequence, restart after a longer delay
      Serial.println("\nCompleted all query operations. Restarting sequence in 20 seconds...");
      delay(20000);
      operationStep = 0;
      break;
  }
}

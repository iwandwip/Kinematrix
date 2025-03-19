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
  Serial.println("\n=== ESP32 Firestore Batch Operations Example (Correct Format) ===");
  
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
  if (currentTime - lastOperationTime < 3000 && lastOperationTime != 0) {
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
      Serial.println("\n1. Creating individual documents for batch operation tests...");
      {
        // Sensor 1 with correct format
        String docData1 = "{"
          "\"fields\": {"
            "\"name\": {\"stringValue\": \"Sensor 1\"},"
            "\"type\": {\"stringValue\": \"temperature\"},"
            "\"value\": {\"doubleValue\": 22.5}"
          "}"
        "}";
        firestore.createDocument("sensors/sensor1", docData1, false);
      }
      operationStep++;
      break;
      
    case 1:
      {
        // Sensor 2 with correct format
        String docData2 = "{"
          "\"fields\": {"
            "\"name\": {\"stringValue\": \"Sensor 2\"},"
            "\"type\": {\"stringValue\": \"humidity\"},"
            "\"value\": {\"doubleValue\": 65.3}"
          "}"
        "}";
        firestore.createDocument("sensors/sensor2", docData2, false);
      }
      operationStep++;
      break;
      
    case 2:
      {
        // Sensor 3 with correct format
        String docData3 = "{"
          "\"fields\": {"
            "\"name\": {\"stringValue\": \"Sensor 3\"},"
            "\"type\": {\"stringValue\": \"pressure\"},"
            "\"value\": {\"doubleValue\": 1013.2}"
          "}"
        "}";
        firestore.createDocument("sensors/sensor3", docData3, false);
      }
      operationStep++;
      break;
      
    case 3:
      Serial.println("\n2. Batch Get - Retrieving multiple documents at once...");
      // Use batchGet to retrieve multiple documents in a single request
      firestore.batchGet("sensors/sensor1,sensors/sensor2,sensors/sensor3", "", false);
      operationStep++;
      break;
      
    case 4:
      Serial.println("\n3. Batch Get with field mask - Retrieving only specific fields...");
      // Use batchGet with a field mask to retrieve only specific fields
      firestore.batchGet("sensors/sensor1,sensors/sensor2,sensors/sensor3", "name,value", false);
      operationStep++;
      break;
      
    case 5:
      Serial.println("\n4. Batch Write - Creating multiple documents at once...");
      {
        // Create multiple documents in a single batch write
        Document<Values::Value> batchDoc1, batchDoc2, batchDoc3;
        
        // First batch document - device info
        Values::MapValue deviceMap;
        deviceMap.add("model", Values::Value(Values::StringValue("ESP32")));
        deviceMap.add("firmware", Values::Value(Values::StringValue("1.0.1")));
        deviceMap.add("lastUpdate", Values::Value(Values::IntegerValue(time(nullptr))));
        batchDoc1.add("device", Values::Value(deviceMap));
        
        // Second batch document - location info
        Values::MapValue locationMap;
        locationMap.add("latitude", Values::Value(Values::DoubleValue(40.7128)));
        locationMap.add("longitude", Values::Value(Values::DoubleValue(-74.0060)));
        locationMap.add("accuracy", Values::Value(Values::DoubleValue(15.5)));
        batchDoc2.add("location", Values::Value(locationMap));
        
        // Third batch document - status info
        Values::MapValue statusMap;
        statusMap.add("batteryLevel", Values::Value(Values::IntegerValue(85)));
        statusMap.add("charging", Values::Value(Values::BooleanValue(false)));
        statusMap.add("uptime", Values::Value(Values::IntegerValue(3600)));
        batchDoc3.add("status", Values::Value(statusMap));
        
        // Create write operations for the batch
        Write write1, write2, write3;
        
        // Set the documents for each write
        write1.update(batchDoc1);
        write1.updatePath("batch/device1");
        
        write2.update(batchDoc2);
        write2.updatePath("batch/location1");
        
        write3.update(batchDoc3);
        write3.updatePath("batch/status1");
        
        // Add all writes to the batch
        Writes writes;
        writes.writes(write1);
        writes.writes(write2);
        writes.writes(write3);
        
        // Execute the batch write
        firestore.batchWrite(writes, false);
      }
      operationStep++;
      break;
      
    case 6:
      Serial.println("\n5. Verifying batch-created documents...");
      firestore.batchGet("batch/device1,batch/location1,batch/status1", "", false);
      operationStep++;
      break;
      
    case 7:
      Serial.println("\n6. Batch Write - Deleting multiple documents at once...");
      {
        // Set up batch delete operation
        Write write1, write2, write3;
        
        // Configure each write as a delete operation
        write1.deletePath("sensors/sensor1");
        write2.deletePath("sensors/sensor2");
        write3.deletePath("sensors/sensor3");
        
        // Add all writes to the batch
        Writes writes;
        writes.writes(write1);
        writes.writes(write2);
        writes.writes(write3);
        
        // Execute the batch delete
        firestore.batchWrite(writes, false);
      }
      operationStep++;
      break;
      
    case 8:
      Serial.println("\n7. Cleaning up batch documents...");
      {
        // Set up batch delete for the last set of documents
        Write write1, write2, write3;
        
        write1.deletePath("batch/device1");
        write2.deletePath("batch/location1");
        write3.deletePath("batch/status1");
        
        Writes writes;
        writes.writes(write1);
        writes.writes(write2);
        writes.writes(write3);
        
        firestore.batchWrite(writes, false);
      }
      operationStep++;
      break;
      
    default:
      // End of sequence, restart after a longer delay
      Serial.println("\nCompleted all batch operations. Restarting sequence in 30 seconds...");
      delay(30000);
      operationStep = 0;
      break;
  }
}

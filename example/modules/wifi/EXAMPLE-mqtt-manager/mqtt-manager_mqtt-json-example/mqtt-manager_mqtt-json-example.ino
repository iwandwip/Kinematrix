#include <Arduino.h>
#if defined(ESP8266)
  #include <ESP8266WiFi.h>
#elif defined(ESP32)
  #include <WiFi.h>
#endif

#define ENABLE_MODULE_MQTT_MANAGER
#include "Kinematrix.h"

// WiFi credentials
const char* WIFI_SSID = "YourWiFiSSID";
const char* WIFI_PASSWORD = "YourWiFiPassword";

// MQTT settings
const char* MQTT_BROKER = "broker.hivemq.com";
const int MQTT_PORT = 1883;

// MQTT topics
const char* TOPIC_SENSOR_DATA = "esp/sensors/data";
const char* TOPIC_COMMAND = "esp/sensors/command";

// Create MQTT manager instance
MQTTManager mqtt;

unsigned long lastPublishTime = 0;
const unsigned long PUBLISH_INTERVAL = 10000; // 10 seconds

// Simulated sensor values
float temperature = 22.5;
float humidity = 65.0;
float pressure = 1013.25;
int light = 500;
bool ledState = false;

void onMessageReceived(char* topic, byte* payload, unsigned int length) {
  Serial.print("Message received on topic: ");
  Serial.println(topic);
  
  // Only process if it's our command topic
  if (strcmp(topic, TOPIC_COMMAND) == 0) {
    // Parse the JSON message
    StaticJsonDocument<256> doc;
    DeserializationError error = deserializeJson(doc, payload, length);
    
    if (error) {
      Serial.print("JSON parsing failed: ");
      Serial.println(error.c_str());
      return;
    }
    
    // Check if the message contains a "led" command
    if (doc.containsKey("led")) {
      ledState = doc["led"].as<bool>();
      Serial.print("Setting LED state to: ");
      Serial.println(ledState ? "ON" : "OFF");
      
      // If using a real LED, control it here
      // digitalWrite(LED_PIN, ledState);
      
      // Send back the current state
      publishSensorData();
    }
  }
}

void publishSensorData() {
  if (!mqtt.isConnected()) {
    Serial.println("Not connected to MQTT broker");
    return;
  }
  
  // Create a JSON document
  StaticJsonDocument<256> doc;
  
  // Add sensor data
  doc["temperature"] = temperature;
  doc["humidity"] = humidity;
  doc["pressure"] = pressure;
  doc["light"] = light;
  doc["led"] = ledState;
  doc["timestamp"] = millis();
  
  // Serialize JSON to string
  String jsonString;
  serializeJson(doc, jsonString);
  
  // Publish to MQTT
  Serial.print("Publishing: ");
  Serial.println(jsonString);
  mqtt.publish(TOPIC_SENSOR_DATA, jsonString.c_str());
}

void setup() {
  Serial.begin(115200);
  Serial.println("\nMQTT JSON Example");
  
  // Connect to WiFi
  Serial.print("Connecting to WiFi...");
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  
  Serial.println(" connected!");
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());
  
  // Setup MQTT
  mqtt.beginMQTT(MQTT_BROKER, MQTT_PORT);
  
  // Set a unique client ID
  String clientId = "ESP_JSON_";
  #if defined(ESP8266)
    clientId += String(ESP.getChipId(), HEX);
  #elif defined(ESP32)
    clientId += String((uint32_t)(ESP.getEfuseMac() & 0xffffff), HEX);
  #endif
  mqtt.setClientId(clientId.c_str());
  
  // Set callback for incoming messages
  mqtt.setCallback(onMessageReceived);
  
  // Connect to MQTT broker
  Serial.print("Connecting to MQTT broker...");
  if (mqtt.connect()) {
    Serial.println(" connected!");
    
    // Subscribe to command topic
    mqtt.subscribe(TOPIC_COMMAND);
    Serial.print("Subscribed to: ");
    Serial.println(TOPIC_COMMAND);
    
    // Publish initial sensor data
    publishSensorData();
  } else {
    Serial.print(" failed with state: ");
    Serial.println(mqtt.getStateString());
  }
}

void loop() {
  // Keep MQTT connection alive
  mqtt.loop();
  
  // Publish sensor data periodically
  unsigned long currentTime = millis();
  if (currentTime - lastPublishTime >= PUBLISH_INTERVAL) {
    lastPublishTime = currentTime;
    
    // Update simulated sensor values with some random variation
    temperature = 22.0 + random(-20, 20) / 10.0;
    humidity = 65.0 + random(-50, 50) / 10.0;
    pressure = 1013.0 + random(-100, 100) / 10.0;
    light = 500 + random(-100, 100);
    
    // Publish the sensor data
    publishSensorData();
  }
}
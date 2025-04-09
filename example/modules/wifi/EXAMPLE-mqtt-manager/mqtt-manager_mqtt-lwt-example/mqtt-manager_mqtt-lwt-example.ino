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
const char* TOPIC_STATUS = "esp/device/status";
const char* TOPIC_COMMAND = "esp/device/command";
const char* TOPIC_DATA = "esp/device/data";

// Status messages
const char* STATUS_ONLINE = "online";
const char* STATUS_OFFLINE = "offline";

// Create MQTT manager instance
MQTTManager mqtt;

unsigned long lastPublishTime = 0;
const unsigned long PUBLISH_INTERVAL = 8000; // 8 seconds

void onMessageReceived(char* topic, byte* payload, unsigned int length) {
  // Convert payload to string
  char message[length + 1];
  memcpy(message, payload, length);
  message[length] = '\0';
  
  Serial.print("Message received on topic: ");
  Serial.print(topic);
  Serial.print(" - ");
  Serial.println(message);
  
  // Process commands here as needed
  if (strcmp(topic, TOPIC_COMMAND) == 0) {
    if (strcmp(message, "restart") == 0) {
      Serial.println("Restart command received! Will restart in 3 seconds...");
      delay(3000);
      ESP.restart();
    }
  }
}

void setup() {
  Serial.begin(115200);
  Serial.println("\nMQTT LWT (Last Will and Testament) Example");
  
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
  String clientId = "ESP_LWT_";
  #if defined(ESP8266)
    clientId += String(ESP.getChipId(), HEX);
  #elif defined(ESP32)
    clientId += String((uint32_t)(ESP.getEfuseMac() & 0xffffff), HEX);
  #endif
  mqtt.setClientId(clientId.c_str());
  
  // Set callback for incoming messages
  mqtt.setCallback(onMessageReceived);
  
  // Set Last Will and Testament message
  // This will be published if the device disconnects unexpectedly
  mqtt.setWill(TOPIC_STATUS, STATUS_OFFLINE, true, 1);
  
  // Connect to MQTT broker
  Serial.print("Connecting to MQTT broker...");
  if (mqtt.connect()) {
    Serial.println(" connected!");
    
    // Subscribe to command topic
    mqtt.subscribe(TOPIC_COMMAND);
    Serial.print("Subscribed to: ");
    Serial.println(TOPIC_COMMAND);
    
    // Publish online status (overrides the Will message)
    mqtt.publish(TOPIC_STATUS, STATUS_ONLINE, true);
    Serial.println("Published online status");
  } else {
    Serial.print(" failed with state: ");
    Serial.println(mqtt.getStateString());
  }
}

void loop() {
  // Keep MQTT connection alive
  mqtt.loop();
  
  // Check WiFi connection and reconnect if needed
  if (WiFi.status() != WL_CONNECTED) {
    Serial.println("WiFi connection lost! Reconnecting...");
    WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
    
    // Wait for reconnection
    int attempts = 0;
    while (WiFi.status() != WL_CONNECTED && attempts < 10) {
      delay(500);
      Serial.print(".");
      attempts++;
    }
    
    if (WiFi.status() == WL_CONNECTED) {
      Serial.println(" reconnected!");
    } else {
      Serial.println(" failed to reconnect!");
    }
  }
  
  // Publish a message periodically
  unsigned long currentTime = millis();
  if (currentTime - lastPublishTime >= PUBLISH_INTERVAL) {
    lastPublishTime = currentTime;
    
    if (mqtt.isConnected()) {
      // Create a data message
      String message = "Device running for " + String(currentTime / 1000) + " seconds";
      
      // Publish the message
      Serial.print("Publishing data: ");
      Serial.println(message);
      mqtt.publish(TOPIC_DATA, message.c_str());
    } else {
      Serial.println("Not connected to MQTT broker");
    }
  }
}
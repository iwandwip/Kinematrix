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

// MQTT base topic
const char* TOPIC_BASE = "home/livingroom";

// Create MQTT manager instance
MQTTManager mqtt;

unsigned long lastPublishTime = 0;
const unsigned long PUBLISH_INTERVAL = 5000; // 5 seconds

// Simulated sensor and device values
float temperature = 21.5;
int humidity = 55;
bool lights = false;
int brightness = 0;

void onMessageReceived(char* topic, byte* payload, unsigned int length) {
  // Convert payload to string
  char message[length + 1];
  memcpy(message, payload, length);
  message[length] = '\0';
  
  Serial.print("Message received on topic: ");
  Serial.print(topic);
  Serial.print(" - ");
  Serial.println(message);
  
  // Check if the topic is for controlling lights
  if (strstr(topic, "/lights/set") != NULL) {
    // Convert message to boolean
    lights = (strcmp(message, "ON") == 0 || strcmp(message, "1") == 0);
    Serial.print("Lights turned ");
    Serial.println(lights ? "ON" : "OFF");
    
    // Publish the new state
    mqtt.publishToTopic(TOPIC_BASE, "lights/state", lights ? "ON" : "OFF", true);
  }
  
  // Check if the topic is for controlling brightness
  else if (strstr(topic, "/brightness/set") != NULL) {
    // Convert message to integer
    brightness = atoi(message);
    Serial.print("Brightness set to: ");
    Serial.println(brightness);
    
    // Publish the new state
    mqtt.publishToTopic(TOPIC_BASE, "brightness/state", String(brightness).c_str(), true);
  }
}

void publishEnvironmentData() {
  if (!mqtt.isConnected()) {
    Serial.println("Not connected to MQTT broker");
    return;
  }
  
  // Publish temperature using publishToTopic helper
  mqtt.publishToTopic(TOPIC_BASE, "temperature", String(temperature).c_str());
  Serial.print("Published temperature: ");
  Serial.println(temperature);
  
  // Publish humidity using publishToTopic helper
  mqtt.publishToTopic(TOPIC_BASE, "humidity", String(humidity).c_str());
  Serial.print("Published humidity: ");
  Serial.println(humidity);
}

void setup() {
  Serial.begin(115200);
  Serial.println("\nMQTT Structured Topics Example");
  
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
  String clientId = "ESP_Topics_";
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
    
    // Subscribe to control topics
    String lightsTopic = String(TOPIC_BASE) + "/lights/set";
    String brightnessTopic = String(TOPIC_BASE) + "/brightness/set";
    
    mqtt.subscribe(lightsTopic.c_str());
    mqtt.subscribe(brightnessTopic.c_str());
    
    Serial.print("Subscribed to: ");
    Serial.println(lightsTopic);
    Serial.print("Subscribed to: ");
    Serial.println(brightnessTopic);
    
    // Publish initial states with retained flag
    mqtt.publishToTopic(TOPIC_BASE, "lights/state", lights ? "ON" : "OFF", true);
    mqtt.publishToTopic(TOPIC_BASE, "brightness/state", String(brightness).c_str(), true);
    
    // Publish initial sensor data
    publishEnvironmentData();
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
    
    if (mqtt.isConnected()) {
      // Simulate some variation in sensor readings
      temperature = 21.0 + random(-10, 10) / 10.0;
      humidity = 55 + random(-5, 5);
      
      // Publish the environment data
      publishEnvironmentData();
    } else {
      Serial.println("Not connected to MQTT broker");
    }
  }
}
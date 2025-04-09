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
const char* TOPIC_PUBLISH = "esp/test/data";
const char* TOPIC_SUBSCRIBE = "esp/test/command";

// Create MQTT manager instance
MQTTManager mqtt;

unsigned long lastPublishTime = 0;
const unsigned long PUBLISH_INTERVAL = 5000; // 5 seconds

void onMessageReceived(char* topic, byte* payload, unsigned int length) {
  // Convert payload to string
  char message[length + 1];
  memcpy(message, payload, length);
  message[length] = '\0';
  
  Serial.print("Message received on topic: ");
  Serial.print(topic);
  Serial.print(" - ");
  Serial.println(message);
}

void setup() {
  Serial.begin(115200);
  Serial.println("\nMQTT Basic Example");
  
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
  String clientId = "ESP_Basic_";
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
    mqtt.subscribe(TOPIC_SUBSCRIBE);
    Serial.print("Subscribed to: ");
    Serial.println(TOPIC_SUBSCRIBE);
    
    // Publish an initial message
    mqtt.publish(TOPIC_PUBLISH, "ESP device connected!");
  } else {
    Serial.print(" failed with state: ");
    Serial.println(mqtt.getStateString());
  }
}

void loop() {
  // Keep MQTT connection alive
  mqtt.loop();
  
  // Publish a message every 5 seconds
  unsigned long currentTime = millis();
  if (currentTime - lastPublishTime >= PUBLISH_INTERVAL) {
    lastPublishTime = currentTime;
    
    if (mqtt.isConnected()) {
      // Create a message with current uptime
      String message = "Uptime: " + String(currentTime / 1000) + " seconds";
      
      // Publish the message
      Serial.print("Publishing: ");
      Serial.println(message);
      mqtt.publish(TOPIC_PUBLISH, message.c_str());
    } else {
      Serial.println("Not connected to MQTT broker");
    }
  }
}
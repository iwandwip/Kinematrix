#ifndef KINEMATRIX_MQTT_MANAGER_H
#define KINEMATRIX_MQTT_MANAGER_H

#include <Arduino.h>
#include <PubSubClient.h>

#if defined(ESP8266)
#include <ESP8266WiFi.h>
#elif defined(ESP32)
#include <WiFi.h>
#else
#error "This library is only compatible with ESP8266 and ESP32!"
#endif

#include "ArduinoJson.h"

#define MAX_SUBSCRIPTIONS 10

class MQTTManager {
public:
    MQTTManager();

    bool beginWiFi(const char *ssid, const char *password, uint32_t timeout = 20000);
    bool isWiFiConnected();
    IPAddress getLocalIP();

    bool beginMQTT(const char *server, uint16_t port = 1883, const char *username = NULL, const char *password = NULL);
    void setClientId(const char *clientId);
    void setCallback(MQTT_CALLBACK_SIGNATURE);

    void setWill(const char *topic, const char *message, bool retained = false, uint8_t qos = 0);
    void clearWill();

    bool connect();
    bool reconnect(unsigned long interval = 5000);
    void disconnect();
    bool isConnected();
    int state();
    const char *getStateString();

    bool publish(const char *topic, const char *payload, bool retained = false);
    bool publish(const char *topic, const uint8_t *payload, unsigned int length, bool retained = false);

    bool publishToTopic(const char *baseTopic, const char *key, const char *value, bool retained = false);

    bool clearRetained(const char *topic);

    bool beginPublish(const char *topic, unsigned int plength, bool retained = false);
    size_t write(uint8_t data);
    size_t write(const uint8_t *buffer, size_t size);
    int endPublish();

    template<typename T>
    bool publishJson(const char *topic, const T &doc, bool retained = false) {
        String output;
        serializeJson(doc, output);
        return publish(topic, output.c_str(), retained);
    }

    bool subscribe(const char *topic, uint8_t qos = 0);
    bool unsubscribe(const char *topic);

    bool connectWithStatus(const char *statusTopic, const char *onlineMessage, const char *offlineMessage);
    void enableStatusMessages(const char *statusTopic, const char *onlineMessage, const char *offlineMessage);
    void disableStatusMessages();

    void loop();

    void setKeepAlive(uint16_t keepAlive);
    void setSocketTimeout(uint16_t timeout);
    bool setBufferSize(uint16_t size);
    uint16_t getBufferSize();
    void enableAutoReconnect(bool enable = true);

    PubSubClient &getClient();

private:
    WiFiClient wifiClient;
    PubSubClient mqttClient;

    String wifiSSID;
    String wifiPassword;
    unsigned long lastWifiCheckTime;

    String mqttServer;
    uint16_t mqttPort;
    String mqttUsername;
    String mqttPassword;
    String mqttClientId;

    String mqttWillTopic;
    String mqttWillMessage;
    bool mqttWillRetained;
    uint8_t mqttWillQos;
    bool hasWill;

    unsigned long lastReconnectAttempt;
    bool autoReconnect;

    bool statusEnabled;
    String statusTopic;
    String onlineMessage;
    String offlineMessage;
    bool wasConnected;

    struct Subscription {
        String topic;
        uint8_t qos;
        bool active;

        Subscription() : qos(0), active(false) {}
    };

    Subscription subscriptions[MAX_SUBSCRIPTIONS];
    int subscriptionCount;

    void reconnectWiFiIfNeeded();
    void resubscribeTopics();
};

#endif
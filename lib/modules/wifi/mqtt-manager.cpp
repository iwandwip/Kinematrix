#include "mqtt-manager.h"

MQTTManager::MQTTManager() :
        mqttClient(wifiClient),
        mqttPort(1883),
        hasWill(false),
        lastReconnectAttempt(0),
        lastWifiCheckTime(0),
        autoReconnect(true),
        statusEnabled(false),
        wasConnected(false),
        subscriptionCount(0) {

    char defaultId[15];
#if defined(ESP8266)
    sprintf(defaultId, "ESP_%06X", ESP.getChipId());
#elif defined(ESP32)
    sprintf(defaultId, "ESP_%06X", (uint32_t) (ESP.getEfuseMac() & 0xffffff));
#endif
    mqttClientId = defaultId;
}

bool MQTTManager::beginWiFi(const char *ssid, const char *password, uint32_t timeout) {
    this->wifiSSID = ssid;
    this->wifiPassword = password;

    WiFi.begin(ssid, password);

    uint32_t startTime = millis();
    while (WiFi.status() != WL_CONNECTED && millis() - startTime < timeout) {
        delay(100);
    }

    return (WiFi.status() == WL_CONNECTED);
}

bool MQTTManager::isWiFiConnected() {
    return WiFi.status() == WL_CONNECTED;
}

IPAddress MQTTManager::getLocalIP() {
    return WiFi.localIP();
}

bool MQTTManager::beginMQTT(const char *server, uint16_t port, const char *username, const char *password) {
    this->mqttServer = server;
    this->mqttPort = port;

    if (username) {
        this->mqttUsername = username;
    }

    if (password) {
        this->mqttPassword = password;
    }

    mqttClient.setServer(server, port);
    return true;
}

void MQTTManager::setClientId(const char *clientId) {
    this->mqttClientId = clientId;
}

void MQTTManager::setCallback(MQTT_CALLBACK_SIGNATURE) {
    mqttClient.setCallback(callback);
}

void MQTTManager::setWill(const char *topic, const char *message, bool retained, uint8_t qos) {
    this->mqttWillTopic = topic;
    this->mqttWillMessage = message;
    this->mqttWillRetained = retained;
    this->mqttWillQos = qos;
    this->hasWill = true;
}

void MQTTManager::clearWill() {
    this->hasWill = false;
}

bool MQTTManager::connect() {
    if (!isWiFiConnected()) {
        return false;
    }

    bool result = false;

    if (hasWill) {
        if (mqttUsername.length() > 0) {
            result = mqttClient.connect(
                    mqttClientId.c_str(),
                    mqttUsername.c_str(),
                    mqttPassword.c_str(),
                    mqttWillTopic.c_str(),
                    mqttWillQos,
                    mqttWillRetained,
                    mqttWillMessage.c_str()
            );
        } else {
            result = mqttClient.connect(
                    mqttClientId.c_str(),
                    mqttWillTopic.c_str(),
                    mqttWillQos,
                    mqttWillRetained,
                    mqttWillMessage.c_str()
            );
        }
    } else {
        if (mqttUsername.length() > 0) {
            result = mqttClient.connect(
                    mqttClientId.c_str(),
                    mqttUsername.c_str(),
                    mqttPassword.c_str()
            );
        } else {
            result = mqttClient.connect(mqttClientId.c_str());
        }
    }

    if (result) {
        resubscribeTopics();
    }

    return result;
}

bool MQTTManager::reconnect(unsigned long interval) {
    if (mqttClient.connected()) {
        return true;
    }

    unsigned long now = millis();
    if (now - lastReconnectAttempt > interval) {
        lastReconnectAttempt = now;

        if (connect()) {
            lastReconnectAttempt = 0;
            return true;
        }
    }

    return false;
}

void MQTTManager::disconnect() {
    mqttClient.disconnect();
}

bool MQTTManager::isConnected() {
    return mqttClient.connected();
}

int MQTTManager::state() {
    return mqttClient.state();
}

const char *MQTTManager::getStateString() {
    switch (mqttClient.state()) {
        case MQTT_CONNECTION_TIMEOUT:
            return "Connection timeout";
        case MQTT_CONNECTION_LOST:
            return "Connection lost";
        case MQTT_CONNECT_FAILED:
            return "Connect failed";
        case MQTT_DISCONNECTED:
            return "Disconnected";
        case MQTT_CONNECTED:
            return "Connected";
        case MQTT_CONNECT_BAD_PROTOCOL:
            return "Bad protocol";
        case MQTT_CONNECT_BAD_CLIENT_ID:
            return "Bad client ID";
        case MQTT_CONNECT_UNAVAILABLE:
            return "Server unavailable";
        case MQTT_CONNECT_BAD_CREDENTIALS:
            return "Bad credentials";
        case MQTT_CONNECT_UNAUTHORIZED:
            return "Unauthorized";
        default:
            return "Unknown state";
    }
}

bool MQTTManager::publish(const char *topic, const char *payload, bool retained) {
    if (!isConnected()) {
        return false;
    }

    return mqttClient.publish(topic, payload, retained);
}

bool MQTTManager::publish(const char *topic, const uint8_t *payload, unsigned int length, bool retained) {
    if (!isConnected()) {
        return false;
    }

    return mqttClient.publish(topic, payload, length, retained);
}

bool MQTTManager::publishToTopic(const char *baseTopic, const char *key, const char *value, bool retained) {
    String fullTopic = String(baseTopic) + "/" + String(key);
    return publish(fullTopic.c_str(), value, retained);
}

bool MQTTManager::clearRetained(const char *topic) {
    return publish(topic, "", true);
}

bool MQTTManager::beginPublish(const char *topic, unsigned int plength, bool retained) {
    if (!isConnected()) {
        return false;
    }

    return mqttClient.beginPublish(topic, plength, retained);
}

size_t MQTTManager::write(uint8_t data) {
    return mqttClient.write(data);
}

size_t MQTTManager::write(const uint8_t *buffer, size_t size) {
    return mqttClient.write(buffer, size);
}

int MQTTManager::endPublish() {
    return mqttClient.endPublish();
}

bool MQTTManager::subscribe(const char *topic, uint8_t qos) {
    if (!isConnected()) {
        return false;
    }

    bool result = mqttClient.subscribe(topic, qos);

    if (result) {
        for (int i = 0; i < subscriptionCount; i++) {
            if (subscriptions[i].active && subscriptions[i].topic == topic) {
                subscriptions[i].qos = qos;
                return result;
            }
        }

        if (subscriptionCount < MAX_SUBSCRIPTIONS) {
            subscriptions[subscriptionCount].topic = topic;
            subscriptions[subscriptionCount].qos = qos;
            subscriptions[subscriptionCount].active = true;
            subscriptionCount++;
        }
    }

    return result;
}

bool MQTTManager::unsubscribe(const char *topic) {
    if (!isConnected()) {
        return false;
    }

    bool result = mqttClient.unsubscribe(topic);

    if (result) {
        for (int i = 0; i < subscriptionCount; i++) {
            if (subscriptions[i].active && subscriptions[i].topic == topic) {
                subscriptions[i].active = false;

                for (int j = i; j < subscriptionCount - 1; j++) {
                    subscriptions[j] = subscriptions[j + 1];
                }

                subscriptionCount--;
                break;
            }
        }
    }

    return result;
}

bool MQTTManager::connectWithStatus(const char *statusTopic, const char *onlineMessage, const char *offlineMessage) {
    setWill(statusTopic, offlineMessage, true);

    bool success = connect();

    if (success) {
        publish(statusTopic, onlineMessage, true);
    }

    return success;
}

void MQTTManager::enableStatusMessages(const char *statusTopic, const char *onlineMessage, const char *offlineMessage) {
    this->statusTopic = statusTopic;
    this->onlineMessage = onlineMessage;
    this->offlineMessage = offlineMessage;
    this->statusEnabled = true;

    setWill(statusTopic, offlineMessage, true);

    if (isConnected()) {
        publish(statusTopic, onlineMessage, true);
    }

    wasConnected = isConnected();
}

void MQTTManager::disableStatusMessages() {
    if (statusEnabled && isConnected()) {
        clearRetained(statusTopic.c_str());

        clearWill();
    }

    statusEnabled = false;
}

void MQTTManager::loop() {
    reconnectWiFiIfNeeded();

    if (autoReconnect && isWiFiConnected() && !isConnected()) {
        reconnect();
    }

    bool connected = isConnected();
    if (statusEnabled && connected != wasConnected) {
        if (connected) {
            publish(statusTopic.c_str(), onlineMessage.c_str(), true);
        }
        wasConnected = connected;
    }

    if (connected) {
        mqttClient.loop();
    }
}

void MQTTManager::setKeepAlive(uint16_t keepAlive) {
    mqttClient.setKeepAlive(keepAlive);
}

void MQTTManager::setSocketTimeout(uint16_t timeout) {
    mqttClient.setSocketTimeout(timeout);
}

bool MQTTManager::setBufferSize(uint16_t size) {
    return mqttClient.setBufferSize(size);
}

uint16_t MQTTManager::getBufferSize() {
    return mqttClient.getBufferSize();
}

void MQTTManager::enableAutoReconnect(bool enable) {
    autoReconnect = enable;
}

PubSubClient &MQTTManager::getClient() {
    return mqttClient;
}

void MQTTManager::reconnectWiFiIfNeeded() {
    unsigned long currentTime = millis();

    if (currentTime - lastWifiCheckTime > 30000) {
        lastWifiCheckTime = currentTime;

        if (WiFi.status() != WL_CONNECTED) {
            WiFi.begin(wifiSSID.c_str(), wifiPassword.c_str());
            delay(500);
        }
    }
}

void MQTTManager::resubscribeTopics() {
    for (int i = 0; i < subscriptionCount; i++) {
        if (subscriptions[i].active) {
            mqttClient.subscribe(subscriptions[i].topic.c_str(), subscriptions[i].qos);
        }
    }
}
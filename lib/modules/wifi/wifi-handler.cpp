/*
 *  wifi_handler.cpp
 *
 *  Wi-Fi handler c
 *  Created on: 2023. 4. 3
 */

#include "wifi-handler.h"

#if defined(ESP32) || defined(ESP8266)

WiFiHandler::WiFiHandler() = default;

bool WiFiHandler::connectToWiFi(const char *ssid, const char *pwd, void (*reconnectCallback)(void)) {
    WiFiClass::mode(WIFI_OFF);
    delay(1000);
    WiFiClass::mode(WIFI_STA);
    WiFi.begin(ssid, pwd);

    Serial.print("Connecting to : ");
    Serial.println(ssid);
    while (WiFiClass::status() != WL_CONNECTED) {
        Serial.print(".");
        if (reconnectCallback != nullptr) reconnectCallback();
        delay(250);
    }
    Serial.println(WiFi.localIP());

    connect = true;
    return WiFiClass::status() == WL_CONNECTED;
}

bool WiFiHandler::isConnect() const {
    return connect;
}

#endif
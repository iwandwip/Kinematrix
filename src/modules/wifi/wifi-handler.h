/*
 *  wifi_handler.h
 *
 *  Wi-Fi handler module
 *  Created on: 2023. 4. 3
 */
#ifdef USE_WIFI_HANDLER
#pragma once

#ifndef WIFI_HANDLER_H
#define WIFI_HANDLER_H

#if defined(ESP32) || defined(ESP8266)

#include "Arduino.h"
#include "WiFi.h"

class WiFiHandler {
private:
    bool connect;
public:
    WiFiHandler();
    bool connectToWiFi(const char *ssid, const char *pwd, void (*reconnectCallback)(void) = nullptr);
    bool isConnect();
};


WiFiHandler::WiFiHandler() {
}

bool WiFiHandler::connectToWiFi(const char *ssid, const char *pwd, void (*reconnectCallback)(void)) {
    WiFi.mode(WIFI_OFF);
    delay(1000);
    WiFi.mode(WIFI_STA);
    WiFi.begin(ssid, pwd);

    Serial.print("Connecting to : ");
    Serial.println(ssid);
    while (WiFi.status() != WL_CONNECTED) {
        Serial.print(".");
        if (reconnectCallback != nullptr) reconnectCallback();
        delay(250);
    }
    Serial.println(WiFi.localIP());

    connect = true;
    return WiFi.status() == WL_CONNECTED;
}

bool WiFiHandler::isConnect() {
    return WiFi.status() == WL_CONNECTED;
}
#endif
#endif
#endif
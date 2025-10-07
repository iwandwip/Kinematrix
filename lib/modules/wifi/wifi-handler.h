/*
 *  wifi-handler.h
 *
 *  Wi-Fi handler module
 *  Created on: 2023. 4. 3
 */

#pragma once

#ifndef WIFI_HANDLER_H
#define WIFI_HANDLER_H

#if defined(ESP32) || defined(ESP8266)

#pragma message("[COMPILED]: wifi-handler.h")

#include "Arduino.h"
#include "WiFi.h"

class WiFiHandler {
private:
    bool connect;
public:
    WiFiHandler();
    bool connectToWiFi(const char *ssid, const char *pwd, void (*reconnectCallback)(void) = nullptr);
    bool isConnect() const;
};

#endif
#endif
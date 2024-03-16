/*
 *  firebase_handler.h
 *
 *  firebase handler module
 *  Created on: 2023. 4. 3
 */

#pragma once

#ifndef FIREBASE_HANDLER_H
#define FIREBASE_HANDLER_H

#if defined(ESP32) || defined(ESP8266)

#include "Arduino.h"
#include "Firebase_ESP_Client.h"
#include "WiFi.h"
#include "stdint.h"

#include "NTPClient.h"
#include "WiFiUdp.h"

#define NTP_SERVER "pool.ntp.org"
#define GMT_OFFSET_SEC 3600
#define GMT_OFFSET_WIB GMT_OFFSET_SEC * 7
#define DAYLIGHT_OFFSET 3600

typedef struct {
    String email;
    String password;
} FirebaseUser;

typedef struct {
    String apiKey;
    String databaseURL;
    FirebaseUser user;
} FirebaseAuthentication;

class FirebaseModule {
private:
    FirebaseData *fbdo;
    FirebaseAuth *auth;
    FirebaseConfig *config;
    FirebaseAuthentication *authentication;

    WiFiUDP *ntpUDP;
    NTPClient *timeClient;

    float *data;
    char **address;
    uint8_t dataLen = 0;
    uint8_t addrLen = 0;

    uint32_t sendTime;
    uint32_t getTime;
    bool connect;

public:
    FirebaseModule();
    ~FirebaseModule();
    bool init(FirebaseAuthentication *_authentication);
    bool connectToWiFi(const char *ssid, const char *pwd);
    bool isConnect();
    bool update(void (*onUpdate)(void) = nullptr);
    void clearData();
    void addData(float newData, const char *newAddressData);
    int getFreeHeapMemory();

    void sendData(uint32_t __time = 2000, void (*onSendData)(void) = nullptr);
    void setFloat(float floatData, const char *addrs);
    void setString(String strData, const char *addrs);
    float getData(const char *getAddress);
    String getStrData(const char *getAddress);
    String getStrTime();
    void waitConnection(uint32_t __tmr);
};

#endif
#endif  // FIREBASE_HANDLER_H
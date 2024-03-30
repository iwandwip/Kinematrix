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

    String *data;
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

    template<typename T>
    void addData(T newData, const char *newAddressData) {
        dataLen++;
        data = (String *) realloc(data, dataLen * sizeof(String));
        data[dataLen - 1] = String(newData);

        char **newAddress = (char **) realloc(address, (addrLen + 1) * sizeof(char *));
        if (newAddress == NULL) return;
        address = newAddress;
        address[addrLen] = (char *) malloc(strlen(newAddressData) + 1);
        if (address[addrLen] == NULL) return;
        strcpy(address[addrLen], newAddressData);
        addrLen++;
    }

    int getFreeHeapMemory();

    void sendDataFloat(void (*onSendData)(void) = nullptr);
    void sendDataAsyncFloat(uint32_t __time = 2000, void (*onSendData)(void) = nullptr);
    void sendDataString(void (*onSendData)(void) = nullptr);
    void sendDataAsyncString(uint32_t __time = 2000, void (*onSendData)(void) = nullptr);

    template<typename T, typename U = const char *>
    void set(T val, U addr) {
        if (Firebase.ready()) {
            Firebase.RTDB.set(fbdo, addr, val);
        }
    }

    void setFloat(float floatData, const char *addrs);
    void setString(String strData, const char *addrs);

    float getData(const char *getAddress);
    String getStrData(const char *getAddress);
    String getStrTime();
    void waitConnection(uint32_t __tmr);
};

#endif
#endif  // FIREBASE_HANDLER_H
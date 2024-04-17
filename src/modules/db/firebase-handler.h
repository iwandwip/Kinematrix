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
    FirebaseData fbdo;
    FirebaseAuth auth;
    FirebaseConfig config;
    FirebaseAuthentication *authentication;

    WiFiUDP *ntpUDP;
    NTPClient *timeClient;

    String *data;
    String *address;
    uint8_t dataLen;
    uint8_t addrLen;
    uint8_t maxDataLen;
    uint8_t dataCount;

    uint32_t sendTime;
    uint32_t getTime;
    bool connect;

public:
    FirebaseModule();
    ~FirebaseModule();

    bool init(FirebaseAuthentication *_authentication, void (*initCallback)(void) = nullptr);
    bool initNTP();
    bool connectToWiFi(const char *ssid, const char *pwd, void (*connectCallback)(void) = nullptr);
    bool isConnect();

    bool update(void (*onUpdate)(void) = nullptr);
    void initData(uint8_t totalDataLen);

    template<typename T>
    void addData(T newData, const char *newAddressData) {
        if (dataLen >= maxDataLen || addrLen >= maxDataLen) return;

        data[dataLen] = String(newData);
        address[addrLen] = String(newAddressData);

        dataLen++;
        addrLen++;
    }

    int getFreeHeapMemory();

    void sendDataFloat(void (*onSendData)(String data, String address) = nullptr);
    void sendDataAsyncFloat(uint32_t __time = 2000, void (*onSendData)(String data, String address) = nullptr);
    void sendDataAsyncOrderFloat(uint32_t __time = 200, void (*onSendData)(String data, String address) = nullptr);
    void sendDataString(void (*onSendData)(String data, String address) = nullptr);
    void sendDataAsyncString(uint32_t __time = 2000, void (*onSendData)(String data, String address) = nullptr);
    void sendDataAsyncOrderString(uint32_t __time = 200, void (*onSendData)(String data, String address) = nullptr);

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
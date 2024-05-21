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

#include "ArduinoJson.h"

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
    String projectID;
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
    bool onTask(void (*onTask)(FirebaseData fbdo, FirebaseAuthentication auth, FirebaseConfig config) = nullptr);
    void initData(uint8_t totalDataLen);

    template<typename T = const char *>
    void addData(T newData, const char *newAddressData, bool _debug = false) {
        if (dataLen >= maxDataLen || addrLen >= maxDataLen) return;
        data[dataLen] = String(newData);
        address[addrLen] = String(newAddressData);
        dataLen++;
        addrLen++;
        if (_debug) debug("addData", String(newData), String(newAddressData), true);
    }

    void sendDataFloat(void (*onSendData)(String data, String address) = nullptr,
                       void (*resultCb)(String, String) = nullptr);
    void sendDataAsyncFloat(uint32_t __time = 2000, void (*onSendData)(String data, String address) = nullptr,
                            void (*resultCb)(String, String) = nullptr);
    void sendDataAsyncOrderFloat(uint32_t __time = 200, void (*onSendData)(String data, String address) = nullptr,
                                 void (*resultCb)(String, String) = nullptr);
    void sendDataString(void (*onSendData)(String data, String address) = nullptr,
                        void (*resultCb)(String, String) = nullptr);
    void sendDataAsyncString(uint32_t __time = 2000, void (*onSendData)(String data, String address) = nullptr,
                             void (*resultCb)(String, String) = nullptr, void (*onDone)() = nullptr);
    void sendDataAsyncOrderString(uint32_t __time = 200, void (*onSendData)(String data, String address) = nullptr,
                                  void (*resultCb)(String, String) = nullptr, void (*onDone)() = nullptr);

    template<typename T, typename U = const char *>
    void set(T val, U addr) {
        if (Firebase.ready()) {
            Firebase.RTDB.set(fbdo, addr, val);
        }
    }

    void setFloat(float floatData, const char *addrs, void (*onData)(float data, String address) = nullptr);
    void setString(String strData, const char *addrs, void (*onData)(String data, String address) = nullptr);

    void setJson(String getAddress, JsonVariant (*jsonCallback)(JsonVariant), void (*resultCb)(String, String) = nullptr);
    void getJson(String getAddress, void (*jsonCallback)(JsonVariant) = nullptr,
                 void (*resultCb)(String, String) = nullptr);

    float getData(const char *getAddress, void (*onData)(float data, String address) = nullptr);
    String getStrData(const char *getAddress, void (*onData)(String data, String address) = nullptr);
    String getStrTime();

    void waitConnection(uint32_t __tmr);
    void debug(String header, String _data, String _address, bool endl = true);
    int getFreeHeapMemory();

    void firestoreListDocument(String collectionId, size_t pageSize, void (*jsonCallback)(JsonVariant, size_t),
                               void (*resultCb)(String, String) = nullptr);
    String firestoreUpdateDocument(String collectionId, String documentId, JsonVariant (*jsonCallback)(JsonVariant),
                                   void (*resultCb)(String, String) = nullptr);

    String firestoreGetDocumentId(JsonVariant res);
    static void resultStatusCallback(String res, String err);
};

#endif
#endif  // FIREBASE_HANDLER_H
/*
 *  firebase-handler.h
 *
 *  firebase handler module
 *  Created on: 2023. 4. 3
 */

#pragma once

#ifndef FIREBASE_HANDLER_H
#define FIREBASE_HANDLER_H

#if defined(ESP32) || defined(ESP8266)

#pragma message("[COMPILED]: firebase-handler.h")

#include "Arduino.h"
#include "Firebase_ESP_Client.h"
#include "addons/RTDBHelper.h"
#include "addons/TokenHelper.h"
#include "WiFi.h"

#include "NTPClient.h"
#include "WiFiUdp.h"

#include "ArduinoJson.h"

#define DEVICE_REGISTRATION_ID_TOKEN "fFJtvdjCQbGFJDMDBXxSra:APA91bFuRqs34AL381zynRS_tIWhXprJUzFOVE2s6WNERx6YrOyxYTHP0B5UD82kU6oVEU_dP8d2VsQ8SyhzuDtZ27rW7cpzcUCMQOUsDEA5bTQmqhkw7HutgjjxbuTIcYgWsOXSwYBH"
const char PRIVATE_KEY[] PROGMEM = "-----BEGIN PRIVATE KEY-----\nMIIEvQIBADANBgkqhkiG9w0BAQEFAASCBKcwggSjAgEAAoIBAQC3A/owTfbnMf5h\ni85wPiQQeZDMPOMaQNdgDic1io+gTKiBEdbNv06ceoSxRtgv7ahGxd04RqwLo+mE\nJjCoy55niE0YNC1OHKbEeTOLVl9mU+ObeRjGvN7gSnkSdLNQimHN5xOMMqPLeJP2\nOlc35NwX8eSgZf1yEqKSuUObyYn8nBDr/fsuT4STIYqL1+sEH8Xj6BwRu/skMy00\no4hO24D0z/mRKV3/sfNIjQNrVH9zy82GjXFWOwcRRNSTl3/fKAnP1cYfXedipwxE\nI/SvZblvzS2TeiMKq7ojmDAOfkVvOKQ0GVK7tgLP8Zh3L7Oy0UML1J/ErxV6T+sR\nt9HqJTSZAgMBAAECggEAE+9EeK99Vdh9t11VmqdCIWNEcOKu8AShEQ7pmWzi0RiY\ngBqT1axhYSz9OBwZ3zqZSkAyJPeZtNjhTiokwbYQJoHTt++zAZ+gUKVNBn/de1Vm\n5N/vYXJgKnkiL+7zOzIfjiB94hb//WFAaKEZL/bnPFp2Rzxku2CpQacZZKYyuAeO\nqfqH664/CmqZ/q3JdNt8iQIW25c6M6ShEd+wPE14xxn+M8fPsB2FaNmU18xfXZz9\nUS8cP29JLHGMh1Wo5idM/UCld8I5kUnNBunbjEw95iwv/VOEXz1kH7JMdm7piCzR\nR/9w/osvwzDj658ucLWBlP/+WF/7IUEKS9eYh5D12wKBgQDeqnTcz0FVw6OLxXR6\nF/xIILfxLfLUn0D/n3bVGSEcs5k+aKDJx/45Q/u2kF+LJdGQIG6N8YTIFCHypSLY\nVvE2gc+eoTwmLO4TPxMbsVi9ph6CsVjwjTqNvVxMBJClzyitLJCSJEZ4WkC94hbw\nN0DZxR0uJobCBYH5vGhSgCGLCwKBgQDSafHpIHPX5Leg4OXwB5fLkKLplC/wI7HV\nj0QdKgMCUDZX+PZV2z4d4T5zzjxwZ7attkrgExbM3ju3x66nEpR903qpXrEnmFRZ\n1tN4rgO5X+wqMUbPKUz5y4vpLTiFtj+SuvqhyWcaMtIlyMm4yCYo9S6QiRO0izrf\nENqlecalawKBgQDE3IORrZOgFeYEb0C5qXcc9gBqtSsaHOrEleHkKeGClK8J5Esz\n2yPC0X48PIycY4VSHbpguIWit1oEQ0f5aXS9sisznm2letfKIFvuge9pTPNwrQdr\nNhHsutxdQTP5xJu778ipzHSZd4IB32BUZSVq48EWym0eIOx3NcIuHqaJXQKBgEJX\nQv2EzOF57Ged4QF/810sWiEZJtApAO3bgNdNIqu9DJVKGqjROlGDbgRnNukd2hwF\nCLCDdEHdo6h7HdbxZb4SSm9SOrrILXgx4nT/MI9m7fgTl0bDt9BeW9uF9Xc7Eeo6\nZ8FQ1eANu/JLz0YiXN23CBpsDblz2HjyQG3B6l7HAoGAI5TgTmrl113iKGVohA1m\n0xDTcjiSldkxWSHBw2Mm0NatQKmkU0Xxkfag9ZFIl7hD+iJhSduJWq7bssI38D3M\ncY0Q/pZVnwdW6lnzsI9rfKV/FFi9Hf3zxy0nr5+cYe11pmUWypM8M6OQBgIDBt+u\ndlg8h/pPwd7yBPqmMY7dVgg=\n-----END PRIVATE KEY-----\n";

typedef struct {
    String email;
    String password;
} FirebaseUser;

typedef struct {
    String apiKey;
    String databaseURL;
    String projectID;
    String clientEmail;
    FirebaseUser user;
} FirebaseAuthentication;

class FirebaseModule {
private:
    FirebaseData fbdo;
    FirebaseAuth auth;
    FirebaseConfig config;
    FirebaseAuthentication *authentication;

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

    bool init(FirebaseAuthentication *_authentication, void (*initCallback)() = nullptr);
    bool connectToWiFi(const char *ssid, const char *pwd, void (*connectCallback)() = nullptr);
    bool isConnect() const;

    bool update(void (*onUpdate)() = nullptr);
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
                       void (*resultCb)(const String &, const String &) = nullptr);
    void sendDataAsyncFloat(uint32_t _time = 2000, void (*onSendData)(String data, String address) = nullptr,
                            void (*resultCb)(const String &, const String &) = nullptr);
    void sendDataAsyncOrderFloat(uint32_t _time = 200, void (*onSendData)(String data, String address) = nullptr,
                                 void (*resultCb)(const String &, const String &) = nullptr);
    void sendDataString(void (*onSendData)(String data, String address) = nullptr,
                        void (*resultCb)(const String &, const String &) = nullptr);
    void sendDataAsyncString(uint32_t _time = 2000, void (*onSendData)(String data, String address) = nullptr,
                             void (*resultCb)(const String &, const String &) = nullptr, void (*onDone)() = nullptr);
    void sendDataAsyncOrderString(uint32_t _time = 200, void (*onSendData)(String data, String address) = nullptr,
                                  void (*resultCb)(const String &, const String &) = nullptr,
                                  void (*onDone)() = nullptr);

    template<typename T, typename U = const char *>
    void set(T val, U addr) {
        if (Firebase.ready()) {
            Firebase.RTDB.set(fbdo, addr, val);
        }
    }

    void setFloat(float floatData, const char *addrs, void (*onData)(float data, String address) = nullptr);
    void setString(const String &strData, const char *addrs, void (*onData)(String data, String address) = nullptr);

    void
    setJson(const String &getAddress, JsonVariant variant, void (*resultCb)(const String &, const String &) = nullptr);
    void
    pushJson(const String &getAddress, JsonVariant variant, void (*resultCb)(const String &, const String &) = nullptr);

    void setJson(const String &getAddress, JsonVariant (*jsonCallback)(JsonVariant),
                 void (*resultCb)(const String &, const String &) = nullptr);
    void pushJson(const String &getAddress, JsonVariant (*jsonCallback)(JsonVariant),
                  void (*resultCb)(const String &, const String &) = nullptr);
    void getJson(const String &getAddress, void (*jsonCallback)(JsonVariant) = nullptr,
                 void (*resultCb)(const String &, const String &) = nullptr);

    float getData(const char *getAddress, void (*onData)(float data, String address) = nullptr);
    String getStrData(const char *getAddress, void (*onData)(String data, String address) = nullptr);

    void waitConnection(uint32_t _tmr);
    void debug(const String &header, const String &_data, const String &_address, bool endl = true);
    int getFreeHeapMemory();

    void firestoreListDocument(const String &collectionId, size_t pageSize, void (*jsonCallback)(JsonVariant, size_t),
                               void (*resultCb)(const String &, const String &) = nullptr);
    String firestoreUpdateDocument(const String &collectionId, const String &documentId,
                                   JsonVariant (*jsonCallback)(JsonVariant),
                                   void (*resultCb)(const String &, const String &) = nullptr);
    String firestoreGetDocumentId(JsonVariant res);
    static void resultStatusCallback(const String &res, const String &err);

    void createDocument();
    void sendMessage();
};

#endif
#endif  // FIREBASE_HANDLER_H
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

#ifndef KINEMATRIX_ENABLE_FIREBASE
#define KINEMATRIX_ENABLE_FIREBASE_GUARD 0
#else
#define KINEMATRIX_ENABLE_FIREBASE_GUARD 1
#endif

#if KINEMATRIX_ENABLE_FIREBASE_GUARD

#include "Arduino.h"
#include "Firebase_ESP_Client.h"
#include "addons/RTDBHelper.h"
#include "addons/TokenHelper.h"
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

    void setJson(String getAddress, JsonVariant variant, void (*resultCb)(String, String) = nullptr);
    void pushJson(String getAddress, JsonVariant variant, void (*resultCb)(String, String) = nullptr);

    void setJson(String getAddress, JsonVariant (*jsonCallback)(JsonVariant),
                 void (*resultCb)(String, String) = nullptr);
    void pushJson(String getAddress, JsonVariant (*jsonCallback)(JsonVariant),
                  void (*resultCb)(String, String) = nullptr);
    void getJson(String getAddress, void (*jsonCallback)(JsonVariant) = nullptr,
                 void (*resultCb)(String, String) = nullptr);

    float getData(const char *getAddress, void (*onData)(float data, String address) = nullptr);
    String getStrData(const char *getAddress, void (*onData)(String data, String address) = nullptr);

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

FirebaseModule::FirebaseModule()
        : data(nullptr),
          address(nullptr),
          dataLen(0),
          addrLen(0) {
}

FirebaseModule::~FirebaseModule() {
}

bool FirebaseModule::init(FirebaseAuthentication *_authentication, void (*initCallback)(void)) {
    authentication = _authentication;

    config.api_key = authentication->apiKey;
    config.database_url = authentication->databaseURL;
    auth.user.email = authentication->user.email;
    auth.user.password = authentication->user.password;

    config.token_status_callback = tokenStatusCallback;
    Firebase.reconnectNetwork(true);

    fbdo.setBSSLBufferSize(4096, 1024);
    fbdo.setResponseSize(4096);

    if (initCallback != nullptr) {
        Serial.print("| [INFO] FreeHeap: ");
        Serial.println(Firebase.getFreeHeap());
    }

    Firebase.reset(&config);
    Firebase.begin(&config, &auth);

    Firebase.setDoubleDigits(2);  // default 5
    Firebase.setFloatDigits(2);

    config.timeout.serverResponse = 10 * 1000;
//    config.timeout.networkReconnect = 10 * 1000;
//    config.timeout.socketConnection = 10 * 1000;

    if (initCallback != nullptr) initCallback();
    return true;
}

bool FirebaseModule::connectToWiFi(const char *ssid, const char *pwd, void (*connectCallback)(void)) {
    WiFi.mode(WIFI_OFF);
    delay(1000);
    WiFi.mode(WIFI_STA);
    WiFi.begin(ssid, pwd);

    Serial.print("Connecting to : ");
    Serial.println(ssid);
    while (WiFi.status() != WL_CONNECTED) {
        Serial.print(".");
        if (connectCallback != nullptr) connectCallback();
        delay(250);
    }
    Serial.println(WiFi.localIP());

    connect = true;
    return WiFi.status() == WL_CONNECTED;
}

bool FirebaseModule::isConnect() {
    return (connect || WiFi.status() == WL_CONNECTED) && Firebase.ready();
}

bool FirebaseModule::update(void (*onUpdate)(void)) {
    if (onUpdate != nullptr) onUpdate();
    return true;
}

bool FirebaseModule::onTask(
        void (*onTask)(FirebaseData fbdo, FirebaseAuthentication auth, FirebaseConfig config)) {
    if (onTask != nullptr) {
        onTask(fbdo, *authentication, config);
    }
}

void FirebaseModule::initData(uint8_t totalDataLen) {
    if (data != nullptr) {
        delete[] data;
    }
    if (address != nullptr) {
        delete[] address;
    }
    dataLen = 0;
    addrLen = 0;

    maxDataLen = totalDataLen;
    data = new String[maxDataLen];
    address = new String[maxDataLen];
}

void FirebaseModule::sendDataFloat(void (*onSendData)(String data, String address), void (*resultCb)(String, String)) {
    if (data == nullptr) return;
    for (uint8_t i = 0; i < dataLen; i++) {
        String res, err;
        if (Firebase.RTDB.setFloat(&fbdo, address[i], data[i].toFloat())) {
            res = "ok";
            if (onSendData != nullptr) onSendData(data[i], address[i]);
        } else {
            err = fbdo.errorReason();
        }
        if (resultCb != nullptr) resultCb(res, err);
    }
}

void FirebaseModule::sendDataAsyncFloat(uint32_t __time, void (*onSendData)(String data, String address),
                                        void (*resultCb)(String, String)) {
    if (data == nullptr) return;
    if (isConnect() && millis() - sendTime >= __time) {
        for (uint8_t i = 0; i < dataLen; i++) {
            String res, err;
            if (Firebase.RTDB.setFloat(&fbdo, address[i], data[i].toFloat())) {
                res = "ok";
                if (onSendData != nullptr) onSendData(data[i], address[i]);
            } else {
                err = fbdo.errorReason();
            }
            if (resultCb != nullptr) resultCb(res, err);
        }
        sendTime = millis();
    }
}

void FirebaseModule::sendDataAsyncOrderFloat(uint32_t __time, void (*onSendData)(String data, String address),
                                             void (*resultCb)(String, String)) {
    if (data == nullptr) return;
    if (isConnect() && millis() - sendTime >= __time) {
        if (dataCount >= maxDataLen) dataCount = 0;
        String res, err;
        if (Firebase.RTDB.setFloat(&fbdo, address[dataCount], data[dataCount].toFloat())) {
            res = "ok";
            if (onSendData != nullptr) onSendData(data[dataCount], address[dataCount]);
        } else {
            err = fbdo.errorReason();
        }
        if (resultCb != nullptr) resultCb(res, err);
        dataCount++;
        sendTime = millis();
    }
}

void FirebaseModule::sendDataString(void (*onSendData)(String data, String address), void (*resultCb)(String, String)) {
    if (data == nullptr) return;
    for (uint8_t i = 0; i < dataLen; i++) {
        String res, err;
        if (Firebase.RTDB.setString(&fbdo, address[i], data[i])) {
            res = "ok";
            if (onSendData != nullptr) onSendData(data[i], address[i]);
        } else {
            err = fbdo.errorReason();
        }
        if (resultCb != nullptr) resultCb(res, err);
    }
}

void FirebaseModule::sendDataAsyncString(uint32_t __time, void (*onSendData)(String data, String address),
                                         void (*resultCb)(String, String), void (*onDone)()) {
    if (data == nullptr) return;
    if (isConnect() && millis() - sendTime >= __time) {
        for (uint8_t i = 0; i < dataLen; i++) {
            String res, err;
            if (Firebase.RTDB.setString(&fbdo, address[i], data[i])) {
                res = "ok";
                if (onSendData != nullptr) onSendData(data[i], address[i]);
            } else {
                err = fbdo.errorReason();
            }
            if (resultCb != nullptr) resultCb(res, err);
        }
        if (onDone != nullptr) onDone();
        sendTime = millis();
    }
}

void FirebaseModule::sendDataAsyncOrderString(uint32_t __time, void (*onSendData)(String data, String address),
                                              void (*resultCb)(String, String), void (*onDone)()) {
    if (data == nullptr) return;
    if (isConnect() && millis() - sendTime >= __time) {
        String res, err;
        if (Firebase.RTDB.setString(&fbdo, address[dataCount], data[dataCount])) {
            res = "ok";
            if (onSendData != nullptr) onSendData(data[dataCount], address[dataCount]);
        } else {
            err = fbdo.errorReason();
        }
        if (resultCb != nullptr) resultCb(res, err);
        dataCount++;
        if (onDone != nullptr) onDone();
        sendTime = millis();
    }
}

void FirebaseModule::setFloat(float floatData, const char *addrs, void (*onData)(float data, String address)) {
    if (isConnect()) {
        Firebase.RTDB.setFloat(&fbdo, addrs, floatData);
        if (onData != nullptr) onData(floatData, addrs);
    }
}

void FirebaseModule::setString(String strData, const char *addrs, void (*onData)(String data, String address)) {
    if (isConnect()) {
        Firebase.RTDB.setString(&fbdo, addrs, strData);
        if (onData != nullptr) onData(strData, addrs);
    }
}

void FirebaseModule::setJson(String getAddress, JsonVariant variant, void (*resultCb)(String, String)) {
    JsonDocument doc;
    JsonVariant jsonRes = variant;
    String json, res, err;
    serializeJson(jsonRes, json);
    FirebaseJson firebaseJson;
    firebaseJson.setJsonData(json.c_str());
    if (Firebase.RTDB.setJSON(&fbdo, getAddress.c_str(), &firebaseJson)) {
        res = "ok";
    } else {
        err = fbdo.errorReason();
    }
    if (resultCb != nullptr) resultCb(res, err);
}

void FirebaseModule::pushJson(String getAddress, JsonVariant variant, void (*resultCb)(String, String)) {
    JsonDocument doc;
    JsonVariant jsonRes = variant;
    String json, res, err;
    serializeJson(jsonRes, json);
    FirebaseJson firebaseJson;
    firebaseJson.setJsonData(json.c_str());
    if (Firebase.RTDB.pushJSON(&fbdo, getAddress.c_str(), &firebaseJson)) {
        res = "ok";
    } else {
        err = fbdo.errorReason();
    }
    if (resultCb != nullptr) resultCb(res, err);
}

void FirebaseModule::setJson(String getAddress, JsonVariant (*jsonCallback)(JsonVariant),
                             void (*resultCb)(String, String)) {
    JsonDocument doc;
    JsonVariant jsonRes = jsonCallback(doc);
    String json, res, err;
    serializeJson(jsonRes, json);
    FirebaseJson firebaseJson;
    firebaseJson.setJsonData(json.c_str());
    if (Firebase.RTDB.setJSON(&fbdo, getAddress.c_str(), &firebaseJson)) {
        res = "ok";
    } else {
        err = fbdo.errorReason();
    }
    if (resultCb != nullptr) resultCb(res, err);
}

void FirebaseModule::pushJson(String getAddress, JsonVariant (*jsonCallback)(JsonVariant),
                              void (*resultCb)(String, String)) {
    JsonDocument doc;
    JsonVariant jsonRes = jsonCallback(doc);
    String json, res, err;
    serializeJson(jsonRes, json);
    FirebaseJson firebaseJson;
    firebaseJson.setJsonData(json.c_str());
    if (Firebase.RTDB.pushJSON(&fbdo, getAddress.c_str(), &firebaseJson)) {
        res = "ok";
    } else {
        err = fbdo.errorReason();
    }
    if (resultCb != nullptr) resultCb(res, err);
}

void FirebaseModule::getJson(String getAddress, void (*jsonCallback)(JsonVariant), void (*resultCb)(String, String)) {
    String res, err;
    if (Firebase.RTDB.getJSON(&fbdo, getAddress.c_str())) {
        JsonDocument doc;
        res = fbdo.to<FirebaseJson>().raw();
        deserializeJson(doc, res.c_str());
        jsonCallback(doc);
    } else {
        err = fbdo.errorReason();
    }
    if (resultCb != nullptr) resultCb(res, err);
}

float FirebaseModule::getData(const char *getAddress, void (*onData)(float data, String address)) {
    float dataRet = 0.0;
    if (isConnect() && Firebase.RTDB.getFloat(&fbdo, F(getAddress))) {
        dataRet = fbdo.to<float>();
        if (onData != nullptr) onData(dataRet, getAddress);
    }
    return dataRet;
}

String FirebaseModule::getStrData(const char *getAddress, void (*onData)(String data, String address)) {
    String dataRet = "";
    if (isConnect() && Firebase.RTDB.getString(&fbdo, F(getAddress))) {
        dataRet = fbdo.to<String>();
        dataRet.replace("\\", "");
        dataRet.replace("\"", "");
        if (onData != nullptr) onData(dataRet, getAddress);
    }
    return dataRet;
}

void FirebaseModule::waitConnection(uint32_t __tmr) {
    Serial.println("Wait for connection ... !!");
    delay(__tmr);
}

void FirebaseModule::debug(String header, String _data, String _address, bool endl) {
    Serial.print("| ");
    Serial.print(header);
    Serial.print(": ");
    Serial.print("| ");
    Serial.print(_address);
    Serial.print(": ");
    Serial.print(_data);
    if (endl) Serial.println();
}

int FirebaseModule::getFreeHeapMemory() {
    return Firebase.getFreeHeap();
}

void FirebaseModule::firestoreListDocument(String collectionId, size_t pageSize,
                                           void (*jsonCallback)(JsonVariant, size_t),
                                           void (*resultCb)(String, String)) {
    String result, error;
    if (Firebase.Firestore.listDocuments(&fbdo, authentication->projectID, "",
                                         collectionId.c_str(), pageSize, "",
                                         "", "", false)) {
        result = fbdo.payload().c_str();
        FirebaseJson res(result);
        String strResult;
        res.toString(strResult);

        JsonDocument doc;
        deserializeJson(doc, strResult.c_str());
        // serializeJson(doc, Serial);
//        serializeJsonPretty(doc, Serial);
        jsonCallback(doc["documents"], pageSize);
    } else {
        error = fbdo.errorReason();
    }
    if (resultCb != nullptr) {
        resultCb(result, error);
    }
}

String FirebaseModule::firestoreUpdateDocument(String collectionId, String documentId,
                                               JsonVariant (*jsonCallback)(JsonVariant),
                                               void (*resultCb)(String, String)) {
    String json, mask, documentPath, result, err;
    JsonDocument doc;
    JsonVariant res = jsonCallback(doc);
    serializeJson(res, json);
    JsonObject root = res["fields"].as<JsonObject>();
    for (JsonPair kv: root) {
        mask += kv.key().c_str();
        mask += ",";
    }
    int lastCommaIndex = mask.lastIndexOf(',');
    mask = mask.substring(0, lastCommaIndex);
    documentPath = collectionId + "/" + documentId;
    if (Firebase.Firestore.patchDocument(&fbdo, authentication->projectID,
                                         "", documentPath.c_str(),
                                         json.c_str(), mask.c_str())) {
        result = fbdo.payload();
    } else {
        err = fbdo.errorReason();
    }
    if (resultCb != nullptr) {
        resultCb(result, err);
    }
    return result;
}

String FirebaseModule::firestoreGetDocumentId(JsonVariant res) {
    String name = res["name"].as<String>();
    int lastSlashIndex = name.lastIndexOf("/");
    String extractedID = name.substring(lastSlashIndex + 1);
    return extractedID;
}

void FirebaseModule::resultStatusCallback(String res, String err) {
    if (!err.isEmpty()) {
        Serial.print("| err: ");
        Serial.print(err);
        Serial.println();
    }
}


#endif
#endif
#endif  // FIREBASE_HANDLER_H
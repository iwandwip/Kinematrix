/*
 *  firebase_handler.cpp
 *
 *  firebase handler c
 *  Created on: 2023. 4. 3
 */

#include "firebase-handler.h"

#if defined(ESP32) || defined(ESP8266)

FirebaseModule::FirebaseModule()
        : data(nullptr),
          address(nullptr),
          dataLen(0),
          addrLen(0) {
}

FirebaseModule::~FirebaseModule() = default;

bool FirebaseModule::init(FirebaseAuthentication *_authentication, void (*initCallback)(void)) {
    authentication = _authentication;

    config.api_key = authentication->apiKey;
    config.database_url = authentication->databaseURL;

    config.service_account.data.client_email = authentication->clientEmail;
    config.service_account.data.project_id = authentication->projectID;
    config.service_account.data.private_key = PRIVATE_KEY;

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
    WiFiClass::mode(WIFI_OFF);
    delay(1000);
    WiFiClass::mode(WIFI_STA);
    WiFi.begin(ssid, pwd);

    Serial.print("Connecting to : ");
    Serial.println(ssid);
    while (WiFiClass::status() != WL_CONNECTED) {
        Serial.print(".");
        if (connectCallback != nullptr) connectCallback();
        delay(250);
    }
    Serial.println(WiFi.localIP());

    connect = true;
    return WiFiClass::status() == WL_CONNECTED;
}

bool FirebaseModule::isConnect() const {
    return (connect || WiFiClass::status() == WL_CONNECTED) && Firebase.ready();
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
    delete[] data;
    delete[] address;

    dataLen = 0;
    addrLen = 0;

    maxDataLen = totalDataLen;
    data = new String[maxDataLen];
    address = new String[maxDataLen];
}

void FirebaseModule::sendDataFloat(void (*onSendData)(String data, String address),
                                   void (*resultCb)(const String &, const String &)) {
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

void FirebaseModule::sendDataAsyncFloat(uint32_t _time, void (*onSendData)(String data, String address),
                                        void (*resultCb)(const String &, const String &)) {
    if (data == nullptr) return;
    if (isConnect() && millis() - sendTime >= _time) {
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

void FirebaseModule::sendDataAsyncOrderFloat(uint32_t _time, void (*onSendData)(String data, String address),
                                             void (*resultCb)(const String &, const String &)) {
    if (data == nullptr) return;
    if (isConnect() && millis() - sendTime >= _time) {
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

void FirebaseModule::sendDataString(void (*onSendData)(String data, String address),
                                    void (*resultCb)(const String &, const String &)) {
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

void FirebaseModule::sendDataAsyncString(uint32_t _time, void (*onSendData)(String data, String address),
                                         void (*resultCb)(const String &, const String &), void (*onDone)()) {
    if (data == nullptr) return;
    if (isConnect() && millis() - sendTime >= _time) {
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

void FirebaseModule::sendDataAsyncOrderString(uint32_t _time, void (*onSendData)(String data, String address),
                                              void (*resultCb)(const String &, const String &), void (*onDone)()) {
    if (data == nullptr) return;
    if (isConnect() && millis() - sendTime >= _time) {
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

void FirebaseModule::setString(const String &strData, const char *addrs, void (*onData)(String data, String address)) {
    if (isConnect()) {
        Firebase.RTDB.setString(&fbdo, addrs, strData);
        if (onData != nullptr) onData(strData, addrs);
    }
}

void FirebaseModule::setJson(const String &getAddress, JsonVariant variant,
                             void (*resultCb)(const String &, const String &)) {
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

void FirebaseModule::pushJson(const String &getAddress, JsonVariant variant,
                              void (*resultCb)(const String &, const String &)) {
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

void FirebaseModule::setJson(const String &getAddress, JsonVariant (*jsonCallback)(JsonVariant),
                             void (*resultCb)(const String &, const String &)) {
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

void FirebaseModule::pushJson(const String &getAddress, JsonVariant (*jsonCallback)(JsonVariant),
                              void (*resultCb)(const String &, const String &)) {
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

void FirebaseModule::getJson(const String &getAddress, void (*jsonCallback)(JsonVariant),
                             void (*resultCb)(const String &, const String &)) {
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

void FirebaseModule::waitConnection(uint32_t _tmr) {
    Serial.println("Wait for connection ... !!");
    delay(_tmr);
}

void FirebaseModule::debug(const String &header, const String &_data, const String &_address, bool endl) {
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

void FirebaseModule::firestoreListDocument(const String &collectionId, size_t pageSize,
                                           void (*jsonCallback)(JsonVariant, size_t),
                                           void (*resultCb)(const String &, const String &)) {
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

String FirebaseModule::firestoreUpdateDocument(const String &collectionId, const String &documentId,
                                               JsonVariant (*jsonCallback)(JsonVariant),
                                               void (*resultCb)(const String &, const String &)) {
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

void FirebaseModule::resultStatusCallback(const String &res, const String &err) {
    if (!err.isEmpty()) {
        Serial.print("| err: ");
        Serial.print(err);
        Serial.println();
    }
}

#endif
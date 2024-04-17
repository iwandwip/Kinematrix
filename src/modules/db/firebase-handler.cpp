/*
 *  firebase_handler.cpp
 *
 *  firebase handler c
 *  Created on: 2023. 4. 3
 */

#include "firebase-handler.h"

#if defined(ESP32) || defined(ESP8266)

#include "addons/RTDBHelper.h"
#include "addons/TokenHelper.h"

FirebaseModule::FirebaseModule()
        : ntpUDP(nullptr),
          timeClient(nullptr),
          data(nullptr),
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

bool FirebaseModule::initNTP() {
    //    ntpUDP = new WiFiUDP();
    //    timeClient = new NTPClient(*ntpUDP);

    //    configTime(GMT_OFFSET_WIB, DAYLIGHT_OFFSET, NTP_SERVER);
    //    timeClient->begin();
    //    timeClient->setTimeOffset(GMT_OFFSET_WIB);
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
    return connect && Firebase.ready();
}

bool FirebaseModule::update(void (*onUpdate)(void)) {
    if (onUpdate != nullptr) onUpdate();
    while (!timeClient->update()) {
        timeClient->forceUpdate();
    }
    return true;
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

int FirebaseModule::getFreeHeapMemory() {
    return Firebase.getFreeHeap();
}

void FirebaseModule::sendDataFloat(void (*onSendData)(String data, String address)) {
    if (data == nullptr) return;
    for (uint8_t i = 0; i < dataLen; i++) {
        Firebase.RTDB.setFloat(&fbdo, address[i], data[i].toFloat());
        if (onSendData != nullptr) onSendData(data[i], address[i]);
    }
}

void FirebaseModule::sendDataAsyncFloat(uint32_t __time, void (*onSendData)(String data, String address)) {
    if (data == nullptr) return;
    if (isConnect() && millis() - sendTime >= __time) {
        for (uint8_t i = 0; i < dataLen; i++) {
            Firebase.RTDB.setFloat(&fbdo, address[i], data[i].toFloat());
            if (onSendData != nullptr) onSendData(data[i], address[i]);
        }
        sendTime = millis();
    }
}

void FirebaseModule::sendDataAsyncOrderFloat(uint32_t __time, void (*onSendData)(String data, String address)) {
    if (data == nullptr) return;
    if (isConnect() && millis() - sendTime >= __time) {
        if (dataCount >= maxDataLen) dataCount = 0;
        Firebase.RTDB.setFloat(&fbdo, address[dataCount], data[dataCount].toFloat());
        if (onSendData != nullptr) onSendData(data[dataCount], address[dataCount]);
        dataCount++;
        sendTime = millis();
    }
}

void FirebaseModule::sendDataString(void (*onSendData)(String data, String address)) {
    if (data == nullptr) return;
    for (uint8_t i = 0; i < dataLen; i++) {
        Firebase.RTDB.setString(&fbdo, address[i], data[i]);
        if (onSendData != nullptr) onSendData(data[i], address[i]);
    }
}

void FirebaseModule::sendDataAsyncString(uint32_t __time, void (*onSendData)(String data, String address)) {
    if (data == nullptr) return;
    if (isConnect() && millis() - sendTime >= __time) {
        for (uint8_t i = 0; i < dataLen; i++) {
            Firebase.RTDB.setString(&fbdo, address[i], data[i]);
            if (onSendData != nullptr) onSendData(data[i], address[i]);
        }
        sendTime = millis();
    }
}

void FirebaseModule::sendDataAsyncOrderString(uint32_t __time, void (*onSendData)(String data, String address)) {
    if (data == nullptr) return;
    if (isConnect() && millis() - sendTime >= __time) {
        Firebase.RTDB.setString(&fbdo, address[dataCount], data[dataCount]);
        if (onSendData != nullptr) onSendData(data[dataCount], address[dataCount]);
        dataCount++;
        sendTime = millis();
    }
}

void FirebaseModule::setFloat(float floatData, const char *addrs) {
    if (isConnect()) {
        Firebase.RTDB.setFloat(&fbdo, addrs, floatData);
    }
}

void FirebaseModule::setString(String strData, const char *addrs) {
    if (isConnect()) {
        Firebase.RTDB.setString(&fbdo, addrs, strData);
    }
}

float FirebaseModule::getData(const char *getAddress) {
    if (isConnect() && Firebase.RTDB.getFloat(&fbdo, F(getAddress))) {
        return fbdo.to<float>();
    }
    return 0.0;
}

String FirebaseModule::getStrData(const char *getAddress) {
    if (isConnect() && Firebase.RTDB.getString(&fbdo, F(getAddress))) {
        return fbdo.to<String>();
    }
    return "";
}

String FirebaseModule::getStrTime() {
    return String(timeClient->getDay()) + ";" + String(timeClient->getHours()) + ";" +
           String(timeClient->getMinutes()) + ";" + String(timeClient->getSeconds());
}

void FirebaseModule::waitConnection(uint32_t __tmr) {
    Serial.println("Wait for connection ... !!");
    delay(__tmr);
}

#endif
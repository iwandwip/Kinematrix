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
        : fbdo(nullptr),
          auth(nullptr),
          config(nullptr),
          ntpUDP(nullptr),
          timeClient(nullptr),
          data(nullptr),
          address(nullptr) {
}

FirebaseModule::~FirebaseModule() {
}

bool FirebaseModule::init(FirebaseAuthentication *_authentication) {
    authentication = _authentication;

    fbdo = new FirebaseData;
    auth = new FirebaseAuth;
    config = new FirebaseConfig;

//    ntpUDP = new WiFiUDP();
//    timeClient = new NTPClient(*ntpUDP);

//    configTime(GMT_OFFSET_WIB, DAYLIGHT_OFFSET, NTP_SERVER);
//    timeClient->begin();
//    timeClient->setTimeOffset(GMT_OFFSET_WIB);

    config->api_key = authentication->apiKey;
    config->database_url = authentication->databaseURL;
    auth->user.email = authentication->user.email;
    auth->user.password = authentication->user.password;
    config->token_status_callback = tokenStatusCallback;

    Serial.print("| [INFO] FreeHeap: ");
    Serial.print(Firebase.getFreeHeap());
    Serial.println();

    Firebase.reconnectNetwork(true);

    fbdo->setBSSLBufferSize(4096 /* Rx buffer size in bytes from 512 to 16384 */, 1024 /* Tx buffer size in bytes from 512 to 16384 */);
    fbdo->setResponseSize(2048); // Limit the size of response payload to be collected in FirebaseData

    Firebase.begin(config, auth);
    Firebase.setDoubleDigits(2);  // default 5
    Firebase.setFloatDigits(2);
    config->timeout.serverResponse = 10 * 10000;
    return true;
}

bool FirebaseModule::connectToWiFi(const char *ssid, const char *pwd) {
    WiFi.mode(WIFI_OFF);
    delay(1000);
    WiFi.mode(WIFI_STA);
    WiFi.begin(ssid, pwd);
    Serial.print("Connecting to : ");
    Serial.println(ssid);
    while (WiFi.status() != WL_CONNECTED) {
        Serial.print(".");
        delay(250);
    }
    Serial.println(WiFi.localIP());

    connect = true;
    return true;
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

void FirebaseModule::clearData() {
    free(data);
    dataLen = 0;
    data = nullptr;
    for (int i = 0; i < addrLen; i++) {
        free(address[i]);
    }
    free(address);
    address = NULL;
    addrLen = 0;
}

void FirebaseModule::addData(float newData, const char *newAddressData) {
    dataLen++;
    data = (float *) realloc(data, dataLen * sizeof(float));
    data[dataLen - 1] = newData;

    char **newAddress = (char **) realloc(address, (addrLen + 1) * sizeof(char *));
    if (newAddress == NULL) return;
    address = newAddress;
    address[addrLen] = (char *) malloc(strlen(newAddressData) + 1);
    if (address[addrLen] == NULL) return;
    strcpy(address[addrLen], newAddressData);
    addrLen++;
}

int FirebaseModule::getFreeHeapMemory() {
    return Firebase.getFreeHeap();
}

void FirebaseModule::sendData(uint32_t __time, void (*onSendData)(void)) {
    if (data == nullptr) return;
    if (Firebase.ready() && millis() - sendTime >= __time) {
        for (uint8_t i = 0; i < dataLen; i++) {
            Firebase.RTDB.setFloatAsync(fbdo, address[i], data[i]);
        }
        onSendData();
        sendTime = millis();
    }
}

void FirebaseModule::setFloat(float floatData, const char *addrs) {
    if (Firebase.ready()) {
        Firebase.RTDB.setFloatAsync(fbdo, addrs, floatData);
    }
}

void FirebaseModule::setString(String strData, const char *addrs) {
    if (Firebase.ready()) {
        Firebase.RTDB.setStringAsync(fbdo, addrs, strData);
    }
}

float FirebaseModule::getData(const char *getAddress) {
    if (Firebase.ready() && Firebase.RTDB.getFloat(fbdo, F(getAddress))) {
        return fbdo->to<float>();
    }
    return 0.0;
}

String FirebaseModule::getStrData(const char *getAddress) {
    if (Firebase.ready() && Firebase.RTDB.getString(fbdo, F(getAddress))) {
        return fbdo->to<String>();
    }
    return "";
}

String FirebaseModule::getStrTime() {
    return String(timeClient->getDay()) + ";" + String(timeClient->getHours()) + ";" + String(timeClient->getMinutes()) + ";" + String(timeClient->getSeconds());
}

void FirebaseModule::waitConnection(uint32_t __tmr) {
    delay(__tmr);
}

#endif
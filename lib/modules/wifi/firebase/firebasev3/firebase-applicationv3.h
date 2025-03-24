#ifndef FIREBASE_APPLICATION_V3_H
#define FIREBASE_APPLICATION_V3_H

#include <Arduino.h>
#include <FirebaseClient.h>

#if defined(ESP32)
#include <WiFiClientSecure.h>
typedef WiFiClientSecure SSL_CLIENT;
#elif defined(ESP8266)
#include <ESP8266WiFi.h>
typedef BearSSL::WiFiClientSecure SSL_CLIENT;
#endif

class FirebaseV3Application {
private:
    static FirebaseV3Application *_instance;
    FirebaseApp _app;
    SSL_CLIENT _sslClient;
    AsyncClientClass _asyncClient;
    bool _initialized;
    String _projectId;

    FirebaseV3Application();

public:
    static FirebaseV3Application *getInstance();

    bool begin(const char *apiKey, const char *userEmail, const char *userPassword, const char *projectId, void (*callback)(AsyncResult &) = nullptr);
    bool begin(const char *clientEmail, const char *projectId, const char *privateKey, void (*callback)(AsyncResult &) = nullptr);
    bool beginAnonymous(const char *projectId, void (*callback)(AsyncResult &) = nullptr);

    void setTime(time_t timestamp);
    void loop();
    bool isReady();
    String getProjectId();

    FirebaseApp &getApp();
    AsyncClientClass &getAsyncClient();
    SSL_CLIENT &getSSLClient();
};

#endif
#ifndef FIREBASE_MESSAGING_V3_H
#define FIREBASE_MESSAGING_V3_H

#if defined(ESP32) || defined(ESP8266)

#include <Arduino.h>
#include <FirebaseClient.h>
#include <time.h>
#include "firebase-applicationv3.h"

#if defined(ESP32)
#include <WiFiClientSecure.h>
typedef WiFiClientSecure SSL_CLIENT;
#elif defined(ESP8266)
#include <ESP8266WiFi.h>
typedef BearSSL::WiFiClientSecure SSL_CLIENT;
#endif

class FirebaseV3Messaging {
public:
    FirebaseV3Messaging();
    FirebaseV3Messaging(FirebaseV3Application *firebaseApp);
    ~FirebaseV3Messaging();

    bool begin(const char *apiKey, const char *userEmail, const char *userPassword, const char *projectId, int timeout = 3000);
    bool begin(const char *clientEmail, const char *projectId, const char *privateKey, int timeout = 3000);
    bool begin(const char *projectId);
    bool begin();
    void loop();
    bool isReady();
    String getLastError();

    bool isResultReady() const;
    String getLastPayload() const;

    void setTopic(const String &topic);
    void setToken(const String &token);
    void setCondition(const String &condition);

    void setNotification(const String &title, const String &body);
    void setData(const String &jsonData);
    void addData(const String &key, const String &value);

    void setAndroidPriority(bool highPriority);
    void setAndroidNotificationPriority(int priority);

    String sendMessage(bool waitForCompletion = false);
    void clearMessage();

private:
    SSL_CLIENT *_sslClient;
    bool _ownSSLClient;
    AsyncClientClass *_asyncClient;
    bool _ownAsyncClient;
    FirebaseApp *_app;
    bool _ownApp;

    String _projectId;
    bool _useUserAuth;
    bool _credsSet = false;
    int _authTimeout = 3000;

    FirebaseV3Application *_firebaseApp;
    bool _usingSharedApp;

    UserAuth *_userAuth = nullptr;
    const char *_apiKey;
    const char *_userEmail;
    const char *_userPassword;

    ServiceAuth *_serviceAuth = nullptr;
    const char *_clientEmail;
    const char *_privateKey;

    Messaging *_messaging = nullptr;

    String _token;
    String _topic;
    String _condition;
    String _title;
    String _body;
    String _dataJson = "{}";
    bool _highPriority = false;
    int _androidNotificationPriority = 0;

    bool _hasNotification = false;
    bool _hasAndroidConfig = false;
    bool _hasAndroidNotificationPriority = false;

    AsyncResult _messagingResult;
    bool _resultReady = false;
    String _resultPayload;

    time_t getValidTimeFromNTP();
    const char *ntpServer1 = "pool.ntp.org";
    const char *ntpServer2 = "time.nist.gov";
    const long gmtOffset_sec = 25200;
    const int daylightOffset_sec = 0;

    void processResult(AsyncResult &aResult);
    static void resultCallback(AsyncResult &aResult, void *userData);
    AsyncClientClass &getClient();
    FirebaseApp &getApp();
};

#endif
#endif
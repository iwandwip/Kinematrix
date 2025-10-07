/*
 *  firebase-messagingv2.h
 *
 *  Simplified Firebase messaging handler
 */

#ifndef FIREBASE_MESSAGING_V2_H
#define FIREBASE_MESSAGING_V2_H

#if defined(ESP32) || defined(ESP8266)

#include <Arduino.h>
#include <FirebaseClient.h>
#include <time.h>

#if defined(ESP32)
#include <WiFiClientSecure.h>
typedef WiFiClientSecure SSL_CLIENT;
#elif defined(ESP8266)
#include <ESP8266WiFi.h>
typedef BearSSL::WiFiClientSecure SSL_CLIENT;
#endif

#ifndef SSL_CLIENT_BUFFER_SET
#define SSL_CLIENT_BUFFER_SET

inline void set_ssl_client_insecure_and_buffer(SSL_CLIENT &client) {
#if defined(ESP32)
    client.setInsecure();
#elif defined(ESP8266)
    client.setInsecure();
    client.setBufferSizes(4096, 1024);
#endif
}

#endif // SSL_CLIENT_BUFFER_SET

class FirebaseV2Messaging {
public:
    FirebaseV2Messaging();
    ~FirebaseV2Messaging();

    // Begin methods with auth credentials
    // timeout: Authentication timeout in milliseconds (default: 3000ms)
    bool begin(const char *apiKey, const char *userEmail, const char *userPassword, const char *projectId, int timeout = 3000);
    bool begin(const char *clientEmail, const char *projectId, const char *privateKey, int timeout = 3000);
    bool begin();

    void loop();
    bool isReady();
    String getLastError();

    // Methods to access result data
    bool isResultReady() const;
    String getLastPayload() const;

    // Message configuration methods
    void setTopic(const String &topic);
    void setToken(const String &token);
    void setCondition(const String &condition);

    void setNotification(const String &title, const String &body);
    void setData(const String &jsonData);
    void addData(const String &key, const String &value);

    void setAndroidPriority(bool highPriority);
    void setAndroidNotificationPriority(int priority); // 0-4, LOW to MAX

    void setApnsHeaders(const String &jsonHeaders);
    void addApnsHeader(const String &key, const String &value);

    void setWebpushHeaders(const String &jsonHeaders);
    void addWebpushHeader(const String &key, const String &value);

    // Send methods
    String sendMessage(bool waitForCompletion = false);
    void clearMessage();

private:
    SSL_CLIENT _sslClient;
    using AsyncClient = AsyncClientClass;
    AsyncClient _aClient;
    FirebaseApp _app;
    String _projectId;
    bool _useUserAuth;
    bool _credsSet = false;
    int _authTimeout = 3000; // Authentication timeout in milliseconds

    // Auth variables
    UserAuth *_userAuth = nullptr;
    const char *_apiKey;
    const char *_userEmail;
    const char *_userPassword;

    ServiceAuth *_serviceAuth = nullptr;
    const char *_clientEmail;
    const char *_privateKey;

    // Firebase classes
    Messaging _messaging;

    // Message components - simplified approach to match working implementation
    String _token;
    String _topic;
    String _condition;
    String _title;
    String _body;
    String _dataJson = "{}";
    bool _highPriority = false;
    int _androidNotificationPriority = 0;

    // Flags
    bool _hasNotification = false;
    bool _hasAndroidConfig = false;
    bool _hasAndroidNotificationPriority = false;

    // Result handling
    AsyncResult _messagingResult;
    bool _resultReady = false;
    String _resultPayload;

    // NTP time sync
    time_t getValidTimeFromNTP();
    const char *ntpServer1 = "pool.ntp.org";
    const char *ntpServer2 = "time.nist.gov";
    const long gmtOffset_sec = 25200;  // GMT+7 for Indonesia (7*60*60)
    const int daylightOffset_sec = 0;  // Indonesia doesn't use DST

    // Helper methods
    void processResult(AsyncResult &aResult);
    static void resultCallback(AsyncResult &aResult, void *userData);
};

#endif
#endif
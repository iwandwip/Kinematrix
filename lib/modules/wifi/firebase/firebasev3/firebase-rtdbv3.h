#ifndef FIREBASE_RTDB_V3_H
#define FIREBASE_RTDB_V3_H

#if defined(ESP32) || defined(ESP8266)

#include <Arduino.h>
#include <FirebaseClient.h>
#include "firebase-applicationv3.h"

#if defined(ESP32)
#include <WiFiClientSecure.h>
typedef WiFiClientSecure SSL_CLIENT;
#elif defined(ESP8266)
#include <ESP8266WiFi.h>
typedef BearSSL::WiFiClientSecure SSL_CLIENT;
#endif

class FirebaseV3RTDB {
public:
    FirebaseV3RTDB();
    FirebaseV3RTDB(FirebaseV3Application *firebaseApp);
    ~FirebaseV3RTDB();

    bool begin(Client &client, const String &databaseUrl, const String &apiKey, const String &email, const String &password);
    bool begin(const String &databaseUrl);
    bool beginAnonymous(Client &client, const String &databaseUrl);
    bool beginStream(Client &stream_client, const String &path);

    bool set(const String &path, const String &value);
    bool set(const String &path, int value);
    bool set(const String &path, bool value);
    bool set(const String &path, float value, int dp = 2);
    bool set(const String &path, const char *jsonString);

    String getString(const String &path);
    int getInt(const String &path);
    bool getBool(const String &path);
    float getFloat(const String &path);
    String getJSON(const String &path);

    String push(const String &path, const String &value);
    String push(const String &path, const char *jsonString);
    bool update(const String &path, const char *jsonString);
    bool remove(const String &path);
    bool exists(const String &path);

    bool ready();
    bool available();
    bool hasStreamData();
    String getStreamData();
    String getStreamPath();
    String getStreamEvent();
    int getStreamDataType();
    bool hasError();
    String getError();

    void loop();

private:
    FirebaseApp *_app;
    bool _ownApp;
    RealtimeDatabase *_rtdb;
    DefaultNetwork _net;
    AsyncClientClass *_asyncClient;
    bool _ownAsyncClient;
    AsyncClientClass *_streamClient;
    bool _ownStreamClient;
    AsyncResult _result;
    AsyncResult _streamResult;

    FirebaseV3Application *_firebaseApp;
    bool _usingSharedApp;

    bool _isInitialized = false;
    bool _streamInitialized = false;
    String _lastError;
    String _streamData;
    String _streamPath;
    String _streamEvent;
    int _streamDataType = 0;
    bool _hasStreamData = false;
    bool _hasError = false;
    String _databaseUrl;

    Client *_clientPtr = nullptr;
    Client *_streamClientPtr = nullptr;

    void processStreamData();
    void checkError();
    void clearStreamData();
    bool processResult();
    bool processStreamResult();
    AsyncClientClass &getClient();
    FirebaseApp &getApp();
};

#endif
#endif
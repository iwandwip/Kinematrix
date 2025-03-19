/*
 *  firebase-rtdbv2.h
 *
 *  Simplified Firebase Realtime Database handler
 */

#ifndef FIREBASE_RTDB_V2_H
#define FIREBASE_RTDB_V2_H

#if defined(ESP32) || defined(ESP8266)

#include <Arduino.h>
#include <FirebaseClient.h>

class FirebaseV2RTDB {
public:
    FirebaseV2RTDB();

    bool begin(Client &client, const String &databaseUrl, const String &apiKey, const String &email, const String &password);
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
    FirebaseApp app;
    RealtimeDatabase rtdb;
    DefaultNetwork net;
    AsyncClientClass auth_client;
    AsyncClientClass stream_client;
    AsyncResult result;
    AsyncResult stream_result;

    bool _is_initialized = false;
    bool _stream_initialized = false;
    String _last_error;
    String _stream_data;
    String _stream_path;
    String _stream_event;
    int _stream_data_type = 0;
    bool _has_stream_data = false;
    bool _has_error = false;

    Client *_client = nullptr;
    Client *_stream_client_ptr = nullptr;

    void processStreamData();
    void checkError();
    void clearStreamData();
    bool processResult();
    bool processStreamResult();
};

#endif
#endif
/*
 *  firebase-storagev2.h
 *
 *  Simplified Firebase Storage handler
 */

#ifndef FIREBASE_STORAGE_V2_H
#define FIREBASE_STORAGE_V2_H

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

class FirebaseV2Storage {
public:
    FirebaseV2Storage();
    ~FirebaseV2Storage();

    // Begin methods with auth credentials
    bool begin(const char *apiKey, const char *userEmail, const char *userPassword,
               const char *projectId, const char *bucketId, int timeout = 3000);
    bool begin(const char *clientEmail, const char *projectId, const char *privateKey,
               const char *bucketId, int timeout = 3000);
    bool begin();
    void loop();
    bool isReady();
    String getLastError();

    // Methods to access result data
    bool isResultReady() const;
    String getLastPayload() const;
    int getDownloadProgress() const;
    int getUploadProgress() const;

    // File operations
    bool uploadFile(const char *localFileName, const char *storageObjectName,
                    const char *mimeType = "application/octet-stream",
                    bool waitForCompletion = false);

    bool downloadFile(const char *storageObjectName, const char *localFileName,
                      bool waitForCompletion = false);

    bool deleteFile(const char *storageObjectName, bool waitForCompletion = false);

    String getFileMetadata(const char *storageObjectName, bool waitForCompletion = false);

    String listFiles(const char *prefix = "", int maxResults = 100, bool waitForCompletion = false);

    bool performOTA(const char *storageObjectName, bool waitForCompletion = false);

private:
    SSL_CLIENT _sslClient;
    using AsyncClient = AsyncClientClass;
    AsyncClient _aClient;
    FirebaseApp _app;
    CloudStorage _storage;
    String _projectId;
    String _bucketId;
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

    // Result handling
    AsyncResult _storageResult;
    bool _resultReady = false;
    String _resultPayload;
    int _downloadProgress = 0;
    int _uploadProgress = 0;

    // Helper methods
    void processResult(AsyncResult &aResult);
    static void resultCallback(AsyncResult &aResult, void *userData);
    time_t getValidTimeFromNTP();

    // NTP time sync
    const char *ntpServer1 = "pool.ntp.org";
    const char *ntpServer2 = "time.nist.gov";
    const long gmtOffset_sec = 0;      // GMT offset in seconds (default UTC)
    const int daylightOffset_sec = 0;  // DST offset (default none)
};

#endif // ESP32 || ESP8266
#endif // FIREBASE_STORAGE_V2_H
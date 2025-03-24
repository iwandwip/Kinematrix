#ifndef FIREBASE_STORAGE_V3_H
#define FIREBASE_STORAGE_V3_H

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

class FirebaseV3Storage {
public:
    FirebaseV3Storage();
    FirebaseV3Storage(FirebaseV3Application *firebaseApp);
    ~FirebaseV3Storage();

    bool begin(const char *apiKey, const char *userEmail, const char *userPassword,
               const char *projectId, const char *bucketId, int timeout = 3000);
    bool begin(const char *clientEmail, const char *projectId, const char *privateKey,
               const char *bucketId, int timeout = 3000);
    bool begin(const char *bucketId);
    bool begin();
    void loop();
    bool isReady();
    String getLastError();

    bool isResultReady() const;
    String getLastPayload() const;
    int getDownloadProgress() const;
    int getUploadProgress() const;

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
    SSL_CLIENT *_sslClient;
    bool _ownSSLClient;
    AsyncClientClass *_asyncClient;
    bool _ownAsyncClient;
    FirebaseApp *_app;
    bool _ownApp;
    CloudStorage *_storage;

    String _projectId;
    String _bucketId;
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

    AsyncResult _storageResult;
    bool _resultReady = false;
    String _resultPayload;
    int _downloadProgress = 0;
    int _uploadProgress = 0;

    void processResult(AsyncResult &aResult);
    static void resultCallback(AsyncResult &aResult, void *userData);
    time_t getValidTimeFromNTP();
    AsyncClientClass &getClient();
    FirebaseApp &getApp();

    const char *ntpServer1 = "pool.ntp.org";
    const char *ntpServer2 = "time.nist.gov";
    const long gmtOffset_sec = 0;
    const int daylightOffset_sec = 0;
};

#endif
#endif
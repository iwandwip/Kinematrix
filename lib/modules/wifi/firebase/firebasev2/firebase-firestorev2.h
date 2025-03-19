/*
 *  firebase-firestorev2.h
 *
 *  Simplified Firebase Firestore Database handler
 */

#ifndef FIREBASE_FIRESTORE_V2_H
#define FIREBASE_FIRESTORE_V2_H

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

class FirebaseV2Firestore {
public:
    FirebaseV2Firestore();
    ~FirebaseV2Firestore();

    bool begin(const char *apiKey, const char *userEmail, const char *userPassword, const char *projectId);
    bool begin(const char *clientEmail, const char *projectId, const char *privateKey);
    bool begin();
    void loop();
    bool isReady();
    String getLastError();

    bool isResultReady() const;
    String getLastPayload() const;

    String createDatabase(const String &databaseId, const String &locationId, bool waitForCompletion = false);
    String deleteDatabase(const String &databaseId, bool waitForCompletion = false);
    String updateDatabase(const String &databaseId, bool enablePointInTimeRecovery, bool waitForCompletion = false);
    String getDatabaseInfo(const String &databaseId, bool waitForCompletion = false);
    String listDatabases(bool waitForCompletion = false);
    String exportDocuments(const String &collectionIds, const String &bucketId, const String &storagePath, bool waitForCompletion = false);
    String importDocuments(const String &collectionIds, const String &bucketId, const String &storagePath, bool waitForCompletion = false);

    String createDatabaseIndex(const String &collectionId, const String &fieldPath1, const String &fieldPath2, bool waitForCompletion = false);
    String deleteDatabaseIndex(const String &indexId, bool waitForCompletion = false);
    String getDatabaseIndex(const String &indexId, bool waitForCompletion = false);
    String listDatabaseIndexes(bool waitForCompletion = false);
    String createCollectionGroupIndex(const String &collectionId, const String &fieldPath1, const String &fieldPath2, bool waitForCompletion = false);
    String deleteCollectionGroupIndex(const String &collectionId, const String &indexId, bool waitForCompletion = false);
    String getCollectionGroupIndex(const String &collectionId, const String &indexId, bool waitForCompletion = false);
    String listCollectionGroupIndexes(const String &collectionId, bool waitForCompletion = false);

    String createDocument(const String &documentPath, const String &data, bool waitForCompletion = false);
    String createDocument(const String &documentPath, Document<Values::Value> &doc, bool waitForCompletion = false);
    String getDocument(const String &documentPath, const String &mask = "", bool waitForCompletion = false);
    String updateDocument(const String &documentPath, const String &data, const String &updateMask, bool waitForCompletion = false);
    String deleteDocument(const String &documentPath, bool waitForCompletion = false);
    String listDocuments(const String &collectionPath, int pageSize = 10, const String &mask = "", bool waitForCompletion = false);
    String listCollectionIds(const String &documentPath, int pageSize = 10, bool waitForCompletion = false);

    String batchGet(const String &documentPaths, const String &mask = "", bool waitForCompletion = false);
    String batchWrite(Writes &writes, bool waitForCompletion = false);

    String runQuery(const String &documentPath, const String &queryJson, bool waitForCompletion = false);
    String appendArray(const String &documentPath, const String &fieldPath, Values::ArrayValue &arrayValue, bool waitForCompletion = false);
    String appendMapValue(const String &documentPath, const String &mapName, const String &key, Values::Value &value, bool waitForCompletion = false);
    String incrementFieldValue(const String &documentPath, const String &fieldPath, int incrementBy, bool waitForCompletion = false);
    String setServerTimestamp(const String &documentPath, const String &fieldPath, bool waitForCompletion = false);

private:
    SSL_CLIENT _sslClient;
    using AsyncClient = AsyncClientClass;
    AsyncClient _aClient;
    FirebaseApp _app;
    String _projectId;
    bool _useUserAuth;
    bool _credsSet = false;

    UserAuth *_userAuth = nullptr;
    const char *_apiKey;
    const char *_userEmail;
    const char *_userPassword;

    ServiceAuth *_serviceAuth = nullptr;
    const char *_clientEmail;
    const char *_privateKey;

    Firestore::Databases *_databases = nullptr;
    Firestore::Databases::Indexes *_dbIndexes = nullptr;
    Firestore::CollectionGroups::Indexes *_cgIndexes = nullptr;
    Firestore::Documents *_docs = nullptr;

    AsyncResult _firestoreResult;
    bool _resultReady = false;
    String _resultPayload;

    void processResult(AsyncResult &aResult);
    static void resultCallback(AsyncResult &aResult, void *userData);
};

#endif
#endif
/*
 *  firebase-firestorev2.cpp
 *
 *  Simplified Firebase Firestore Database handler implementation
 */

#include "firebase-firestorev2.h"

#if defined(ESP32) || defined(ESP8266)

#include <time.h>

FirebaseV2Firestore::FirebaseV2Firestore()
        : _aClient(_sslClient), _projectId(""), _useUserAuth(false),
          _apiKey(nullptr), _userEmail(nullptr), _userPassword(nullptr),
          _clientEmail(nullptr), _privateKey(nullptr), _credsSet(false) {

    set_ssl_client_insecure_and_buffer(_sslClient);
}

FirebaseV2Firestore::~FirebaseV2Firestore() {
    if (_userAuth != nullptr) {
        delete _userAuth;
        _userAuth = nullptr;
    }

    if (_serviceAuth != nullptr) {
        delete _serviceAuth;
        _serviceAuth = nullptr;
    }

    if (_databases != nullptr) {
        delete _databases;
        _databases = nullptr;
    }

    if (_dbIndexes != nullptr) {
        delete _dbIndexes;
        _dbIndexes = nullptr;
    }

    if (_cgIndexes != nullptr) {
        delete _cgIndexes;
        _cgIndexes = nullptr;
    }

    if (_docs != nullptr) {
        delete _docs;
        _docs = nullptr;
    }
}

bool FirebaseV2Firestore::begin(const char *apiKey, const char *userEmail, const char *userPassword, const char *projectId) {
    _useUserAuth = true;
    _apiKey = apiKey;
    _userEmail = userEmail;
    _userPassword = userPassword;
    _projectId = projectId;

    if (_userAuth != nullptr) {
        delete _userAuth;
        _userAuth = nullptr;
    }
    if (_serviceAuth != nullptr) {
        delete _serviceAuth;
        _serviceAuth = nullptr;
    }

    _userAuth = new UserAuth(_apiKey, _userEmail, _userPassword, 3000);
    if (_userAuth == nullptr) {
        return false;
    }

    _credsSet = true;
    return begin();
}

bool FirebaseV2Firestore::begin(const char *clientEmail, const char *projectId, const char *privateKey) {
    _useUserAuth = false;
    _clientEmail = clientEmail;
    _projectId = projectId;
    _privateKey = privateKey;

    if (_userAuth != nullptr) {
        delete _userAuth;
        _userAuth = nullptr;
    }
    if (_serviceAuth != nullptr) {
        delete _serviceAuth;
        _serviceAuth = nullptr;
    }

    _serviceAuth = new ServiceAuth(_clientEmail, _projectId.c_str(), _privateKey, 3000);
    if (_serviceAuth == nullptr) {
        return false;
    }

    _credsSet = true;
    return begin();
}

bool FirebaseV2Firestore::begin() {
    if (!_credsSet) {
        return false;
    }

    if (_useUserAuth) {
        _app.setTime(time(nullptr));
        if (_userAuth != nullptr) {
            initializeApp(_aClient, _app, getAuth(*_userAuth), [](AsyncResult &result) {}, "authTask");
        } else {
            return false;
        }
    } else {
        _app.setTime(time(nullptr));
        if (_serviceAuth != nullptr) {
            initializeApp(_aClient, _app, getAuth(*_serviceAuth), [](AsyncResult &result) {}, "authTask");
        } else {
            return false;
        }
    }

    _databases = new Firestore::Databases();
    _dbIndexes = new Firestore::Databases::Indexes();
    _cgIndexes = new Firestore::CollectionGroups::Indexes();
    _docs = new Firestore::Documents();

    if (_databases == nullptr || _dbIndexes == nullptr || _cgIndexes == nullptr || _docs == nullptr) {
        return false;
    }

    _app.getApp<Firestore::Databases>(*_databases);
    _app.getApp<Firestore::Databases::Indexes>(*_dbIndexes);
    _app.getApp<Firestore::CollectionGroups::Indexes>(*_cgIndexes);
    _app.getApp<Firestore::Documents>(*_docs);

    return true;
}

void FirebaseV2Firestore::loop() {
    _app.loop();
    processResult(_firestoreResult);
}

bool FirebaseV2Firestore::isReady() {
    return _app.ready();
}

String FirebaseV2Firestore::getLastError() {
    return _aClient.lastError().message();
}

bool FirebaseV2Firestore::isResultReady() const {
    return _resultReady;
}

String FirebaseV2Firestore::getLastPayload() const {
    return _resultPayload;
}

void FirebaseV2Firestore::processResult(AsyncResult &aResult) {
    if (!aResult.isResult())
        return;

    if (aResult.isError()) {
        _resultReady = true;
        _resultPayload = aResult.error().message();
    }

    if (aResult.available()) {
        _resultReady = true;
        _resultPayload = aResult.c_str();
    }
}

void FirebaseV2Firestore::resultCallback(AsyncResult &aResult, void *userData) {
    FirebaseV2Firestore *wrapper = static_cast<FirebaseV2Firestore *>(userData);
    if (wrapper != nullptr) {
        wrapper->processResult(aResult);
    }
}

String FirebaseV2Firestore::createDatabase(const String &databaseId, const String &locationId, bool waitForCompletion) {
    if (_databases == nullptr) {
        return "Error: Databases not initialized";
    }

    Firestore::Database db;
    db.concurrencyMode(Firestore::ConcurrencyMode::PESSIMISTIC);
    db.databaseType(Firestore::DatabaseType::FIRESTORE_NATIVE);
    db.name(databaseId);
    db.locationId(locationId);

    if (waitForCompletion) {
        return _databases->create(_aClient, Firestore::Parent(_projectId.c_str(), databaseId), db);
    } else {
        _resultReady = false;
        _databases->create(_aClient, Firestore::Parent(_projectId.c_str(), databaseId), db,
                           _firestoreResult);
        return "";
    }
}

String FirebaseV2Firestore::deleteDatabase(const String &databaseId, bool waitForCompletion) {
    if (_databases == nullptr) {
        return "Error: Databases not initialized";
    }

    if (waitForCompletion) {
        return _databases->deleteDatabase(_aClient, Firestore::Parent(_projectId.c_str(), databaseId), "");
    } else {
        _resultReady = false;
        _databases->deleteDatabase(_aClient, Firestore::Parent(_projectId.c_str(), databaseId), "",
                                   _firestoreResult);
        return "";
    }
}

String FirebaseV2Firestore::updateDatabase(const String &databaseId, bool enablePointInTimeRecovery, bool waitForCompletion) {
    if (_databases == nullptr) {
        return "Error: Databases not initialized";
    }

    Firestore::Database db;
    if (enablePointInTimeRecovery) {
        db.pointInTimeRecoveryEnablement(Firestore::PointInTimeRecoveryEnablement::POINT_IN_TIME_RECOVERY_ENABLED);
    } else {
        db.pointInTimeRecoveryEnablement(Firestore::PointInTimeRecoveryEnablement::POINT_IN_TIME_RECOVERY_DISABLED);
    }

    if (waitForCompletion) {
        return _databases->patch(_aClient, Firestore::Parent(_projectId.c_str(), databaseId), db, "");
    } else {
        _resultReady = false;
        _databases->patch(_aClient, Firestore::Parent(_projectId.c_str(), databaseId), db, "",
                          _firestoreResult);
        return "";
    }
}

String FirebaseV2Firestore::getDatabaseInfo(const String &databaseId, bool waitForCompletion) {
    if (_databases == nullptr) {
        return "Error: Databases not initialized";
    }

    if (waitForCompletion) {
        return _databases->get(_aClient, Firestore::Parent(_projectId.c_str(), databaseId));
    } else {
        _resultReady = false;
        _databases->get(_aClient, Firestore::Parent(_projectId.c_str(), databaseId), _firestoreResult);
        return "";
    }
}

String FirebaseV2Firestore::listDatabases(bool waitForCompletion) {
    if (_databases == nullptr) {
        return "Error: Databases not initialized";
    }

    if (waitForCompletion) {
        return _databases->list(_aClient, Firestore::Parent(_projectId.c_str()));
    } else {
        _resultReady = false;
        _databases->list(_aClient, Firestore::Parent(_projectId.c_str()), _firestoreResult);
        return "";
    }
}

String FirebaseV2Firestore::exportDocuments(const String &collectionIds, const String &bucketId, const String &storagePath, bool waitForCompletion) {
    if (_databases == nullptr) {
        return "Error: Databases not initialized";
    }

    EximDocumentOptions exportOptions(collectionIds, bucketId, storagePath);

    if (waitForCompletion) {
        return _databases->exportDocuments(_aClient, Firestore::Parent(_projectId.c_str()), exportOptions);
    } else {
        _resultReady = false;
        _databases->exportDocuments(_aClient, Firestore::Parent(_projectId.c_str()), exportOptions, _firestoreResult);
        return "";
    }
}

String FirebaseV2Firestore::importDocuments(const String &collectionIds, const String &bucketId, const String &storagePath, bool waitForCompletion) {
    if (_databases == nullptr) {
        return "Error: Databases not initialized";
    }

    EximDocumentOptions importOptions(collectionIds, bucketId, storagePath);

    if (waitForCompletion) {
        return _databases->importDocuments(_aClient, Firestore::Parent(_projectId.c_str()), importOptions);
    } else {
        _resultReady = false;
        _databases->importDocuments(_aClient, Firestore::Parent(_projectId.c_str()), importOptions, _firestoreResult);
        return "";
    }
}

String FirebaseV2Firestore::createDatabaseIndex(const String &collectionId, const String &fieldPath1, const String &fieldPath2, bool waitForCompletion) {
    if (_dbIndexes == nullptr) {
        return "Error: Database Indexes not initialized";
    }

    DatabaseIndex::Index index(collectionId);
    DatabaseIndex::IndexField indexField1, indexField2;

    indexField1.fieldPath(fieldPath1);
    indexField1.mode(DatabaseIndex::IndexMode::ASCENDING);

    indexField2.fieldPath(fieldPath2);
    indexField2.mode(DatabaseIndex::IndexMode::ASCENDING);

    index.fields(indexField1);
    index.fields(indexField2);

    if (waitForCompletion) {
        return _dbIndexes->create(_aClient, Firestore::Parent(_projectId.c_str()), index);
    } else {
        _resultReady = false;
        _dbIndexes->create(_aClient, Firestore::Parent(_projectId.c_str()), index, _firestoreResult);
        return "";
    }
}

String FirebaseV2Firestore::deleteDatabaseIndex(const String &indexId, bool waitForCompletion) {
    if (_dbIndexes == nullptr) {
        return "Error: Database Indexes not initialized";
    }

    if (waitForCompletion) {
        return _dbIndexes->deleteIndex(_aClient, Firestore::Parent(_projectId.c_str()), indexId);
    } else {
        _resultReady = false;
        _dbIndexes->deleteIndex(_aClient, Firestore::Parent(_projectId.c_str()), indexId, _firestoreResult);
        return "";
    }
}

String FirebaseV2Firestore::getDatabaseIndex(const String &indexId, bool waitForCompletion) {
    if (_dbIndexes == nullptr) {
        return "Error: Database Indexes not initialized";
    }

    if (waitForCompletion) {
        return _dbIndexes->get(_aClient, Firestore::Parent(_projectId.c_str()), indexId);
    } else {
        _resultReady = false;
        _dbIndexes->get(_aClient, Firestore::Parent(_projectId.c_str()), indexId, _firestoreResult);
        return "";
    }
}

String FirebaseV2Firestore::listDatabaseIndexes(bool waitForCompletion) {
    if (_dbIndexes == nullptr) {
        return "Error: Database Indexes not initialized";
    }

    if (waitForCompletion) {
        return _dbIndexes->list(_aClient, Firestore::Parent(_projectId.c_str()));
    } else {
        _resultReady = false;
        _dbIndexes->list(_aClient, Firestore::Parent(_projectId.c_str()), _firestoreResult);
        return "";
    }
}

String FirebaseV2Firestore::createCollectionGroupIndex(const String &collectionId, const String &fieldPath1, const String &fieldPath2, bool waitForCompletion) {
    if (_cgIndexes == nullptr) {
        return "Error: Collection Group Indexes not initialized";
    }

    CollectionGroupsIndex::VectorConfig vectorConfig;
    vectorConfig.dimension(1);
    vectorConfig.flat();

    CollectionGroupsIndex::IndexField indexField;
    indexField.fieldPath(fieldPath1);
    indexField.order(CollectionGroupsIndex::Order::ASCENDING);

    CollectionGroupsIndex::IndexField indexField2;
    indexField2.fieldPath(fieldPath2);
    indexField2.order(CollectionGroupsIndex::Order::ASCENDING);

    CollectionGroupsIndex::Index index;
    index.apiScope(CollectionGroupsIndex::ApiScope::ANY_API);
    index.queryScope(CollectionGroupsIndex::QueryScope::COLLECTION);
    index.fields(indexField);
    index.fields(indexField2);

    if (waitForCompletion) {
        return _cgIndexes->create(_aClient, Firestore::Parent(_projectId.c_str()), collectionId, index);
    } else {
        _resultReady = false;
        _cgIndexes->create(_aClient, Firestore::Parent(_projectId.c_str()), collectionId, index, _firestoreResult);
        return "";
    }
}

String FirebaseV2Firestore::deleteCollectionGroupIndex(const String &collectionId, const String &indexId, bool waitForCompletion) {
    if (_cgIndexes == nullptr) {
        return "Error: Collection Group Indexes not initialized";
    }

    if (waitForCompletion) {
        return _cgIndexes->deleteIndex(_aClient, Firestore::Parent(_projectId.c_str()), collectionId, indexId);
    } else {
        _resultReady = false;
        _cgIndexes->deleteIndex(_aClient, Firestore::Parent(_projectId.c_str()), collectionId, indexId, _firestoreResult);
        return "";
    }
}

String FirebaseV2Firestore::getCollectionGroupIndex(const String &collectionId, const String &indexId, bool waitForCompletion) {
    if (_cgIndexes == nullptr) {
        return "Error: Collection Group Indexes not initialized";
    }

    if (waitForCompletion) {
        return _cgIndexes->get(_aClient, Firestore::Parent(_projectId.c_str()), collectionId, indexId);
    } else {
        _resultReady = false;
        _cgIndexes->get(_aClient, Firestore::Parent(_projectId.c_str()), collectionId, indexId, _firestoreResult);
        return "";
    }
}

String FirebaseV2Firestore::listCollectionGroupIndexes(const String &collectionId, bool waitForCompletion) {
    if (_cgIndexes == nullptr) {
        return "Error: Collection Group Indexes not initialized";
    }

    if (waitForCompletion) {
        return _cgIndexes->list(_aClient, Firestore::Parent(_projectId.c_str()), collectionId);
    } else {
        _resultReady = false;
        _cgIndexes->list(_aClient, Firestore::Parent(_projectId.c_str()), collectionId, _firestoreResult);
        return "";
    }
}

String FirebaseV2Firestore::createDocument(const String &documentPath, const String &data, bool waitForCompletion) {
    if (_docs == nullptr) {
        return "Error: Documents not initialized";
    }

    Document<Values::Value> doc;
    doc.setContent(data);

    if (waitForCompletion) {
        return _docs->createDocument(_aClient, Firestore::Parent(_projectId.c_str()), documentPath, DocumentMask(), doc);
    } else {
        _resultReady = false;
        _docs->createDocument(_aClient, Firestore::Parent(_projectId.c_str()), documentPath, DocumentMask(), doc, _firestoreResult);
        return "";
    }
}

String FirebaseV2Firestore::createDocument(const String &documentPath, Document<Values::Value> &doc, bool waitForCompletion) {
    if (_docs == nullptr) {
        return "Error: Documents not initialized";
    }

    if (waitForCompletion) {
        return _docs->createDocument(_aClient, Firestore::Parent(_projectId.c_str()), documentPath, DocumentMask(), doc);
    } else {
        _resultReady = false;
        _docs->createDocument(_aClient, Firestore::Parent(_projectId.c_str()), documentPath, DocumentMask(), doc, _firestoreResult);
        return "";
    }
}

String FirebaseV2Firestore::getDocument(const String &documentPath, const String &mask, bool waitForCompletion) {
    if (_docs == nullptr) {
        return "Error: Documents not initialized";
    }

    GetDocumentOptions options;
    if (mask.length() > 0) {
        DocumentMask docMask(mask);
        options = GetDocumentOptions(docMask);
    }

    if (waitForCompletion) {
        return _docs->get(_aClient, Firestore::Parent(_projectId.c_str()), documentPath, options);
    } else {
        _resultReady = false;
        _docs->get(_aClient, Firestore::Parent(_projectId.c_str()), documentPath, options, _firestoreResult);
        return "";
    }
}

String FirebaseV2Firestore::updateDocument(const String &documentPath, const String &data, const String &updateMask, bool waitForCompletion) {
    if (_docs == nullptr) {
        return "Error: Documents not initialized";
    }

    Document<Values::Value> doc;
    doc.setContent(data);

    PatchDocumentOptions patchOptions((DocumentMask(updateMask)), DocumentMask(), Precondition());

    if (waitForCompletion) {
        return _docs->patch(_aClient, Firestore::Parent(_projectId.c_str()), documentPath, patchOptions, doc);
    } else {
        _resultReady = false;
        _docs->patch(_aClient, Firestore::Parent(_projectId.c_str()), documentPath, patchOptions, doc, _firestoreResult);
        return "";
    }
}

String FirebaseV2Firestore::deleteDocument(const String &documentPath, bool waitForCompletion) {
    if (_docs == nullptr) {
        return "Error: Documents not initialized";
    }

    if (waitForCompletion) {
        return _docs->deleteDoc(_aClient, Firestore::Parent(_projectId.c_str()), documentPath, Precondition());
    } else {
        _resultReady = false;
        _docs->deleteDoc(_aClient, Firestore::Parent(_projectId.c_str()), documentPath, Precondition(), _firestoreResult);
        return "";
    }
}

String FirebaseV2Firestore::listDocuments(const String &collectionPath, int pageSize, const String &mask, bool waitForCompletion) {
    if (_docs == nullptr) {
        return "Error: Documents not initialized";
    }

    ListDocumentsOptions listDocsOptions;
    listDocsOptions.pageSize(pageSize);

    if (mask.length() > 0) {
        listDocsOptions.mask(DocumentMask(mask));
    }

    if (waitForCompletion) {
        return _docs->list(_aClient, Firestore::Parent(_projectId.c_str()), collectionPath, listDocsOptions);
    } else {
        _resultReady = false;
        _docs->list(_aClient, Firestore::Parent(_projectId.c_str()), collectionPath, listDocsOptions, _firestoreResult);
        return "";
    }
}

String FirebaseV2Firestore::listCollectionIds(const String &documentPath, int pageSize, bool waitForCompletion) {
    if (_docs == nullptr) {
        return "Error: Documents not initialized";
    }

    ListCollectionIdsOptions listCollectionIdsOptions;
    listCollectionIdsOptions.pageSize(pageSize);

    if (waitForCompletion) {
        return _docs->listCollectionIds(_aClient, Firestore::Parent(_projectId.c_str()), documentPath, listCollectionIdsOptions);
    } else {
        _resultReady = false;
        _docs->listCollectionIds(_aClient, Firestore::Parent(_projectId.c_str()), documentPath, listCollectionIdsOptions, _firestoreResult);
        return "";
    }
}

String FirebaseV2Firestore::batchGet(const String &documentPaths, const String &mask, bool waitForCompletion) {
    if (_docs == nullptr) {
        return "Error: Documents not initialized";
    }

    BatchGetDocumentOptions options;

    int start = 0;
    int end = documentPaths.indexOf(',');
    while (end >= 0) {
        options.documents(documentPaths.substring(start, end));
        start = end + 1;
        end = documentPaths.indexOf(',', start);
    }
    options.documents(documentPaths.substring(start));

    if (mask.length() > 0) {
        options.mask(DocumentMask(mask));
    }

    if (waitForCompletion) {
        return _docs->batchGet(_aClient, Firestore::Parent(_projectId.c_str()), options);
    } else {
        _resultReady = false;
        _docs->batchGet(_aClient, Firestore::Parent(_projectId.c_str()), options, _firestoreResult);
        return "";
    }
}

String FirebaseV2Firestore::batchWrite(Writes &writes, bool waitForCompletion) {
    if (_docs == nullptr) {
        return "Error: Documents not initialized";
    }

    if (waitForCompletion) {
        return _docs->batchWrite(_aClient, Firestore::Parent(_projectId.c_str()), writes);
    } else {
        _resultReady = false;
        _docs->batchWrite(_aClient, Firestore::Parent(_projectId.c_str()), writes, _firestoreResult);
        return "";
    }
}

String FirebaseV2Firestore::runQuery(const String &documentPath, const String &queryJson, bool waitForCompletion) {
    if (_docs == nullptr) {
        return "Error: Documents not initialized";
    }

    QueryOptions queryOptions;

    if (queryJson.length() > 0) {
        StructuredQuery query;
        query.setContent(queryJson);
        queryOptions.structuredQuery(query);
    }

    if (waitForCompletion) {
        return _docs->runQuery(_aClient, Firestore::Parent(_projectId.c_str()), documentPath, queryOptions);
    } else {
        _resultReady = false;
        _docs->runQuery(_aClient, Firestore::Parent(_projectId.c_str()), documentPath, queryOptions, _firestoreResult);
        return "";
    }
}

String FirebaseV2Firestore::appendArray(const String &documentPath, const String &fieldPath, Values::ArrayValue &arrayValue, bool waitForCompletion) {
    if (_docs == nullptr) {
        return "Error: Documents not initialized";
    }

    FieldTransform::AppendMissingElements<Values::ArrayValue> appendValue(arrayValue);
    FieldTransform::FieldTransform fieldTransforms(fieldPath, appendValue);
    DocumentTransform transform(documentPath, fieldTransforms);

    Writes writes(Write(transform, Precondition()));

    if (waitForCompletion) {
        return _docs->commit(_aClient, Firestore::Parent(_projectId.c_str()), writes);
    } else {
        _resultReady = false;
        _docs->commit(_aClient, Firestore::Parent(_projectId.c_str()), writes, _firestoreResult);
        return "";
    }
}

String FirebaseV2Firestore::appendMapValue(const String &documentPath, const String &mapName, const String &key, Values::Value &value, bool waitForCompletion) {
    if (_docs == nullptr) {
        return "Error: Documents not initialized";
    }

    Values::MapValue mapV(key, value);

    Document<Values::Value> updateDoc;
    updateDoc.setName(documentPath);
    updateDoc.add(mapName, Values::Value(mapV));

    Writes writes(Write(DocumentMask(mapName
    +"." + key), updateDoc, Precondition()));

    if (waitForCompletion) {
        return _docs->commit(_aClient, Firestore::Parent(_projectId.c_str()), writes);
    } else {
        _resultReady = false;
        _docs->commit(_aClient, Firestore::Parent(_projectId.c_str()), writes, _firestoreResult);
        return "";
    }
}

String FirebaseV2Firestore::incrementFieldValue(const String &documentPath, const String &fieldPath, int incrementBy, bool waitForCompletion) {
    if (_docs == nullptr) {
        return "Error: Documents not initialized";
    }

    Values::IntegerValue intValue(incrementBy);
    FieldTransform::Increment incr(intValue);
    FieldTransform::FieldTransform fieldTransforms(fieldPath, incr);
    DocumentTransform transform(documentPath, fieldTransforms);

    Writes writes(Write(transform, Precondition()));

    if (waitForCompletion) {
        return _docs->commit(_aClient, Firestore::Parent(_projectId.c_str()), writes);
    } else {
        _resultReady = false;
        _docs->commit(_aClient, Firestore::Parent(_projectId.c_str()), writes, _firestoreResult);
        return "";
    }
}

String FirebaseV2Firestore::setServerTimestamp(const String &documentPath, const String &fieldPath, bool waitForCompletion) {
    if (_docs == nullptr) {
        return "Error: Documents not initialized";
    }

    FieldTransform::SetToServerValue setValue(FieldTransform::REQUEST_TIME);
    FieldTransform::FieldTransform fieldTransforms(fieldPath, setValue);
    DocumentTransform transform(documentPath, fieldTransforms);

    Writes writes(Write(transform, Precondition()));

    if (waitForCompletion) {
        return _docs->commit(_aClient, Firestore::Parent(_projectId.c_str()), writes);
    } else {
        _resultReady = false;
        _docs->commit(_aClient, Firestore::Parent(_projectId.c_str()), writes, _firestoreResult);
        return "";
    }
}

#endif
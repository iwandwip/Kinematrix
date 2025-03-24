#include "firebase-firestorev3.h"

#if defined(ESP32) || defined(ESP8266)

#include <time.h>

FirebaseV3Firestore::FirebaseV3Firestore()
        : _firebaseApp(nullptr), _usingSharedApp(false) {
    _sslClient = new SSL_CLIENT();
    _ownSSLClient = true;

#if defined(ESP32)
    _sslClient->setInsecure();
#elif defined(ESP8266)
    _sslClient->setInsecure();
    _sslClient->setBufferSizes(4096, 1024);
#endif

    _asyncClient = new AsyncClientClass(*_sslClient);
    _ownAsyncClient = true;

    _app = new FirebaseApp();
    _ownApp = true;

    _projectId = "";
    _useUserAuth = false;
    _apiKey = nullptr;
    _userEmail = nullptr;
    _userPassword = nullptr;
    _clientEmail = nullptr;
    _privateKey = nullptr;
    _credsSet = false;
}

FirebaseV3Firestore::FirebaseV3Firestore(FirebaseV3Application *firebaseApp)
        : _firebaseApp(firebaseApp), _usingSharedApp(true) {
    _sslClient = nullptr;
    _ownSSLClient = false;
    _asyncClient = nullptr;
    _ownAsyncClient = false;
    _app = nullptr;
    _ownApp = false;

    _projectId = "";
    _useUserAuth = false;
    _apiKey = nullptr;
    _userEmail = nullptr;
    _userPassword = nullptr;
    _clientEmail = nullptr;
    _privateKey = nullptr;
    _credsSet = false;
}

FirebaseV3Firestore::~FirebaseV3Firestore() {
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

    if (_ownAsyncClient && _asyncClient != nullptr) {
        delete _asyncClient;
        _asyncClient = nullptr;
    }

    if (_ownSSLClient && _sslClient != nullptr) {
        delete _sslClient;
        _sslClient = nullptr;
    }

    if (_ownApp && _app != nullptr) {
        delete _app;
        _app = nullptr;
    }
}

AsyncClientClass &FirebaseV3Firestore::getClient() {
    if (_usingSharedApp) {
        return _firebaseApp->getAsyncClient();
    } else {
        return *_asyncClient;
    }
}

FirebaseApp &FirebaseV3Firestore::getApp() {
    if (_usingSharedApp) {
        return _firebaseApp->getApp();
    } else {
        return *_app;
    }
}

bool FirebaseV3Firestore::begin(const char *apiKey, const char *userEmail, const char *userPassword, const char *projectId) {
    if (_usingSharedApp) {
        return begin(projectId);
    }

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

bool FirebaseV3Firestore::begin(const char *clientEmail, const char *projectId, const char *privateKey) {
    if (_usingSharedApp) {
        return begin(projectId);
    }

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

bool FirebaseV3Firestore::begin(const char *projectId) {
    if (!_usingSharedApp) {
        return false;
    }

    _projectId = projectId;

    if (_databases == nullptr) _databases = new Firestore::Databases();
    if (_dbIndexes == nullptr) _dbIndexes = new Firestore::Databases::Indexes();
    if (_cgIndexes == nullptr) _cgIndexes = new Firestore::CollectionGroups::Indexes();
    if (_docs == nullptr) _docs = new Firestore::Documents();

    if (_databases == nullptr || _dbIndexes == nullptr ||
        _cgIndexes == nullptr || _docs == nullptr) {
        return false;
    }

    getApp().getApp<Firestore::Databases>(*_databases);
    getApp().getApp<Firestore::Databases::Indexes>(*_dbIndexes);
    getApp().getApp<Firestore::CollectionGroups::Indexes>(*_cgIndexes);
    getApp().getApp<Firestore::Documents>(*_docs);

    return true;
}

bool FirebaseV3Firestore::begin() {
    if (_usingSharedApp) {
        return begin(_firebaseApp->getProjectId().c_str());
    }

    if (!_credsSet) {
        return false;
    }

    if (_useUserAuth) {
        getApp().setTime(time(nullptr));
        if (_userAuth != nullptr) {
            initializeApp(getClient(), getApp(), getAuth(*_userAuth), [](AsyncResult &result) {}, "authTask");
        } else {
            return false;
        }
    } else {
        getApp().setTime(time(nullptr));
        if (_serviceAuth != nullptr) {
            initializeApp(getClient(), getApp(), getAuth(*_serviceAuth), [](AsyncResult &result) {}, "authTask");
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

    getApp().getApp<Firestore::Databases>(*_databases);
    getApp().getApp<Firestore::Databases::Indexes>(*_dbIndexes);
    getApp().getApp<Firestore::CollectionGroups::Indexes>(*_cgIndexes);
    getApp().getApp<Firestore::Documents>(*_docs);

    return true;
}

void FirebaseV3Firestore::loop() {
    if (_usingSharedApp) {
        processResult(_firestoreResult);
    } else {
        getApp().loop();
        processResult(_firestoreResult);
    }
}

bool FirebaseV3Firestore::isReady() {
    if (_usingSharedApp) {
        return _firebaseApp->isReady();
    } else {
        return getApp().ready();
    }
}

String FirebaseV3Firestore::getLastError() {
    return getClient().lastError().message();
}

bool FirebaseV3Firestore::isResultReady() const {
    return _resultReady;
}

String FirebaseV3Firestore::getLastPayload() const {
    return _resultPayload;
}

void FirebaseV3Firestore::processResult(AsyncResult &aResult) {
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

void FirebaseV3Firestore::resultCallback(AsyncResult &aResult, void *userData) {
    FirebaseV3Firestore *wrapper = static_cast<FirebaseV3Firestore *>(userData);
    if (wrapper != nullptr) {
        wrapper->processResult(aResult);
    }
}

String FirebaseV3Firestore::createDatabase(const String &databaseId, const String &locationId, bool waitForCompletion) {
    if (_databases == nullptr) {
        return "Error: Databases not initialized";
    }

    Firestore::Database db;
    db.concurrencyMode(Firestore::ConcurrencyMode::PESSIMISTIC);
    db.databaseType(Firestore::DatabaseType::FIRESTORE_NATIVE);
    db.name(databaseId);
    db.locationId(locationId);

    if (waitForCompletion) {
        return _databases->create(getClient(), Firestore::Parent(_projectId.c_str(), databaseId), db);
    } else {
        _resultReady = false;
        _databases->create(getClient(), Firestore::Parent(_projectId.c_str(), databaseId), db,
                           _firestoreResult);
        return "";
    }
}

String FirebaseV3Firestore::deleteDatabase(const String &databaseId, bool waitForCompletion) {
    if (_databases == nullptr) {
        return "Error: Databases not initialized";
    }

    if (waitForCompletion) {
        return _databases->deleteDatabase(getClient(), Firestore::Parent(_projectId.c_str(), databaseId), "");
    } else {
        _resultReady = false;
        _databases->deleteDatabase(getClient(), Firestore::Parent(_projectId.c_str(), databaseId), "",
                                   _firestoreResult);
        return "";
    }
}

String FirebaseV3Firestore::updateDatabase(const String &databaseId, bool enablePointInTimeRecovery, bool waitForCompletion) {
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
        return _databases->patch(getClient(), Firestore::Parent(_projectId.c_str(), databaseId), db, "");
    } else {
        _resultReady = false;
        _databases->patch(getClient(), Firestore::Parent(_projectId.c_str(), databaseId), db, "",
                          _firestoreResult);
        return "";
    }
}

String FirebaseV3Firestore::getDatabaseInfo(const String &databaseId, bool waitForCompletion) {
    if (_databases == nullptr) {
        return "Error: Databases not initialized";
    }

    if (waitForCompletion) {
        return _databases->get(getClient(), Firestore::Parent(_projectId.c_str(), databaseId));
    } else {
        _resultReady = false;
        _databases->get(getClient(), Firestore::Parent(_projectId.c_str(), databaseId), _firestoreResult);
        return "";
    }
}

String FirebaseV3Firestore::listDatabases(bool waitForCompletion) {
    if (_databases == nullptr) {
        return "Error: Databases not initialized";
    }

    if (waitForCompletion) {
        return _databases->list(getClient(), Firestore::Parent(_projectId.c_str()));
    } else {
        _resultReady = false;
        _databases->list(getClient(), Firestore::Parent(_projectId.c_str()), _firestoreResult);
        return "";
    }
}

String FirebaseV3Firestore::exportDocuments(const String &collectionIds, const String &bucketId, const String &storagePath, bool waitForCompletion) {
    if (_databases == nullptr) {
        return "Error: Databases not initialized";
    }

    EximDocumentOptions exportOptions(collectionIds, bucketId, storagePath);

    if (waitForCompletion) {
        return _databases->exportDocuments(getClient(), Firestore::Parent(_projectId.c_str()), exportOptions);
    } else {
        _resultReady = false;
        _databases->exportDocuments(getClient(), Firestore::Parent(_projectId.c_str()), exportOptions, _firestoreResult);
        return "";
    }
}

String FirebaseV3Firestore::importDocuments(const String &collectionIds, const String &bucketId, const String &storagePath, bool waitForCompletion) {
    if (_databases == nullptr) {
        return "Error: Databases not initialized";
    }

    EximDocumentOptions importOptions(collectionIds, bucketId, storagePath);

    if (waitForCompletion) {
        return _databases->importDocuments(getClient(), Firestore::Parent(_projectId.c_str()), importOptions);
    } else {
        _resultReady = false;
        _databases->importDocuments(getClient(), Firestore::Parent(_projectId.c_str()), importOptions, _firestoreResult);
        return "";
    }
}

String FirebaseV3Firestore::createDatabaseIndex(const String &collectionId, const String &fieldPath1, const String &fieldPath2, bool waitForCompletion) {
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
        return _dbIndexes->create(getClient(), Firestore::Parent(_projectId.c_str()), index);
    } else {
        _resultReady = false;
        _dbIndexes->create(getClient(), Firestore::Parent(_projectId.c_str()), index, _firestoreResult);
        return "";
    }
}

String FirebaseV3Firestore::deleteDatabaseIndex(const String &indexId, bool waitForCompletion) {
    if (_dbIndexes == nullptr) {
        return "Error: Database Indexes not initialized";
    }

    if (waitForCompletion) {
        return _dbIndexes->deleteIndex(getClient(), Firestore::Parent(_projectId.c_str()), indexId);
    } else {
        _resultReady = false;
        _dbIndexes->deleteIndex(getClient(), Firestore::Parent(_projectId.c_str()), indexId, _firestoreResult);
        return "";
    }
}

String FirebaseV3Firestore::getDatabaseIndex(const String &indexId, bool waitForCompletion) {
    if (_dbIndexes == nullptr) {
        return "Error: Database Indexes not initialized";
    }

    if (waitForCompletion) {
        return _dbIndexes->get(getClient(), Firestore::Parent(_projectId.c_str()), indexId);
    } else {
        _resultReady = false;
        _dbIndexes->get(getClient(), Firestore::Parent(_projectId.c_str()), indexId, _firestoreResult);
        return "";
    }
}

String FirebaseV3Firestore::listDatabaseIndexes(bool waitForCompletion) {
    if (_dbIndexes == nullptr) {
        return "Error: Database Indexes not initialized";
    }

    if (waitForCompletion) {
        return _dbIndexes->list(getClient(), Firestore::Parent(_projectId.c_str()));
    } else {
        _resultReady = false;
        _dbIndexes->list(getClient(), Firestore::Parent(_projectId.c_str()), _firestoreResult);
        return "";
    }
}

String FirebaseV3Firestore::createCollectionGroupIndex(const String &collectionId, const String &fieldPath1, const String &fieldPath2, bool waitForCompletion) {
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
        return _cgIndexes->create(getClient(), Firestore::Parent(_projectId.c_str()), collectionId, index);
    } else {
        _resultReady = false;
        _cgIndexes->create(getClient(), Firestore::Parent(_projectId.c_str()), collectionId, index, _firestoreResult);
        return "";
    }
}

String FirebaseV3Firestore::deleteCollectionGroupIndex(const String &collectionId, const String &indexId, bool waitForCompletion) {
    if (_cgIndexes == nullptr) {
        return "Error: Collection Group Indexes not initialized";
    }

    if (waitForCompletion) {
        return _cgIndexes->deleteIndex(getClient(), Firestore::Parent(_projectId.c_str()), collectionId, indexId);
    } else {
        _resultReady = false;
        _cgIndexes->deleteIndex(getClient(), Firestore::Parent(_projectId.c_str()), collectionId, indexId, _firestoreResult);
        return "";
    }
}

String FirebaseV3Firestore::getCollectionGroupIndex(const String &collectionId, const String &indexId, bool waitForCompletion) {
    if (_cgIndexes == nullptr) {
        return "Error: Collection Group Indexes not initialized";
    }

    if (waitForCompletion) {
        return _cgIndexes->get(getClient(), Firestore::Parent(_projectId.c_str()), collectionId, indexId);
    } else {
        _resultReady = false;
        _cgIndexes->get(getClient(), Firestore::Parent(_projectId.c_str()), collectionId, indexId, _firestoreResult);
        return "";
    }
}

String FirebaseV3Firestore::listCollectionGroupIndexes(const String &collectionId, bool waitForCompletion) {
    if (_cgIndexes == nullptr) {
        return "Error: Collection Group Indexes not initialized";
    }

    if (waitForCompletion) {
        return _cgIndexes->list(getClient(), Firestore::Parent(_projectId.c_str()), collectionId);
    } else {
        _resultReady = false;
        _cgIndexes->list(getClient(), Firestore::Parent(_projectId.c_str()), collectionId, _firestoreResult);
        return "";
    }
}

String FirebaseV3Firestore::createDocument(const String &documentPath, const String &data, bool waitForCompletion) {
    if (_docs == nullptr) {
        return "Error: Documents not initialized";
    }

    Document<Values::Value> doc;
    doc.setContent(data);

    if (waitForCompletion) {
        return _docs->createDocument(getClient(), Firestore::Parent(_projectId.c_str()), documentPath, DocumentMask(), doc);
    } else {
        _resultReady = false;
        _docs->createDocument(getClient(), Firestore::Parent(_projectId.c_str()), documentPath, DocumentMask(), doc, _firestoreResult);
        return "";
    }
}

String FirebaseV3Firestore::createDocument(const String &documentPath, Document<Values::Value> &doc, bool waitForCompletion) {
    if (_docs == nullptr) {
        return "Error: Documents not initialized";
    }

    if (waitForCompletion) {
        return _docs->createDocument(getClient(), Firestore::Parent(_projectId.c_str()), documentPath, DocumentMask(), doc);
    } else {
        _resultReady = false;
        _docs->createDocument(getClient(), Firestore::Parent(_projectId.c_str()), documentPath, DocumentMask(), doc, _firestoreResult);
        return "";
    }
}

String FirebaseV3Firestore::getDocument(const String &documentPath, const String &mask, bool waitForCompletion) {
    if (_docs == nullptr) {
        return "Error: Documents not initialized";
    }

    GetDocumentOptions options;
    if (mask.length() > 0) {
        DocumentMask docMask(mask);
        options = GetDocumentOptions(docMask);
    }

    if (waitForCompletion) {
        return _docs->get(getClient(), Firestore::Parent(_projectId.c_str()), documentPath, options);
    } else {
        _resultReady = false;
        _docs->get(getClient(), Firestore::Parent(_projectId.c_str()), documentPath, options, _firestoreResult);
        return "";
    }
}

String FirebaseV3Firestore::updateDocument(const String &documentPath, const String &data, const String &updateMask, bool waitForCompletion) {
    if (_docs == nullptr) {
        return "Error: Documents not initialized";
    }

    Document<Values::Value> doc;
    doc.setContent(data);

    PatchDocumentOptions patchOptions((DocumentMask(updateMask)), DocumentMask(), Precondition());

    if (waitForCompletion) {
        return _docs->patch(getClient(), Firestore::Parent(_projectId.c_str()), documentPath, patchOptions, doc);
    } else {
        _resultReady = false;
        _docs->patch(getClient(), Firestore::Parent(_projectId.c_str()), documentPath, patchOptions, doc, _firestoreResult);
        return "";
    }
}

String FirebaseV3Firestore::deleteDocument(const String &documentPath, bool waitForCompletion) {
    if (_docs == nullptr) {
        return "Error: Documents not initialized";
    }

    if (waitForCompletion) {
        return _docs->deleteDoc(getClient(), Firestore::Parent(_projectId.c_str()), documentPath, Precondition());
    } else {
        _resultReady = false;
        _docs->deleteDoc(getClient(), Firestore::Parent(_projectId.c_str()), documentPath, Precondition(), _firestoreResult);
        return "";
    }
}

String FirebaseV3Firestore::listDocuments(const String &collectionPath, int pageSize, const String &mask, bool waitForCompletion) {
    if (_docs == nullptr) {
        return "Error: Documents not initialized";
    }

    ListDocumentsOptions listDocsOptions;
    listDocsOptions.pageSize(pageSize);

    if (mask.length() > 0) {
        listDocsOptions.mask(DocumentMask(mask));
    }

    if (waitForCompletion) {
        return _docs->list(getClient(), Firestore::Parent(_projectId.c_str()), collectionPath, listDocsOptions);
    } else {
        _resultReady = false;
        _docs->list(getClient(), Firestore::Parent(_projectId.c_str()), collectionPath, listDocsOptions, _firestoreResult);
        return "";
    }
}

String FirebaseV3Firestore::listCollectionIds(const String &documentPath, int pageSize, bool waitForCompletion) {
    if (_docs == nullptr) {
        return "Error: Documents not initialized";
    }

    ListCollectionIdsOptions listCollectionIdsOptions;
    listCollectionIdsOptions.pageSize(pageSize);

    if (waitForCompletion) {
        return _docs->listCollectionIds(getClient(), Firestore::Parent(_projectId.c_str()), documentPath, listCollectionIdsOptions);
    } else {
        _resultReady = false;
        _docs->listCollectionIds(getClient(), Firestore::Parent(_projectId.c_str()), documentPath, listCollectionIdsOptions, _firestoreResult);
        return "";
    }
}

String FirebaseV3Firestore::batchGet(const String &documentPaths, const String &mask, bool waitForCompletion) {
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
        return _docs->batchGet(getClient(), Firestore::Parent(_projectId.c_str()), options);
    } else {
        _resultReady = false;
        _docs->batchGet(getClient(), Firestore::Parent(_projectId.c_str()), options, _firestoreResult);
        return "";
    }
}

String FirebaseV3Firestore::batchWrite(Writes &writes, bool waitForCompletion) {
    if (_docs == nullptr) {
        return "Error: Documents not initialized";
    }

    if (waitForCompletion) {
        return _docs->batchWrite(getClient(), Firestore::Parent(_projectId.c_str()), writes);
    } else {
        _resultReady = false;
        _docs->batchWrite(getClient(), Firestore::Parent(_projectId.c_str()), writes, _firestoreResult);
        return "";
    }
}

String FirebaseV3Firestore::runQuery(const String &documentPath, const String &queryJson, bool waitForCompletion) {
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
        return _docs->runQuery(getClient(), Firestore::Parent(_projectId.c_str()), documentPath, queryOptions);
    } else {
        _resultReady = false;
        _docs->runQuery(getClient(), Firestore::Parent(_projectId.c_str()), documentPath, queryOptions, _firestoreResult);
        return "";
    }
}

String FirebaseV3Firestore::appendArray(const String &documentPath, const String &fieldPath, Values::ArrayValue &arrayValue, bool waitForCompletion) {
    if (_docs == nullptr) {
        return "Error: Documents not initialized";
    }

    FieldTransform::AppendMissingElements<Values::ArrayValue> appendValue(arrayValue);
    FieldTransform::FieldTransform fieldTransforms(fieldPath, appendValue);
    DocumentTransform transform(documentPath, fieldTransforms);

    Writes writes(Write(transform, Precondition()));

    if (waitForCompletion) {
        return _docs->commit(getClient(), Firestore::Parent(_projectId.c_str()), writes);
    } else {
        _resultReady = false;
        _docs->commit(getClient(), Firestore::Parent(_projectId.c_str()), writes, _firestoreResult);
        return "";
    }
}

String FirebaseV3Firestore::appendMapValue(const String &documentPath, const String &mapName, const String &key, Values::Value &value, bool waitForCompletion) {
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
        return _docs->commit(getClient(), Firestore::Parent(_projectId.c_str()), writes);
    } else {
        _resultReady = false;
        _docs->commit(getClient(), Firestore::Parent(_projectId.c_str()), writes, _firestoreResult);
        return "";
    }
}

String FirebaseV3Firestore::incrementFieldValue(const String &documentPath, const String &fieldPath, int incrementBy, bool waitForCompletion) {
    if (_docs == nullptr) {
        return "Error: Documents not initialized";
    }

    Values::IntegerValue intValue(incrementBy);
    FieldTransform::Increment incr(intValue);
    FieldTransform::FieldTransform fieldTransforms(fieldPath, incr);
    DocumentTransform transform(documentPath, fieldTransforms);

    Writes writes(Write(transform, Precondition()));

    if (waitForCompletion) {
        return _docs->commit(getClient(), Firestore::Parent(_projectId.c_str()), writes);
    } else {
        _resultReady = false;
        _docs->commit(getClient(), Firestore::Parent(_projectId.c_str()), writes, _firestoreResult);
        return "";
    }
}

String FirebaseV3Firestore::setServerTimestamp(const String &documentPath, const String &fieldPath, bool waitForCompletion) {
    if (_docs == nullptr) {
        return "Error: Documents not initialized";
    }

    FieldTransform::SetToServerValue setValue(FieldTransform::REQUEST_TIME);
    FieldTransform::FieldTransform fieldTransforms(fieldPath, setValue);
    DocumentTransform transform(documentPath, fieldTransforms);

    Writes writes(Write(transform, Precondition()));

    if (waitForCompletion) {
        return _docs->commit(getClient(), Firestore::Parent(_projectId.c_str()), writes);
    } else {
        _resultReady = false;
        _docs->commit(getClient(), Firestore::Parent(_projectId.c_str()), writes, _firestoreResult);
        return "";
    }
}

#endif
#include "firebase-storagev3.h"

#if defined(ESP32) || defined(ESP8266)

#include <time.h>

FirebaseV3Storage::FirebaseV3Storage()
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

    _storage = new CloudStorage();

    _projectId = "";
    _bucketId = "";
    _useUserAuth = false;
    _apiKey = nullptr;
    _userEmail = nullptr;
    _userPassword = nullptr;
    _clientEmail = nullptr;
    _privateKey = nullptr;
    _credsSet = false;
    _authTimeout = 3000;
    _resultReady = false;
    _downloadProgress = 0;
    _uploadProgress = 0;
}

FirebaseV3Storage::FirebaseV3Storage(FirebaseV3Application *firebaseApp)
        : _firebaseApp(firebaseApp), _usingSharedApp(true) {
    _sslClient = nullptr;
    _ownSSLClient = false;
    _asyncClient = nullptr;
    _ownAsyncClient = false;
    _app = nullptr;
    _ownApp = false;

    _storage = new CloudStorage();

    _projectId = "";
    _bucketId = "";
    _useUserAuth = false;
    _apiKey = nullptr;
    _userEmail = nullptr;
    _userPassword = nullptr;
    _clientEmail = nullptr;
    _privateKey = nullptr;
    _credsSet = false;
    _authTimeout = 3000;
    _resultReady = false;
    _downloadProgress = 0;
    _uploadProgress = 0;
}

FirebaseV3Storage::~FirebaseV3Storage() {
    if (_userAuth != nullptr) {
        delete _userAuth;
        _userAuth = nullptr;
    }

    if (_serviceAuth != nullptr) {
        delete _serviceAuth;
        _serviceAuth = nullptr;
    }

    if (_storage != nullptr) {
        delete _storage;
        _storage = nullptr;
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

AsyncClientClass &FirebaseV3Storage::getClient() {
    if (_usingSharedApp) {
        return _firebaseApp->getAsyncClient();
    } else {
        return *_asyncClient;
    }
}

FirebaseApp &FirebaseV3Storage::getApp() {
    if (_usingSharedApp) {
        return _firebaseApp->getApp();
    } else {
        return *_app;
    }
}

bool FirebaseV3Storage::begin(const char *apiKey, const char *userEmail, const char *userPassword,
                              const char *projectId, const char *bucketId, int timeout) {
    if (_usingSharedApp) {
        return begin(bucketId);
    }

    _useUserAuth = true;
    _apiKey = apiKey;
    _userEmail = userEmail;
    _userPassword = userPassword;
    _projectId = projectId;
    _bucketId = bucketId;
    _authTimeout = timeout;

    if (_userAuth != nullptr) {
        delete _userAuth;
        _userAuth = nullptr;
    }
    if (_serviceAuth != nullptr) {
        delete _serviceAuth;
        _serviceAuth = nullptr;
    }

    _userAuth = new UserAuth(_apiKey, _userEmail, _userPassword, _authTimeout);
    if (_userAuth == nullptr) {
        return false;
    }

    _credsSet = true;
    return begin();
}

bool FirebaseV3Storage::begin(const char *clientEmail, const char *projectId, const char *privateKey,
                              const char *bucketId, int timeout) {
    if (_usingSharedApp) {
        return begin(bucketId);
    }

    _useUserAuth = false;
    _clientEmail = clientEmail;
    _projectId = projectId;
    _privateKey = privateKey;
    _bucketId = bucketId;
    _authTimeout = timeout;

    if (_userAuth != nullptr) {
        delete _userAuth;
        _userAuth = nullptr;
    }
    if (_serviceAuth != nullptr) {
        delete _serviceAuth;
        _serviceAuth = nullptr;
    }

    _serviceAuth = new ServiceAuth(_clientEmail, _projectId.c_str(), _privateKey, _authTimeout);
    if (_serviceAuth == nullptr) {
        return false;
    }

    _credsSet = true;
    return begin();
}

bool FirebaseV3Storage::begin(const char *bucketId) {
    if (!_usingSharedApp) {
        return false;
    }

    _bucketId = bucketId;
    _projectId = _firebaseApp->getProjectId();

    _firebaseApp->getApp().getApp<CloudStorage>(*_storage);
    return true;
}

time_t FirebaseV3Storage::getValidTimeFromNTP() {
    time_t now = 0;

#if defined(ESP32)
    configTime(gmtOffset_sec, daylightOffset_sec, ntpServer1, ntpServer2);
#elif defined(ESP8266)
    configTime(gmtOffset_sec, daylightOffset_sec, ntpServer1, ntpServer2);
#endif

    Serial.println("Waiting for NTP server time...");

    int retry = 0;
    const int maxRetries = 10;
    while (now < 24 * 3600 && retry < maxRetries) {
        Serial.print(".");
        delay(1000);
        time(&now);
        retry++;
    }

    Serial.println();
    if (now > 24 * 3600) {
        struct tm timeinfo;
        gmtime_r(&now, &timeinfo);
        Serial.print("Current time: ");
        Serial.print(timeinfo.tm_hour);
        Serial.print(":");
        Serial.print(timeinfo.tm_min);
        Serial.print(":");
        Serial.println(timeinfo.tm_sec);
        return now;
    } else {
        Serial.println("Failed to get NTP time!");
        return 0;
    }
}

bool FirebaseV3Storage::begin() {
    if (_usingSharedApp) {
        return begin(_bucketId.c_str());
    }

    if (!_credsSet) {
        return false;
    }

    time_t validTime = getValidTimeFromNTP();
    if (validTime == 0) {
        Serial.println("FATAL: Could not get a valid time!");
        return false;
    }

    getApp().setTime(validTime);

    if (_useUserAuth) {
        if (_userAuth != nullptr) {
            initializeApp(getClient(), getApp(), getAuth(*_userAuth), [](AsyncResult &result) {
                if (result.isError()) {
                    Serial.println("Auth Error: " + result.error().message());
                }
            }, "authTask");
        } else {
            return false;
        }
    } else {
        if (_serviceAuth != nullptr) {
            initializeApp(getClient(), getApp(), getAuth(*_serviceAuth), [](AsyncResult &result) {
                if (result.isError()) {
                    Serial.println("Auth Error: " + result.error().message());
                }
            }, "authTask");
        } else {
            return false;
        }
    }

    getApp().getApp<CloudStorage>(*_storage);
    return true;
}

void FirebaseV3Storage::loop() {
    if (_usingSharedApp) {
        processResult(_storageResult);
    } else {
        getApp().loop();
        processResult(_storageResult);
    }
}

bool FirebaseV3Storage::isReady() {
    if (_usingSharedApp) {
        return _firebaseApp->isReady();
    } else {
        return getApp().ready();
    }
}

String FirebaseV3Storage::getLastError() {
    return getClient().lastError().message();
}

bool FirebaseV3Storage::isResultReady() const {
    return _resultReady;
}

String FirebaseV3Storage::getLastPayload() const {
    return _resultPayload;
}

int FirebaseV3Storage::getDownloadProgress() const {
    return _downloadProgress;
}

int FirebaseV3Storage::getUploadProgress() const {
    return _uploadProgress;
}

void FirebaseV3Storage::processResult(AsyncResult &aResult) {
    if (!aResult.isResult())
        return;

    if (aResult.isError()) {
        _resultReady = true;
        _resultPayload = aResult.error().message();
    }

    if (aResult.isDebug()) {
    }

    if (aResult.available()) {
        _resultReady = true;
        _resultPayload = aResult.c_str();
    }

    if (aResult.downloadProgress()) {
        _downloadProgress = aResult.downloadInfo().progress;
        if (aResult.downloadInfo().total == aResult.downloadInfo().downloaded) {
            _resultReady = true;
            _resultPayload = "Download complete";
        }
    }

    if (aResult.uploadProgress()) {
        _uploadProgress = aResult.uploadInfo().progress;
        if (aResult.uploadInfo().total == aResult.uploadInfo().uploaded) {
            _resultReady = true;
            _resultPayload = "Upload complete";
            _resultPayload += "\nDownload URL: " + aResult.uploadInfo().downloadUrl;
        }
    }
}

void FirebaseV3Storage::resultCallback(AsyncResult &aResult, void *userData) {
    FirebaseV3Storage *wrapper = static_cast<FirebaseV3Storage *>(userData);
    if (wrapper != nullptr) {
        wrapper->processResult(aResult);
    }
}

bool FirebaseV3Storage::uploadFile(const char *localFileName, const char *storageObjectName,
                                   const char *mimeType, bool waitForCompletion) {
    if (!isReady()) {
        return false;
    }

    _resultReady = false;
    _uploadProgress = 0;

    FileConfig fileConfig(localFileName);

    GoogleCloudStorage::UploadOptions options;
    options.mime = mimeType;
    options.uploadType = GoogleCloudStorage::upload_type_resumable;

    if (waitForCompletion) {
        bool status = _storage->upload(getClient(),
                                       GoogleCloudStorage::Parent(_bucketId.c_str(), storageObjectName),
                                       getFile(fileConfig),
                                       options);
        _resultReady = true;
        if (!status) {
            _resultPayload = getClient().lastError().message();
        }
        return status;
    } else {
        _storage->upload(getClient(),
                         GoogleCloudStorage::Parent(_bucketId.c_str(), storageObjectName),
                         getFile(fileConfig),
                         options,
                         _storageResult);
        return true;
    }
}

bool FirebaseV3Storage::downloadFile(const char *storageObjectName, const char *localFileName,
                                     bool waitForCompletion) {
    if (!isReady()) {
        return false;
    }

    _resultReady = false;
    _downloadProgress = 0;

    FileConfig fileConfig(localFileName);

    GoogleCloudStorage::GetOptions options;

    if (waitForCompletion) {
        bool status = _storage->download(getClient(),
                                         GoogleCloudStorage::Parent(_bucketId.c_str(), storageObjectName),
                                         getFile(fileConfig),
                                         options);
        _resultReady = true;
        if (!status) {
            _resultPayload = getClient().lastError().message();
        }
        return status;
    } else {
        _storage->download(getClient(),
                           GoogleCloudStorage::Parent(_bucketId.c_str(), storageObjectName),
                           getFile(fileConfig),
                           options,
                           _storageResult);
        return true;
    }
}

bool FirebaseV3Storage::deleteFile(const char *storageObjectName, bool waitForCompletion) {
    if (!isReady()) {
        return false;
    }

    _resultReady = false;

    GoogleCloudStorage::DeleteOptions options;

    if (waitForCompletion) {
        bool status = _storage->deleteObject(getClient(),
                                             GoogleCloudStorage::Parent(_bucketId.c_str(), storageObjectName),
                                             options);
        _resultReady = true;
        if (!status) {
            _resultPayload = getClient().lastError().message();
        }
        return status;
    } else {
        _storage->deleteObject(getClient(),
                               GoogleCloudStorage::Parent(_bucketId.c_str(), storageObjectName),
                               options,
                               _storageResult);
        return true;
    }
}

String FirebaseV3Storage::getFileMetadata(const char *storageObjectName, bool waitForCompletion) {
    if (!isReady()) {
        return "Error: Firebase not ready";
    }

    _resultReady = false;

    GoogleCloudStorage::GetOptions options;

    if (waitForCompletion) {
        String payload = _storage->getMetadata(getClient(),
                                               GoogleCloudStorage::Parent(_bucketId.c_str(), storageObjectName),
                                               options);
        _resultReady = true;
        if (getClient().lastError().code() != 0) {
            _resultPayload = getClient().lastError().message();
            return "Error: " + _resultPayload;
        }
        return payload;
    } else {
        _storage->getMetadata(getClient(),
                              GoogleCloudStorage::Parent(_bucketId.c_str(), storageObjectName),
                              options,
                              _storageResult);
        return "";
    }
}

String FirebaseV3Storage::listFiles(const char *prefix, int maxResults, bool waitForCompletion) {
    if (!isReady()) {
        return "Error: Firebase not ready";
    }

    _resultReady = false;

    GoogleCloudStorage::ListOptions options;
    options.maxResults(maxResults);

    if (prefix != nullptr && strlen(prefix) > 0) {
        options.prefix(prefix);
    }

    if (waitForCompletion) {
        String payload = _storage->list(getClient(),
                                        GoogleCloudStorage::Parent(_bucketId.c_str()),
                                        options);
        _resultReady = true;
        if (getClient().lastError().code() != 0) {
            _resultPayload = getClient().lastError().message();
            return "Error: " + _resultPayload;
        }
        return payload;
    } else {
        _storage->list(getClient(),
                       GoogleCloudStorage::Parent(_bucketId.c_str()),
                       options,
                       _storageResult);
        return "";
    }
}

bool FirebaseV3Storage::performOTA(const char *storageObjectName, bool waitForCompletion) {
    if (!isReady()) {
        return false;
    }

    _resultReady = false;

    GoogleCloudStorage::GetOptions options;

    if (waitForCompletion) {
        bool status = _storage->ota(getClient(),
                                    GoogleCloudStorage::Parent(_bucketId.c_str(), storageObjectName),
                                    options);
        _resultReady = true;
        if (!status) {
            _resultPayload = getClient().lastError().message();
        }
        return status;
    } else {
        _storage->ota(getClient(),
                      GoogleCloudStorage::Parent(_bucketId.c_str(), storageObjectName),
                      options,
                      _storageResult);
        return true;
    }
}

#endif
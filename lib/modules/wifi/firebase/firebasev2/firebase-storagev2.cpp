/*
 *  firebase-storagev2.cpp
 *
 *  Simplified Firebase Storage handler implementation
 */

#include "firebase-storagev2.h"

#if defined(ESP32) || defined(ESP8266)

#include <time.h>

FirebaseV2Storage::FirebaseV2Storage()
        : _aClient(_sslClient), _projectId(""), _bucketId(""), _useUserAuth(false),
          _apiKey(nullptr), _userEmail(nullptr), _userPassword(nullptr),
          _clientEmail(nullptr), _privateKey(nullptr), _credsSet(false),
          _authTimeout(3000) {

    set_ssl_client_insecure_and_buffer(_sslClient);
}

FirebaseV2Storage::~FirebaseV2Storage() {
    if (_userAuth != nullptr) {
        delete _userAuth;
        _userAuth = nullptr;
    }

    if (_serviceAuth != nullptr) {
        delete _serviceAuth;
        _serviceAuth = nullptr;
    }
}

bool FirebaseV2Storage::begin(const char *apiKey, const char *userEmail, const char *userPassword,
                              const char *projectId, const char *bucketId, int timeout) {
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

bool FirebaseV2Storage::begin(const char *clientEmail, const char *projectId, const char *privateKey,
                              const char *bucketId, int timeout) {
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

time_t FirebaseV2Storage::getValidTimeFromNTP() {
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

bool FirebaseV2Storage::begin() {
    if (!_credsSet) {
        return false;
    }

    time_t validTime = getValidTimeFromNTP();
    if (validTime == 0) {
        Serial.println("FATAL: Could not get a valid time!");
        return false;
    }

    _app.setTime(validTime);

    if (_useUserAuth) {
        if (_userAuth != nullptr) {
            initializeApp(_aClient, _app, getAuth(*_userAuth), [](AsyncResult &result) {
                if (result.isError()) {
                    Serial.println("Auth Error: " + result.error().message());
                }
            }, "authTask");
        } else {
            return false;
        }
    } else {
        if (_serviceAuth != nullptr) {
            initializeApp(_aClient, _app, getAuth(*_serviceAuth), [](AsyncResult &result) {
                if (result.isError()) {
                    Serial.println("Auth Error: " + result.error().message());
                }
            }, "authTask");
        } else {
            return false;
        }
    }

    _app.getApp<CloudStorage>(_storage);
    return true;
}

void FirebaseV2Storage::loop() {
    _app.loop();
    processResult(_storageResult);
}

bool FirebaseV2Storage::isReady() {
    return _app.ready();
}

String FirebaseV2Storage::getLastError() {
    return _aClient.lastError().message();
}

bool FirebaseV2Storage::isResultReady() const {
    return _resultReady;
}

String FirebaseV2Storage::getLastPayload() const {
    return _resultPayload;
}

int FirebaseV2Storage::getDownloadProgress() const {
    return _downloadProgress;
}

int FirebaseV2Storage::getUploadProgress() const {
    return _uploadProgress;
}

void FirebaseV2Storage::processResult(AsyncResult &aResult) {
    if (!aResult.isResult())
        return;

    if (aResult.isError()) {
        _resultReady = true;
        _resultPayload = aResult.error().message();
    }

    if (aResult.isDebug()) {
        // Process debug info if needed
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

void FirebaseV2Storage::resultCallback(AsyncResult &aResult, void *userData) {
    FirebaseV2Storage *wrapper = static_cast<FirebaseV2Storage *>(userData);
    if (wrapper != nullptr) {
        wrapper->processResult(aResult);
    }
}

bool FirebaseV2Storage::uploadFile(const char *localFileName, const char *storageObjectName,
                                   const char *mimeType, bool waitForCompletion) {
    if (!isReady()) {
        return false;
    }

    // Reset result status
    _resultReady = false;
    _uploadProgress = 0;

    // Create file config
    FileConfig fileConfig(localFileName);

    // Configure upload options
    GoogleCloudStorage::UploadOptions options;
    options.mime = mimeType;
    options.uploadType = GoogleCloudStorage::upload_type_resumable;

    if (waitForCompletion) {
        bool status = _storage.upload(_aClient,
                                      GoogleCloudStorage::Parent(_bucketId.c_str(), storageObjectName),
                                      getFile(fileConfig),
                                      options);
        _resultReady = true;
        if (!status) {
            _resultPayload = _aClient.lastError().message();
        }
        return status;
    } else {
        _storage.upload(_aClient,
                        GoogleCloudStorage::Parent(_bucketId.c_str(), storageObjectName),
                        getFile(fileConfig),
                        options,
                        _storageResult);
        return true;
    }
}

bool FirebaseV2Storage::downloadFile(const char *storageObjectName, const char *localFileName,
                                     bool waitForCompletion) {
    if (!isReady()) {
        return false;
    }

    // Reset result status
    _resultReady = false;
    _downloadProgress = 0;

    // Create file config
    FileConfig fileConfig(localFileName);

    // Setup options
    GoogleCloudStorage::GetOptions options;

    if (waitForCompletion) {
        bool status = _storage.download(_aClient,
                                        GoogleCloudStorage::Parent(_bucketId.c_str(), storageObjectName),
                                        getFile(fileConfig),
                                        options);
        _resultReady = true;
        if (!status) {
            _resultPayload = _aClient.lastError().message();
        }
        return status;
    } else {
        _storage.download(_aClient,
                          GoogleCloudStorage::Parent(_bucketId.c_str(), storageObjectName),
                          getFile(fileConfig),
                          options,
                          _storageResult);
        return true;
    }
}

bool FirebaseV2Storage::deleteFile(const char *storageObjectName, bool waitForCompletion) {
    if (!isReady()) {
        return false;
    }

    // Reset result status
    _resultReady = false;

    // Setup options
    GoogleCloudStorage::DeleteOptions options;

    if (waitForCompletion) {
        bool status = _storage.deleteObject(_aClient,
                                            GoogleCloudStorage::Parent(_bucketId.c_str(), storageObjectName),
                                            options);
        _resultReady = true;
        if (!status) {
            _resultPayload = _aClient.lastError().message();
        }
        return status;
    } else {
        _storage.deleteObject(_aClient,
                              GoogleCloudStorage::Parent(_bucketId.c_str(), storageObjectName),
                              options,
                              _storageResult);
        return true;
    }
}

String FirebaseV2Storage::getFileMetadata(const char *storageObjectName, bool waitForCompletion) {
    if (!isReady()) {
        return "Error: Firebase not ready";
    }

    // Reset result status
    _resultReady = false;

    // Setup options
    GoogleCloudStorage::GetOptions options;

    if (waitForCompletion) {
        String payload = _storage.getMetadata(_aClient,
                                              GoogleCloudStorage::Parent(_bucketId.c_str(), storageObjectName),
                                              options);
        _resultReady = true;
        if (_aClient.lastError().code() != 0) {
            _resultPayload = _aClient.lastError().message();
            return "Error: " + _resultPayload;
        }
        return payload;
    } else {
        _storage.getMetadata(_aClient,
                             GoogleCloudStorage::Parent(_bucketId.c_str(), storageObjectName),
                             options,
                             _storageResult);
        return "";
    }
}

String FirebaseV2Storage::listFiles(const char *prefix, int maxResults, bool waitForCompletion) {
    if (!isReady()) {
        return "Error: Firebase not ready";
    }

    // Reset result status
    _resultReady = false;

    // Setup options
    GoogleCloudStorage::ListOptions options;
    options.maxResults(maxResults);

    if (prefix != nullptr && strlen(prefix) > 0) {
        options.prefix(prefix);
    }

    if (waitForCompletion) {
        String payload = _storage.list(_aClient,
                                       GoogleCloudStorage::Parent(_bucketId.c_str()),
                                       options);
        _resultReady = true;
        if (_aClient.lastError().code() != 0) {
            _resultPayload = _aClient.lastError().message();
            return "Error: " + _resultPayload;
        }
        return payload;
    } else {
        _storage.list(_aClient,
                      GoogleCloudStorage::Parent(_bucketId.c_str()),
                      options,
                      _storageResult);
        return "";
    }
}

bool FirebaseV2Storage::performOTA(const char *storageObjectName, bool waitForCompletion) {
    if (!isReady()) {
        return false;
    }

    // Reset result status
    _resultReady = false;

    // Setup options
    GoogleCloudStorage::GetOptions options;

    if (waitForCompletion) {
        bool status = _storage.ota(_aClient,
                                   GoogleCloudStorage::Parent(_bucketId.c_str(), storageObjectName),
                                   options);
        _resultReady = true;
        if (!status) {
            _resultPayload = _aClient.lastError().message();
        }
        return status;
    } else {
        _storage.ota(_aClient,
                     GoogleCloudStorage::Parent(_bucketId.c_str(), storageObjectName),
                     options,
                     _storageResult);
        return true;
    }
}

#endif // ESP32 || ESP8266
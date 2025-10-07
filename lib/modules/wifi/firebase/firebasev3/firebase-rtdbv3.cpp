#include "firebase-rtdbv3.h"

#if defined(ESP32) || defined(ESP8266)

FirebaseV3RTDB::FirebaseV3RTDB()
        : _firebaseApp(nullptr), _usingSharedApp(false) {
    _app = new FirebaseApp();
    _ownApp = true;

    _rtdb = new RealtimeDatabase();

    _asyncClient = new AsyncClientClass();
    _ownAsyncClient = true;

    _streamClient = nullptr;
    _ownStreamClient = false;

    _isInitialized = false;
    _streamInitialized = false;
    _hasStreamData = false;
    _hasError = false;
    _streamDataType = 0;
}

FirebaseV3RTDB::FirebaseV3RTDB(FirebaseV3Application *firebaseApp)
        : _firebaseApp(firebaseApp), _usingSharedApp(true) {
    _app = nullptr;
    _ownApp = false;

    _rtdb = new RealtimeDatabase();

    _asyncClient = nullptr;
    _ownAsyncClient = false;

    _streamClient = nullptr;
    _ownStreamClient = false;

    _isInitialized = false;
    _streamInitialized = false;
    _hasStreamData = false;
    _hasError = false;
    _streamDataType = 0;
}

FirebaseV3RTDB::~FirebaseV3RTDB() {
    if (_rtdb != nullptr) {
        delete _rtdb;
        _rtdb = nullptr;
    }

    if (_ownStreamClient && _streamClient != nullptr) {
        delete _streamClient;
        _streamClient = nullptr;
    }

    if (_ownAsyncClient && _asyncClient != nullptr) {
        delete _asyncClient;
        _asyncClient = nullptr;
    }

    if (_ownApp && _app != nullptr) {
        delete _app;
        _app = nullptr;
    }
}

AsyncClientClass &FirebaseV3RTDB::getClient() {
    if (_usingSharedApp) {
        return _firebaseApp->getAsyncClient();
    } else {
        return *_asyncClient;
    }
}

FirebaseApp &FirebaseV3RTDB::getApp() {
    if (_usingSharedApp) {
        return _firebaseApp->getApp();
    } else {
        return *_app;
    }
}

bool FirebaseV3RTDB::begin(Client &client, const String &databaseUrl, const String &apiKey, const String &email, const String &password) {
    if (_usingSharedApp) {
        return begin(databaseUrl);
    }

    _clientPtr = &client;
    _databaseUrl = databaseUrl;
    UserAuth user_auth(apiKey, email, password);

    if (_asyncClient != nullptr) {
        _asyncClient->setNetwork(client, getNetwork(_net));
    } else {
        _asyncClient = new AsyncClientClass();
        _ownAsyncClient = true;
        _asyncClient->setNetwork(client, getNetwork(_net));
    }

    _rtdb->url(databaseUrl);
    initializeApp(getClient(), getApp(), getAuth(user_auth), _result);
    getApp().getApp<RealtimeDatabase>(*_rtdb);
    _isInitialized = getApp().isInitialized();

    if (!_isInitialized) {
        _lastError = "Failed to initialize Firebase";
        _hasError = true;
    }
    return _isInitialized;
}

bool FirebaseV3RTDB::begin(const String &databaseUrl) {
    if (!_usingSharedApp) {
        return false;
    }

    _databaseUrl = databaseUrl;
    _rtdb->url(databaseUrl);
    _firebaseApp->getApp().getApp<RealtimeDatabase>(*_rtdb);
    _isInitialized = true;

    return true;
}

bool FirebaseV3RTDB::beginAnonymous(Client &client, const String &databaseUrl) {
    if (_usingSharedApp) {
        return begin(databaseUrl);
    }

    _clientPtr = &client;
    _databaseUrl = databaseUrl;
    NoAuth no_auth;

    if (_asyncClient != nullptr) {
        _asyncClient->setNetwork(client, getNetwork(_net));
    } else {
        _asyncClient = new AsyncClientClass();
        _ownAsyncClient = true;
        _asyncClient->setNetwork(client, getNetwork(_net));
    }

    _rtdb->url(databaseUrl);
    initializeApp(getClient(), getApp(), getAuth(no_auth), _result);
    getApp().getApp<RealtimeDatabase>(*_rtdb);
    _isInitialized = getApp().isInitialized();

    if (!_isInitialized) {
        _lastError = "Failed to initialize Firebase";
        _hasError = true;
    }
    return _isInitialized;
}

bool FirebaseV3RTDB::beginStream(Client &stream_client_param, const String &path) {
    if (!_isInitialized) {
        _lastError = "Firebase not initialized. Call begin() first";
        _hasError = true;
        return false;
    }

    _streamClientPtr = &stream_client_param;

    if (_streamClient == nullptr) {
        _streamClient = new AsyncClientClass();
        _ownStreamClient = true;
    }

    _streamClient->setNetwork(stream_client_param, getNetwork(_net));
    _rtdb->get(*_streamClient, path, _streamResult, true);
    _streamInitialized = true;
    return true;
}

bool FirebaseV3RTDB::set(const String &path, const String &value) {
    if (!_isInitialized) return false;

    bool success = _rtdb->set<String>(getClient(), path, value);
    checkError();
    return success;
}

bool FirebaseV3RTDB::set(const String &path, int value) {
    if (!_isInitialized) return false;

    bool success = _rtdb->set<int>(getClient(), path, value);
    checkError();
    return success;
}

bool FirebaseV3RTDB::set(const String &path, bool value) {
    if (!_isInitialized) return false;

    bool success = _rtdb->set<bool>(getClient(), path, value);
    checkError();
    return success;
}

bool FirebaseV3RTDB::set(const String &path, float value, int dp) {
    if (!_isInitialized) return false;

    bool success = _rtdb->set<number_t>(getClient(), path, number_t(value, dp));
    checkError();
    return success;
}

bool FirebaseV3RTDB::set(const String &path, const char *jsonString) {
    if (!_isInitialized) return false;

    bool success = _rtdb->set<object_t>(getClient(), path, object_t(jsonString));
    checkError();
    return success;
}

String FirebaseV3RTDB::getString(const String &path) {
    if (!_isInitialized) return "";

    String result = _rtdb->get<String>(getClient(), path);
    checkError();
    return result;
}

int FirebaseV3RTDB::getInt(const String &path) {
    if (!_isInitialized) return 0;

    int result = _rtdb->get<int>(getClient(), path);
    checkError();
    return result;
}

bool FirebaseV3RTDB::getBool(const String &path) {
    if (!_isInitialized) return false;

    bool result = _rtdb->get<bool>(getClient(), path);
    checkError();
    return result;
}

float FirebaseV3RTDB::getFloat(const String &path) {
    if (!_isInitialized) return 0.0;

    float result = _rtdb->get<float>(getClient(), path);
    checkError();
    return result;
}

String FirebaseV3RTDB::getJSON(const String &path) {
    if (!_isInitialized) return "{}";

    String result = _rtdb->get<String>(getClient(), path);
    checkError();
    return result;
}

String FirebaseV3RTDB::push(const String &path, const String &value) {
    if (!_isInitialized) return "";

    String result = _rtdb->push<String>(getClient(), path, value);
    checkError();
    return result;
}

String FirebaseV3RTDB::push(const String &path, const char *jsonString) {
    if (!_isInitialized) return "";

    String result = _rtdb->push<object_t>(getClient(), path, object_t(jsonString));
    checkError();
    return result;
}

bool FirebaseV3RTDB::update(const String &path, const char *jsonString) {
    if (!_isInitialized) return false;

    bool success = _rtdb->update<object_t>(getClient(), path, object_t(jsonString));
    checkError();
    return success;
}

bool FirebaseV3RTDB::remove(const String &path) {
    if (!_isInitialized) return false;

    bool success = _rtdb->remove(getClient(), path);
    checkError();
    return success;
}

bool FirebaseV3RTDB::exists(const String &path) {
    if (!_isInitialized) return false;

    bool exists = _rtdb->existed(getClient(), path);
    checkError();
    return exists;
}

bool FirebaseV3RTDB::ready() {
    if (_isInitialized) {
        if (_usingSharedApp) {
            return _firebaseApp->isReady();
        } else {
            getApp().loop();
            return getApp().ready();
        }
    }
    return false;
}

bool FirebaseV3RTDB::available() {
    clearStreamData();
    bool res = processResult();
    if (_streamInitialized) {
        res |= processStreamResult();
    }
    return res;
}

bool FirebaseV3RTDB::hasStreamData() {
    return _hasStreamData;
}

String FirebaseV3RTDB::getStreamData() {
    String data = _streamData;
    clearStreamData();
    return data;
}

String FirebaseV3RTDB::getStreamPath() {
    return _streamPath;
}

String FirebaseV3RTDB::getStreamEvent() {
    return _streamEvent;
}

int FirebaseV3RTDB::getStreamDataType() {
    return _streamDataType;
}

bool FirebaseV3RTDB::hasError() {
    return _hasError;
}

String FirebaseV3RTDB::getError() {
    String error = _lastError;
    _hasError = false;
    _lastError = "";
    return error;
}

void FirebaseV3RTDB::loop() {
    if (_isInitialized && !_usingSharedApp) {
        getApp().loop();
    }
}

void FirebaseV3RTDB::processStreamData() {
    processStreamResult();
}

bool FirebaseV3RTDB::processResult() {
    bool has_data = false;

    if (_result.isEvent()) {
        _lastError = _result.appEvent().message();
        _hasError = true;
        has_data = true;
    }

    if (_result.isDebug()) {
        has_data = true;
    }

    if (_result.isError()) {
        _lastError = _result.error().message();
        _hasError = true;
        has_data = true;
    }

    if (_result.available()) {
        has_data = true;
    }

    return has_data;
}

bool FirebaseV3RTDB::processStreamResult() {
    bool has_data = false;

    if (_streamResult.isEvent()) {
        has_data = true;
    }

    if (_streamResult.isDebug()) {
        has_data = true;
    }

    if (_streamResult.isError()) {
        _lastError = _streamResult.error().message();
        _hasError = true;
        has_data = true;
    }

    if (_streamResult.available()) {
        RealtimeDatabaseResult &rtdbResult = _streamResult.to<RealtimeDatabaseResult>();
        if (rtdbResult.isStream()) {
            _streamData = rtdbResult.to<const char *>();
            _streamPath = rtdbResult.dataPath();
            _streamEvent = rtdbResult.event();
            _streamDataType = rtdbResult.type();
            _hasStreamData = true;
            has_data = true;
        }
    }

    return has_data;
}

void FirebaseV3RTDB::checkError() {
    if (_usingSharedApp) {
        AsyncClientClass &client = _firebaseApp->getAsyncClient();
        if (client.lastError().code() != 0) {
            _lastError = client.lastError().message();
            _hasError = true;
        }
    } else if (_result.isError()) {
        _lastError = _result.error().message();
        _hasError = true;
    }
}

void FirebaseV3RTDB::clearStreamData() {
    _hasStreamData = false;
    _streamData = "";
    _streamPath = "";
    _streamEvent = "";
}

#endif
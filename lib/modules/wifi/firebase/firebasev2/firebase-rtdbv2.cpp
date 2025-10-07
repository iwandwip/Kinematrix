#include "firebase-rtdbv2.h"

#if defined(ESP32) || defined(ESP8266)

FirebaseV2RTDB::FirebaseV2RTDB() {
    _is_initialized = false;
    _stream_initialized = false;
    _has_stream_data = false;
    _has_error = false;
    _stream_data_type = 0;
}

bool FirebaseV2RTDB::begin(Client &client, const String &databaseUrl, const String &apiKey, const String &email, const String &password) {
    _client = &client;
    UserAuth user_auth(apiKey, email, password);
    auth_client.setNetwork(client, getNetwork(net));
    rtdb.url(databaseUrl);
    initializeApp(auth_client, app, getAuth(user_auth), result);
    app.getApp<RealtimeDatabase>(rtdb);
    _is_initialized = app.isInitialized();
    if (!_is_initialized) {
        _last_error = "Failed to initialize Firebase";
        _has_error = true;
    }
    return _is_initialized;
}

bool FirebaseV2RTDB::beginAnonymous(Client &client, const String &databaseUrl) {
    _client = &client;
    NoAuth no_auth;
    auth_client.setNetwork(client, getNetwork(net));
    rtdb.url(databaseUrl);
    initializeApp(auth_client, app, getAuth(no_auth), result);
    app.getApp<RealtimeDatabase>(rtdb);
    _is_initialized = app.isInitialized();
    if (!_is_initialized) {
        _last_error = "Failed to initialize Firebase";
        _has_error = true;
    }
    return _is_initialized;
}

bool FirebaseV2RTDB::beginStream(Client &stream_client_param, const String &path) {
    if (!_is_initialized) {
        _last_error = "Firebase not initialized. Call begin() first";
        _has_error = true;
        return false;
    }
    _stream_client_ptr = &stream_client_param;
    this->stream_client.setNetwork(stream_client_param, getNetwork(net));
    rtdb.get(this->stream_client, path, stream_result, true);
    _stream_initialized = true;
    return true;
}

bool FirebaseV2RTDB::set(const String &path, const String &value) {
    if (!_is_initialized) return false;
    bool success = rtdb.set<String>(auth_client, path, value);
    checkError();
    return success;
}

bool FirebaseV2RTDB::set(const String &path, int value) {
    if (!_is_initialized) return false;
    bool success = rtdb.set<int>(auth_client, path, value);
    checkError();
    return success;
}

bool FirebaseV2RTDB::set(const String &path, bool value) {
    if (!_is_initialized) return false;
    bool success = rtdb.set<bool>(auth_client, path, value);
    checkError();
    return success;
}

bool FirebaseV2RTDB::set(const String &path, float value, int dp) {
    if (!_is_initialized) return false;
    bool success = rtdb.set<number_t>(auth_client, path, number_t(value, dp));
    checkError();
    return success;
}

bool FirebaseV2RTDB::set(const String &path, const char *jsonString) {
    if (!_is_initialized) return false;
    bool success = rtdb.set<object_t>(auth_client, path, object_t(jsonString));
    checkError();
    return success;
}

String FirebaseV2RTDB::getString(const String &path) {
    if (!_is_initialized) return "";
    String result = rtdb.get<String>(auth_client, path);
    checkError();
    return result;
}

int FirebaseV2RTDB::getInt(const String &path) {
    if (!_is_initialized) return 0;
    int result = rtdb.get<int>(auth_client, path);
    checkError();
    return result;
}

bool FirebaseV2RTDB::getBool(const String &path) {
    if (!_is_initialized) return false;
    bool result = rtdb.get<bool>(auth_client, path);
    checkError();
    return result;
}

float FirebaseV2RTDB::getFloat(const String &path) {
    if (!_is_initialized) return 0.0;
    float result = rtdb.get<float>(auth_client, path);
    checkError();
    return result;
}

String FirebaseV2RTDB::getJSON(const String &path) {
    if (!_is_initialized) return "{}";
    String result = rtdb.get<String>(auth_client, path);
    checkError();
    return result;
}

String FirebaseV2RTDB::push(const String &path, const String &value) {
    if (!_is_initialized) return "";
    String result = rtdb.push<String>(auth_client, path, value);
    checkError();
    return result;
}

String FirebaseV2RTDB::push(const String &path, const char *jsonString) {
    if (!_is_initialized) return "";
    String result = rtdb.push<object_t>(auth_client, path, object_t(jsonString));
    checkError();
    return result;
}

bool FirebaseV2RTDB::update(const String &path, const char *jsonString) {
    if (!_is_initialized) return false;
    bool success = rtdb.update<object_t>(auth_client, path, object_t(jsonString));
    checkError();
    return success;
}

bool FirebaseV2RTDB::remove(const String &path) {
    if (!_is_initialized) return false;
    bool success = rtdb.remove(auth_client, path);
    checkError();
    return success;
}

bool FirebaseV2RTDB::exists(const String &path) {
    if (!_is_initialized) return false;
    bool exists = rtdb.existed(auth_client, path);
    checkError();
    return exists;
}

bool FirebaseV2RTDB::ready() {
    if (_is_initialized) {
        app.loop();
        return app.ready();
    }
    return false;
}

bool FirebaseV2RTDB::available() {
    clearStreamData();
    bool res = processResult();
    if (_stream_initialized) {
        res |= processStreamResult();
    }
    return res;
}

bool FirebaseV2RTDB::hasStreamData() {
    return _has_stream_data;
}

String FirebaseV2RTDB::getStreamData() {
    String data = _stream_data;
    clearStreamData();
    return data;
}

String FirebaseV2RTDB::getStreamPath() {
    return _stream_path;
}

String FirebaseV2RTDB::getStreamEvent() {
    return _stream_event;
}

int FirebaseV2RTDB::getStreamDataType() {
    return _stream_data_type;
}

bool FirebaseV2RTDB::hasError() {
    return _has_error;
}

String FirebaseV2RTDB::getError() {
    String error = _last_error;
    _has_error = false;
    _last_error = "";
    return error;
}

void FirebaseV2RTDB::loop() {
    if (_is_initialized) {
        app.loop();
    }
}

void FirebaseV2RTDB::processStreamData() {
    processStreamResult();
}

bool FirebaseV2RTDB::processResult() {
    bool has_data = false;

    if (result.isEvent()) {
        _last_error = result.appEvent().message();
        _has_error = true;
        has_data = true;
    }

    if (result.isDebug()) {
        has_data = true;
    }

    if (result.isError()) {
        _last_error = result.error().message();
        _has_error = true;
        has_data = true;
    }

    if (result.available()) {
        has_data = true;
    }

    return has_data;
}

bool FirebaseV2RTDB::processStreamResult() {
    bool has_data = false;

    if (stream_result.isEvent()) {
        has_data = true;
    }

    if (stream_result.isDebug()) {
        has_data = true;
    }

    if (stream_result.isError()) {
        _last_error = stream_result.error().message();
        _has_error = true;
        has_data = true;
    }

    if (stream_result.available()) {
        RealtimeDatabaseResult &rtdbResult = stream_result.to<RealtimeDatabaseResult>();
        if (rtdbResult.isStream()) {
            _stream_data = rtdbResult.to<const char *>();
            _stream_path = rtdbResult.dataPath();
            _stream_event = rtdbResult.event();
            _stream_data_type = rtdbResult.type();
            _has_stream_data = true;
            has_data = true;
        }
    }

    return has_data;
}

void FirebaseV2RTDB::checkError() {
    if (result.isError()) {
        _last_error = result.error().message();
        _has_error = true;
    }
}

void FirebaseV2RTDB::clearStreamData() {
    _has_stream_data = false;
    _stream_data = "";
    _stream_path = "";
    _stream_event = "";
}

#endif
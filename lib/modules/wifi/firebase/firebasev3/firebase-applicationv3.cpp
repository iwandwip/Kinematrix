#include "firebase-applicationv3.h"

FirebaseV3Application *FirebaseV3Application::_instance = nullptr;

FirebaseV3Application::FirebaseV3Application()
        : _asyncClient(_sslClient), _initialized(false), _projectId("") {

#if defined(ESP32)
    _sslClient.setInsecure();
#elif defined(ESP8266)
    _sslClient.setInsecure();
    _sslClient.setBufferSizes(4096, 1024);
#endif
}

FirebaseV3Application *FirebaseV3Application::getInstance() {
    if (_instance == nullptr) {
        _instance = new FirebaseV3Application();
    }
    return _instance;
}

bool FirebaseV3Application::begin(const char *apiKey, const char *userEmail, const char *userPassword, const char *projectId, void (*callback)(AsyncResult &)) {
    UserAuth user_auth(apiKey, userEmail, userPassword, 3000);
    _projectId = projectId;

    if (callback == nullptr) {
        callback = [](AsyncResult &result) {
            if (result.isError()) {
                Serial.println("Auth Error: " + result.error().message());
            }
        };
    }

    initializeApp(_asyncClient, _app, getAuth(user_auth), callback, "authTask");
    _initialized = true;

    return true;
}

bool FirebaseV3Application::begin(const char *clientEmail, const char *projectId, const char *privateKey, void (*callback)(AsyncResult &)) {
    ServiceAuth service_auth(clientEmail, projectId, privateKey, 3000);
    _projectId = projectId;

    if (callback == nullptr) {
        callback = [](AsyncResult &result) {
            if (result.isError()) {
                Serial.println("Auth Error: " + result.error().message());
            }
        };
    }

    initializeApp(_asyncClient, _app, getAuth(service_auth), callback, "authTask");
    _initialized = true;

    return true;
}

bool FirebaseV3Application::beginAnonymous(const char *projectId, void (*callback)(AsyncResult &)) {
    NoAuth no_auth;
    _projectId = projectId;

    if (callback == nullptr) {
        callback = [](AsyncResult &result) {
            if (result.isError()) {
                Serial.println("Auth Error: " + result.error().message());
            }
        };
    }

    initializeApp(_asyncClient, _app, getAuth(no_auth), callback, "authTask");
    _initialized = true;

    return true;
}

void FirebaseV3Application::setTime(time_t timestamp) {
    _app.setTime(timestamp);
}

void FirebaseV3Application::loop() {
    if (_initialized) {
        _app.loop();
    }
}

bool FirebaseV3Application::isReady() {
    return _initialized && _app.ready();
}

String FirebaseV3Application::getProjectId() {
    return _projectId;
}

FirebaseApp &FirebaseV3Application::getApp() {
    return _app;
}

AsyncClientClass &FirebaseV3Application::getAsyncClient() {
    return _asyncClient;
}

SSL_CLIENT &FirebaseV3Application::getSSLClient() {
    return _sslClient;
}
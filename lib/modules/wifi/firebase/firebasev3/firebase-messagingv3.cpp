#include "firebase-messagingv3.h"

#if defined(ESP32) || defined(ESP8266)

#include <time.h>

FirebaseV3Messaging::FirebaseV3Messaging()
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

    _messaging = new Messaging();

    _projectId = "";
    _useUserAuth = false;
    _apiKey = nullptr;
    _userEmail = nullptr;
    _userPassword = nullptr;
    _clientEmail = nullptr;
    _privateKey = nullptr;
    _credsSet = false;
    _authTimeout = 3000;
    _resultReady = false;
}

FirebaseV3Messaging::FirebaseV3Messaging(FirebaseV3Application *firebaseApp)
        : _firebaseApp(firebaseApp), _usingSharedApp(true) {
    _sslClient = nullptr;
    _ownSSLClient = false;
    _asyncClient = nullptr;
    _ownAsyncClient = false;
    _app = nullptr;
    _ownApp = false;

    _messaging = new Messaging();

    _projectId = "";
    _useUserAuth = false;
    _apiKey = nullptr;
    _userEmail = nullptr;
    _userPassword = nullptr;
    _clientEmail = nullptr;
    _privateKey = nullptr;
    _credsSet = false;
    _authTimeout = 3000;
    _resultReady = false;
}

FirebaseV3Messaging::~FirebaseV3Messaging() {
    if (_userAuth != nullptr) {
        delete _userAuth;
        _userAuth = nullptr;
    }

    if (_serviceAuth != nullptr) {
        delete _serviceAuth;
        _serviceAuth = nullptr;
    }

    if (_messaging != nullptr) {
        delete _messaging;
        _messaging = nullptr;
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

AsyncClientClass &FirebaseV3Messaging::getClient() {
    if (_usingSharedApp) {
        return _firebaseApp->getAsyncClient();
    } else {
        return *_asyncClient;
    }
}

FirebaseApp &FirebaseV3Messaging::getApp() {
    if (_usingSharedApp) {
        return _firebaseApp->getApp();
    } else {
        return *_app;
    }
}

bool FirebaseV3Messaging::begin(const char *apiKey, const char *userEmail, const char *userPassword, const char *projectId, int timeout) {
    if (_usingSharedApp) {
        return begin(projectId);
    }

    _useUserAuth = true;
    _apiKey = apiKey;
    _userEmail = userEmail;
    _userPassword = userPassword;
    _projectId = projectId;
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

bool FirebaseV3Messaging::begin(const char *clientEmail, const char *projectId, const char *privateKey, int timeout) {
    if (_usingSharedApp) {
        return begin(projectId);
    }

    _useUserAuth = false;
    _clientEmail = clientEmail;
    _projectId = projectId;
    _privateKey = privateKey;
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

bool FirebaseV3Messaging::begin(const char *projectId) {
    if (!_usingSharedApp) {
        return false;
    }

    _projectId = projectId;

    _firebaseApp->getApp().getApp<Messaging>(*_messaging);
    clearMessage();
    return true;
}

time_t FirebaseV3Messaging::getValidTimeFromNTP() {
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
        Serial.print("UNIX Timestamp: ");
        Serial.println(now);
        return now;
    } else {
        Serial.println("Failed to get NTP time!");
        return 0;
    }
}

bool FirebaseV3Messaging::begin() {
    if (_usingSharedApp) {
        return begin(_firebaseApp->getProjectId().c_str());
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
    Serial.println("Time successfully set for JWT: " + String(validTime));

    if (_useUserAuth) {
        if (_userAuth != nullptr) {
            initializeApp(getClient(), getApp(), getAuth(*_userAuth), [](AsyncResult &result) {
                if (result.isError()) {
                    Serial.println("Auth Error: " + result.error().message());
                } else if (result.isDebug()) {
                    Serial.println("Auth Debug: " + result.debug());
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
                } else if (result.isDebug()) {
                    Serial.println("Auth Debug: " + result.debug());
                }
            }, "authTask");
        } else {
            return false;
        }
    }

    getApp().getApp<Messaging>(*_messaging);
    clearMessage();
    return true;
}

void FirebaseV3Messaging::loop() {
    if (_usingSharedApp) {
        processResult(_messagingResult);
    } else {
        getApp().loop();
        processResult(_messagingResult);
    }
}

bool FirebaseV3Messaging::isReady() {
    if (_usingSharedApp) {
        return _firebaseApp->isReady();
    } else {
        return getApp().ready();
    }
}

String FirebaseV3Messaging::getLastError() {
    return getClient().lastError().message();
}

bool FirebaseV3Messaging::isResultReady() const {
    return _resultReady;
}

String FirebaseV3Messaging::getLastPayload() const {
    return _resultPayload;
}

void FirebaseV3Messaging::processResult(AsyncResult &aResult) {
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

void FirebaseV3Messaging::resultCallback(AsyncResult &aResult, void *userData) {
    FirebaseV3Messaging *wrapper = static_cast<FirebaseV3Messaging *>(userData);
    if (wrapper != nullptr) {
        wrapper->processResult(aResult);
    }
}

void FirebaseV3Messaging::setTopic(const String &topic) {
    _topic = topic;
}

void FirebaseV3Messaging::setToken(const String &token) {
    _token = token;
}

void FirebaseV3Messaging::setCondition(const String &condition) {
    _condition = condition;
}

void FirebaseV3Messaging::setNotification(const String &title, const String &body) {
    _title = title;
    _body = body;
    _hasNotification = true;
}

void FirebaseV3Messaging::setData(const String &jsonData) {
    _dataJson = jsonData;
}

void FirebaseV3Messaging::addData(const String &key, const String &value) {
    if (_dataJson.length() == 0 || _dataJson == "{}") {
        _dataJson = "{\"" + key + "\":\"" + value + "\"}";
    } else {
        String temp = _dataJson;
        if (temp.endsWith("}")) {
            temp.remove(temp.length() - 1);
            temp += ",\"" + key + "\":\"" + value + "\"}";
            _dataJson = temp;
        }
    }
}

void FirebaseV3Messaging::setAndroidPriority(bool highPriority) {
    _highPriority = highPriority;
    _hasAndroidConfig = true;
}

void FirebaseV3Messaging::setAndroidNotificationPriority(int priority) {
    _androidNotificationPriority = priority;
    _hasAndroidNotificationPriority = true;
}

String FirebaseV3Messaging::sendMessage(bool waitForCompletion) {
    if (!isReady()) {
        return "Error: Firebase not ready";
    }

    _resultReady = false;

    Messages::Message msg;

    if (_token.length() > 0) {
        msg.token(_token.c_str());
    }

    if (_hasNotification) {
        Messages::Notification notification;
        notification.title(_title.c_str());
        notification.body(_body.c_str());
        msg.notification(notification);
    }

    object_t data(("{\"device\":\"ESP32\",\"timestamp\":\"" + String(millis()) + "\"}").c_str());
    msg.data(data);

    if (_hasAndroidConfig) {
        Messages::AndroidConfig androidConfig;
        if (_highPriority) {
            androidConfig.priority(Messages::AndroidMessagePriority::_HIGH);
        } else {
            androidConfig.priority(Messages::AndroidMessagePriority::NORMAL);
        }
        msg.android(androidConfig);
    }

    Messages::Parent parent(_projectId.c_str());
    Serial.println("Sending message to project ID: " + String(_projectId));

    if (waitForCompletion) {
        return _messaging->send(getClient(), parent, msg);
    } else {
        _messaging->send(getClient(), parent, msg, _messagingResult);
        return "";
    }
}

void FirebaseV3Messaging::clearMessage() {
    _token = "";
    _topic = "";
    _condition = "";
    _title = "";
    _body = "";
    _dataJson = "{}";
    _highPriority = false;
    _androidNotificationPriority = 0;

    _hasNotification = false;
    _hasAndroidConfig = false;
    _hasAndroidNotificationPriority = false;
}

#endif
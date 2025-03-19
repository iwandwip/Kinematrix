/*
 *  firebase-messagingv2.cpp
 *
 *  Simplified Firebase messaging handler implementation
 */

#include "firebase-messagingv2.h"

#if defined(ESP32) || defined(ESP8266)

#include <time.h>

FirebaseV2Messaging::FirebaseV2Messaging()
        : _aClient(_sslClient), _projectId(""), _useUserAuth(false),
          _apiKey(nullptr), _userEmail(nullptr), _userPassword(nullptr),
          _clientEmail(nullptr), _privateKey(nullptr), _credsSet(false),
          _authTimeout(3000) {

    set_ssl_client_insecure_and_buffer(_sslClient);
}

FirebaseV2Messaging::~FirebaseV2Messaging() {
    if (_userAuth != nullptr) {
        delete _userAuth;
        _userAuth = nullptr;
    }

    if (_serviceAuth != nullptr) {
        delete _serviceAuth;
        _serviceAuth = nullptr;
    }
}

bool FirebaseV2Messaging::begin(const char *apiKey, const char *userEmail, const char *userPassword, const char *projectId, int timeout) {
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

bool FirebaseV2Messaging::begin(const char *clientEmail, const char *projectId, const char *privateKey, int timeout) {
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

time_t FirebaseV2Messaging::getValidTimeFromNTP() {
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

bool FirebaseV2Messaging::begin() {
    if (!_credsSet) {
        return false;
    }

    time_t validTime = getValidTimeFromNTP();
    if (validTime == 0) {
        Serial.println("FATAL: Could not get a valid time!");
        return false;
    }

    _app.setTime(validTime);
    Serial.println("Time successfully set for JWT: " + String(validTime));

    if (_useUserAuth) {
        if (_userAuth != nullptr) {
            initializeApp(_aClient, _app, getAuth(*_userAuth), [](AsyncResult &result) {
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
            initializeApp(_aClient, _app, getAuth(*_serviceAuth), [](AsyncResult &result) {
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

    _app.getApp<Messaging>(_messaging);
    clearMessage();
    return true;
}

void FirebaseV2Messaging::loop() {
    _app.loop();
    processResult(_messagingResult);
}

bool FirebaseV2Messaging::isReady() {
    return _app.ready();
}

String FirebaseV2Messaging::getLastError() {
    return _aClient.lastError().message();
}

bool FirebaseV2Messaging::isResultReady() const {
    return _resultReady;
}

String FirebaseV2Messaging::getLastPayload() const {
    return _resultPayload;
}

void FirebaseV2Messaging::processResult(AsyncResult &aResult) {
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

void FirebaseV2Messaging::resultCallback(AsyncResult &aResult, void *userData) {
    FirebaseV2Messaging *wrapper = static_cast<FirebaseV2Messaging *>(userData);
    if (wrapper != nullptr) {
        wrapper->processResult(aResult);
    }
}

void FirebaseV2Messaging::setTopic(const String &topic) {
    _topic = topic;
}

void FirebaseV2Messaging::setToken(const String &token) {
    _token = token;
}

void FirebaseV2Messaging::setCondition(const String &condition) {
    _condition = condition;
}

void FirebaseV2Messaging::setNotification(const String &title, const String &body) {
    _title = title;
    _body = body;
    _hasNotification = true;
}

void FirebaseV2Messaging::setData(const String &jsonData) {
    _dataJson = jsonData;
}

void FirebaseV2Messaging::addData(const String &key, const String &value) {
    if (_dataJson.length() == 0 || _dataJson == "{}") {
        _dataJson = "{\"" + key + "\":\"" + value + "\"}";
    } else {
        // Parse and modify
        String temp = _dataJson;
        if (temp.endsWith("}")) {
            temp.remove(temp.length() - 1);
            temp += ",\"" + key + "\":\"" + value + "\"}";
            _dataJson = temp;
        }
    }
}

void FirebaseV2Messaging::setAndroidPriority(bool highPriority) {
    _highPriority = highPriority;
    _hasAndroidConfig = true;
}

void FirebaseV2Messaging::setAndroidNotificationPriority(int priority) {
    _androidNotificationPriority = priority;
    _hasAndroidNotificationPriority = true;
}

String FirebaseV2Messaging::sendMessage(bool waitForCompletion) {
    if (!isReady()) {
        return "Error: Firebase not ready";
    }

    // Reset result status
    _resultReady = false;

    // Create a new message exactly like in the working example
    Messages::Message msg;

    // Set token
    if (_token.length() > 0) {
        msg.token(_token.c_str());
    }

    // Set notification
    if (_hasNotification) {
        Messages::Notification notification;
        notification.title(_title.c_str());
        notification.body(_body.c_str());
        msg.notification(notification);
    }

    // Set data EXACTLY like in the working example,
    // This is the critical part that matches the original implementation
    object_t data(("{\"device\":\"ESP32\",\"timestamp\":\"" + String(millis()) + "\"}").c_str());
    msg.data(data);

    // Set Android config
    if (_hasAndroidConfig) {
        Messages::AndroidConfig androidConfig;
        if (_highPriority) {
            androidConfig.priority(Messages::AndroidMessagePriority::_HIGH);
        } else {
            androidConfig.priority(Messages::AndroidMessagePriority::NORMAL);
        }
        msg.android(androidConfig);
    }

    // Create parent
    Messages::Parent parent(_projectId.c_str());
    Serial.println("Sending message to project ID: " + String(_projectId));

    if (waitForCompletion) {
        return _messaging.send(_aClient, parent, msg);
    } else {
        _messaging.send(_aClient, parent, msg, _messagingResult);
        return "";
    }
}

void FirebaseV2Messaging::clearMessage() {
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
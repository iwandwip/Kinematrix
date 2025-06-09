#include "WiFiModule.h"

#if defined(ESP32) || defined(ESP8266)

WiFiModule::WiFiModule() {
    _lastReconnectAttempt = 0;
    _reconnectInterval = 30000;
    _connectionTimeout = 10000;
    _autoReconnect = true;
    _status = WIFI_MODULE_IDLE;
    _useStaticIP = false;
    _eventCallback = NULL;
}

bool WiFiModule::begin() {
    WiFi.mode(WIFI_STA);
    return true;
}

bool WiFiModule::connect(const char *ssid, const char *password) {
    return connectWithTimeout(ssid, password, _connectionTimeout);
}

bool WiFiModule::connectWithTimeout(const char *ssid, const char *password, unsigned long timeout) {
    _ssid = ssid;
    _password = password;
    _connectionTimeout = timeout;
    _updateStatus(WIFI_MODULE_CONNECTING);

    if (_useStaticIP) {
        WiFi.config(_staticIP, _gateway, _subnet, _dns1, _dns2);
    }

    WiFi.begin(ssid, password);

    unsigned long startTime = millis();
    while (WiFi.status() != WL_CONNECTED) {
        if (millis() - startTime > timeout) {
            _updateStatus(WIFI_MODULE_CONNECTION_FAILED);
            return false;
        }
        delay(100);
    }

    _updateStatus(WIFI_MODULE_CONNECTED);
    return true;
}

bool WiFiModule::checkConnection() {
    if (WiFi.status() == WL_CONNECTED) {
        if (_status != WIFI_MODULE_CONNECTED) {
            _updateStatus(WIFI_MODULE_CONNECTED);
        }
        return true;
    } else {
        if (_status == WIFI_MODULE_CONNECTED) {
            _updateStatus(WIFI_MODULE_CONNECTION_LOST);
        }

        if (_autoReconnect && millis() - _lastReconnectAttempt > _reconnectInterval) {
            _lastReconnectAttempt = millis();
            return reconnect();
        }

        return false;
    }
}

bool WiFiModule::reconnect() {
    _updateStatus(WIFI_MODULE_CONNECTING);
    return _tryConnect();
}

void WiFiModule::disconnect() {
    WiFi.disconnect();
    _updateStatus(WIFI_MODULE_DISCONNECTED);
}

void WiFiModule::setAutoReconnect(bool enable) {
    _autoReconnect = enable;
    WiFi.setAutoReconnect(enable);
}

bool WiFiModule::getAutoReconnect() {
    return _autoReconnect;
}

void WiFiModule::setReconnectInterval(unsigned long interval) {
    _reconnectInterval = interval;
}

void WiFiModule::setStaticIP(IPAddress ip, IPAddress gateway, IPAddress subnet, IPAddress dns1, IPAddress dns2) {
    _staticIP = ip;
    _gateway = gateway;
    _subnet = subnet;
    _dns1 = dns1;
    _dns2 = dns2;
}

void WiFiModule::useStaticIP(bool enable) {
    _useStaticIP = enable;
}

wifi_module_status_t WiFiModule::getStatus() {
    return _status;
}

const char *WiFiModule::getStatusString() {
    switch (_status) {
        case WIFI_MODULE_IDLE:
            return "IDLE";
        case WIFI_MODULE_CONNECTING:
            return "CONNECTING";
        case WIFI_MODULE_CONNECTED:
            return "CONNECTED";
        case WIFI_MODULE_DISCONNECTED:
            return "DISCONNECTED";
        case WIFI_MODULE_CONNECTION_LOST:
            return "CONNECTION_LOST";
        case WIFI_MODULE_CONNECTION_FAILED:
            return "CONNECTION_FAILED";
        default:
            return "UNKNOWN";
    }
}

IPAddress WiFiModule::getIP() {
    return WiFi.localIP();
}

String WiFiModule::getSSID() {
    return WiFi.SSID();
}

int32_t WiFiModule::getRSSI() {
    return WiFi.RSSI();
}

void WiFiModule::onEvent(WifiEventCallback callback) {
    _eventCallback = callback;
}

bool WiFiModule::_tryConnect() {
    if (_useStaticIP) {
        WiFi.config(_staticIP, _gateway, _subnet, _dns1, _dns2);
    }

    WiFi.begin(_ssid.c_str(), _password.c_str());

    unsigned long startTime = millis();
    while (WiFi.status() != WL_CONNECTED) {
        if (millis() - startTime > _connectionTimeout) {
            _updateStatus(WIFI_MODULE_CONNECTION_FAILED);
            return false;
        }
        delay(100);
    }

    _updateStatus(WIFI_MODULE_CONNECTED);
    return true;
}

void WiFiModule::_updateStatus(wifi_module_status_t status) {
    _status = status;
    if (_eventCallback != NULL) {
        _eventCallback(status);
    }
}

#endif
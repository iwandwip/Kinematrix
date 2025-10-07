/*
 *  wifi-handlerv2.cpp - Simplified version
 *  Wi-Fi handler implementation
 */

#include "wifi-handlerv2.h"

#if defined(ESP32) || defined(ESP8266)

#include <esp_wifi.h>

// Initialize static member
WiFiHandlerV2 *WiFiHandlerV2::_instance = NULL;

WiFiHandlerV2::WiFiHandlerV2() {
    _connected = false;
    _apActive = false;
    _watchdogEnabled = false;
    _watchdogInterval = 30000;
    _lastWatchdogCheck = 0;
    _useStaticIP = false;

    _onConnectCallback = NULL;
    _onDisconnectCallback = NULL;

    // Save instance for static event handling
    _instance = this;

    _initWiFiEvents();
}

void WiFiHandlerV2::_initWiFiEvents() {
    // Register WiFi event handlers
    WiFi.onEvent(_wifiEventHandler);
}

void WiFiHandlerV2::_wifiEventHandler(WiFiEvent_t event) {
    if (!_instance) return;

    switch (event) {
        case SYSTEM_EVENT_STA_CONNECTED:
            _instance->_connected = true;
            if (_instance->_onConnectCallback) _instance->_onConnectCallback();
            break;
        case SYSTEM_EVENT_STA_DISCONNECTED:
            _instance->_connected = false;
            if (_instance->_onDisconnectCallback) _instance->_onDisconnectCallback();
            break;
    }
}

// ===== CONNECTION METHODS =====

bool WiFiHandlerV2::connect(const char *ssid, const char *password, unsigned long timeout) {
    _ssid = String(ssid);
    _password = password ? String(password) : "";

    WiFi.mode(WIFI_STA);

    // Apply static IP if configured
    if (_useStaticIP) {
        WiFi.config(_staticIP, _staticGateway, _staticSubnet, _staticDNS1);
    }

    WiFi.begin(ssid, password);

    unsigned long startTime = millis();
    while (WiFi.status() != WL_CONNECTED) {
        if (millis() - startTime > timeout) {
            return false;
        }
        delay(100);
    }

    _connected = true;
    return true;
}

bool WiFiHandlerV2::reconnect(unsigned long timeout) {
    if (_ssid.length() == 0) {
        return false;
    }

    return connect(_ssid.c_str(), _password.c_str(), timeout);
}

bool WiFiHandlerV2::disconnect() {
    bool result = WiFi.disconnect();
    _connected = false;
    return result;
}

// ===== AP MODE METHODS =====

bool WiFiHandlerV2::startAP(const char *ssid, const char *password, int channel, bool hidden) {
    WiFi.mode(WIFI_AP);

    bool result;
    if (password && strlen(password) >= 8) {
        result = WiFi.softAP(ssid, password, channel, hidden);
    } else {
        result = WiFi.softAP(ssid, NULL, channel, hidden);
    }

    if (result) {
        _apActive = true;
    }

    return result;
}

bool WiFiHandlerV2::startDualMode(const char *staSSID, const char *staPassword, const char *apSSID, const char *apPassword) {
    WiFi.mode(WIFI_AP_STA);

    bool apResult;
    if (apPassword && strlen(apPassword) >= 8) {
        apResult = WiFi.softAP(apSSID, apPassword);
    } else {
        apResult = WiFi.softAP(apSSID);
    }

    if (apResult) {
        _apActive = true;
    }

    WiFi.begin(staSSID, staPassword);

    // Wait for connection (max 30 seconds)
    unsigned long startTime = millis();
    while (WiFi.status() != WL_CONNECTED) {
        if (millis() - startTime > 30000) {
            // Return true if AP started successfully, even if STA connection failed
            return _apActive;
        }
        delay(100);
    }

    _connected = true;
    _ssid = String(staSSID);
    _password = staPassword ? String(staPassword) : "";

    return _connected && _apActive;
}

bool WiFiHandlerV2::stopAP() {
    bool result = WiFi.softAPdisconnect(true);
    if (result) {
        _apActive = false;
        // Set mode to STA if we're still connected as station
        if (_connected) {
            WiFi.mode(WIFI_STA);
        } else {
            WiFi.mode(WIFI_OFF);
        }
    }
    return result;
}

// ===== STATUS METHODS =====

bool WiFiHandlerV2::isConnected() {
    return WiFi.status() == WL_CONNECTED;
}

bool WiFiHandlerV2::isAPActive() {
    return _apActive;
}

int WiFiHandlerV2::getSignalStrength() {
    return WiFi.RSSI();
}

int WiFiHandlerV2::getSignalQuality() {
    int rssi = WiFi.RSSI();
    // Convert RSSI to percentage quality
    if (rssi >= -50) return 100;
    else if (rssi <= -100) return 0;
    else return 2 * (rssi + 100);
}

// ===== NETWORK INFO METHODS =====

IPAddress WiFiHandlerV2::getIP() {
    return WiFi.localIP();
}

IPAddress WiFiHandlerV2::getAPIP() {
    return WiFi.softAPIP();
}

String WiFiHandlerV2::getMacAddress() {
    return WiFi.macAddress();
}

String WiFiHandlerV2::getSSID() {
    return WiFi.SSID();
}

// ===== NETWORK MANAGEMENT METHODS =====

bool WiFiHandlerV2::setStaticIP(IPAddress ip, IPAddress gateway, IPAddress subnet, IPAddress dns1) {
    _staticIP = ip;
    _staticGateway = gateway;
    _staticSubnet = subnet;
    _staticDNS1 = dns1;
    _useStaticIP = true;

    if (_connected) {
        // Apply immediately if already connected
        return WiFi.config(ip, gateway, subnet, dns1);
    }

    return true;
}

bool WiFiHandlerV2::setDHCP() {
    _useStaticIP = false;

    if (_connected) {
        // Need to reconnect to apply DHCP
        String ssid = _ssid;
        String password = _password;
        disconnect();
        delay(100);
        return connect(ssid.c_str(), password.c_str());
    }

    return true;
}

bool WiFiHandlerV2::setHostname(const char *hostname) {
    return WiFi.setHostname(hostname);
}

// ===== SCANNING METHODS =====

int WiFiHandlerV2::scanNetworks() {
    return WiFi.scanNetworks();
}

String WiFiHandlerV2::getScannedSSID(int index) {
    return WiFi.SSID(index);
}

int WiFiHandlerV2::getScannedRSSI(int index) {
    return WiFi.RSSI(index);
}

String WiFiHandlerV2::getScannedEncryption(int index) {
    switch (WiFi.encryptionType(index)) {
        case WIFI_AUTH_OPEN:
            return "Open";
        case WIFI_AUTH_WEP:
            return "WEP";
        case WIFI_AUTH_WPA_PSK:
            return "WPA";
        case WIFI_AUTH_WPA2_PSK:
            return "WPA2";
        case WIFI_AUTH_WPA_WPA2_PSK:
            return "WPA/WPA2";
        default:
            return "Unknown";
    }
}

// ===== POWER MANAGEMENT METHODS =====

void WiFiHandlerV2::enablePowerSave() {
    WiFi.setSleep(true);
}

void WiFiHandlerV2::disablePowerSave() {
    WiFi.setSleep(false);
}

// ===== WATCHDOG METHODS =====

void WiFiHandlerV2::enableWatchdog(unsigned long interval) {
    _watchdogEnabled = true;
    _watchdogInterval = interval;
    _lastWatchdogCheck = millis();
}

void WiFiHandlerV2::disableWatchdog() {
    _watchdogEnabled = false;
}

// ===== EVENT HANDLING METHODS =====

void WiFiHandlerV2::setOnConnectCallback(WifiEventCallback callback) {
    _onConnectCallback = callback;
}

void WiFiHandlerV2::setOnDisconnectCallback(WifiEventCallback callback) {
    _onDisconnectCallback = callback;
}

// ===== MAINTENANCE METHODS =====

void WiFiHandlerV2::loop() {
    // Handle watchdog if enabled
    if (_watchdogEnabled && _connected) {
        if (millis() - _lastWatchdogCheck > _watchdogInterval) {
            _checkConnection();
            _lastWatchdogCheck = millis();
        }
    }
}

void WiFiHandlerV2::_checkConnection() {
    if (WiFi.status() != WL_CONNECTED) {
        // Try to reconnect
        WiFi.reconnect();

        // Wait briefly to see if reconnection is successful
        unsigned long startTime = millis();
        while (WiFi.status() != WL_CONNECTED) {
            if (millis() - startTime > 5000) {
                // If reconnection failed, try a full reconnecting
                reconnect();
                break;
            }
            delay(100);
        }
    }
}

void WiFiHandlerV2::reset() {
    WiFi.disconnect(true);
    delay(100);
    WiFi.mode(WIFI_OFF);
    _connected = false;
    _apActive = false;
}

#endif
#include "WiFiManager.h"

WiFiManager::WiFiManager() : server(80) {
    state = WM_IDLE;
    configMode = false;
    useStaticIP = false;
    autoReconnect = true;

    connectionTimeout = 30000;
    configTimeout = 300000;
    retryDelay = 5000;
    reconnectInterval = 30000;
    minSignalQuality = -80;

    apSSID = "ESP32-WiFiManager";
    apPassword = "";
    hostname = "ESP32-WiFiManager";
    mdnsName = "wifimanager";
    mdnsEnabled = true;
    mdnsStarted = false;

    onConnectedCallback = nullptr;
    onDisconnectedCallback = nullptr;
    onConfigModeCallback = nullptr;

    _lastscan = 0;
    _startscan = 0;
    _scancachetime = 30000;
    _asyncScan = true;
    _scanStarted = false;
    _scanComplete = false;
    _scanStartTime = 0;
    _scanTimeout = 10000;
    _numNetworks = 0;
    _preloadwifiscan = false;

#ifdef ESP32
    wm_event_id = WiFi.onEvent([this](WiFiEvent_t event, WiFiEventInfo_t info) {
        this->WiFiEvent(event, info);
    });
#endif
}

bool WiFiManager::begin() {
    if (!LittleFS.begin()) {
        Serial.println("LittleFS Mount Failed");
        return false;
    }

    loadSettings();
    WiFi.setHostname(hostname.c_str());

    return true;
}

bool WiFiManager::autoConnect() {
    if (loadConfig()) {
        state = WM_CONNECTING;
        stateTimer = millis();

        if (useStaticIP) {
            IPAddress ip, gw, sn;
            ip.fromString(staticIP);
            gw.fromString(gateway);
            sn.fromString(subnet);
            WiFi.config(ip, gw, sn);
        }

        WiFi.begin(ssid.c_str(), password.c_str());
        return true;
    } else {
        startConfigMode();
        return false;
    }
}

void WiFiManager::loop() {
    handleState();
}

void WiFiManager::handleState() {
    unsigned long now = millis();

    switch (state) {
        case WM_IDLE:
            break;

        case WM_CONNECTING:
            if (WiFi.status() == WL_CONNECTED) {
                state = WM_CONNECTED;
                Serial.println("WiFi Connected!");
                Serial.print("IP: ");
                Serial.println(WiFi.localIP());

                stopConfigMode();
                setupServer();
                server.begin();

                if (mdnsEnabled && !mdnsStarted) {
                    startMDNS();
                }

                if (onConnectedCallback) onConnectedCallback();
            } else if (now - stateTimer > connectionTimeout) {
                state = WM_RETRY;
                stateTimer = now;
                WiFi.disconnect();
                Serial.println("Connection timeout, retrying...");
            }
            break;

        case WM_CONNECTED:
            if (WiFi.status() != WL_CONNECTED && autoReconnect) {
                Serial.println("Connection lost, reconnecting...");
                state = WM_CONNECTING;
                stateTimer = now;
                WiFi.reconnect();

                if (onDisconnectedCallback) onDisconnectedCallback();
            }
            break;

        case WM_RETRY:
            if (now - stateTimer > retryDelay) {
                startConfigMode();
            }
            break;

        case WM_CONFIG_MODE:
            if (configTimeout > 0 && now - stateTimer > configTimeout) {
                restart();
            }
            break;
    }

    if (_scanStarted && !_scanComplete) {
        if (now - _scanStartTime > _scanTimeout) {
            Serial.println("Scan timeout in handleState, resetting scan");
            _scanStarted = false;
            _scanComplete = false;
        } else {
            scanComplete();
        }
    }
}

void WiFiManager::startConfigMode() {
    if (configMode) return;

    state = WM_CONFIG_MODE;
    configMode = true;
    stateTimer = millis();

    WiFi.disconnect();
    WiFi.mode(WIFI_AP);

    if (apPassword.length() > 0) {
        WiFi.softAP(apSSID.c_str(), apPassword.c_str());
    } else {
        WiFi.softAP(apSSID.c_str());
    }

    setupServer();
    server.begin();

    if (mdnsEnabled && !mdnsStarted) {
        startMDNS();
    }

    Serial.println("Config mode started");
    Serial.print("AP: ");
    Serial.println(apSSID);
    Serial.print("IP: ");
    Serial.println(WiFi.softAPIP());

    if (onConfigModeCallback) onConfigModeCallback();
}

void WiFiManager::stopConfigMode() {
    if (!configMode) return;

    configMode = false;
    WiFi.softAPdisconnect(true);
    WiFi.mode(WIFI_STA);
}

void WiFiManager::setupServer() {
    server.on("/", HTTP_GET, [this](AsyncWebServerRequest *request) {
        handleRoot(request);
    });

    server.on("/scan", HTTP_GET, [this](AsyncWebServerRequest *request) {
        handleScan(request);
    });

    server.on("/connect", HTTP_POST, [this](AsyncWebServerRequest *request) {
        handleConnect(request);
    });

    server.on("/status", HTTP_GET, [this](AsyncWebServerRequest *request) {
        handleStatus(request);
    });

    server.on("/reset", HTTP_POST, [this](AsyncWebServerRequest *request) {
        handleReset(request);
    });

    server.on("/resetall", HTTP_POST, [this](AsyncWebServerRequest *request) {
        handleResetAll(request);
    });

    server.serveStatic("/", LittleFS, "/");

    server.onNotFound([](AsyncWebServerRequest *request) {
        request->redirect("/");
    });

}

int WiFiManager::getSignalQuality(int rssi) {
    if (rssi <= -100) return 0;
    if (rssi >= -50) return 100;
    return 2 * (rssi + 100);
}

int WiFiManager::getSignalQuality() {
    return getSignalQuality(WiFi.RSSI());
}

void WiFiManager::handleRoot(AsyncWebServerRequest *request) {
    if (LittleFS.exists("/index.html")) {
        request->send(LittleFS, "/index.html", "text/html");
    } else {
        String html = "<!DOCTYPE html><html><head><title>WiFi Manager</title></head><body>";
        html += "<h1>WiFi Configuration</h1>";
        html += "<p>Please upload web files to LittleFS root directory</p>";
        html += "</body></html>";
        request->send(200, "text/html", html);
    }
}

void WiFiManager::handleScan(AsyncWebServerRequest *request) {
    JsonDocument doc;

    bool forceRefresh = request->hasParam("refresh");
    bool scanResult = WiFi_scanNetworks(forceRefresh, true);

    if (!scanResult && !getScanCompleteCached()) {
        doc["scanning"] = true;
        doc["message"] = "Scan in progress...";
        String response;
        serializeJson(doc, response);
        request->send(200, "application/json", response);
        return;
    }

    JsonArray networksArray = doc["networks"].to<JsonArray>();
    doc["scanning"] = false;

    if (!_numNetworks) {
        WiFi_scanNetworks();
    }

    int networks = _numNetworks;
    if (networks == 0) {
        doc["message"] = "No networks found";
    } else {
        for (int i = 0; i < networks && i < 20; i++) {
            if (WiFi.RSSI(i) >= minSignalQuality) {
                JsonObject network = networksArray.add<JsonObject>();
                network["ssid"] = WiFi.SSID(i);
                network["rssi"] = WiFi.RSSI(i);
                network["quality"] = getSignalQuality(WiFi.RSSI(i));
                network["encryption"] = (WiFi.encryptionType(i) == WIFI_AUTH_OPEN) ? "open" : "secured";
            }
        }
    }

    String response;
    serializeJson(doc, response);
    request->send(200, "application/json", response);
}

void WiFiManager::handleConnect(AsyncWebServerRequest *request) {
    if (request->hasParam("s", true) && request->hasParam("p", true)) {
        ssid = request->getParam("s", true)->value();
        password = request->getParam("p", true)->value();

        // Validate SSID
        if (ssid.length() == 0) {
            JsonDocument doc;
            doc["success"] = false;
            doc["message"] = "Network name (SSID) is required";
            doc["validation_error"] = true;

            String response;
            serializeJson(doc, response);
            request->send(400, "application/json", response);
            return;
        }

        // Validate password (minimum 8 characters if provided)
        if (password.length() > 0 && password.length() < 8) {
            JsonDocument doc;
            doc["success"] = false;
            doc["message"] = "Password must be at least 8 characters long";
            doc["validation_error"] = true;

            String response;
            serializeJson(doc, response);
            request->send(400, "application/json", response);
            return;
        }

        if (request->hasParam("ip", true)) {
            staticIP = request->getParam("ip", true)->value();
            gateway = request->getParam("gateway", true)->value();
            subnet = request->getParam("subnet", true)->value();
            useStaticIP = (staticIP.length() > 0);
        }

        saveConfig();

        JsonDocument doc;
        doc["success"] = true;
        doc["message"] = "Configuration saved successfully!";
        doc["ssid"] = ssid;
        doc["connecting"] = true;
        doc["restart_delay"] = 3000;

        String response;
        serializeJson(doc, response);
        request->send(200, "application/json", response);

        delay(3000);
        restart();
    } else {
        JsonDocument doc;
        doc["success"] = false;
        doc["message"] = "Missing SSID or password";

        String response;
        serializeJson(doc, response);
        request->send(400, "application/json", response);
    }
}

void WiFiManager::handleStatus(AsyncWebServerRequest *request) {
    JsonDocument doc;
    doc["connected"] = isConnected();
    doc["ssid"] = getSSID();
    doc["ip"] = WiFi.localIP().toString();
    doc["rssi"] = getRSSI();
    doc["quality"] = getSignalQuality();
    doc["state"] = (int) state;
    doc["configMode"] = configMode;
    doc["mdnsEnabled"] = mdnsEnabled;
    doc["mdnsStarted"] = mdnsStarted;
    doc["mdnsName"] = mdnsName;
    if (mdnsStarted) {
        doc["mdnsUrl"] = "http://" + mdnsName + ".local";
    }

    String response;
    serializeJson(doc, response);
    request->send(200, "application/json", response);
}

void WiFiManager::handleReset(AsyncWebServerRequest *request) {
    reset();

    JsonDocument doc;
    doc["success"] = true;
    doc["message"] = "WiFi configuration reset. Please restart device manually.";

    String response;
    serializeJson(doc, response);
    request->send(200, "application/json", response);
}

void WiFiManager::handleResetAll(AsyncWebServerRequest *request) {
    JsonDocument doc;
    doc["success"] = true;
    doc["message"] = "Complete system reset. Device restarting...";

    String response;
    serializeJson(doc, response);
    request->send(200, "application/json", response);

    delay(1000);
    resetAll();
}

bool WiFiManager::loadConfig() {
    if (!LittleFS.exists("/config.json")) return false;

    File file = LittleFS.open("/config.json", "r");
    if (!file) return false;

    JsonDocument doc;
    DeserializationError error = deserializeJson(doc, file);
    file.close();

    if (error) return false;

    ssid = doc["ssid"].as<String>();
    password = doc["password"].as<String>();
    staticIP = doc["ip"].as<String>();
    gateway = doc["gateway"].as<String>();
    subnet = doc["subnet"].as<String>();
    useStaticIP = (staticIP.length() > 0);

    return (ssid.length() > 0);
}

void WiFiManager::saveConfig() {
    JsonDocument doc;
    doc["ssid"] = ssid;
    doc["password"] = password;
    doc["ip"] = staticIP;
    doc["gateway"] = gateway;
    doc["subnet"] = subnet;

    File file = LittleFS.open("/config.json", "w");
    if (file) {
        serializeJson(doc, file);
        file.close();
    }
}

void WiFiManager::loadSettings() {
    if (!LittleFS.exists("/settings.json")) return;

    File file = LittleFS.open("/settings.json", "r");
    if (!file) return;

    JsonDocument doc;
    DeserializationError error = deserializeJson(doc, file);
    file.close();

    if (error) return;

    connectionTimeout = doc["connectionTimeout"] | 30000;
    configTimeout = doc["configTimeout"] | 300000;
    retryDelay = doc["retryDelay"] | 5000;
    autoReconnect = doc["autoReconnect"] | true;
    minSignalQuality = doc["minSignalQuality"] | -80;
    hostname = doc["hostname"] | "ESP32-WiFiManager";
    mdnsName = doc["mdnsName"] | "wifimanager";
    mdnsEnabled = doc["mdnsEnabled"] | true;
}

void WiFiManager::saveSettings() {
    JsonDocument doc;
    doc["connectionTimeout"] = connectionTimeout;
    doc["configTimeout"] = configTimeout;
    doc["retryDelay"] = retryDelay;
    doc["autoReconnect"] = autoReconnect;
    doc["minSignalQuality"] = minSignalQuality;
    doc["hostname"] = hostname;
    doc["mdnsName"] = mdnsName;
    doc["mdnsEnabled"] = mdnsEnabled;

    File file = LittleFS.open("/settings.json", "w");
    if (file) {
        serializeJson(doc, file);
        file.close();
    }
}

bool WiFiManager::isConnected() {
    return WiFi.status() == WL_CONNECTED;
}

bool WiFiManager::isConfigMode() {
    return configMode;
}

String WiFiManager::getSSID() {
    return WiFi.SSID();
}

String WiFiManager::getPassword() {
    return password;
}

IPAddress WiFiManager::getIP() {
    return WiFi.localIP();
}

int WiFiManager::getRSSI() {
    return WiFi.RSSI();
}

WiFiManagerState WiFiManager::getState() {
    return state;
}

void WiFiManager::setTimeout(unsigned long timeout) {
    connectionTimeout = timeout;
    saveSettings();
}

void WiFiManager::setConfigTimeout(unsigned long timeout) {
    configTimeout = timeout;
    saveSettings();
}

void WiFiManager::setRetryDelay(unsigned long delay) {
    retryDelay = delay;
    saveSettings();
}

void WiFiManager::setReconnectInterval(unsigned long interval) {
    reconnectInterval = interval;
    saveSettings();
}

void WiFiManager::setAPCredentials(const char *apSsid, const char *apPass) {
    apSSID = apSsid;
    if (apPass) apPassword = apPass;
    else apPassword = "";
}

void WiFiManager::setHostname(const char *name) {
    hostname = name;
    saveSettings();
}

void WiFiManager::setMinSignalQuality(int quality) {
    minSignalQuality = quality;
    saveSettings();
}

void WiFiManager::enableAutoReconnect(bool enable) {
    autoReconnect = enable;
    saveSettings();
}

void WiFiManager::setMDNSName(const char *name) {
    mdnsName = name;
    saveSettings();
}

void WiFiManager::enableMDNS(bool enable) {
    mdnsEnabled = enable;
    if (!enable && mdnsStarted) {
        stopMDNS();
    }
    saveSettings();
}

String WiFiManager::getMDNSName() {
    return mdnsName;
}

bool WiFiManager::isMDNSEnabled() {
    return mdnsEnabled;
}

void WiFiManager::startConfig() {
    startConfigMode();
}

void WiFiManager::stopConfig() {
    stopConfigMode();
}

void WiFiManager::disconnect() {
    WiFi.disconnect();
    state = WM_IDLE;
}

void WiFiManager::reset() {
    Serial.println("Resetting WiFi configuration...");

    WiFi_enableSTA(true, true);
    delay(500);

#ifdef ESP32
    WiFi.disconnect(true, true);
#elif defined(ESP8266)
    WiFi.persistent(true);
    WiFi.disconnect(true);
    WiFi.persistent(false);
#endif

    LittleFS.remove("/config.json");
    LittleFS.remove("/settings.json");

    delay(1000);
    Serial.println("WiFi configuration reset complete");
}

void WiFiManager::resetAll() {
    Serial.println("Performing complete system reset...");

    if (mdnsStarted) {
        stopMDNS();
    }

    WiFi_enableSTA(true, true);
    delay(500);

#ifdef ESP32
    WiFi.disconnect(true, true);
#elif defined(ESP8266)
    WiFi.persistent(true);
    WiFi.disconnect(true);
    WiFi.persistent(false);
#endif

    LittleFS.remove("/config.json");
    LittleFS.remove("/settings.json");

    delay(1000);
    Serial.println("Complete system reset done. Restarting...");
    ESP.restart();
}

void WiFiManager::restart() {
    if (mdnsStarted) {
        stopMDNS();
    }
    ESP.restart();
}

bool WiFiManager::startMDNS() {
    if (mdnsStarted || !mdnsEnabled) {
        return mdnsStarted;
    }

#ifdef ESP32
    if (MDNS.begin(mdnsName.c_str())) {
        MDNS.addService("http", "tcp", 80);
        mdnsStarted = true;
        Serial.println("mDNS responder started");
        Serial.print("Access via: http://");
        Serial.print(mdnsName);
        Serial.println(".local");
        return true;
    }
#elif defined(ESP8266)
    if (MDNS.begin(mdnsName)) {
        MDNS.addService("http", "tcp", 80);
        mdnsStarted = true;
        Serial.println("mDNS responder started");
        Serial.print("Access via: http://");
        Serial.print(mdnsName);
        Serial.println(".local");
        return true;
    }
#endif

    Serial.println("Error setting up mDNS responder!");
    return false;
}

void WiFiManager::stopMDNS() {
    if (!mdnsStarted) {
        return;
    }

#ifdef ESP32
    MDNS.end();
#elif defined(ESP8266)
    MDNS.end();
#endif

    mdnsStarted = false;
    Serial.println("mDNS responder stopped");
}

bool WiFiManager::isMDNSStarted() {
    return mdnsStarted;
}

unsigned long WiFiManager::getConnectionTimeoutMs() {
    return connectionTimeout;
}

unsigned long WiFiManager::getConnectionTimeoutS() {
    return connectionTimeout / 1000;
}

unsigned long WiFiManager::getConfigTimeoutMs() {
    return configTimeout;
}

unsigned long WiFiManager::getConfigTimeoutS() {
    return configTimeout / 1000;
}

unsigned long WiFiManager::getRetryDelayMs() {
    return retryDelay;
}

unsigned long WiFiManager::getRetryDelayS() {
    return retryDelay / 1000;
}

unsigned long WiFiManager::getReconnectIntervalMs() {
    return reconnectInterval;
}

unsigned long WiFiManager::getReconnectIntervalS() {
    return reconnectInterval / 1000;
}

unsigned long WiFiManager::getStateTimerMs() {
    return millis() - stateTimer;
}

unsigned long WiFiManager::getStateTimerS() {
    return (millis() - stateTimer) / 1000;
}

unsigned long WiFiManager::getRemainingTimeoutMs() {
    unsigned long now = millis();
    unsigned long elapsed = now - stateTimer;

    switch (state) {
        case WM_CONNECTING:
            return (elapsed < connectionTimeout) ? (connectionTimeout - elapsed) : 0;
        case WM_RETRY:
            return (elapsed < retryDelay) ? (retryDelay - elapsed) : 0;
        case WM_CONFIG_MODE:
            return (configTimeout > 0 && elapsed < configTimeout) ? (configTimeout - elapsed) : 0;
        default:
            return 0;
    }
}

unsigned long WiFiManager::getRemainingTimeoutS() {
    return getRemainingTimeoutMs() / 1000;
}

int WiFiManager::getMinSignalQuality() {
    return minSignalQuality;
}

bool WiFiManager::isAutoReconnectEnabled() {
    return autoReconnect;
}

void WiFiManager::onConnected(WiFiManagerCallback callback) {
    onConnectedCallback = callback;
}

void WiFiManager::onDisconnected(WiFiManagerCallback callback) {
    onDisconnectedCallback = callback;
}

void WiFiManager::onConfigMode(WiFiManagerCallback callback) {
    onConfigModeCallback = callback;
}

bool WiFiManager::WiFi_scanNetworks() {
    return WiFi_scanNetworks(false, false);
}

bool WiFiManager::WiFi_scanNetworks(unsigned int cachetime, bool async) {
    return WiFi_scanNetworks(millis() - _lastscan > cachetime, async);
}

bool WiFiManager::WiFi_scanNetworks(unsigned int cachetime) {
    return WiFi_scanNetworks(millis() - _lastscan > cachetime, false);
}

bool WiFiManager::WiFi_scanNetworks(bool force, bool async) {
    if (_numNetworks == 0) {
        Serial.println("NO APs found forcing new scan");
        force = true;
    }

    if (!_lastscan || (_lastscan > 0 && (millis() - _lastscan > _scancachetime))) {
        force = true;
    }

    if (force) {
        int8_t res;
        _startscan = millis();
        if (async && _asyncScan) {
#ifdef ESP32
            Serial.println("WiFi Scan ASYNC started");
            res = WiFi.scanNetworks(true);
#endif
            return false;
        } else {
            Serial.println("WiFi Scan SYNC started");
            res = WiFi.scanNetworks();
        }

        if (res == WIFI_SCAN_FAILED) {
            Serial.println("[ERROR] scan failed");
        } else if (res == WIFI_SCAN_RUNNING) {
            Serial.println("[ERROR] scan waiting");
            while (WiFi.scanComplete() == WIFI_SCAN_RUNNING) {
                Serial.print(".");
                delay(100);
            }
            _numNetworks = WiFi.scanComplete();
        } else if (res >= 0) {
            _numNetworks = res;
        }

        _lastscan = millis();
        Serial.println("WiFi Scan completed in " + String(_lastscan - _startscan) + " ms");
        return true;
    } else {
        Serial.println("Scan is cached " + String(millis() - _lastscan) + " ms ago");
    }
    return false;
}

void WiFiManager::WiFi_scanComplete(int networksFound) {
    _lastscan = millis();
    _numNetworks = networksFound;
    Serial.println("WiFi Scan ASYNC completed in " + String(_lastscan - _startscan) + " ms");
    Serial.println("WiFi Scan ASYNC found: " + String(_numNetworks));
}

#ifdef ESP32

void WiFiManager::WiFiEvent(WiFiEvent_t event, WiFiEventInfo_t info) {
    if (event == ARDUINO_EVENT_WIFI_SCAN_DONE && _asyncScan) {
        uint16_t scans = WiFi.scanComplete();
        WiFi_scanComplete(scans);
    }
}

#endif

bool WiFiManager::startScan() {
    return WiFi_scanNetworks(_scancachetime, true);
}

bool WiFiManager::scanComplete() {
    return _lastscan > 0 && (millis() - _lastscan <= _scancachetime);
}

void WiFiManager::startScanWiFi() {
    WiFi_scanNetworks(true, _asyncScan);
}

bool WiFiManager::getScanCompleteCached() {
    return _lastscan > 0 && (millis() - _lastscan <= _scancachetime);
}

int WiFiManager::getScanCacheTime() {
    return _scancachetime / 1000;
}

void WiFiManager::setScanCacheTime(int seconds) {
    _scancachetime = seconds * 1000;
}

bool WiFiManager::WiFi_enableSTA(bool enable) {
    return WiFi_enableSTA(enable, false);
}

bool WiFiManager::WiFi_enableSTA(bool enable, bool persistent) {
#ifdef ESP32
    bool ret;
    if (persistent) WiFi.persistent(true);
    ret = WiFi.enableSTA(enable);
    if (persistent) WiFi.persistent(false);
    return ret;
#elif defined(ESP8266)
    WiFiMode_t newMode;
    WiFiMode_t currentMode = WiFi.getMode();
    bool isEnabled = (currentMode & WIFI_STA) != 0;
    if(enable) newMode = (WiFiMode_t)(currentMode | WIFI_STA);
    else newMode = (WiFiMode_t)(currentMode & (~WIFI_STA));

    if((isEnabled != enable) || persistent) {
        if(persistent) WiFi.persistent(true);
        bool ret = WiFi.mode(newMode);
        if(persistent) WiFi.persistent(false);
        return ret;
    }
    return true;
#endif
}
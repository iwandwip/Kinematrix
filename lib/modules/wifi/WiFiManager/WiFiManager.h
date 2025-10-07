#ifndef WIFI_MANAGER_H
#define WIFI_MANAGER_H

#if defined(ESP32) || defined(ESP8266)

#include <Arduino.h>
#include <ArduinoJson.h>

#ifdef ESP32
#include <WiFi.h>
#include <ESPAsyncWebServer.h>
#include <LittleFS.h>
#include <ESPmDNS.h>
#define WIFI_getChipId() (uint32_t)ESP.getEfuseMac()
#define WM_JSON_BUFFER_SIZE 2048
#define WM_MAX_NETWORKS 20
#elif defined(ESP8266)
#include <ESP8266WiFi.h>
#include <ESPAsyncWebServer.h>
#include <LittleFS.h>
#include <ESP8266mDNS.h>
#define WIFI_getChipId() ESP.getChipId()
#define WM_JSON_BUFFER_SIZE 1024
#define WM_MAX_NETWORKS 10

#if !defined(ARDUINO_ESP8266_RELEASE_2_7_0) && \
        !defined(ARDUINO_ESP8266_RELEASE_3_0_0) && \
        !defined(ARDUINO_ESP8266_RELEASE_3_1_0)
#warning "ESP8266 requires Arduino Core 2.7.0+ for LittleFS support"
#endif
#endif

typedef void (*WiFiManagerCallback)(void);

enum WiFiManagerState {
    WM_IDLE,
    WM_CONNECTING,
    WM_CONNECTED,
    WM_CONFIG_MODE,
    WM_RETRY
};

class WiFiManager {
private:
    AsyncWebServer server;
    WiFiManagerState state;

    String ssid;
    String password;
    String staticIP;
    String gateway;
    String subnet;

    String apSSID;
    String apPassword;
    String hostname;
    String mdnsName;

    bool mdnsEnabled;
    bool mdnsStarted;

    unsigned long connectionTimeout;
    unsigned long configTimeout;
    unsigned long retryDelay;
    unsigned long reconnectInterval;
    unsigned long stateTimer;

    bool autoReconnect;
    bool configMode;
    bool useStaticIP;
    int minSignalQuality;

    WiFiManagerCallback onConnectedCallback;
    WiFiManagerCallback onDisconnectedCallback;
    WiFiManagerCallback onConfigModeCallback;

    unsigned long _lastscan;
    unsigned long _startscan;
    unsigned int _scancachetime;
    bool _asyncScan;
    bool _scanStarted;
    bool _scanComplete;
    unsigned long _scanStartTime;
    unsigned long _scanTimeout;
    int _numNetworks;
    bool _preloadwifiscan;

#ifdef ESP32
    wifi_event_id_t wm_event_id;
#endif

    bool loadConfig();
    void saveConfig();
    void loadSettings();
    void saveSettings();
    void setupServer();
    void handleState();
    void startConfigMode();
    void stopConfigMode();
    int getSignalQuality();
    int getSignalQuality(int rssi);

    bool startScan();
    bool scanComplete();
    void startScanWiFi();
    bool getScanCompleteCached();
    int getScanCacheTime();
    void setScanCacheTime(int seconds);

    bool WiFi_scanNetworks();
    bool WiFi_scanNetworks(bool force, bool async);
    bool WiFi_scanNetworks(unsigned int cachetime, bool async);
    bool WiFi_scanNetworks(unsigned int cachetime);
    void WiFi_scanComplete(int networksFound);

    bool WiFi_enableSTA(bool enable);
    bool WiFi_enableSTA(bool enable, bool persistent);

#ifdef ESP32
    void WiFiEvent(WiFiEvent_t event, WiFiEventInfo_t info);
#endif

    void handleRoot(AsyncWebServerRequest *request);
    void handleScan(AsyncWebServerRequest *request);
    void handleConnect(AsyncWebServerRequest *request);
    void handleStatus(AsyncWebServerRequest *request);
    void handleReset(AsyncWebServerRequest *request);
    void handleResetAll(AsyncWebServerRequest *request);

    bool startMDNS();
    void stopMDNS();
    bool isMDNSStarted();

public:
    WiFiManager();

    bool begin();
    bool autoConnect();
    void loop();

    bool isConnected();
    bool isConfigMode();
    String getSSID();
    String getPassword();
    IPAddress getIP();
    int getRSSI();
    WiFiManagerState getState();

    void setTimeout(unsigned long timeout);
    void setConfigTimeout(unsigned long timeout);
    void setRetryDelay(unsigned long delay);
    void setReconnectInterval(unsigned long interval);
    void setAPCredentials(const char *apSsid, const char *apPass = nullptr);
    void setHostname(const char *name);
    void setMinSignalQuality(int quality);
    void enableAutoReconnect(bool enable);

    void setMDNSName(const char *name);
    void enableMDNS(bool enable);
    String getMDNSName();
    bool isMDNSEnabled();

    unsigned long getConnectionTimeoutMs();
    unsigned long getConnectionTimeoutS();
    unsigned long getConfigTimeoutMs();
    unsigned long getConfigTimeoutS();
    unsigned long getRetryDelayMs();
    unsigned long getRetryDelayS();
    unsigned long getReconnectIntervalMs();
    unsigned long getReconnectIntervalS();
    unsigned long getStateTimerMs();
    unsigned long getStateTimerS();
    unsigned long getRemainingTimeoutMs();
    unsigned long getRemainingTimeoutS();
    int getMinSignalQuality();
    bool isAutoReconnectEnabled();

    void startConfig();
    void stopConfig();
    void disconnect();
    void reset();
    void resetAll();
    void restart();

    void onConnected(WiFiManagerCallback callback);
    void onDisconnected(WiFiManagerCallback callback);
    void onConfigMode(WiFiManagerCallback callback);
};

#endif
#endif
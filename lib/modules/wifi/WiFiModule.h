#ifndef WIFI_MODULE_H
#define WIFI_MODULE_H

#if defined(ESP32) || defined(ESP8266)

#include "WiFi.h"

typedef enum {
    WIFI_MODULE_IDLE,
    WIFI_MODULE_CONNECTING,
    WIFI_MODULE_CONNECTED,
    WIFI_MODULE_DISCONNECTED,
    WIFI_MODULE_CONNECTION_LOST,
    WIFI_MODULE_CONNECTION_FAILED
} wifi_module_status_t;

typedef void (*WifiEventCallback)(wifi_module_status_t status);

class WiFiModule {
private:
    String _ssid;
    String _password;
    unsigned long _lastReconnectAttempt;
    unsigned long _reconnectInterval;
    unsigned long _connectionTimeout;
    bool _autoReconnect;
    wifi_module_status_t _status;
    IPAddress _staticIP;
    IPAddress _gateway;
    IPAddress _subnet;
    IPAddress _dns1;
    IPAddress _dns2;
    bool _useStaticIP;
    WifiEventCallback _eventCallback;

    bool _tryConnect();
    void _updateStatus(wifi_module_status_t status);

public:
    WiFiModule();

    bool begin();
    bool connect(const char *ssid, const char *password);
    bool connectWithTimeout(const char *ssid, const char *password, unsigned long timeout);
    bool checkConnection();
    bool reconnect();
    void disconnect();

    void setAutoReconnect(bool enable);
    bool getAutoReconnect();
    void setReconnectInterval(unsigned long interval);

    void setStaticIP(IPAddress ip, IPAddress gateway, IPAddress subnet, IPAddress dns1 = (uint32_t) 0, IPAddress dns2 = (uint32_t) 0);
    void useStaticIP(bool enable);

    wifi_module_status_t getStatus();
    const char *getStatusString();
    IPAddress getIP();
    String getSSID();
    int32_t getRSSI();

    void onEvent(WifiEventCallback callback);
};

#endif
#endif
/*
 *  wifi-handlerv2.h - Simplified version
 *  Wi-Fi handler module for ESP32
 */

#ifndef WIFI_HANDLER_V2_H
#define WIFI_HANDLER_V2_H

#if defined(ESP32) || defined(ESP8266)

#include <Arduino.h>
#include <WiFi.h>

// Callback function typedef using a standard C function pointer
typedef void (*WifiEventCallback)(void);

class WiFiHandlerV2 {
public:
    WiFiHandlerV2();

    // Basic connection methods
    bool connect(const char *ssid, const char *password, unsigned long timeout = 30000);
    bool reconnect(unsigned long timeout = 30000);
    bool disconnect();

    // AP Mode
    bool startAP(const char *ssid, const char *password = NULL, int channel = 1, bool hidden = false);
    bool startDualMode(const char *staSSID, const char *staPassword, const char *apSSID, const char *apPassword = NULL);
    bool stopAP();

    // Status methods
    bool isConnected();
    bool isAPActive();
    int getSignalStrength();  // RSSI
    int getSignalQuality();   // As percentage

    // Network info
    IPAddress getIP();
    IPAddress getAPIP();
    String getMacAddress();
    String getSSID();

    // Network management
    bool setStaticIP(IPAddress ip, IPAddress gateway, IPAddress subnet, IPAddress dns1 = IPAddress(0, 0, 0, 0));
    bool setDHCP();
    bool setHostname(const char *hostname);

    // WiFi Scanning
    int scanNetworks();
    String getScannedSSID(int index);
    int getScannedRSSI(int index);
    String getScannedEncryption(int index);

    // Power management
    void enablePowerSave();
    void disablePowerSave();

    // Watchdog
    void enableWatchdog(unsigned long interval = 30000);
    void disableWatchdog();

    // Event handling
    void setOnConnectCallback(WifiEventCallback callback);
    void setOnDisconnectCallback(WifiEventCallback callback);

    // Maintenance
    void loop();
    void reset();

private:
    bool _connected;
    bool _apActive;
    unsigned long _watchdogInterval;
    unsigned long _lastWatchdogCheck;
    bool _watchdogEnabled;
    String _ssid;
    String _password;
    IPAddress _staticIP;
    IPAddress _staticGateway;
    IPAddress _staticSubnet;
    IPAddress _staticDNS1;
    bool _useStaticIP;

    // Callbacks
    WifiEventCallback _onConnectCallback;
    WifiEventCallback _onDisconnectCallback;

    // Internal methods
    void _checkConnection();
    void _initWiFiEvents();
    static void _wifiEventHandler(WiFiEvent_t event);
    static WiFiHandlerV2 *_instance;
};

#endif
#endif
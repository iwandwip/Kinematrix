#pragma once

#ifndef MAC_ADDRESS_MANAGER_H
#define MAC_ADDRESS_MANAGER_H

#if defined(ESP32) || defined(ESP8266)

#include "Arduino.h"
#include "WiFi.h"
#include "esp_now.h"

typedef struct {
    String msg;
    char buffer[250];
} ESPNowMessage;

class ESPNow {
private:
    uint8_t **macAddressList;
    int macAddressLen;
    int macAddressThisDeviceIndex;
    const uint8_t MAX_MAC_COUNT = 10;

public:
    ESPNow();
    ~ESPNow();
    bool begin(esp_now_send_cb_t sendCb, esp_now_recv_cb_t recvCb);
    void setThisDeviceIndex(uint8_t index);
    int getThisDeviceIndex();
    int getThisDeviceIndexByMacAddress();
    void addMacAddress(String macStr, int master = -1);
    void debugMacAddressList();

    esp_err_t sendData(const uint8_t *peer_addr, const uint8_t *data, size_t len);
    bool broadcastData(const uint8_t *data, size_t len, void (*err_callback)() = nullptr);
    String macAddressToString(uint8_t *macArray);
    void macStringToUint8(const char *macStr, uint8_t *macArray);

    void getData(void *__restrict, const void *__restrict, size_t);

    int getMacIndex(const uint8_t *left);
    bool isThisDevice(uint8_t *left, uint8_t *right);
    bool isThisDevice(const uint8_t *left, uint8_t *right);
    bool notThisDevice(uint8_t *left, uint8_t *right);

    String parseStr(String data, char separator[], int index);

    static const int MASTER = 1;
    static void defaultSendCallback(const uint8_t *mac_addr, esp_now_send_status_t status);
    static void defaultReceiveCallback(const uint8_t *mac, const uint8_t *incomingData, int len);
};

#endif
#endif

#pragma once

#ifndef LORA_COM_H
#define LORA_COM_H

#include "Arduino.h"
#include "SPI.h"
#include "lora-base.h"

#pragma message("[COMPILED]: lora-com.h")

#define NONE ""
#define SEPARATOR ";"

typedef struct {
    uint8_t cs;
    uint8_t rst;
    uint8_t irq;
    uint8_t count;
    uint8_t addr;
    uint8_t destination;
    char outgoing[250];
} lora_config;

class LoRaModule {
private:
    int isLoRaReady;
    String dataSend;
    uint32_t sendTime;
    uint32_t receiveTime;

    bool waitingForResponse;
    unsigned long responseStartTime;
    unsigned long responseTimeout;
    int responseRetries;
    int currentResponseRetry;
    void (*responseCallback)(const String &);
    void (*timeoutCallback)();

    String parseStr(String data, char separator[], int index);

public:
    LoRaModule();
    ~LoRaModule();
    int init(long frequency = 915E6, void (*onInit)() = nullptr);
    int init(uint8_t ss, uint8_t reset, uint8_t dio0, long frequency = 915E6, void (*onInit)() = nullptr);
    bool isReady(void (*onReady)() = nullptr);
    void end();

    int packetRssi();
    float packetSnr();
    long packetFrequencyError();

    int getSpreadingFactor();
    long getSignalBandwidth();

    void sleep();
    void idle();

    template<typename T>
    void addData(T newData, const char *separator = ";") {
        dataSend += String(newData);
        dataSend += separator;
    }

    void clearData();
    void sendData();
    void sendDataCb(void (*onReceive)(const String &));
    String sendDataCbWaitData(void (*onSend)(const String &));
    String sendDataCbWaitDataWithTimeout(void (*onSend)(const String &), unsigned long timeout, int maxRetries, bool wdtTimeout = false);

    void sendDataAndExpectResponse(void (*onSend)(const String &), void (*onResponse)(const String &));
    void sendDataAndExpectResponseWithTimeout(void (*onSend)(const String &), void (*onResponse)(const String &), void (*onTimeout)() = nullptr, unsigned long timeout = 1000, int maxRetries = 3);

    void update();
    void cancelResponseWait();
    bool isWaitingForResponse();

    void sendDataAsync(uint32_t _time = 500);
    void sendDataAsyncCb(uint32_t _time = 500, void (*onReceive)(const String &) = nullptr);
    void sendBytes(int next);
    void receive(void (*onReceive)(const String &));
    void receiveAsync(uint32_t _time = 500, void (*onReceive)(const String &) = nullptr);
    void receiveString(void (*onReceive)(const String &));
    float getData(String data, uint8_t index, char separator[]);
    String getStrData(String data, uint8_t index, char separator[]);
};

#endif
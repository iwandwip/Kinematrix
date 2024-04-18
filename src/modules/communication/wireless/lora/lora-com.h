/*
 *  lora-com.h
 *
 *  lora communication handler
 *  Created on: 2023. 4. 3
 */

#pragma once

#ifndef LORA_COM_H
#define LORA_COM_H

#include "Arduino.h"
#include "SPI.h"
#include "lora-base.h"

#define NONE ""
#define SEPARATOR ";"

typedef struct {
    uint8_t cs;  // ns ss
    uint8_t rst;
    uint8_t irq;

    uint8_t count;
    uint8_t addr;
    uint8_t destination;
    char outgoing[250];
} lora_config;

class LoRaModule {
private:
    String dataSend;
    uint32_t sendTime;
    uint32_t receiveTime;
    String parseStr(String data, char separator[], int index);
public:
    LoRaModule();
    ~LoRaModule();
    int init(long frequency = 915E6);
    int init(uint8_t ss, uint8_t reset, uint8_t dio0, long frequency = 915E6);
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
    void sendDataAsync(uint32_t _time = 500);
    void sendDataAsyncCb(uint32_t _time = 500, void (*onReceive)(const String &) = nullptr);
    void sendBytes(int next);
    void receive(void (*onReceive)(const String &));
    void receiveAsync(uint32_t _time = 500, void (*onReceive)(const String &) = nullptr);
    void receiveString(void (*onReceive)(const String &));
    float getData(String data, uint8_t index, char separator[]);
    String getStrData(String data, uint8_t index, char separator[]);
};

#endif  // LORA_COM_H
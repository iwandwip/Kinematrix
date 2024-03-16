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
    String data;
    uint32_t sendTime;
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

    template<typename T>
    void addData(T value) {
        data += String(value);
        data += SEPARATOR;
    }

    void clearData();
    void sendData();
    void sendDataCb(void (*callback)() = nullptr);
    void sendDataAsync(uint32_t _time = 500);
    void sendDataAsyncCb(uint32_t _time = 500, void (*callback)() = nullptr);
    void receive(void (*onReceive)(String) = nullptr);
    void sleep();
    void idle();
    float getData(String data, uint8_t index = 0);
    String getStrData(String data, uint8_t index = 0);
};

#endif  // LORA_COM_H
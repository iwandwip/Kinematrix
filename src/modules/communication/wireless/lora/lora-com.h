/*
 *  lora-com.h
 *
 *  lora communication handler
 *  Created on: 2023. 4. 3
 */

#ifdef USE_LORA_COM
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
    int isLoRaReady;
    String dataSend;
    uint32_t sendTime;
    uint32_t receiveTime;
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
    void sendDataAsync(uint32_t _time = 500);
    void sendDataAsyncCb(uint32_t _time = 500, void (*onReceive)(const String &) = nullptr);
    void sendBytes(int next);
    void receive(void (*onReceive)(const String &));
    void receiveAsync(uint32_t _time = 500, void (*onReceive)(const String &) = nullptr);
    void receiveString(void (*onReceive)(const String &));
    float getData(String data, uint8_t index, char separator[]);
    String getStrData(String data, uint8_t index, char separator[]);
};



LoRaModule::LoRaModule() {
}

LoRaModule::~LoRaModule() {
}

int LoRaModule::init(long frequency, void (*onInit)()) {
    int success = LoRa.begin(frequency);
    isLoRaReady = success;
    if (onInit != nullptr && success) onInit();
    return success;
}

int LoRaModule::init(uint8_t ss, uint8_t reset, uint8_t dio0, long frequency, void (*onInit)()) {
    LoRa.setPins(ss, reset, dio0);
    int success = LoRa.begin(frequency);
    isLoRaReady = success;
    if (onInit != nullptr && success) onInit();
    return success;
}

bool LoRaModule::isReady(void (*onReady)()) {
    if (onReady != nullptr && isLoRaReady) onReady();
    return isLoRaReady;
}

void LoRaModule::end() {
    LoRa.end();
}

int LoRaModule::packetRssi() {
    return LoRa.packetRssi();
}

float LoRaModule::packetSnr() {
    return LoRa.packetSnr();
}

long LoRaModule::packetFrequencyError() {
    return LoRa.packetFrequencyError();
}

int LoRaModule::getSpreadingFactor() {
    return LoRa.getSpreadingFactor();
}

long LoRaModule::getSignalBandwidth() {
    return LoRa.getSignalBandwidth();
}

void LoRaModule::sleep() {
    LoRa.sleep();
}

void LoRaModule::idle() {
    LoRa.idle();
}

void LoRaModule::clearData() {
    dataSend = "";
}

void LoRaModule::sendData() {
    LoRa.beginPacket();
    LoRa.print(dataSend);
    LoRa.endPacket();
}

void LoRaModule::sendDataCb(void (*onReceive)(const String &)) {
    LoRa.beginPacket();
    LoRa.print(dataSend);
    LoRa.endPacket();
    onReceive(dataSend);
}

void LoRaModule::sendDataAsync(uint32_t _time) {
    if (millis() - sendTime >= _time) {
        sendTime = millis();
        LoRa.beginPacket();
        LoRa.print(dataSend);
        LoRa.endPacket();
    }
}

void LoRaModule::sendDataAsyncCb(uint32_t _time, void (*onReceive)(const String &)) {
    if (millis() - sendTime >= _time) {
        sendTime = millis();
        LoRa.beginPacket();
        LoRa.print(dataSend);
        LoRa.endPacket();
        onReceive(dataSend);
    }
}

void LoRaModule::sendBytes(int next) {
    LoRa.write(next);
}

void LoRaModule::receive(void (*onReceive)(const String &)) {
    if (onReceive == nullptr) return;
    int packetSize = LoRa.parsePacket();
    if (packetSize) {
        String data;
        while (LoRa.available()) {
            data += (char) LoRa.read();
        }
        onReceive(data);
    }
}

void LoRaModule::receiveAsync(uint32_t _time, void (*onReceive)(const String &)) {
    if (onReceive == nullptr) return;
    if (millis() - receiveTime >= _time) {
        receiveTime = millis();
        int packetSize = LoRa.parsePacket();
        if (packetSize) {
            String data;
            while (LoRa.available()) {
                data += (char) LoRa.read();
            }
            onReceive(data);
        }
    }
}

void LoRaModule::receiveString(void (*onReceive)(const String &)) {
    int packetSize = LoRa.parsePacket();
    if (packetSize) {
        String data = LoRa.readStringUntil('\n');
        onReceive(data);
    }
}

float LoRaModule::getData(String data, uint8_t index, char separator[]) {
    return parseStr(data, separator, index).toFloat();
}

String LoRaModule::getStrData(String data, uint8_t index, char separator[]) {
    return parseStr(data, separator, index);
}

String LoRaModule::parseStr(String data, char separator[], int index) {
    int found = 0;
    int strIndex[] = {0, -1};
    int maxIndex = data.length() - 1;
    for (int i = 0; i <= maxIndex && found <= index; i++) {
        if (data.charAt(i) == separator[0] || i == maxIndex) {
            found++;
            strIndex[0] = strIndex[1] + 1;
            strIndex[1] = (i == maxIndex) ? i + 1 : i;
        }
    }
    return found > index ? data.substring(strIndex[0], strIndex[1]) : "";
}
#endif  // LORA_COM_H
#endif
/*
 *  hard-serial.h
 *
 *  serial communication lib
 *  Created on: 2023. 4. 3
 */
#ifdef USE_HARD_SERIAL

#pragma once

#ifndef HARD_SERIAL_COM_H
#define HARD_SERIAL_COM_H

#include "Arduino.h"
#include "HardwareSerial.h"

class HardSerial {
private:
    HardwareSerial *serialPtr;
    String dataSend;
    uint32_t sendTime;
    uint32_t receiveTime;
    String parseStr(String data, char separator[], int index);

public:
    HardSerial();
    void begin(HardwareSerial *_serialPtr = &Serial, long baud = 9600);

    template<typename T>
    void addData(T newData, const char *separator = ";") {
        dataSend += String(newData);
        dataSend += separator;
    }

    template<typename T>
    void sendString(T newData) {
        serialPtr->println(newData);
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

HardSerial::HardSerial() {
    clearData();
}

void HardSerial::begin(HardwareSerial *_serialPtr, long baud) {
    serialPtr = _serialPtr;
    serialPtr->begin(baud);
//    serialPtr->println();
}

void HardSerial::clearData() {
    dataSend = "";
}

void HardSerial::sendData() {
    serialPtr->println(dataSend);
}

void HardSerial::sendDataCb(void (*onReceive)(const String &)) {
    serialPtr->println(dataSend);
    onReceive(dataSend);
}

void HardSerial::sendDataAsync(uint32_t _time) {
    if (millis() - sendTime >= _time) {
        sendTime = millis();
        serialPtr->println(dataSend);
    }
}

void HardSerial::sendDataAsyncCb(uint32_t _time, void (*onReceive)(const String &)) {
    if (millis() - sendTime >= _time) {
        sendTime = millis();
        serialPtr->println(dataSend);
        onReceive(dataSend);
    }
}

void HardSerial::sendBytes(int next) {
    serialPtr->write(next);
}

void HardSerial::receive(void (*onReceive)(const String &)) {
    if (onReceive == nullptr) return;
    if (serialPtr->available()) {
        char rxBuffer[250];
        uint8_t rxBufferPtr = 0;
        rxBuffer[rxBufferPtr++] = serialPtr->read();
        while (1) {
            if (serialPtr->available()) {
                rxBuffer[rxBufferPtr++] = serialPtr->read();
                if (rxBuffer[rxBufferPtr - 1] == '\n') break;
            }
        }
        rxBuffer[rxBufferPtr] = 0;
        String dataCb = String(rxBuffer);
        dataCb.replace("\n", "");
        onReceive(dataCb);
    }
}

void HardSerial::receiveAsync(uint32_t _time, void (*onReceive)(const String &)) {
    if (onReceive == nullptr) return;
    if (serialPtr->available()) {
        char rxBuffer[250];
        uint8_t rxBufferPtr = 0;
        rxBuffer[rxBufferPtr++] = serialPtr->read();
        if (millis() - receiveTime >= _time) {
            receiveTime = millis();
            while (1) {
                if (serialPtr->available()) {
                    rxBuffer[rxBufferPtr++] = serialPtr->read();
                    if (rxBuffer[rxBufferPtr - 1] == '\n') break;
                }
            }
            rxBuffer[rxBufferPtr] = 0;
            String dataCb = String(rxBuffer);
            dataCb.replace("\n", "");
            onReceive(dataCb);
        }
    }
}

void HardSerial::receiveString(void (*onReceive)(const String &)) {
    if (serialPtr->available()) {
        String dataCb = serialPtr->readStringUntil('\n');
        onReceive(dataCb);
    }
}

float HardSerial::getData(String data, uint8_t index, char separator[]) {
    return parseStr(data, separator, index).toFloat();
}

String HardSerial::getStrData(String data, uint8_t index, char separator[]) {
    return parseStr(data, separator, index);
}

String HardSerial::parseStr(String data, char separator[], int index) {
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
#endif
#endif
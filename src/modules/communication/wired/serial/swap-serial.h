/*
 *  swap-serial.h
 *
 *  serial communication lib
 *  Created on: 2023. 4. 3
 */

#ifdef USE_SWAP_SERIAL
#pragma once

#ifndef SWAP_SERIAL_COM_H
#define SWAP_SERIAL_COM_H

/* GPIO TABLE https://mikroavr.com/pin-out-esp32/
 * GPIO	FUNCTION	GPIO	FUNCTION
 * GPIO32	RX/TX	GPIO15	RX/TX
 * GPIO33	RX/TX	GPIO21	RX/TX
 * GPIO25	RX/TX	GPIO22	RX/TX
 * GPIO26	RX/TX	GPIO17	RX/TX
 * GPIO27	RX/TX	GPIO16	RX/TX
 * GPIO14	RX/TX
 * GPIO13	RX/TX
 */

#if defined(ESP8266) || defined(ESP32)

#include "Arduino.h"
#include "HardwareSerial.h"

class SwapSerial {
private:
    HardwareSerial *serialPtr;
    String dataSend;
    uint32_t sendTime;
    uint32_t receiveTime;
    String parseStr(String data, char separator[], int index);

public:
    SwapSerial();
    void begin(HardwareSerial *_serialPtr = &Serial2,
               long baud = 9600,
               SerialConfig cfg = SERIAL_8N1,
               uint8_t rxPin = 15, // 16
               uint8_t txPin = 14); // 17

    template<typename T>
    void addData(T newData, const char *separator = ";") {
        dataSend += String(newData);
        dataSend += separator;
    }

    void setTimeOut(uint32_t time);

    void clearData();
    void sendData(bool debug = false, bool endln = false);
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


SwapSerial::SwapSerial() {
    clearData();
}

void SwapSerial::begin(HardwareSerial *_serialPtr,
                       long baud,
                       SerialConfig cfg,
                       uint8_t rxPin,
                       uint8_t txPin) {
    serialPtr = _serialPtr;
    serialPtr->begin(baud, cfg, rxPin, txPin);
}

void SwapSerial::setTimeOut(uint32_t time) {
    serialPtr->setTimeout(time);
}

void SwapSerial::clearData() {
    dataSend = "";
}

void SwapSerial::sendData(bool debug , bool endln) {
    serialPtr->println(dataSend);
    if (debug) {
        Serial.print(dataSend);
        if (endln) Serial.println();
    }
}

void SwapSerial::sendDataCb(void (*onReceive)(const String &)) {
    serialPtr->println(dataSend);
    onReceive(dataSend);
}

void SwapSerial::sendDataAsync(uint32_t _time) {
    if (millis() - sendTime >= _time) {
        sendTime = millis();
        serialPtr->println(dataSend);
    }
}

void SwapSerial::sendDataAsyncCb(uint32_t _time, void (*onReceive)(const String &)) {
    if (millis() - sendTime >= _time) {
        sendTime = millis();
        serialPtr->println(dataSend);
        onReceive(dataSend);
    }
}

void SwapSerial::sendBytes(int next) {
    serialPtr->write(next);
}

void SwapSerial::receive(void (*onReceive)(const String &)) {
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

void SwapSerial::receiveAsync(uint32_t _time, void (*onReceive)(const String &)) {
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

void SwapSerial::receiveString(void (*onReceive)(const String &)) {
    if (serialPtr->available()) {
        String dataCb = serialPtr->readStringUntil('\n');
        onReceive(dataCb);
    }
}

float SwapSerial::getData(String data, uint8_t index, char separator[]) {
    return parseStr(data, separator, index).toFloat();
}

String SwapSerial::getStrData(String data, uint8_t index, char separator[]) {
    return parseStr(data, separator, index);
}

String SwapSerial::parseStr(String data, char separator[], int index) {
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

#endif // defined(ESP8266) || defined(ESP32)
#endif // SWAP_SERIAL_COM_H
#endif
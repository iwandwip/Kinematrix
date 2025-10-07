/*
 *  swap-serial.cpp
 *
 *  serial communication c
 *  Created on: 2023. 4. 3
 */

#include "swap-serial.h"

#if defined(ESP8266) || defined(ESP32)

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

#endif
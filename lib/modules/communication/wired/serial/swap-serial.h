/*
 *  swap-serial.h
 *
 *  serial communication lib
 *  Created on: 2023. 4. 3
 */

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

#pragma message("[COMPILED]: swap-serial.h")

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

#endif // defined(ESP8266) || defined(ESP32)
#endif // SWAP_SERIAL_COM_H

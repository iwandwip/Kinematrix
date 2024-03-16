/*
 *  hard-serial.h
 *
 *  serial communication lib
 *  Created on: 2023. 4. 3
 */

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

    void clearData();
    void sendData();
    void sendDataCb(void (*onReceive)());
    void sendDataAsync(uint32_t _time = 500);
    void sendDataAsyncCb(uint32_t _time = 500, void (*onReceive)() = nullptr);
    void receive(void (*onReceive)(String));
    void receiveAsync(uint32_t _time = 500, void (*onReceive)(String) = nullptr);
    float getData(String data, uint8_t index = 0);
    String getStrData(String data, uint8_t index = 0);
};

#endif

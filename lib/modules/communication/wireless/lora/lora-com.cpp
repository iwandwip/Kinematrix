/*
 *  lora-com.h
 *
 *  lora communication c
 *  Created on: 2023. 4. 3
 *
 *  These are the parameters you can change in the library:
 *
 *  txPower - TX power in dB, defaults to 17
 *  spreadingFactor - spreading factor, defaults to 7
 *  signalBandwidth - signal bandwidth in Hz, defaults to 125E3
 *  codingRateDenominator - denominator of the coding rate, defaults to 5
 *  preambleLength - preamble length in symbols, defaults to 8
 *  syncWord - byte value to use as the sync word, defaults to 0x34
 *  Enable or disable CRC usage, by default a CRC is not used.
 *  In the "Testing and comparing transmitters, receivers and antennas" part of Sr net's guide he mentioned these parameters:
 *
 *  10 mw Power
 *  62.5 kHz bandwidth
 *  Spread factor 12
 *  Code rate 4:5
 *
 */

#include "lora-com.h"

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

String LoRaModule::sendDataCbWaitData(void (*onSend)(const String &)) {
    LoRa.beginPacket();
    LoRa.print(dataSend);
    LoRa.endPacket();
    onSend(dataSend);

    int packetSize = 0;
    while (packetSize == 0) {
        packetSize = LoRa.parsePacket();
    }
    String data = LoRa.readStringUntil('\n');
    return data;
}

String LoRaModule::sendDataCbWaitDataWithTimeout(void (*onSend)(const String &), unsigned long timeout, int maxRetries) {
    int retryAttempts = 0;
    String data;
#ifdef ESP32
    disableLoopWDT();
#endif
    while (retryAttempts < maxRetries) {
        LoRa.beginPacket();
        LoRa.print(dataSend);
        LoRa.endPacket();
        onSend(dataSend);

        uint32_t startTime = millis();
        while (millis() - startTime < timeout) {
//            Serial.print("| time: ");
//            Serial.print(time);
//            Serial.print("| timeout: ");
//            Serial.print(timeout);
            int packetSize = LoRa.parsePacket();
            if (packetSize > 0) {
                data = LoRa.readStringUntil('\n');
#ifdef ESP32
                enableLoopWDT();
#endif
                return data;
            }
//            Serial.print("| packetSize: ");
//            Serial.print(packetSize);
//            Serial.println();
        }
        retryAttempts++;
    }
#ifdef ESP32
    enableLoopWDT();
#endif
    return "";
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
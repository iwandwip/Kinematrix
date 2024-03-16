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

int LoRaModule::init(long frequency) {
    return LoRa.begin(frequency);
}

int LoRaModule::init(uint8_t ss, uint8_t reset, uint8_t dio0, long frequency) {
    LoRa.setPins(ss, reset, dio0);
    return LoRa.begin(frequency);
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

void LoRaModule::clearData() {
    data = NONE;
}

void LoRaModule::sendData() {
    LoRa.beginPacket();
    LoRa.print(data);
    LoRa.endPacket();
}

void LoRaModule::sendDataCb(void (*callback)()) {
    LoRa.beginPacket();
    LoRa.print(data);
    LoRa.endPacket();  // true = async
    callback();
}

void LoRaModule::sendDataAsync(uint32_t _time) {
    if (millis() - sendTime >= _time) {
        LoRa.beginPacket();
        LoRa.print(data);
        LoRa.endPacket();  // true = async
        // LoRa.receive();
        // Serial.println(data);
        sendTime = millis();
    }
}

void LoRaModule::sendDataAsyncCb(uint32_t _time, void (*callback)()) {
    if (millis() - sendTime >= _time) {
        LoRa.beginPacket();
        LoRa.print(data);
        LoRa.endPacket();  // true = async
        callback();
        // LoRa.receive();
        // Serial.println(data);
        sendTime = millis();
    }
}

void LoRaModule::receive(void (*onReceive)(String)) {
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

void LoRaModule::sleep() {
    LoRa.sleep();
}

void LoRaModule::idle() {
    LoRa.idle();
}

float LoRaModule::getData(String data, uint8_t index) {
    return parseStr(data, ";", index).toFloat();
}

String LoRaModule::getStrData(String data, uint8_t index) {
    return parseStr(data, ";", index);
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
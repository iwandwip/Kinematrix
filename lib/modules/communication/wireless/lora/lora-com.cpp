#include "lora-com.h"

LoRaModule::LoRaModule() {
    isLoRaReady = 0;
    waitingForResponse = false;
    responseStartTime = 0;
    responseTimeout = 0;
    responseRetries = 0;
    currentResponseRetry = 0;
    responseCallback = nullptr;
    timeoutCallback = nullptr;
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
    if (onReceive != nullptr) onReceive(dataSend);
}

String LoRaModule::sendDataCbWaitData(void (*onSend)(const String &)) {
    LoRa.beginPacket();
    LoRa.print(dataSend);
    LoRa.endPacket();
    if (onSend != nullptr) onSend(dataSend);

    int packetSize = 0;
    while (packetSize == 0) {
        packetSize = LoRa.parsePacket();
    }
    String data = LoRa.readStringUntil('\n');
    return data;
}

String LoRaModule::sendDataCbWaitDataWithTimeout(void (*onSend)(const String &), unsigned long timeout, int maxRetries, bool wdtTimeout) {
    int retryAttempts = 0;
    String data;
#ifdef ESP32
    if (wdtTimeout) disableLoopWDT();
#endif
    while (retryAttempts < maxRetries) {
        LoRa.beginPacket();
        LoRa.print(dataSend);
        LoRa.endPacket();
        if (onSend != nullptr) onSend(dataSend);

        uint32_t startTime = millis();
        while (millis() - startTime < timeout) {
            int packetSize = LoRa.parsePacket();
            if (packetSize > 0) {
                data = LoRa.readStringUntil('\n');
#ifdef ESP32
                if (wdtTimeout) enableLoopWDT();
#endif
                return data;
            }
        }
        retryAttempts++;
    }
#ifdef ESP32
    if (wdtTimeout) enableLoopWDT();
#endif
    return "";
}

void LoRaModule::sendDataAndExpectResponse(void (*onSend)(const String &), void (*onResponse)(const String &)) {
    cancelResponseWait();

    LoRa.beginPacket();
    LoRa.print(dataSend);
    LoRa.endPacket();

    if (onSend != nullptr) onSend(dataSend);

    waitingForResponse = true;
    responseCallback = onResponse;
    responseStartTime = millis();
    responseTimeout = 0;
    responseRetries = 0;
}

void LoRaModule::sendDataAndExpectResponseWithTimeout(void (*onSend)(const String &), void (*onResponse)(const String &), void (*onTimeout)(), unsigned long timeout, int maxRetries) {
    cancelResponseWait();

    LoRa.beginPacket();
    LoRa.print(dataSend);
    LoRa.endPacket();

    if (onSend != nullptr) onSend(dataSend);

    waitingForResponse = true;
    responseCallback = onResponse;
    timeoutCallback = onTimeout;
    responseStartTime = millis();
    responseTimeout = timeout;
    responseRetries = maxRetries;
    currentResponseRetry = 1;
}

void LoRaModule::update() {
    if (!waitingForResponse) return;

    int packetSize = LoRa.parsePacket();
    if (packetSize > 0) {
        String response = "";
        while (LoRa.available()) {
            response += (char) LoRa.read();
        }

        waitingForResponse = false;

        if (responseCallback != nullptr) {
            responseCallback(response);
        }

        return;
    }

    if (responseTimeout == 0) return;

    if (millis() - responseStartTime >= responseTimeout) {
        if (currentResponseRetry < responseRetries) {
            LoRa.beginPacket();
            LoRa.print(dataSend);
            LoRa.endPacket();

            responseStartTime = millis();
            currentResponseRetry++;
        } else {
            waitingForResponse = false;

            if (timeoutCallback != nullptr) {
                timeoutCallback();
            }
        }
    }
}

void LoRaModule::cancelResponseWait() {
    if (waitingForResponse) {
        waitingForResponse = false;
        responseCallback = nullptr;
        timeoutCallback = nullptr;
    }
}

bool LoRaModule::isWaitingForResponse() {
    return waitingForResponse;
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
        if (onReceive != nullptr) onReceive(dataSend);
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
    if (onReceive == nullptr) return;
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
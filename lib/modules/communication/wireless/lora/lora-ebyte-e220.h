#pragma once

#ifndef LORA_MODULE_EBYTE_E220_H
#define LORA_MODULE_EBYTE_E220_H

#include "Arduino.h"
#include "LoRa_E220.h"

typedef struct {
    uint8_t addressHigh;
    uint8_t addressLow;
    uint8_t channel;

    uint8_t uartBaudRate;
    uint8_t airDataRate;
    uint8_t uartParity;

    uint8_t subPacketSetting;
    uint8_t transmissionPower;
    uint8_t rssiAmbientNoise;

    uint8_t enableRSSI;
    uint8_t fixedTransmission;
    uint8_t enableLBT;
    uint8_t worPeriod;
} LoRaConfig;

class LoRaModuleEByteE220 {
private:
    LoRa_E220 *_lora;
    bool _rssiEnabled;
    String _lastErrorMessage;

    bool _checkResponseStatus(ResponseStatus rs);
    LoRaConfig _convertFromConfiguration(Configuration config);
    Configuration _convertToConfiguration(LoRaConfig config);

public:
    LoRaModuleEByteE220(HardwareSerial *serial, UART_BPS_RATE bpsRate = UART_BPS_RATE_9600);
    LoRaModuleEByteE220(HardwareSerial* serial, byte auxPin, UART_BPS_RATE bpsRate = UART_BPS_RATE_9600);
    LoRaModuleEByteE220(HardwareSerial* serial, byte auxPin, byte m0Pin, byte m1Pin, UART_BPS_RATE bpsRate = UART_BPS_RATE_9600);

#ifdef ACTIVATE_SOFTWARE_SERIAL
    LoRaModuleEByteE220(SoftwareSerial* serial, UART_BPS_RATE bpsRate = UART_BPS_RATE_9600);
#endif

    ~LoRaModuleEByteE220();

    bool begin();

    bool getConfiguration(LoRaConfig *config);
    bool setConfiguration(LoRaConfig config, bool permanent = false);
    bool resetModule();

    bool sendMessage(const String &message);
    bool receiveMessage(String *message, int *rssi = NULL);
    bool receiveMessageUntil(String *message, char delimiter = '\0', int *rssi = NULL);

    bool sendFixedMessage(uint8_t addh, uint8_t addl, uint8_t channel, const String &message);
    bool sendBroadcastMessage(uint8_t channel, const String &message);

    template<typename T>
    bool sendStructure(const T &data) {
        if (_lora == nullptr) return false;
        ResponseStatus rs = _lora->sendMessage(&data, sizeof(T));
        return _checkResponseStatus(rs);
    }

    template<typename T>
    bool sendFixedStructure(uint8_t addh, uint8_t addl, uint8_t channel, const T &data) {
        if (_lora == nullptr) return false;
        ResponseStatus rs = _lora->sendFixedMessage(addh, addl, channel, &data, sizeof(T));
        return _checkResponseStatus(rs);
    }

    template<typename T>
    bool receiveStructure(T *data, int *rssi = NULL) {
        if (!data || _lora == nullptr) return false;

        if (_rssiEnabled && rssi != NULL) {
            ResponseStructContainer rsc = _lora->receiveMessageRSSI(sizeof(T));
            if (rsc.status.code != E220_SUCCESS) {
                _lastErrorMessage = rsc.status.getResponseDescription();
                rsc.close();
                return false;
            }

            memcpy(data, rsc.data, sizeof(T));
            *rssi = rsc.rssi;
            rsc.close();
        } else {
            ResponseStructContainer rsc = _lora->receiveMessage(sizeof(T));
            if (rsc.status.code != E220_SUCCESS) {
                _lastErrorMessage = rsc.status.getResponseDescription();
                rsc.close();
                return false;
            }

            memcpy(data, rsc.data, sizeof(T));
            rsc.close();
        }

        return true;
    }

    bool setAddress(uint8_t high, uint8_t low);
    bool setChannel(uint8_t channel);
    bool setAirDataRate(uint8_t rate);
    bool setUartBaudRate(uint8_t rate);
    bool setTransmissionPower(uint8_t power);
    bool setTransmissionMode(bool fixed);
    bool configureWOR(uint8_t period);

    int available();
    bool hasError() const;
    void clearError();
    String getLastError() const;

    bool enableRSSI(bool enable);
    bool isRSSIEnabled() const;

    bool waitAvailableTimeout(uint32_t timeout);
};

#endif
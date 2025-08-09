#pragma once

#ifndef CERIA_SERIAL_I2C_H
#define CERIA_SERIAL_I2C_H

#pragma message("[COMPILED]: CeriaSerialI2C.h - Simple & Cross-Platform I2C Library")

#include "Arduino.h"
#include "Wire.h"

namespace CeriaDevOP {

    enum I2CMode : uint8_t {
        I2C_MASTER = 0,
        I2C_SLAVE = 1
    };

    enum I2CDataType : uint8_t {
        I2C_TYPE_UINT16 = 0x10,
        I2C_TYPE_FLOAT = 0x20,
        I2C_TYPE_STRING = 0x30
    };

    class CeriaSerialI2C {
    private:
        static CeriaSerialI2C* _instance;
        
        // Simple response system
        static uint8_t _responseBuffer[20];
        static uint8_t _responseLength;
        static I2CDataType _responseType;
        
        // Simple received data storage
        static uint8_t _receivedBuffer[20];
        static uint8_t _receivedLength;
        static I2CDataType _receivedType;
        static bool _hasNewData;
        
        I2CMode _mode;
        uint8_t _slaveAddress;
        uint32_t _framesSent;
        uint32_t _framesReceived;
        uint32_t _errorCount;
        
        static void onReceiveHandler(int numBytes);
        static void onRequestHandler();

    public:
        CeriaSerialI2C();
        ~CeriaSerialI2C();

        // Simple initialization
        bool begin(I2CMode mode, uint8_t slaveAddress = 0);
        void update();
        
        // Simple send functions - cross-platform compatible
        bool send(uint8_t slaveAddr, uint16_t data);
        bool send(uint8_t slaveAddr, float data);
        bool send(uint8_t slaveAddr, const String& data);
        
        // Simple request functions - cross-platform compatible  
        bool request(uint8_t slaveAddr, uint16_t& data);
        bool request(uint8_t slaveAddr, float& data);
        bool request(uint8_t slaveAddr, String& data);
        
        // Simple response setup for slave mode
        void setResponseUint16(uint16_t data);
        void setResponseFloat(float data);
        void setResponseString(const String& data);
        
        // Simple data access for slave mode
        bool hasNewData();
        bool getReceivedData(uint16_t& data);
        bool getReceivedData(float& data);
        bool getReceivedData(String& data);
        I2CDataType getReceivedType();
        
        // Utility functions
        bool isConnected(uint8_t slaveAddr);
        void getStats(uint32_t& sent, uint32_t& received, uint32_t& errors);
        void reset();
    };

}

#endif
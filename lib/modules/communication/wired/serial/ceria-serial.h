#pragma once

#ifndef CERIA_SERIAL_H
#define CERIA_SERIAL_H

#pragma message("[COMPILED]: ceria-serial.h")

#include "Arduino.h"
#include "Stream.h"

namespace CeriaDevOP {

enum FrameSize : uint16_t {
    FRAME_AUTO  = 0,
    FRAME_32B   = 32,
    FRAME_64B   = 64,
    FRAME_128B  = 128,
    FRAME_256B  = 256,
    FRAME_512B  = 512
};

enum DuplexMode : uint8_t {
    FULL_DUPLEX = 0,
    HALF_DUPLEX = 1
};

#define CERIA_SERIAL_STX           0x7E
#define CERIA_SERIAL_ETX           0x7F
#define CERIA_SERIAL_ESC           0x7D
#define CERIA_SERIAL_XOR           0x20

#define CERIA_SERIAL_CMD_DATA            0x00
#define CERIA_SERIAL_CMD_ACK             0x01
#define CERIA_SERIAL_CMD_PING            0x02
#define CERIA_SERIAL_CMD_HEARTBEAT       0x03

#define CERIA_SERIAL_TYPE_STRING         0x00
#define CERIA_SERIAL_TYPE_INT            0x01
#define CERIA_SERIAL_TYPE_FLOAT          0x02
#define CERIA_SERIAL_TYPE_JSON           0x03
#define CERIA_SERIAL_TYPE_BINARY         0x04

enum CeriaSerialError : uint8_t {
    CERIA_SERIAL_ERR_NONE = 0,
    CERIA_SERIAL_ERR_TIMEOUT,
    CERIA_SERIAL_ERR_CRC_FAILED,
    CERIA_SERIAL_ERR_BUFFER_FULL,
    CERIA_SERIAL_ERR_FRAME_TOO_LARGE,
    CERIA_SERIAL_ERR_NO_CONNECTION,
    CERIA_SERIAL_ERR_COLLISION
};

typedef void (*CeriaSerialReceiveCallback)(const String& key, const String& value);
typedef void (*CeriaSerialErrorCallback)(CeriaSerialError error);

class CeriaSerial {
private:
    Stream* _serial;
    uint32_t _baudRate;
    FrameSize _frameSize;
    DuplexMode _duplexMode;
    
    uint8_t _sequenceNumber;
    uint32_t _lastHeartbeat;
    uint32_t _timeout;
    uint8_t _retryCount;
    uint8_t _maxRetries;
    
    uint8_t* _txBuffer;
    uint8_t* _rxBuffer;
    uint16_t _bufferSize;
    uint16_t _rxIndex;
    bool _inFrame;
    bool _escaped;
    
    uint32_t _lastActivity;
    uint32_t _backoffTime;
    bool _channelBusy;
    
    CeriaSerialReceiveCallback _onReceive;
    CeriaSerialErrorCallback _onError;
    
    uint32_t _framesSent;
    uint32_t _framesReceived;
    uint32_t _crcErrors;
    uint32_t _collisions;
    
    uint16_t getOptimalFrameSize();
    bool initializeBuffers();
    void freeBuffers();
    uint16_t calculateCRC16(const uint8_t* data, uint16_t length);
    bool buildFrame(const String& key, const String& value, uint8_t cmd, uint8_t type);
    bool sendFrame(const uint8_t* frame, uint16_t length, bool reliable = false);
    void processIncomingByte(uint8_t byte);
    void processCompleteFrame();
    bool isChannelClear();
    void handleCollision();
    void triggerError(CeriaSerialError error);
    
public:
    CeriaSerial();
    ~CeriaSerial();
    
    bool begin(Stream& serial, uint32_t baud = 115200, FrameSize size = FRAME_AUTO);
    void setMode(DuplexMode mode);
    void setRetries(uint8_t count);
    
    void onReceive(CeriaSerialReceiveCallback callback);
    void onError(CeriaSerialErrorCallback callback);
    
    template<typename T>
    bool send(const String& key, T value, bool reliable = false) {
        return sendTyped(key, value, reliable);
    }
    
    void tick();
    bool isConnected();
    void setTimeout(uint32_t ms);
    void ping();
    
    void getStats(uint32_t& sent, uint32_t& received, uint32_t& errors);
    void reset();
    
    bool sendTyped(const String& key, int value, bool reliable);
    bool sendTyped(const String& key, long value, bool reliable);
    bool sendTyped(const String& key, uint8_t value, bool reliable);
    bool sendTyped(const String& key, uint16_t value, bool reliable);
    bool sendTyped(const String& key, uint32_t value, bool reliable);
    bool sendTyped(const String& key, float value, bool reliable);
    bool sendTyped(const String& key, double value, bool reliable);
    bool sendTyped(const String& key, const char* value, bool reliable);
    bool sendTyped(const String& key, String value, bool reliable);
    
    uint8_t detectStringType(String& str);
    
private:
    bool sendInternal(const String& key, const String& value, uint8_t dataType, bool reliable);
};

#if defined(ESP32)
    #define CERIA_SERIAL_DEFAULT_FRAME    FRAME_256B
    #define CERIA_SERIAL_DEFAULT_TIMEOUT  2000
#elif defined(ESP8266)
    #define CERIA_SERIAL_DEFAULT_FRAME    FRAME_128B
    #define CERIA_SERIAL_DEFAULT_TIMEOUT  3000
#else
    #define CERIA_SERIAL_DEFAULT_FRAME    FRAME_64B
    #define CERIA_SERIAL_DEFAULT_TIMEOUT  5000
#endif

}

#endif
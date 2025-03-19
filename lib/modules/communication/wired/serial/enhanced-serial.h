/*
 *  enhanced_serial.h
 *  Enhanced serial communication library with Stream inheritance
 *  Created on: 2024
 */

#pragma once

#ifndef ENHANCED_SERIAL_H
#define ENHANCED_SERIAL_H

#pragma message("[COMPILED]: enhanced-serial.h")

#include "Arduino.h"
#include "Stream.h"

class EnhancedSerial : public Stream {
public:
    // Error handling enum
    enum class Error {
        NONE,
        TIMEOUT,
        BUFFER_OVERFLOW,
        INVALID_DATA,
        MEMORY_ERROR
    };

private:
    // Internal variables
    Stream *serialPtr;
    String dataSend;
    uint32_t sendTime;
    uint32_t receiveTime;
    uint32_t timeout;
    bool echoEnabled;
    void (*onDataCallback)(const String &);
    Error lastError;

    // Buffer management
    static const size_t BUFFER_SIZE = 256;
    char *buffer;
    size_t bufferSize;
    size_t bufferHead;
    size_t bufferTail;

    // Internal methods
    void writeToBuffer(char c);
    bool isBufferFull() const;
    bool isBufferEmpty() const;
    void setError(Error error);
    bool initializeBuffer(size_t size);
    void freeBuffer();

public:
    // Constructor
    explicit EnhancedSerial(size_t bufferSize = 2);
    ~EnhancedSerial();
    EnhancedSerial(const EnhancedSerial &) = delete;
    EnhancedSerial &operator=(const EnhancedSerial &) = delete;

    // Basic setup
    void begin(Stream *_serialPtr, unsigned long timeout = 1000);

    // Stream virtual functions
    virtual int available() override;
    virtual int read() override;
    virtual int peek() override;
    virtual size_t write(uint8_t byte) override;
    virtual void flush() override;

    // Data sending templates
    template<typename T>
    void addData(T newData, char separator = ';') {
        dataSend += String(newData);
        dataSend += separator;
    }

    template<typename T>
    size_t sendString(T newData) {
        String str = String(newData);
        if (echoEnabled) {
            Serial.println(str);
        }
        return serialPtr->println(str);
    }

    // Basic operations
    void clearData();
    bool sendData();
    bool sendDataCb(void (*onReceive)(const String &));
    void sendDataAsync(uint32_t interval = 500);
    void sendDataAsyncCb(uint32_t interval = 500, void (*onReceive)(const String &) = nullptr);

    // Receiving methods
    void receiveString(void (*onReceive)(const String &));
    void setDataCallback(void (*callback)(const String &));
    void handleCallback();
    void checkCallback();
    String readStringUntil(char terminator = '\n', unsigned long timeout = 1000);
    bool readLine(String &buffer, unsigned long timeout = 1000);
    int readBytesUntil(char terminator, char *buffer, size_t length);
    size_t readAvailable(char *buffer, size_t maxSize);

    // Configuration
    void setEcho(bool enable);
    bool getEcho() const;
    void setTimeout(unsigned long ms);
    unsigned long getTimeout() const;
    void clear();

    // Data parsing
    float getFloat(const String &data, uint8_t index, const char* separator = ";");
    int getInt(const String &data, uint8_t index, const char* separator = ";");
    String getString(const String &data, uint8_t index, const char* separator);
    String parseStr(const String &data, const char* separator, int index);

    // Search functions
    bool find(const char *target, size_t length = 0);
    bool waitForData(unsigned long timeout = 1000);

    // Error handling
    Error getLastError() const;
    String getErrorString() const;
};

#endif // ENHANCED_SERIAL_H
/*
 *  enhanced_serial.cpp
 *  Enhanced serial communication library implementation
 *  Created on: 2024
 */

#include "enhanced-serial.h"

// Private methods
void EnhancedSerial::writeToBuffer(char c) {
    if (isBufferFull()) {
        setError(Error::BUFFER_OVERFLOW);
        return;
    }
    buffer[bufferTail] = c;
    bufferTail = (bufferTail + 1) % BUFFER_SIZE;
}

bool EnhancedSerial::isBufferFull() const {
    return ((bufferTail + 1) % BUFFER_SIZE) == bufferHead;
}

bool EnhancedSerial::isBufferEmpty() const {
    return bufferHead == bufferTail;
}

void EnhancedSerial::setError(Error error) {
    lastError = error;
}

// Constructor
EnhancedSerial::EnhancedSerial(size_t _bufferSize)
        : serialPtr(nullptr),
          timeout(1000),
          echoEnabled(false),
          onDataCallback(nullptr),
          lastError(Error::NONE),
          buffer(nullptr),
          bufferSize(0),
          bufferHead(0),
          bufferTail(0) {
    if (!initializeBuffer(_bufferSize)) {
        setError(Error::MEMORY_ERROR);
    }
    clearData();
}

// Destructor to free dynamic memory
EnhancedSerial::~EnhancedSerial() {
    freeBuffer();
}

// Initialize buffer with given size
bool EnhancedSerial::initializeBuffer(size_t size) {
    freeBuffer(); // Free any existing buffer

    buffer = new char[size];
    if (buffer == nullptr) {
        bufferSize = 0;
        return false;
    }

    bufferSize = size;
    bufferHead = 0;
    bufferTail = 0;
    return true;
}

// Free buffer memory
void EnhancedSerial::freeBuffer() {
    if (buffer != nullptr) {
        delete[] buffer;
        buffer = nullptr;
        bufferSize = 0;
    }
}


// Basic setup
void EnhancedSerial::begin(Stream *_serialPtr, unsigned long _timeout) {
    serialPtr = _serialPtr;
    timeout = _timeout;
    clearData();
    clear();
}

// Stream virtual functions
int EnhancedSerial::available() {
    if (!serialPtr) return 0;
    return serialPtr->available();
}

int EnhancedSerial::read() {
    if (!serialPtr) return -1;
    int data = serialPtr->read();
    if (data != -1 && echoEnabled) {
        Serial.write(data);
    }
    return data;
}

int EnhancedSerial::peek() {
    if (!serialPtr) return -1;
    return serialPtr->peek();
}

size_t EnhancedSerial::write(uint8_t byte) {
    if (!serialPtr) return 0;
    if (echoEnabled) {
        Serial.write(byte);
    }
    return serialPtr->write(byte);
}

void EnhancedSerial::flush() {
    if (serialPtr) serialPtr->flush();
}

// Basic operations
void EnhancedSerial::clearData() {
    dataSend = "";
}

bool EnhancedSerial::sendData() {
    if (!serialPtr) return false;
    size_t written = serialPtr->println(dataSend);
    return written == dataSend.length() + 2; // +2 for \r\n
}

bool EnhancedSerial::sendDataCb(void (*onReceive)(const String &)) {
    if (!sendData()) return false;
    if (onReceive) onReceive(dataSend);
    return true;
}

void EnhancedSerial::sendDataAsync(uint32_t interval) {
    if (millis() - sendTime >= interval) {
        sendTime = millis();
        sendData();
    }
}

void EnhancedSerial::sendDataAsyncCb(uint32_t interval, void (*onReceive)(const String &)) {
    if (millis() - sendTime >= interval) {
        sendTime = millis();
        if (sendData() && onReceive) {
            onReceive(dataSend);
        }
    }
}

// Receiving methods
void EnhancedSerial::receiveString(void (*onReceive)(const String &)) {
    if (!serialPtr || !onReceive) return;

    if (serialPtr->available()) {
        String dataCb = serialPtr->readStringUntil('\n');
        dataCb.trim();
        onReceive(dataCb);
    }
}

void EnhancedSerial::setDataCallback(void (*callback)(const String &)) {
    onDataCallback = callback;
}

void EnhancedSerial::handleCallback() {
    if (!serialPtr || !onDataCallback) return;

    if (serialPtr->available()) {
        String dataCb = serialPtr->readStringUntil('\n');
        dataCb.trim();
        onDataCallback(dataCb);
    }
}

void EnhancedSerial::checkCallback() {
    if (serialPtr && onDataCallback && serialPtr->available()) {
        handleCallback();
    }
}

String EnhancedSerial::readStringUntil(char terminator, unsigned long timeout) {
    String result;
    if (!readLine(result, timeout)) {
        return "";
    }
    return result;
}

bool EnhancedSerial::readLine(String &buffer, unsigned long timeout) {
    if (!serialPtr) return false;

    unsigned long startTime = millis();
    buffer = "";

    while (millis() - startTime < timeout) {
        if (serialPtr->available()) {
            char c = serialPtr->read();
            if (c == '\n') {
                return true;
            }
            if (c != '\r') {
                buffer += c;
            }
        }
        yield();
    }

    setError(Error::TIMEOUT);
    return false;
}

int EnhancedSerial::readBytesUntil(char terminator, char *buffer, size_t length) {
    if (!serialPtr || !buffer) return 0;

    size_t count = 0;
    unsigned long startTime = millis();

    while (count < length && (millis() - startTime < timeout)) {
        if (serialPtr->available()) {
            char c = serialPtr->read();
            if (c == terminator) break;
            buffer[count++] = c;
        }
        yield();
    }

    return count;
}

size_t EnhancedSerial::readAvailable(char *buffer, size_t maxSize) {
    if (!buffer || !serialPtr) return 0;

    size_t count = 0;
    while (count < maxSize && available()) {
        buffer[count++] = read();
    }
    return count;
}

// Configuration methods
void EnhancedSerial::setEcho(bool enable) {
    echoEnabled = enable;
}

bool EnhancedSerial::getEcho() const {
    return echoEnabled;
}

void EnhancedSerial::setTimeout(unsigned long ms) {
    timeout = ms;
}

unsigned long EnhancedSerial::getTimeout() const {
    return timeout;
}

void EnhancedSerial::clear() {
    while (available()) {
        read();
    }
    bufferHead = bufferTail = 0;
}

// Data parsing methods
float EnhancedSerial::getFloat(const String &data, uint8_t index, const char* separator) {
    return parseStr(data, separator, index).toFloat();
}

int EnhancedSerial::getInt(const String &data, uint8_t index, const char* separator) {
    return parseStr(data, separator, index).toInt();
}

String EnhancedSerial::getString(const String &data, uint8_t index, const char* separator) {
    return parseStr(data, separator, index);
}

String EnhancedSerial::parseStr(const String& data, const char* separator, int index) {
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

// Search functions
bool EnhancedSerial::find(const char *target, size_t length) {
    if (!target) return false;
    if (length == 0) length = strlen(target);

    size_t matched = 0;
    unsigned long startTime = millis();

    while (millis() - startTime < timeout) {
        if (available()) {
            if (read() == target[matched]) {
                matched++;
                if (matched == length) return true;
            } else {
                matched = 0;
            }
        }
        yield();
    }

    setError(Error::TIMEOUT);
    return false;
}

bool EnhancedSerial::waitForData(unsigned long timeout) {
    unsigned long startTime = millis();
    while (millis() - startTime < timeout) {
        if (available()) return true;
        yield();
    }
    setError(Error::TIMEOUT);
    return false;
}

// Error handling methods
EnhancedSerial::Error EnhancedSerial::getLastError() const {
    return lastError;
}

String EnhancedSerial::getErrorString() const {
    switch (lastError) {
        case Error::NONE:
            return F("No error");
        case Error::TIMEOUT:
            return F("Timeout");
        case Error::BUFFER_OVERFLOW:
            return F("Buffer overflow");
        case Error::INVALID_DATA:
            return F("Invalid data");
        case Error::MEMORY_ERROR:
            return F("Memory allocation error");
        default:
            return F("Unknown error");
    }
}
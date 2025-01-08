#pragma once

#ifndef EEPROM_LIB_ARDUINO
#define EEPROM_LIB_ARDUINO

#if defined(ESP32) || defined(ESP8266)
#else

#pragma message("[COMPILED]: EEPROMLib.h")

#include "Arduino.h"
#include "EEPROM.h"

class EEPROMLib {
public:
    EEPROMLib();
    int writeByte(int address, byte value);
    byte readByte(int address);
    int writeInt(int address, int value);
    int readInt(int address);
    int writeUint8(int address, uint8_t value);
    uint8_t readUint8(int address);
    int writeUint16(int address, uint16_t value);
    uint16_t readUint16(int address);
    int writeUint32(int address, uint32_t value);
    uint32_t readUint32(int address);
    int writeInt32(int address, int32_t value);
    int32_t readInt32(int address);
    int writeLong(int address, long value);
    long readLong(int address);
    int writeUlong(int address, unsigned long value);
    unsigned long readUlong(int address);
    int writeFloat(int address, float value);
    float readFloat(int address);
    int writeDouble(int address, double value);
    double readDouble(int address);
    int writeString(int address, const String &value);
    String readString(int address);
    int writeChar(int address, char value);
    char readChar(int address);
    int writeBool(int address, bool value);
    bool readBool(int address);
    void reset();

    // Verification methods
    bool verifyWrite(int address, byte value);
    int getChecksum(int startAddress, int endAddress);
    // Utility methods
    void backup(int startAddress, int endAddress, byte* buffer);
    void restore(int startAddress, int endAddress, byte* buffer);
    int getFirstEmptyAddress();
    bool isAddressEmpty(int address);
    int getFreeSpace();
    // Block operations
    void copyBlock(int sourceAddress, int destAddress, int length);
    void moveBlock(int sourceAddress, int destAddress, int length);
    void eraseBlock(int startAddress, int length);
    // Data structure methods
    template<typename T>
    int writeStruct(int address, const T& data) {
        const byte* p = (const byte*)(const void*)&data;
        for (int i = 0; i < sizeof(T); i++) {
            EEPROM.write(address + i, *p++);
        }
        return address + sizeof(T);
    }
    template<typename T>
    T readStruct(int address) {
        T data;
        byte* p = (byte*)(void*)&data;
        for (int i = 0; i < sizeof(T); i++) {
            *p++ = EEPROM.read(address + i);
        }
        return data;
    }
    // Diagnostic methods
    void printMemoryMap();
    bool performTest();
    int checkMemoryHealth();
private:
    byte calculateChecksum(byte* data, int length);
};

#endif
#endif  // EEPROM_LIB_ARDUINO
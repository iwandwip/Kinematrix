#pragma once

#ifndef EEPROM_LIB_ARDUINO
#define EEPROM_LIB_ARDUINO

#if defined(ESP32) || defined(ESP8266)
#else

#include "Arduino.h"
#include "EEPROM.h"

class EEPROMLib {
public:
    EEPROMLib();
    int writeByte(int address, byte value);
    byte readByte(int address);
    int writeInt(int address, int value);
    int readInt(int address);
    int writeFloat(int address, float value);
    float readFloat(int address);
    int writeDouble(int address, double value);
    double readDouble(int address);
    int writeString(int address, const String &value);
    String readString(int address);
    void reset();
private:
    // private variable
};

#endif
#endif  // EEPROM_LIB_ARDUINO
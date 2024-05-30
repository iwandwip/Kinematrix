#include "EEPROMLib.h"

#if defined(ESP32) || defined(ESP8266)
#else

EEPROMLib::EEPROMLib() {

}

int EEPROMLib::writeByte(int address, byte value) {
    EEPROM.write(address, value);
    return address + sizeof(value);
}

byte EEPROMLib::readByte(int address) {
    return EEPROM.read(address);
}

int EEPROMLib::writeInt(int address, int value) {
    byte lowByte = value & 0xFF;
    byte highByte = (value >> 8) & 0xFF;
    EEPROM.write(address, lowByte);
    EEPROM.write(address + 1, highByte);
    return address + sizeof(value);
}

int EEPROMLib::readInt(int address) {
    byte lowByte = EEPROM.read(address);
    byte highByte = EEPROM.read(address + 1);
    return (highByte << 8) | lowByte;
}

int EEPROMLib::writeFloat(int address, float value) {
    byte *bytePointer = (byte *) (void *) &value;
    for (int i = 0; i < sizeof(value); i++) {
        EEPROM.write(address + i, bytePointer[i]);
    }
    return address + sizeof(value);
}

float EEPROMLib::readFloat(int address) {
    float value;
    byte *bytePointer = (byte *) (void *) &value;
    for (int i = 0; i < sizeof(value); i++) {
        bytePointer[i] = EEPROM.read(address + i);
    }
    return value;
}

int EEPROMLib::writeDouble(int address, double value) {
    byte *bytePointer = (byte *) (void *) &value;
    for (int i = 0; i < sizeof(value); i++) {
        EEPROM.write(address + i, bytePointer[i]);
    }
    return address + sizeof(value);
}

double EEPROMLib::readDouble(int address) {
    double value;
    byte *bytePointer = (byte *) (void *) &value;
    for (int i = 0; i < sizeof(value); i++) {
        bytePointer[i] = EEPROM.read(address + i);
    }
    return value;
}

int EEPROMLib::writeString(int address, const String &value) {
    int length = value.length();
    EEPROM.write(address, length);
    for (int i = 0; i < length; i++) {
        EEPROM.write(address + 1 + i, value[i]);
    }
    return address + 1 + length;
}

String EEPROMLib::readString(int address) {
    int length = EEPROM.read(address);
    char data[length + 1];
    for (int i = 0; i < length; i++) {
        data[i] = EEPROM.read(address + 1 + i);
    }
    data[length] = '\0';
    return String(data);
}

void EEPROMLib::reset() {
    for (int i = 0; i < EEPROM.length(); i++) {
        EEPROM.write(i, 0);
    }
}

#endif
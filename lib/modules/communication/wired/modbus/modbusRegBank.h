#ifndef _MODBUSREGBANK
#define _MODBUSREGBANK

#include "Arduino.h"
#include "modbus.h"

struct modbusDigReg {
    word address;
    byte value;
    modbusDigReg *next;
};

struct modbusAnaReg {
    word address;
    word value;
    modbusAnaReg *next;
};

class modbusRegBank {
public:
    modbusRegBank(void);
    void add(word);
    word get(word);
    void set(word, word);

    void sendDataInt(int value, long address);
    void sendDataLong(long value, long address);
    void sendDataString(const String &value, long address, size_t maxLength);
    void sendDataFloat(float value, long address);
    void sendDataDouble(double value, long address);

    int getDataInt(long address);
    long getDataLong(long address);
    String getDataString(long address, size_t maxLength);
    float getDataFloat(long address);
    double getDataDouble(long address);

    void stringToRegisters(const String &str, uint16_t *regs, size_t length);
    String registersToString(const uint16_t *regs, size_t length);

    double mapDouble(double x, double in_min, double in_max, double out_min, double out_max);

private:
    void *search(word);

    modbusDigReg *_digRegs, *_lastDigReg;
    modbusAnaReg *_anaRegs, *_lastAnaReg;
};

#endif

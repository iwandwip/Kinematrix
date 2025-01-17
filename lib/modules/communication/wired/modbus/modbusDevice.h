#ifndef _MODBUSDEVICE
#define _MODBUSDEVICE

#include "Arduino.h"
#include "modbusRegBank.h"
#include "modbus.h"

class modbusDevice : public modbusRegBank {
public:
    modbusDevice(void);
    void setId(byte id);
    byte getId(void);

private:
    byte _id;
};

#endif

#include "modbusRegBank.h"

modbusRegBank::modbusRegBank(void) {
    _digRegs = 0;
    _lastDigReg = 0;
    _anaRegs = 0;
    _lastAnaReg = 0;
}

void modbusRegBank::add(word addr) {
    if (addr < 20000) {
        modbusDigReg *temp;

        temp = (modbusDigReg *) malloc(sizeof(modbusDigReg));
        temp->address = addr;
        temp->value = 0;
        temp->next = 0;

        if (_digRegs == 0) {
            _digRegs = temp;
            _lastDigReg = _digRegs;
        } else {
            _lastDigReg->next = temp; // Assign the last register's next pointer to temp;
            _lastDigReg = temp; // then make temp the last register in the list.
        }
    } else {
        modbusAnaReg *temp;

        temp = (modbusAnaReg *) malloc(sizeof(modbusAnaReg));
        temp->address = addr;
        temp->value = 0;
        temp->next = 0;

        if (_anaRegs == 0) {
            _anaRegs = temp;
            _lastAnaReg = _anaRegs;
        } else {
            _lastAnaReg->next = temp;
            _lastAnaReg = temp;
        }
    }
}

word modbusRegBank::get(word addr) {
    if (addr < 20000) {
        modbusDigReg *regPtr;
        regPtr = (modbusDigReg *) this->search(addr);
        if (regPtr) return (regPtr->value);
        else return (0);
    } else {
        modbusAnaReg *regPtr;
        regPtr = (modbusAnaReg *) this->search(addr);
        if (regPtr) return (regPtr->value);
        else return (0);
    }
}

void modbusRegBank::set(word addr, word value) {
    // for digital data
    if (addr < 20000) {
        modbusDigReg *regPtr;
        // search for the register address
        regPtr = (modbusDigReg *) this->search(addr);
        // if a pointer was returned, the set the register value to true if value is non-zero
        if (regPtr) {
            if (value) regPtr->value = 0xFF;
            else regPtr->value = 0x00;
        }
    } else {
        modbusAnaReg *regPtr;
        // search for the register address
        regPtr = (modbusAnaReg *) this->search(addr);
        // if found, then assign the register value to the new value.
        if (regPtr) regPtr->value = value;
    }
}

void *modbusRegBank::search(word addr) {
    // if the requested address is 0-19999
    // use a digital register pointer assigned to the first digital register,
    // else use an analog register pointer assigned the first analog register

    if (addr < 20000) {
        modbusDigReg *regPtr = _digRegs;

        // if there is no register configured, bail
        if (regPtr == 0) return (0);

        // scan through the linked list until the end of the list or the register is found.
        // return the pointer.
        do {
            if (regPtr->address == addr) return (regPtr);
            regPtr = regPtr->next;
        } while (regPtr);
    } else {
        modbusAnaReg *regPtr = _anaRegs;

        // if there is no register configured, bail
        if (regPtr == 0) return (0);

        // scan through the linked list until the end of the list or the register is found.
        // return the pointer.
        do {
            if (regPtr->address == addr)
                return (regPtr);
            regPtr = regPtr->next;
        } while (regPtr);
    }
    return (0);
}

void modbusRegBank::sendDataInt(int value, long address) {
    this->set(address, (uint16_t) value);
}

void modbusRegBank::sendDataLong(long value, long address) {
    this->set(address, (uint16_t) (value & 0xFFFF));           // Lower 16 bits
    this->set(address + 1, (uint16_t) ((value >> 16) & 0xFFFF)); // Upper 16 bits
}

void modbusRegBank::sendDataString(const String &value, long address, size_t maxLength) {
    uint16_t regs[maxLength];
    stringToRegisters(value, regs, maxLength);
    for (size_t i = 0; i < maxLength; i++) {
        this->set(address + i, regs[i]);
    }
}

void modbusRegBank::sendDataFloat(float value, long address) {
    union {
        float floatData;
        uint16_t regs[2];
    } u;
    u.floatData = value;
    this->set(address, u.regs[0]);
    this->set(address + 1, u.regs[1]);
}

void modbusRegBank::sendDataDouble(double value, long address) {
    union {
        double doubleData;
        uint16_t regs[4];
    } u;
    u.doubleData = value;
    for (int i = 0; i < 4; i++) {
        this->set(address + i, u.regs[i]);
    }
}

int modbusRegBank::getDataInt(long address) {
    return (int) this->get(address);
}

long modbusRegBank::getDataLong(long address) {
    uint16_t lower = this->get(address);
    uint16_t upper = this->get(address + 1);
    return ((long) upper << 16) | lower;
}

String modbusRegBank::getDataString(long address, size_t maxLength) {
    uint16_t regs[maxLength];
    for (size_t i = 0; i < maxLength; i++) {
        regs[i] = this->get(address + i);
    }
    return registersToString(regs, maxLength);
}

float modbusRegBank::getDataFloat(long address) {
    union {
        float floatData;
        uint16_t regs[2];
    } u;
    u.regs[0] = this->get(address);
    u.regs[1] = this->get(address + 1);
    return u.floatData;
}

double modbusRegBank::getDataDouble(long address) {
    union {
        double doubleData;
        uint16_t regs[4];
    } u;
    for (int i = 0; i < 4; i++) {
        u.regs[i] = this->get(address + i);
    }
    return u.doubleData;
}

void modbusRegBank::stringToRegisters(const String &str, uint16_t *regs, size_t length) {
    for (size_t i = 0; i < length; i++) {
        if (i < str.length()) {
            regs[i] = (uint16_t) str[i];
        } else {
            regs[i] = 0; // Padding dengan null karakter
        }
    }
}

String modbusRegBank::registersToString(const uint16_t *regs, size_t length) {
    String result = "";
    for (size_t i = 0; i < length; i++) {
        if (regs[i] == 0) break;
        result += (char) regs[i];
    }
    return result;
}

double modbusRegBank::mapDouble(double x, double in_min, double in_max, double out_min, double out_max) {
    return (x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
}


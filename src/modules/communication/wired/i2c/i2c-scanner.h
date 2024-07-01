/*
 *  i2c-scanner.h
 *
 *  i2c scanner header
 *  Created on: 2023. 4. 3
 */

#ifndef KADITA_I2C_SCANNER_H
#define KADITA_I2C_SCANNER_H

#include "Arduino.h"
#include "Wire.h"

class I2CScanner {
public:
    I2CScanner();
    virtual ~I2CScanner();

    void beginTransmission();
    void scanI2CAddress();
    void endTransmission();
private:
    int n_address_[8];
    int n_devices_;
};

#endif  // KADITA_I2C_SCANNER_H

/*  
 *  12c-expander.h
 *
 *  12c expander module header
 *  Created on: 2023. 4. 3
 */

#ifndef KADITA_I2C_EXPANDER_H
#define KADITA_I2C_EXPANDER_H

#include "Arduino.h"
#include "TCA9548A.h"

class I2CExpander : public TCA9548A {
private:
    uint8_t addr;
public:
    using TCA9548A::TCA9548A;
};

#endif  // KADITA_I2C_EXPANDER_H

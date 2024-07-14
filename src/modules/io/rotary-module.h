/*  
 *  rotary-module.h
 *
 *  rotary module header
 *  Created on: 2023. 4. 3
 */

#ifdef USE_ROTARY_MODULE
#ifndef KADITA_ROTARY_MODULE_H
#define KADITA_ROTARY_MODULE_H

#include "Arduino.h"
#include "RotaryEncoder.h"

class RotaryModule {
private:
    RotaryEncoder *encoder;
    uint8_t _pin_clock;
    uint8_t _pin_data;
    int8_t direction;
public:
    explicit RotaryModule(uint8_t pin_clock_, uint8_t pin_data_);
    void init(void (*pin_callback_)() = nullptr);
    void readPosition();
    int getPosition();
    int getDirection();
    bool isMoveCW() const;
    bool isMoveCCW() const;
    bool isStop() const;
};

#endif  // KADITA_ROTARY_MODULE_H
#endif

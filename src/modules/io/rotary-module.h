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
RotaryModule::RotaryModule(uint8_t pin_clock_, uint8_t pin_data_)
        : encoder(nullptr),
          _pin_clock(pin_clock_),
          _pin_data(pin_data_) {
}

void RotaryModule::init(void (*pin_callback_)()) {
    encoder = new RotaryEncoder(_pin_clock, _pin_data, RotaryEncoder::LatchMode::TWO03);
    attachInterrupt(digitalPinToInterrupt(_pin_clock), pin_callback_, CHANGE);
    attachInterrupt(digitalPinToInterrupt(_pin_data), pin_callback_, CHANGE);
}

void RotaryModule::readPosition() {
    encoder->tick();
}

int RotaryModule::getPosition() {
    return encoder->getPosition();
}

int RotaryModule::getDirection() {
    direction = (int) (encoder->getDirection());
    return direction;
}

bool RotaryModule::isMoveCW() const {
    return direction == 1;
}

bool RotaryModule::isMoveCCW() const {
    return direction == -1;
}

bool RotaryModule::isStop() const {
    return direction == 0;
}
#endif  // KADITA_ROTARY_MODULE_H
#endif

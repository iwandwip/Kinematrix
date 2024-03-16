/*  
 *  input-module.h
 *
 *  input module header
 *  Created on: 2023. 4. 3
 */

#ifndef KADITA_INPUT_MODULE_H
#define KADITA_INPUT_MODULE_H

#include "Arduino.h"

#define COUNT_FALLING 0
#define COUNT_RISING 1
#define COUNT_BOTH 2

class DigitalIn {
private:
    int btnPin;
    unsigned long debounceTime;
    unsigned long count;
    int countMode;

    int previousSteadyState;
    int lastSteadyState;
    int lastFlickerableState;

    unsigned long lastDebounceTime;

public:
    explicit DigitalIn(int pin);
    DigitalIn(int pin, int mode);
    void setInterrupt(void (*_callback)(), int mode = RISING);
    void setDebounceTime(unsigned long time);
    int getState() const;
    virtual int getStateRaw() const;
    bool isPressed(unsigned long time);
    bool isPressed() const;
    bool isReleased() const;
    void setCountMode(int mode);
    unsigned long getCount() const;
    void resetCount();
    void update();
};

#endif  // KADITA_INPUT_MODULE_H

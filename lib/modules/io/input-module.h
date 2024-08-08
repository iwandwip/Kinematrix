/*  
 *  input-module.h
 *
 *  input module header
 *  Created on: 2023. 4. 3
 */

#ifndef KADITA_INPUT_MODULE_H
#define KADITA_INPUT_MODULE_H

#include "Arduino.h"
#include "PCF8574.h"

#pragma message("[COMPILED]: input-module.h")

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
    void setDebounceTime(unsigned long time);
    int getState() const;
    int getStateRaw() const;
    bool isPressed(unsigned long time);
    bool isPressed() const;
    bool isLongPressed(unsigned long time) const;
    bool isReleased() const;
    void setCountMode(int mode);
    unsigned long getCount() const;
    void resetCount();
    void resetState();
    void update();
    static void updateAll(DigitalIn *ptr, ...);
    static DigitalIn *stop();
};

////////////////////////////////////////////////////////////////////////

class DigitalInBase {
protected:
    int btnPin;
    unsigned long debounceTime;
    unsigned long count;
    int countMode;

    int previousSteadyState;
    int lastSteadyState;
    int lastFlickerableState;

    unsigned long lastDebounceTime;

public:
    explicit DigitalInBase(int pin);
    DigitalInBase(int pin, int mode);
    void setDebounceTime(unsigned long time);
    int getState() const;
    virtual int getStateRaw() const;
    bool isPressed(unsigned long time);
    bool isPressed() const;
    bool isLongPressed(unsigned long time) const;
    bool isReleased() const;
    void setCountMode(int mode);
    unsigned long getCount() const;
    void resetCount();
    void resetState();
    void update();
    static void updateAll(DigitalInBase *ptr, ...);
    static DigitalInBase *stop();
};

////////////////////////////////////////////////////////////////////////

class DigitalInI2C : public DigitalInBase {
private:
    PCF8574 *pcf8574;
public:
    explicit DigitalInI2C(int pin);
    DigitalInI2C(int pin, int mode);
    void init(PCF8574 *_pcf8574);
    int getStateRaw() const override;
};

#endif  // KADITA_INPUT_MODULE_H

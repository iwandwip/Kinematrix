#ifndef KINEMATRIX_PCF8574_DIGITAL_IN_H
#define KINEMATRIX_PCF8574_DIGITAL_IN_H

#include "Arduino.h"
#include "pcf8574-module.h"

#define COUNT_FALLING 0
#define COUNT_RISING 1
#define COUNT_BOTH 2

class PCF8574DigitalIn {
private:
    uint8_t btnPin;
    PCF8574Module* pcfModule;
    unsigned long debounceTime;
    unsigned long count;
    int countMode;

    int previousSteadyState;
    int lastSteadyState;
    int lastFlickerableState;

    unsigned long lastDebounceTime;

public:
    explicit PCF8574DigitalIn(uint8_t pin);
    PCF8574DigitalIn(uint8_t pin, PCF8574Module* module);

    void init(PCF8574Module* module);
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

    static void updateAll(PCF8574DigitalIn* ptr, ...);
    static PCF8574DigitalIn* stop();
};

#endif
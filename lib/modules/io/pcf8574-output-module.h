#ifndef KINEMATRIX_PCF8574_DIGITAL_OUT_H
#define KINEMATRIX_PCF8574_DIGITAL_OUT_H

#include "Arduino.h"
#include "pcf8574-module.h"

class PCF8574DigitalOut {
private:
    uint8_t _pin;
    PCF8574Module *pcfModule;
    unsigned long _onDelayTime;
    unsigned long _onDelayStart;
    bool _onDelayState;
    unsigned long _offDelayTime;
    unsigned long _offDelayStart;
    bool _offDelayState;
    bool _state;
    bool _reverse;
    unsigned long _toggleTime;

public:
    PCF8574DigitalOut();
    explicit PCF8574DigitalOut(uint8_t pin, PCF8574Module *module, bool isReverse = false);

    void init(PCF8574Module *module);
    void on();
    void off();
    void onToOffDelay(unsigned long delay_time);
    void offToOnDelay(unsigned long delay_time);
    void set(bool state);
    void toggle(void (*toggleCallback)() = nullptr);
    void toggleDelay(unsigned long delay_time, void (*toggleCallback)() = nullptr);
    void toggleInit(unsigned long delay_time = 100, int num = 1, void (*toggleCallback)() = nullptr);
    void toggleAsync(unsigned long delay_time = 100, void (*toggleCallback)(bool state) = nullptr);
    void onDelay(unsigned long delay_time);
    void offDelay(unsigned long delay_time);
    void update();
    void reverse();
    void setPin(uint8_t pin, bool isReverse = false);
    bool getState() const;

    static void updateAll(PCF8574DigitalOut *ptr, ...);
    static PCF8574DigitalOut *stop();
};

#endif
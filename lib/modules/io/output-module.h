/*
 *  output-module.h
 *
 *  output module header
 *  Created on: 2023. 4. 3
 */

#pragma once

#ifndef KADITA_OUTPUT_MODULE_H
#define KADITA_OUTPUT_MODULE_H

#include "Arduino.h"

#pragma message("[COMPILED]: output-module.h")

class DigitalOut {
private:
    int _pin;
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
    DigitalOut();
    explicit DigitalOut(int pin, bool isReverse = false);
    void init();
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
    void setPins(uint8_t pin, bool isReverse = false);
    bool getState() const;
    static void updateAll(DigitalOut *ptr, ...);
    static DigitalOut *stop();
};

#endif  // KADITA_OUTPUT_MODULE_H
/*
 *  KeedBase.h
 *
 *  Kastara Electronics Embedded Development
 *  Created on: 2023. 4. 3
 */

#pragma once

#ifndef KEED_BASE_H
#define KEED_BASE_H

#include "AutoLightConfig/KeedConfig.h"

struct interrupt_t {
    uint8_t pin = 0;
    volatile uint32_t num = 0;
    volatile bool pressed = false;
    volatile bool changed = false;
    void (*isrCallback)() = nullptr;

    void attachInterrupt(uint8_t _pin, void (*_callback)()) {
        pin = _pin;
        isrCallback = _callback;
    }
};

class KeedBase {
public:
    KeedBase() = default;
    ~KeedBase() = default;
    // pure virtual function
    virtual void init(IOExpander **ioBase, configuration_t _cfg) = 0;
    virtual void update() = 0;
    virtual void run() = 0;

    // basic virtual function
    __attribute__((unused)) virtual void setInterruptConfig(interrupt_t _cfg) {
        /*implemented on a derived class*/
    }

    __attribute__((unused)) virtual interrupt_t getInterruptConfig() {
        interrupt_t interrupt;
        return interrupt;
    }

    __attribute__((unused)) virtual void changeModes() {
        /*implemented on a derived class*/
    }

    __attribute__((unused)) virtual void readModes() {
        /*implemented on a derived class*/
    }

    __attribute__((unused)) virtual void setBaseDelay(uint32_t _time) {
        /*implemented on a derived class*/
    }

    __attribute__((unused)) virtual uint8_t getSequenceIndex() {
        return 0;
    }

    KeedBase &operator=(const KeedBase &) = default;
    KeedBase &operator=(KeedBase &&) = default;
};

#endif // KEED_BASE_H
/*
 *  ButtonInterrupt.h
 *
 *  Kastara Electronics Embedded Development
 *  Created on: 2023. 4. 3
 */
#ifdef  USE_AUTOLIGHT_v2
#pragma once

#ifndef BUTTON_INTERRUPT_H
#define BUTTON_INTERRUPT_H

#include "Arduino.h"

namespace AutoLight {
    typedef struct {
        volatile int num_of_pressed_;
        volatile bool is_pressed_;
        volatile bool is_changed_;
    } VolatileData;

    class ButtonInterrupt {
    public:
        ButtonInterrupt(bool _enable_unit_test = false);
        virtual ~ButtonInterrupt();

        void attach(uint8_t _pin, int _pin_mode, int _isr_mode, void (*_callback)(), uint32_t _timer = 250);
        void readInterrupt(void (*_callback)() = nullptr);
        void resetButton();

        volatile bool isPressed();
        volatile int getNumOfPressed();

        int getPins();
        ButtonInterrupt *operator()();

        void debug(int8_t _sequence_index = -1);
    private:
        uint8_t pin_interrupt_;
        uint32_t interrupt_timer_;
        uint32_t debounce_time;
        bool is_unit_test_;

        VolatileData volatile_data_;
        void (*isr_callback_)();
    };
}

#endif // BUTTON_INTERRUPT_H
#endif
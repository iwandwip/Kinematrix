/*
 *  ButtonInterrupt.h
 *
 *  Kastara Electronics Embedded Development
 *  Created on: 2023. 4. 3
 */

#pragma once

#ifndef BUTTON_INTERRUPT_H
#define BUTTON_INTERRUPT_H

#include <Arduino.h>

namespace AutoLight {
#define MAX_BUTTONS 4

    typedef struct {
        volatile int num_of_pressed_[MAX_BUTTONS];
        volatile bool is_pressed_[MAX_BUTTONS];
        volatile bool is_changed_[MAX_BUTTONS];
    } VolatileData;

    class ButtonInterrupt {
    public:
        explicit ButtonInterrupt(bool _enable_unit_test = false);
        virtual ~ButtonInterrupt();

        ButtonInterrupt &attach(uint8_t _button_index, uint8_t _pin, int _pin_mode, int _isr_mode, void (*_callback)(), uint32_t _timer = 250);
        void readInterrupt(uint8_t _button_index, void (*_callback)() = nullptr);
        void resetButton(uint8_t _button_index);

        volatile bool isPressed(uint8_t _button_index) const;
        volatile int getNumOfPressed(uint8_t _button_index) const;

        int getPin(uint8_t _button_index) const;
        ButtonInterrupt *operator()();

        void debug(int8_t _sequence_index = -1);
    private:
        uint8_t pin_interrupt_[MAX_BUTTONS]{};
        uint32_t interrupt_timer_[MAX_BUTTONS]{};
        uint32_t debounce_time;
        bool is_unit_test_;

        VolatileData volatile_data_{};
        void (*isr_callback_[MAX_BUTTONS])(){};
    };
}

#endif // BUTTON_INTERRUPT_H
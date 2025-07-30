/*
 *  ButtonInterrupt.cpp
 *
 *  Kastara Electronics Embedded Development
 *  Created on: 2023. 4. 3
 *  Modified for 4 button support
 */

#include "ButtonInterrupt.h"

namespace AutoLight {
    ButtonInterrupt::ButtonInterrupt(bool _enable_unit_test)
            : debounce_time(250),
              is_unit_test_(_enable_unit_test) {

        // Initialize all button states
        for (uint8_t i = 0; i < Constants::MAX_BUTTONS; i++) {
            pin_interrupt_[i] = 0;
            interrupt_timer_[i] = 0;
            volatile_data_.num_of_pressed_[i] = 0;
            volatile_data_.is_pressed_[i] = false;
            volatile_data_.is_changed_[i] = false;
            isr_callback_[i] = nullptr;
        }
    }

    ButtonInterrupt::~ButtonInterrupt() {
        // Don't free(this) as it can cause issues
    }

    ButtonInterrupt &ButtonInterrupt::attach(uint8_t _button_index, uint8_t _pin, int _pin_mode, int _isr_mode, void (*_callback)(), uint32_t _timer) {
        if (_button_index >= Constants::MAX_BUTTONS) return *this;  // Safety check

        pin_interrupt_[_button_index] = _pin;
        isr_callback_[_button_index] = _callback;
        interrupt_timer_[_button_index] = _timer;

        pinMode(pin_interrupt_[_button_index], _pin_mode);
        attachInterrupt(pin_interrupt_[_button_index], isr_callback_[_button_index], _isr_mode);

        return *this;
    }

    void ButtonInterrupt::readInterrupt(uint8_t _button_index, void (*_callback)()) {
        if (_button_index >= Constants::MAX_BUTTONS) return;  // Safety check

        if (millis() - interrupt_timer_[_button_index] >= debounce_time) {
            volatile_data_.num_of_pressed_[_button_index]++;
            volatile_data_.is_pressed_[_button_index] = true;

            if (_callback != nullptr) {
                _callback();
            } else if (isr_callback_[_button_index] != nullptr) {
                isr_callback_[_button_index]();
            }

            interrupt_timer_[_button_index] = millis();
        }
    }

    void ButtonInterrupt::resetButton(uint8_t _button_index) {
        if (_button_index >= Constants::MAX_BUTTONS) return;  // Safety check

        volatile_data_.is_pressed_[_button_index] = false;
    }

    volatile bool ButtonInterrupt::isPressed(uint8_t _button_index) const {
        if (_button_index >= Constants::MAX_BUTTONS) return false;  // Safety check

        return volatile_data_.is_pressed_[_button_index] || volatile_data_.is_changed_[_button_index];
    }

    volatile int ButtonInterrupt::getNumOfPressed(uint8_t _button_index) const {
        if (_button_index >= Constants::MAX_BUTTONS) return 0;  // Safety check

        return volatile_data_.num_of_pressed_[_button_index];
    }

    int ButtonInterrupt::getPin(uint8_t _button_index) const {
        if (_button_index >= Constants::MAX_BUTTONS) return -1;  // Safety check

        return pin_interrupt_[_button_index];
    }

    ButtonInterrupt *ButtonInterrupt::operator()() {
        return this;
    }

    void ButtonInterrupt::debug(int8_t _sequence_index) {
        if (!is_unit_test_) return;

        Serial.println("==============================");
        Serial.println("| START DEBUG BUTTON INTERRUPT");
        Serial.println("==============================");

        if (_sequence_index != -1) {
            Serial.print("| _sequence_index              : ");
            Serial.print("| ");
            Serial.print(_sequence_index);
            Serial.println();
        }

        for (uint8_t i = 0; i < Constants::MAX_BUTTONS; i++) {
            Serial.println("|------------------------------");
            Serial.print("| BUTTON                       : ");
            Serial.print("| ");
            Serial.print(i);
            Serial.println();

            Serial.print("| num_of_pressed_              : ");
            Serial.print("| ");
            Serial.print(volatile_data_.num_of_pressed_[i]);
            Serial.println();

            Serial.print("| is_pressed_                  : ");
            Serial.print("| ");
            Serial.print(volatile_data_.is_pressed_[i] ? "true" : "false");
            Serial.println();

            Serial.print("| is_changed_                  : ");
            Serial.print("| ");
            Serial.print(volatile_data_.is_changed_[i] ? "true" : "false");
            Serial.println();

            Serial.print("| pin_interrupt_               : ");
            Serial.print("| ");
            Serial.print(pin_interrupt_[i]);
            Serial.println();

            Serial.print("| interrupt_timer_             : ");
            Serial.print("| ");
            Serial.print(interrupt_timer_[i]);
            Serial.println();

            Serial.print("| isr_callback_                : ");
            Serial.print("| ");
            Serial.print((isr_callback_[i] != nullptr) ? "true" : "false");
            Serial.println();
        }

        Serial.println("| END DEBUG BUTTON INTERRUPT");
        Serial.println("==============================");
    }
}
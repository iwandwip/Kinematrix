/*
 *  ButtonInterrupt.cpp
 *
 *  Kastara Electronics Embedded Development
 *  Created on: 2023. 4. 3
 */

#include "ButtonInterrupt.h"

namespace AutoLight {
    ButtonInterrupt::ButtonInterrupt(bool _enable_unit_test)
            : pin_interrupt_(0),
              interrupt_timer_(0),
              debounce_time(250),
              is_unit_test_(_enable_unit_test),
              volatile_data_{
                      .num_of_pressed_ = 0,
                      .is_pressed_ = false,
                      .is_changed_ = false,
              },
              isr_callback_(nullptr) {
    }

    ButtonInterrupt::~ButtonInterrupt() {
        free(this);
    }

    void ButtonInterrupt::attach(uint8_t _pin, int _pin_mode, int _isr_mode, void (*_callback)(), uint32_t _timer) {
        pin_interrupt_ = _pin;
        isr_callback_ = _callback;
        interrupt_timer_ = _timer;

        pinMode(pin_interrupt_, _pin_mode);
        attachInterrupt(pin_interrupt_, isr_callback_, _isr_mode);
    }

    void ButtonInterrupt::readInterrupt(void (*_callback)()) {
        if (millis() - interrupt_timer_ >= debounce_time) {
            volatile_data_.num_of_pressed_++;
            volatile_data_.is_pressed_ = true;
            if (_callback != nullptr) _callback();
            interrupt_timer_ = millis();
        }
    }

    void ButtonInterrupt::resetButton() {
        volatile_data_.is_pressed_ = false;
    }

    volatile bool ButtonInterrupt::isPressed() const {
        return volatile_data_.is_pressed_ || volatile_data_.is_changed_;
    }

    volatile int ButtonInterrupt::getNumOfPressed() const {
        return volatile_data_.num_of_pressed_;
    }

    int ButtonInterrupt::getPins() const {
        return pin_interrupt_;
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

        Serial.print("| num_of_pressed_              : ");
        Serial.print("| ");
        Serial.print(volatile_data_.num_of_pressed_);
        Serial.println();

        Serial.print("| pin_interrupt_               : ");
        Serial.print("| ");
        Serial.print(pin_interrupt_);
        Serial.println();

        Serial.print("| interrupt_timer_             : ");
        Serial.print("| ");
        Serial.print(interrupt_timer_);
        Serial.println();

        Serial.print("| isr_callback_                : ");
        Serial.print("| ");
        Serial.print((isr_callback_ != nullptr) ? "true" : "false");
        Serial.println();

        Serial.println("| END DEBUG BUTTON INTERRUPT");
        Serial.println("==============================");
    }
}
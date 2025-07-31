/*
 *  Indicator.cpp
 *
 *  Kastara Electronics Embedded Development
 *  Created on: 2023. 4. 3
 */

// Indicator.cpp - Implementation file
#include "Indicator.h"

#if defined(ESP32)

namespace AutoLight {
    Indicator::Indicator()
            : output_data_{
            .led_red_= DigitalOut(Constants::LED_RED_PIN),
            .led_green_= DigitalOut(Constants::LED_GREEN_PIN),
            .led_blue_= DigitalOut(Constants::LED_BLUE_PIN),
            .buzzer_ = DigitalOut(Constants::BUZZER_PIN),
            .timer_{TimerTask(500),
                    TimerTask(500),
                    TimerTask(100),
                    TimerTask(100)}
    } {
    }

    Indicator::~Indicator() {
        delete this;
    }

    void Indicator::show(uint8_t _sequence) {
        state_ = (_sequence == 0) ? 1 : 0;
        if (_sequence != sequence_before_) {
            output_data_.led_blue_.on();
            output_data_.led_blue_.offDelay(600);
            output_data_.buzzer_.on();
            output_data_.buzzer_.offDelay(50);
            sequence_before_ = _sequence;
        }
        if (state_) {
            output_data_.led_red_.on();
            output_data_.led_green_.off();
        } else {
            output_data_.led_green_.on();
            output_data_.led_red_.off();
        }
    }

    void Indicator::update() {
        output_data_.led_red_.update();
        output_data_.led_green_.update();
        output_data_.led_blue_.update();
        output_data_.buzzer_.update();
    }

}

#endif
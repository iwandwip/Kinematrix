/*
 *  Indicator.h
 *
 *  Kastara Electronics Embedded Development
 *  Created on: 2023. 4. 3
 */

#ifndef INDICATOR_H
#define INDICATOR_H

#if defined(ESP32)

#include "Arduino.h"

#ifndef ENABLE_MODULE_NODEF_DIGITAL_OUTPUT
#define ENABLE_MODULE_NODEF_DIGITAL_OUTPUT
#endif
#ifndef ENABLE_MODULE_NODEF_TIMER_TASK
#define ENABLE_MODULE_NODEF_TIMER_TASK
#endif

#include "KinematrixModulesNoDef.h"

namespace AutoLight {
    const int MAXNUM_INDICATOR = 4;
    const int LED_RED_PIN = 14;
    const int LED_GREEN_PIN = 12;
    const int LED_BLUE_PIN = 13;
    const int BUZZER_PIN = 15; // 2

    class OutputData {
    public:
        DigitalOut led_red_;
        DigitalOut led_green_;
        DigitalOut led_blue_;
        DigitalOut buzzer_;
        TimerTask timer_[MAXNUM_INDICATOR];
    };

    class Indicator {
    public:
        Indicator();
        virtual ~Indicator();

        void show(uint8_t _sequence = 0);
        void update();
    private:
        uint8_t state_;
        uint8_t sequence_before_;
        OutputData output_data_;
    };

}

#endif

#endif // INDICATOR_H
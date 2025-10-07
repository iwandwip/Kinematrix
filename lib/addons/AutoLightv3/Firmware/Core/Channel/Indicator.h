#pragma once
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
#include "../Common/Constants.h"

namespace AutoLight {
    using namespace Constants;

    class OutputData {
    public:
        DigitalOut led_red_;
        DigitalOut led_green_;
        DigitalOut led_blue_;
        DigitalOut buzzer_;
        TimerTask timer_[Constants::MAXNUM_INDICATOR];
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

#endif

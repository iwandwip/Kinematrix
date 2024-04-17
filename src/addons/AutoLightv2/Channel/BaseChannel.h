/*
 *  BaseChannel.h
 *
 *  Kastara Electronics Embedded Development
 *  Created on: 2023. 4. 3
 */

#pragma once

#ifndef BASE_CHANNEL_H
#define BASE_CHANNEL_H

#include "Arduino.h"
#include "modules/communication/wired/i2c/io-expander.h"
#include "modules/io/output-module.h"
#include "modules/time/timer-task.h"

#include "../Config/BaseConfig.h"
#include "../ButtonInterrupt.h"
#include "../Cores/Task.h"

namespace AutoLight {
    const int MAXNUM_TASK_SEQUENCE = 10;
    const int MAXNUM_TASK_SEQUENCE_ADDITION = 2;
    const int MAXNUM_TOTAL_TASK_SEQUENCE = 12;
    const int MAXNUM_TASK_TIME = 250;

    const int DEFAULT_DELAY_TIME = 50;
    const int DEFAULT_INIT_SEQUENCE = 0;

    void taskCallback();

    class ChannelData {
    public:
        volatile uint32_t delay_time_;
        volatile uint32_t sequence_index_;
        volatile uint32_t sequence_index_apps_;
        volatile bool is_reverse_;
        volatile bool is_mode_changed_;
        volatile bool is_mode_changed_apps_;
    };

    class ExpanderIo {
    public:
        IOExpander **io_base_;
        uint8_t io_length;
    };

    class BaseChannel {
    public:
        BaseChannel(bool _enable_unit_test = false);
        virtual ~BaseChannel();

        bool initialize();
        void enableI2CConfig();

        void attachConfig(ConfigData *_config_data_ptr);
        void attachInterrupt(ButtonInterrupt *_button_interrupt_ptr);

        void setParamCallback(void (*_callback)());
        void setInitDelay(uint32_t _time);
        void setInitSequence(uint8_t _sequence);
        void setTotalSequence(uint8_t _nums);
        void reverse(bool _is_reverse = false);

        volatile uint32_t getSequenceIndex();
        volatile uint32_t getDelayTime();
        ChannelData getChannelData();

        void runAutoLight(void (*_callbackRun)(uint32_t sequence) = nullptr);
        void changeMode();
        void changeModeApp(uint32_t num);
        volatile bool isChangeMode();

        void setStateHigh(int index, ...);
        void setStateLow(int index, ...);

        void set(uint8_t _pin, uint8_t _state);
        void sleep(uint32_t _delay);

        void forceOff();
        void off();
        void on();

        bool isReady();
        void debug();

        // task sequence for each mode
        void taskSequence2();
        void taskSequence3();
        void taskSequence4();
        void taskSequence5();
        void taskSequence6();
        void taskSequence7();
        void taskSequence8();
        void taskSequence9();
        void taskSequence10();
        void taskSequence11();

    private:
        bool addIoExpander(IOExpander *_io_expander);
        bool beginIoExpander();

        void setTaskSequenceFunction();

        bool is_using_i2c_config_;
        bool is_initialize_;
        bool is_unit_test_;

        uint8_t total_task_sequence_;
        uint32_t task_sequence_timer_;

        void (BaseChannel::*temp_mode_)();
        void (BaseChannel::*total_mode_[(MAXNUM_TOTAL_TASK_SEQUENCE)])();

        ExpanderIo expander_io_;
        ChannelData channel_data_;

        ConfigData *config_data_ptr_;
        ButtonInterrupt *button_interrupt_ptr_;
    };
}

#endif // BASE_CHANNEL_H
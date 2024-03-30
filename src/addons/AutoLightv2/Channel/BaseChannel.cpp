/*
 *  BaseChannel.cpp
 *
 *  Kastara Electronics Embedded Development
 *  Created on: 2023. 4. 3
 */

#include "BaseChannel.h"

namespace AutoLight {
    BaseChannel::BaseChannel(bool _enable_unit_test)
            : is_unit_test_(_enable_unit_test) {
        setFunction();
    }

    BaseChannel::~BaseChannel() {
        free(this);
    }

    bool BaseChannel::initialize() {
        is_initialize_ = beginIoExpander();
        return is_initialize_;
    }

    void BaseChannel::enableI2CConfig() {
        is_using_i2c_config_ = true;
    }

    void BaseChannel::attachConfig(ConfigData *_config_data_ptr) {
        config_data_ptr_ = _config_data_ptr;
    }

    void BaseChannel::attachInterrupt(ButtonInterrupt *_button_interrupt_ptr) {
        button_interrupt_ptr_ = _button_interrupt_ptr;
    }

    void BaseChannel::setParamCallback(void (*_callback)()) {
        _callback();
    }

    void BaseChannel::setInitDelay(uint32_t _time) {
        channel_data_.delay_time_ = _time;
    }

    void BaseChannel::setInitSequence(uint8_t _sequence) {
        channel_data_.sequence_index_ = _sequence;
        temp_mode_ = total_mode_[channel_data_.sequence_index_];
    }

    void BaseChannel::setTotalSequence(uint8_t _nums) {
        total_task_sequence_ = _nums;
    }

    void BaseChannel::reverse(bool _is_reverse) {
        channel_data_.is_reverse_ = _is_reverse;
    }

    volatile uint8_t BaseChannel::getSequenceIndex() {
        return channel_data_.sequence_index_;
    }

    void BaseChannel::changeMode() {
        is_mode_changed_ = true;
    }

    void BaseChannel::runAutoLight() {
        if (is_mode_changed_) {
            forceOff();
            channel_data_.sequence_index_++;
            if (channel_data_.sequence_index_ > (total_task_sequence_ - 1) + 2) {
                channel_data_.sequence_index_ = 0;
            }
            temp_mode_ = total_mode_[channel_data_.sequence_index_];
            is_mode_changed_ = false;
            if (is_unit_test_) {
                Serial.println("==============================");
                Serial.println("| START DEBUG BUTTON INTERRUPT");
                Serial.println("==============================");

                Serial.print("| _sequence_index              : ");
                Serial.print("| ");
                Serial.print(channel_data_.sequence_index_);
                Serial.println();

                Serial.print("| total_task_sequence_         : ");
                Serial.print("| ");
                Serial.print(total_task_sequence_ + 1);
                Serial.println();

                Serial.println("| END DEBUG BUTTON INTERRUPT");
                Serial.println("==============================");
            }
        }
        (this->*temp_mode_)();

    }

    void BaseChannel::setStateHigh(int index, ...) {
        for (int i = 0; i < config_data_ptr_->header.pin_size_; i++) {
            set(config_data_ptr_->header.pin_ptr_[i], LOW);
        }
        va_list args;
        va_start(args, index);
        int currentIndex = index;
        while (currentIndex != -1) {
            set(config_data_ptr_->header.pin_ptr_[currentIndex], HIGH);
            currentIndex = va_arg(args, int);
        }
        va_end(args);
    }

    void BaseChannel::setStateLow(int index, ...) {
        for (int i = 0; i < config_data_ptr_->header.pin_size_; i++) {
            set(config_data_ptr_->header.pin_ptr_[i], LOW);
        }
        va_list args;
        va_start(args, index);
        int currentIndex = index;
        while (currentIndex != -1) {
            set(config_data_ptr_->header.pin_ptr_[currentIndex], LOW);
            currentIndex = va_arg(args, int);
        }
        va_end(args);
    }

    void BaseChannel::set(uint8_t _pin, uint8_t _state) {
        if (is_mode_changed_) return;
        if (is_using_i2c_config_) {
            int index = ceil((_pin + 1) / 8.0) - 1;
            int pins_mod = _pin % 8;
            if (channel_data_.is_reverse_) expander_io_.io_base_[index]->digitalWrite(pins_mod, !_state);
            else expander_io_.io_base_[index]->digitalWrite(pins_mod, _state);
        } else {
            if (channel_data_.is_reverse_) digitalWrite(_pin, !_state);
            else digitalWrite(_pin, _state);
        }
    }

    void BaseChannel::sleep(uint32_t _time) {
        if (is_mode_changed_) return;
        delay(_time);
    }

    void BaseChannel::forceOff() {
        if (is_using_i2c_config_) {
            for (int i = 0; i < config_data_ptr_->header.io_size_; ++i) {
                for (int j = 0; j < 8; ++j) {
                    if (channel_data_.is_reverse_) expander_io_.io_base_[i]->digitalWrite(j, HIGH);
                    else expander_io_.io_base_[i]->digitalWrite(j, LOW);
                }
            }
        } else {
            for (int i = 0; i < config_data_ptr_->header.pin_size_; ++i) {
                if (channel_data_.is_reverse_) digitalWrite(config_data_ptr_->header.pin_ptr_[i], HIGH);
                else digitalWrite(config_data_ptr_->header.pin_ptr_[i], LOW);
            }
        }
    }

    void BaseChannel::off() {
        for (int i = 0; i < config_data_ptr_->header.pin_size_; i++) {
            set(config_data_ptr_->header.pin_ptr_[i], LOW);
        }
    }

    void BaseChannel::on() {
        for (int i = 0; i < config_data_ptr_->header.pin_size_; i++) {
            set(config_data_ptr_->header.pin_ptr_[i], HIGH);
        }
    }

    bool BaseChannel::isReady() {
        return is_initialize_;
    }

    bool BaseChannel::addIoExpander(IOExpander *_io_expander) {
        IOExpander **newIoBase = (IOExpander **) realloc(expander_io_.io_base_,
                                                         (expander_io_.io_length + 1) * sizeof(IOExpander *));
        if (newIoBase == nullptr) {
            return false;
        }

        expander_io_.io_base_ = newIoBase;
        expander_io_.io_base_[expander_io_.io_length] = _io_expander;
        expander_io_.io_length++;

        return true;
    }

    bool BaseChannel::beginIoExpander() {
        if (is_unit_test_) {
            Serial.print("| INITIALIZE IO EXPANDER");
            Serial.println();

            Serial.print("| i2c_address_ available       : ");
            for (int i = 0; i < MAXNUM_IO_EXPANDER; ++i) {
                Serial.print("| 0x000");
                Serial.print(config_data_ptr_->table.i2c_address_[i], HEX);
            }
            Serial.println();

            Serial.print("| io_size_                     : ");
            Serial.print("| ");
            Serial.print(config_data_ptr_->header.io_size_);
            Serial.println();

        }

        for (int i = 0; i < config_data_ptr_->header.io_size_; ++i) {
            if (!addIoExpander(new IOExpander(config_data_ptr_->table.i2c_address_[i]))) {
                if (is_unit_test_) {
                    Serial.print("| Failed Add Io Expander at address: 0x000");
                    Serial.print(config_data_ptr_->table.i2c_address_[i], HEX);
                    Serial.println();
                }
            }
        }
        bool check_io_expander = false;
        for (int i = 0; i < expander_io_.io_length; i++) {
            for (int j = 0; j < MAXNUM_IO_EXPANDER; j++) {
                expander_io_.io_base_[i]->pinMode(j, OUTPUT);
            }
            check_io_expander = true;
            if (!expander_io_.io_base_[i]->begin()) {
                if (is_unit_test_) {
                    Serial.print("| [ERROR] Failed Add Io Expander at address: 0x000");
                    Serial.print(config_data_ptr_->table.i2c_address_[i], HEX);
                    Serial.println();
                }
                check_io_expander = false;
            } else {
                if (is_unit_test_) {
                    Serial.print("| [SUCCESS] Success Add Io Expander at address: 0x000");
                    Serial.print(config_data_ptr_->table.i2c_address_[i], HEX);
                    Serial.println();
                }
            }
            for (int j = 0; j < MAXNUM_IO_EXPANDER; j++) {
                expander_io_.io_base_[i]->digitalWrite(j, HIGH);
            }
        }
        if (!check_io_expander) {
            if (is_unit_test_) {
                Serial.println("| [ERROR] Init Io Expander Error ");
            }
            return false;
        }
        if (is_unit_test_) {
            Serial.println("| [SUCCESS] Init Io Expander All Success ");
        }
        return true;
    }

    void BaseChannel::setFunction() {
        total_mode_[0] = &BaseChannel::off;
        total_mode_[1] = &BaseChannel::on;
        total_mode_[2] = &BaseChannel::taskSequence0;
        total_mode_[3] = &BaseChannel::taskSequence1;
        total_mode_[4] = &BaseChannel::taskSequence2;
        total_mode_[5] = &BaseChannel::taskSequence3;
        total_mode_[6] = &BaseChannel::taskSequence4;
        total_mode_[7] = &BaseChannel::taskSequence5;
        total_mode_[8] = &BaseChannel::taskSequence6;
        total_mode_[9] = &BaseChannel::taskSequence7;
        total_mode_[10] = &BaseChannel::taskSequence8;
        total_mode_[11] = &BaseChannel::taskSequence9;
    }

    void BaseChannel::debug() {
        if (!is_unit_test_) return;
        Serial.println("==============================");
        Serial.println("| START DEBUG BASE CHANNEL");
        Serial.println("==============================");

        Serial.print("| delay_time_                  : ");
        Serial.print("| ");
        Serial.print(channel_data_.delay_time_);
        Serial.println();

        Serial.print("| sequence_index_              : ");
        Serial.print("| ");
        Serial.print(channel_data_.sequence_index_);
        Serial.println();

        Serial.print("| is_reverse_                  : ");
        Serial.print("| ");
        Serial.print(channel_data_.is_reverse_);
        Serial.println();

        Serial.print("| config_data_ptr_             : ");
        Serial.print("| ");
        Serial.print((config_data_ptr_ != nullptr) ? "true" : "false");
        Serial.println();

        Serial.print("| button_interrupt_ptr_        : ");
        Serial.print("| ");
        Serial.print((button_interrupt_ptr_ != nullptr) ? "true" : "false");
        Serial.println();

        Serial.print("| is_initialize_               : ");
        Serial.print("| ");
        Serial.print(is_initialize_);
        Serial.println();

        Serial.println("| END DEBUG BASE CHANNEL");
        Serial.println("==============================");
    }
}
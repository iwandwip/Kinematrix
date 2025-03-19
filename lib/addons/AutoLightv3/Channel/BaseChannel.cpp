/*
 *  BaseChannel.cpp
 *
 *  Kastara Electronics Embedded Development
 *  Created on: 2023. 4. 3
 *  Modified for 4 button support
 */

#include "BaseChannel.h"

namespace AutoLight {
    BaseChannel::BaseChannel(bool _enable_unit_test)
            : is_unit_test_(_enable_unit_test) {
        channel_data_.is_on_ = false;
        channel_data_.last_active_sequence_ = 1; // Default to mode 1 (on) as last active
        setTaskSequenceFunction();
    }

    BaseChannel::~BaseChannel() {
        // Removed free(this) to prevent potential issues
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
        if (_time < 30) {
            channel_data_.delay_time_ = 30;
        } else {
            channel_data_.delay_time_ = _time;
        }
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

    volatile uint32_t BaseChannel::getSequenceIndex() {
        return channel_data_.sequence_index_;
    }

    volatile uint32_t BaseChannel::getDelayTime() {
        return channel_data_.delay_time_;
    }

    ChannelData BaseChannel::getChannelData() {
        return channel_data_;
    }

    // Legacy method - kept for compatibility
    void BaseChannel::changeMode() {
        channel_data_.is_mode_changed_ = true;
    }

    // New button functions
    void BaseChannel::nextMode() {
        if (!channel_data_.is_on_) return; // Only work when a system is on

        channel_data_.is_mode_changed_ = true;
        channel_data_.sequence_index_++; // Will be properly handled in runAutoLight

        if (channel_data_.sequence_index_ > (total_task_sequence_)) {
            channel_data_.sequence_index_ = 1; // Cycle back to mode 1 (skip off mode)
        }
    }

    void BaseChannel::previousMode() {
        if (!channel_data_.is_on_) return; // Only work when a system is on

        channel_data_.is_mode_changed_ = true;

        if (channel_data_.sequence_index_ <= 1) {
            channel_data_.sequence_index_ = total_task_sequence_; // Cycle to the highest mode
        } else {
            channel_data_.sequence_index_--;
        }
    }

    void BaseChannel::onMode() {
        if (channel_data_.is_on_) return; // Already on, do nothing

        channel_data_.is_on_ = true;
        channel_data_.is_mode_changed_ = true;

        // Restore the last active sequence
        if (channel_data_.last_active_sequence_ < 1) {
            channel_data_.sequence_index_ = 1; // Default to mode 1 if no last active mode
        } else {
            channel_data_.sequence_index_ = channel_data_.last_active_sequence_;
        }
    }

    void BaseChannel::offMode() {
        if (!channel_data_.is_on_) return; // Already off, do nothing

        // Store the current sequence for later restoration
        if (channel_data_.sequence_index_ > 0) {
            channel_data_.last_active_sequence_ = channel_data_.sequence_index_;
        }

        channel_data_.is_on_ = false;
        channel_data_.is_mode_changed_ = true;
        channel_data_.sequence_index_ = 0; // Mode 0 is off
    }

    volatile bool BaseChannel::isOn() {
        return channel_data_.is_on_;
    }

    void BaseChannel::changeModeApp(uint32_t num) {
        channel_data_.is_mode_changed_apps_ = true;
        channel_data_.sequence_index_apps_ = num;

        // If turning on via app
        if (num > 0 && !channel_data_.is_on_) {
            channel_data_.is_on_ = true;
        }
            // If turning off via app
        else if (num == 0 && channel_data_.is_on_) {
            channel_data_.is_on_ = false;
            // Store the current sequence for later restoration if not already going to off mode
            if (channel_data_.sequence_index_ > 0) {
                channel_data_.last_active_sequence_ = channel_data_.sequence_index_;
            }
        }
    }

    volatile bool BaseChannel::isChangeMode() {
        return channel_data_.is_mode_changed_ || channel_data_.is_mode_changed_apps_;
    }

    void BaseChannel::runAutoLight(void (*_callbackRun)(uint32_t sequence)) {
        if (isChangeMode()) {
            forceOff();

            // This will be handled by the specific mode functions now
            // Don't auto-increment when coming from on/off methods
            /*channel_data_.sequence_index_++;
            if (channel_data_.sequence_index_ > (total_task_sequence_)) {
                channel_data_.sequence_index_ = 0;
            }*/

            if (channel_data_.is_mode_changed_apps_) {
                if (channel_data_.sequence_index_apps_ > total_task_sequence_) {
                    channel_data_.sequence_index_apps_ = total_task_sequence_;
                }
                channel_data_.sequence_index_ = channel_data_.sequence_index_apps_;

                // Update on/off state based on the selected mode
                channel_data_.is_on_ = (channel_data_.sequence_index_ > 0);
            }

            if (_callbackRun != nullptr) {
                _callbackRun(channel_data_.sequence_index_);
            }

            temp_mode_ = total_mode_[channel_data_.sequence_index_];

            channel_data_.is_mode_changed_ = false;
            channel_data_.is_mode_changed_apps_ = false;

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
                Serial.print(total_task_sequence_);
                Serial.println();

                Serial.print("| is_on_                       : ");
                Serial.print("| ");
                Serial.print(channel_data_.is_on_ ? "true" : "false");
                Serial.println();

                Serial.print("| last_active_sequence_        : ");
                Serial.print("| ");
                Serial.print(channel_data_.last_active_sequence_);
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
        if (isChangeMode()) return;
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
        if (isChangeMode()) return;
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

    int BaseChannel::findMax(int num, ...) {
        va_list args;
        va_start(args, num);
        int maxVal = num;
        while (true) {
            int next = va_arg(args, int);
            if (next == -1) break;
            if (next > maxVal) {
                maxVal = next;
            }
        }
        va_end(args);
        return maxVal;
    }

    int BaseChannel::shiftArrayAccordingToOrder(const int patterns[][8], int patternSize, uint8_t *mainArray, int mainArraySize, int *arr, int &lastIndex, int targetIndex, bool reverse) {
        int patternIndex = (24 - mainArraySize) / 2;
        if (patternIndex < 0 || patternIndex >= patternSize) return 0;

        int subArraySize = patterns[patternIndex][targetIndex];
        if (subArraySize == 0) return 0;

        int currentIndex = 0;
        for (int i = 0; i < targetIndex; i++) {
            currentIndex += patterns[patternIndex][i];
        }

        for (int i = 0; i < subArraySize; i++) {
            if (!reverse) {
                arr[lastIndex + i] = mainArray[currentIndex + i];
            } else {
                arr[lastIndex + i] = mainArray[currentIndex + subArraySize - 1 - i];
            }
        }

        lastIndex += subArraySize;
        return subArraySize;
    }

    int BaseChannel::shiftArrayAccordingToPosition(int *mainArray, int mainArraySize, int *arr, int &lastIndex, int targetIndex) {
        const int patterns[9][8] = {
                {3, 4, 4, 1, 1, 4, 4, 3},  // For size 24
                {3, 3, 4, 1, 1, 4, 3, 3},  // For size 22
                {3, 3, 3, 1, 1, 3, 3, 3},  // For size 20
                {3, 2, 3, 1, 1, 3, 2, 3},  // For size 18
                {3, 2, 2, 1, 1, 2, 2, 3},  // For size 16
                {3, 1, 2, 1, 1, 1, 2, 3},  // For size 14
                {3, 1, 1, 1, 1, 1, 1, 3},  // For size 12
                {3, 0, 1, 1, 1, 1, 0, 3},  // For size 10
                {3, 0, 0, 1, 1, 0, 0, 3}   // For size 8
        };

        int patternIndex = (24 - mainArraySize) / 2;
        if (patternIndex < 0 || patternIndex >= 9) return 0;

        int subArraySize = patterns[patternIndex][targetIndex];
        if (subArraySize == 0) return 0;

        int currentIndex = 0;
        for (int i = 0; i < targetIndex; i++) {
            currentIndex += patterns[patternIndex][i];
        }

        for (int i = 0; i < subArraySize; i++) {
            arr[lastIndex + i] = mainArray[currentIndex + i];
        }

        lastIndex += subArraySize;
        return subArraySize;
    }

    void BaseChannel::resetArray(int *mainArray, int &arrSize, int &arrLastIndex) {
        for (int i = 0; i < arrSize; i++) {
            mainArray[i] = 0;
        }
        arrSize = 0;
        arrLastIndex = 0;
    }

    void BaseChannel::splitArrayDynamically(int *mainArray, int mainArraySize) {
        int currentIndex = 0;
        // Define patterns for different sizes
        const int patterns[9][8] = {
                {3, 4, 4, 1, 1, 4, 4, 3},  // For size 24
                {3, 3, 4, 1, 1, 4, 3, 3},  // For size 22
                {3, 3, 3, 1, 1, 3, 3, 3},  // For size 20
                {3, 2, 3, 1, 1, 3, 2, 3},  // For size 18
                {3, 2, 2, 1, 1, 2, 2, 3},  // For size 16
                {3, 1, 2, 1, 1, 1, 2, 3},  // For size 14
                {3, 1, 1, 1, 1, 1, 1, 3},  // For size 12
                {3, 0, 1, 1, 1, 1, 0, 3},  // For size 10
                {3, 0, 0, 1, 1, 0, 0, 3}   // For size 8
        };

        int patternIndex = (24 - mainArraySize) / 2;
        if (patternIndex < 0 || patternIndex >= 9) return;

        Serial.print("| patternIndex: ");
        Serial.println(patternIndex);

        const int *subArraySizes = patterns[patternIndex];

        for (int i = 0; i < 8; i++) {
            int subArraySize = subArraySizes[i];
            Serial.print("Sub-array ");
            Serial.print(i + 1);
            Serial.print(": ");

            for (int j = 0; j < subArraySize; j++) {
                if (currentIndex < mainArraySize) {
                    Serial.print(mainArray[currentIndex]);
                    Serial.print(" ");
                    currentIndex++;
                }
            }
            Serial.println();
        }
    }

    void BaseChannel::setTaskSequenceFunction() {
        total_mode_[0] = &BaseChannel::off;
        total_mode_[1] = &BaseChannel::on;
        total_mode_[2] = &BaseChannel::taskSequence2;
        total_mode_[3] = &BaseChannel::taskSequence3;
        total_mode_[4] = &BaseChannel::taskSequence4;
        total_mode_[5] = &BaseChannel::taskSequence5;
        total_mode_[6] = &BaseChannel::taskSequence6;
        total_mode_[7] = &BaseChannel::taskSequence7;
        total_mode_[8] = &BaseChannel::taskSequence8;
        total_mode_[9] = &BaseChannel::taskSequence9;
        total_mode_[10] = &BaseChannel::taskSequence10;
        total_mode_[11] = &BaseChannel::taskSequence11;
        total_mode_[12] = &BaseChannel::taskSequence12;
        total_mode_[13] = &BaseChannel::taskSequence13;
        total_mode_[14] = &BaseChannel::taskSequence14;
        total_mode_[15] = &BaseChannel::taskSequence15;
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

        Serial.print("| is_on_                       : ");
        Serial.print("| ");
        Serial.print(channel_data_.is_on_ ? "true" : "false");
        Serial.println();

        Serial.print("| last_active_sequence_        : ");
        Serial.print("| ");
        Serial.print(channel_data_.last_active_sequence_);
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
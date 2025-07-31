#include "BaseChannel.h"
#include "../Web/WebManager.h"

namespace AutoLight {
    BaseChannel::BaseChannel(bool _enable_unit_test)
            : is_unit_test_(_enable_unit_test), web_manager_(nullptr) {
        channel_data_.is_on_ = false;
        channel_data_.last_active_sequence_ = 1;
        current_button_mode_ = BUTTON_MODE_4BUTTON;
        button_config_.button_count = 4;
        button_config_.mode = BUTTON_MODE_4BUTTON;
        button_config_.auto_cycle_enabled = false;
        button_config_.auto_cycle_timeout = 30000;
        for (int i = 0; i < 4; i++) {
            button_config_.custom_handlers[i] = nullptr;
        }

        initializeDefaultMapping();

        setTaskSequenceFunction();
    }

    BaseChannel::~BaseChannel() {
        if (web_manager_ != nullptr) {
            delete web_manager_;
            web_manager_ = nullptr;
        }
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
        if (!sequence_mapper_.is_mapping_enabled) {
            return channel_data_.sequence_index_;
        }

        if (channel_data_.is_mode_changed_apps_) {
            return channel_data_.sequence_index_apps_;
        } else {
            uint8_t api_index = mapActualSequenceToApiIndex(channel_data_.sequence_index_);
            return (api_index == 0xFF) ? channel_data_.sequence_index_ : api_index;
        }
    }

    volatile uint32_t BaseChannel::getDelayTime() {
        return channel_data_.delay_time_;
    }

    ChannelData BaseChannel::getChannelData() {
        return channel_data_;
    }

    void BaseChannel::changeMode() {
        channel_data_.is_mode_changed_ = true;
    }

    void BaseChannel::nextMode() {
        if (!channel_data_.is_on_) {
            return;
        }

        if (!sequence_mapper_.is_mapping_enabled) {
            channel_data_.is_mode_changed_ = true;
            channel_data_.sequence_index_++;

            if (channel_data_.sequence_index_ > (total_task_sequence_)) {
                channel_data_.sequence_index_ = 1;
            }

            return;
        }

        uint8_t current_api_index = mapActualSequenceToApiIndex(channel_data_.sequence_index_);
        if (current_api_index == 0xFF) {
            uint8_t first_non_off = (sequence_mapper_.active_sequences[0] == 0 && sequence_mapper_.total_active > 1) ? 1 : 0;
            changeModeApp(first_non_off);
            return;
        }

        uint8_t next_api_index = current_api_index + 1;

        if (next_api_index >= sequence_mapper_.total_active) {
            if (sequence_mapper_.active_sequences[0] == 0 && sequence_mapper_.total_active > 1) {
                next_api_index = 1;
            } else {
                next_api_index = 0;
            }
        }

        if (sequence_mapper_.active_sequences[next_api_index] == 0 && sequence_mapper_.total_active > 1) {
            next_api_index = (next_api_index + 1) % sequence_mapper_.total_active;
        }

        changeModeApp(next_api_index);
    }

    void BaseChannel::previousMode() {
        if (!channel_data_.is_on_) {
            return;
        }

        if (!sequence_mapper_.is_mapping_enabled) {
            channel_data_.is_mode_changed_ = true;

            if (channel_data_.sequence_index_ <= 1) {
                channel_data_.sequence_index_ = total_task_sequence_;
            } else {
                channel_data_.sequence_index_--;
            }

            return;
        }

        uint8_t current_api_index = mapActualSequenceToApiIndex(channel_data_.sequence_index_);
        if (current_api_index == 0xFF) {
            uint8_t last_index = sequence_mapper_.total_active - 1;
            if (sequence_mapper_.active_sequences[last_index] == 0 && sequence_mapper_.total_active > 1) {
                last_index = sequence_mapper_.total_active - 2;
            }
            changeModeApp(last_index);
            return;
        }

        uint8_t prev_api_index;

        if (current_api_index == 0) {
            prev_api_index = sequence_mapper_.total_active - 1;
            if (sequence_mapper_.active_sequences[prev_api_index] == 0 && sequence_mapper_.total_active > 1) {
                prev_api_index = sequence_mapper_.total_active - 2;
            }
        } else {
            prev_api_index = current_api_index - 1;
        }

        if (sequence_mapper_.active_sequences[prev_api_index] == 0 && sequence_mapper_.total_active > 1) {
            if (prev_api_index == 0) {
                prev_api_index = sequence_mapper_.total_active - 1;
            } else {
                prev_api_index--;
            }
        }

        changeModeApp(prev_api_index);
    }

    void BaseChannel::onMode() {
        if (channel_data_.is_on_) {
            return;
        }

        channel_data_.is_on_ = true;
        channel_data_.is_mode_changed_ = true;

        if (channel_data_.last_active_sequence_ < 1) {
            channel_data_.sequence_index_ = 1;
        } else {
            channel_data_.sequence_index_ = channel_data_.last_active_sequence_;
        }
    }

    void BaseChannel::offMode() {
        if (!channel_data_.is_on_) {
            return;
        }

        if (channel_data_.sequence_index_ > 0) {
            channel_data_.last_active_sequence_ = channel_data_.sequence_index_;
        }

        channel_data_.is_on_ = false;
        channel_data_.is_mode_changed_ = true;
        channel_data_.sequence_index_ = 0;
    }

    volatile bool BaseChannel::isOn() {
        return channel_data_.is_on_;
    }

    void BaseChannel::singleButtonCycle() {
        if (!sequence_mapper_.is_mapping_enabled) {
            channel_data_.is_mode_changed_ = true;

            if (!channel_data_.is_on_) {
                channel_data_.sequence_index_ = 1;
                channel_data_.is_on_ = true;
            } else {
                channel_data_.sequence_index_++;

                if (channel_data_.sequence_index_ > total_task_sequence_) {
                    channel_data_.sequence_index_ = 0;
                    channel_data_.is_on_ = false;
                } else {
                    channel_data_.last_active_sequence_ = channel_data_.sequence_index_;
                }
            }

            temp_mode_ = total_mode_[channel_data_.sequence_index_];
            return;
        }

        if (!channel_data_.is_on_) {
            uint8_t first_api_index = 0;
            if (sequence_mapper_.active_sequences[0] == 0 && sequence_mapper_.total_active > 1) {
                first_api_index = 1;
            }
            changeModeApp(first_api_index);
        } else {
            uint8_t current_api_index = mapActualSequenceToApiIndex(channel_data_.sequence_index_);
            if (current_api_index == 0xFF) {
                changeModeApp(0);
                return;
            }

            uint8_t next_api_index = current_api_index + 1;
            if (next_api_index >= sequence_mapper_.total_active) {
                for (uint8_t i = 0; i < sequence_mapper_.total_active; i++) {
                    if (sequence_mapper_.active_sequences[i] == 0) {
                        changeModeApp(i);
                        return;
                    }
                }
                channel_data_.is_on_ = false;
                channel_data_.sequence_index_ = 0;
                channel_data_.is_mode_changed_ = true;
                temp_mode_ = total_mode_[0];
            } else {
                changeModeApp(next_api_index);
            }
        }
    }

    void BaseChannel::toggleOnOff() {
        if (channel_data_.is_on_) {
            offMode();
        } else {
            onMode();
        }
    }

    void BaseChannel::setButtonMode(button_mode_t mode) {
        current_button_mode_ = mode;
        button_config_.mode = mode;

        switch (mode) {
            case BUTTON_MODE_1BUTTON:
                button_config_.button_count = 1;
                break;
            case BUTTON_MODE_2BUTTON:
                button_config_.button_count = 2;
                break;
            case BUTTON_MODE_3BUTTON:
                button_config_.button_count = 3;
                break;
            case BUTTON_MODE_4BUTTON:
            default:
                button_config_.button_count = 4;
                break;
        }
    }

    void BaseChannel::setButtonConfig(ButtonConfig *config) {
        button_config_ = *config;
        current_button_mode_ = config->mode;
    }

    void BaseChannel::smartButtonPress(uint8_t button_index) {
        switch (current_button_mode_) {
            case BUTTON_MODE_1BUTTON:
                singleButtonCycle();
                break;

            case BUTTON_MODE_2BUTTON:
                if (button_index == 0) {
                    toggleOnOff();
                } else {
                    if (channel_data_.is_on_) nextMode();
                }
                break;

            case BUTTON_MODE_3BUTTON:
                if (button_index == 0) {
                    onMode();
                } else if (button_index == 1) {
                    offMode();
                } else {
                    if (channel_data_.is_on_) nextMode();
                }
                break;

            case BUTTON_MODE_4BUTTON:
                if (button_index == 0) {
                    onMode();
                } else if (button_index == 1) {
                    offMode();
                } else if (button_index == 2) {
                    if (channel_data_.is_on_) nextMode();
                } else if (button_index == 3) {
                    if (channel_data_.is_on_) previousMode();
                }
                break;

            case BUTTON_MODE_CUSTOM:
                if (button_config_.custom_handlers[button_index]) {
                    button_config_.custom_handlers[button_index]();
                }
                break;
        }
    }

    void BaseChannel::executeButtonAction(uint8_t button_index) {
        if (button_index >= button_config_.button_count) return;
        smartButtonPress(button_index);
    }

    void BaseChannel::changeModeApp(uint32_t api_index) {
        if (!sequence_mapper_.is_mapping_enabled) {
            channel_data_.is_mode_changed_apps_ = true;
            channel_data_.sequence_index_apps_ = api_index;

            if (api_index > 0 && !channel_data_.is_on_) {
                channel_data_.is_on_ = true;
            } else if (api_index == 0 && channel_data_.is_on_) {
                channel_data_.is_on_ = false;
                if (channel_data_.sequence_index_ > 0) {
                    channel_data_.last_active_sequence_ = channel_data_.sequence_index_;
                }
            }
            return;
        }

        if (!isApiIndexValid(api_index)) {
            Serial.printf("[MAPPING] Invalid API index %d, ignoring\n", api_index);
            return;
        }

        uint8_t actual_sequence = mapApiIndexToActualSequence(api_index);

        channel_data_.is_mode_changed_apps_ = true;
        channel_data_.sequence_index_apps_ = api_index;

        if (actual_sequence == 0) {
            channel_data_.is_on_ = false;
            if (channel_data_.sequence_index_ > 0) {
                channel_data_.last_active_sequence_ = channel_data_.sequence_index_;
            }
        } else {
            channel_data_.is_on_ = true;
            channel_data_.last_active_sequence_ = actual_sequence;
        }

        Serial.printf("[MAPPING] API index %d \u2192 Actual sequence %d\n", api_index, actual_sequence);
    }

    volatile bool BaseChannel::isChangeMode() {
        return channel_data_.is_mode_changed_ || channel_data_.is_mode_changed_apps_;
    }

    void BaseChannel::runAutoLight(void (*_callbackRun)(uint32_t sequence)) {
        if (isChangeMode()) {
            forceOff();

            if (channel_data_.is_mode_changed_apps_) {
                if (!sequence_mapper_.is_mapping_enabled) {
                    if (channel_data_.sequence_index_apps_ > total_task_sequence_) {
                        channel_data_.sequence_index_apps_ = total_task_sequence_;
                    }
                    channel_data_.sequence_index_ = channel_data_.sequence_index_apps_;
                    channel_data_.is_on_ = (channel_data_.sequence_index_ > 0);
                } else {
                    uint8_t actual_sequence = mapApiIndexToActualSequence(channel_data_.sequence_index_apps_);
                    if (actual_sequence != 0xFF) {
                        channel_data_.sequence_index_ = actual_sequence;
                    }
                }
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
            for (int i = 0; i < Constants::MAXNUM_IO_EXPANDER; ++i) {
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
            for (int j = 0; j < Constants::MAXNUM_IO_EXPANDER; j++) {
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
            for (int j = 0; j < Constants::MAXNUM_IO_EXPANDER; j++) {
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
                {3, 4, 4, 1, 1, 4, 4, 3},
                {3, 3, 4, 1, 1, 4, 3, 3},
                {3, 3, 3, 1, 1, 3, 3, 3},
                {3, 2, 3, 1, 1, 3, 2, 3},
                {3, 2, 2, 1, 1, 2, 2, 3},
                {3, 1, 2, 1, 1, 1, 2, 3},
                {3, 1, 1, 1, 1, 1, 1, 3},
                {3, 0, 1, 1, 1, 1, 0, 3},
                {3, 0, 0, 1, 1, 0, 0, 3}
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
        const int patterns[9][8] = {
                {3, 4, 4, 1, 1, 4, 4, 3},
                {3, 3, 4, 1, 1, 4, 3, 3},
                {3, 3, 3, 1, 1, 3, 3, 3},
                {3, 2, 3, 1, 1, 3, 2, 3},
                {3, 2, 2, 1, 1, 2, 2, 3},
                {3, 1, 2, 1, 1, 1, 2, 3},
                {3, 1, 1, 1, 1, 1, 1, 3},
                {3, 0, 1, 1, 1, 1, 0, 3},
                {3, 0, 0, 1, 1, 0, 0, 3}
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
        total_mode_[2] = &BaseChannel::taskSequence2BlinkAll;
        total_mode_[3] = &BaseChannel::taskSequence3FillTwoPoint;
        total_mode_[4] = &BaseChannel::taskSequence4FillRight;
        total_mode_[5] = &BaseChannel::taskSequence5FillIn;
        total_mode_[6] = &BaseChannel::taskSequence6BlinkOneByOne;
        total_mode_[7] = &BaseChannel::taskSequence7BlinkTwoFill;
        total_mode_[8] = &BaseChannel::taskSequence8SnakeAndReverse;
        total_mode_[9] = &BaseChannel::taskSequence9Random;
        total_mode_[10] = &BaseChannel::taskSequence10Wave;
        total_mode_[11] = &BaseChannel::taskSequence11Complex;
        total_mode_[12] = &BaseChannel::taskSequence12PatternMatrix;
        total_mode_[13] = &BaseChannel::taskSequence13BlinkPattern;
        total_mode_[14] = &BaseChannel::taskSequence14AdvancedPattern;
        total_mode_[15] = &BaseChannel::taskSequence15AllSequences;
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

    bool BaseChannel::getLEDState(uint8_t led_index) {
        if (!config_data_ptr_ || led_index >= config_data_ptr_->header.channel_) {
            return false;
        }

        uint8_t actual_pin = config_data_ptr_->header.pin_ptr_[led_index];

        if (is_using_i2c_config_) {
            int pcf_index = ceil((actual_pin + 1) / 8.0) - 1;
            int pin_index = actual_pin % 8;

            if (pcf_index >= expander_io_.io_length || !expander_io_.io_base_[pcf_index]) {
                return false;
            }

            bool pin_state = expander_io_.io_base_[pcf_index]->digitalRead(pin_index) == HIGH;

            if (channel_data_.is_reverse_) {
                return !pin_state;
            } else {
                return pin_state;
            }
        } else {
            bool pin_state = digitalRead(actual_pin) == HIGH;
            if (channel_data_.is_reverse_) {
                return !pin_state;
            } else {
                return pin_state;
            }
        }
    }

    void BaseChannel::initializeDefaultMapping() {
        sequence_mapper_.is_mapping_enabled = false;
        sequence_mapper_.total_active = Constants::MAXNUM_TOTAL_TASK_SEQUENCE;

        for (uint8_t i = 0; i < Constants::MAXNUM_TOTAL_TASK_SEQUENCE; i++) {
            sequence_mapper_.active_sequences[i] = i;
        }
    }

    void BaseChannel::enableSequenceMapping(bool enable) {
        sequence_mapper_.is_mapping_enabled = enable;
        if (enable) {
            Serial.println("[MAPPING] Sequence mapping enabled");
        } else {
            Serial.println("[MAPPING] Sequence mapping disabled (default behavior)");
        }
    }

    void BaseChannel::setActiveSequences(uint8_t *sequences, uint8_t count) {
        sequence_mapper_.total_active = (count > Constants::MAXNUM_TOTAL_TASK_SEQUENCE) ? Constants::MAXNUM_TOTAL_TASK_SEQUENCE : count;

        for (uint8_t i = 0; i < sequence_mapper_.total_active; i++) {
            if (sequences[i] < Constants::MAXNUM_TOTAL_TASK_SEQUENCE) {
                sequence_mapper_.active_sequences[i] = sequences[i];
            } else {
                Serial.printf("[MAPPING] Invalid sequence %d, using 0\n", sequences[i]);
                sequence_mapper_.active_sequences[i] = 0;
            }
        }

        for (uint8_t i = sequence_mapper_.total_active; i < Constants::MAXNUM_TOTAL_TASK_SEQUENCE; i++) {
            sequence_mapper_.active_sequences[i] = 0;
        }

        Serial.printf("[MAPPING] Active sequences configured: %d total\n", sequence_mapper_.total_active);
        printSequenceMapping();
    }

    void BaseChannel::setActiveSequences(uint8_t count, ...) {
        uint8_t sequences[Constants::MAXNUM_TOTAL_TASK_SEQUENCE];

        va_list args;
        va_start(args, count);

        for (uint8_t i = 0; i < count && i < Constants::MAXNUM_TOTAL_TASK_SEQUENCE; i++) {
            sequences[i] = va_arg(args, int);
        }

        va_end(args);

        setActiveSequences(sequences, count);
    }

    void BaseChannel::reorderActiveSequences(uint8_t *new_order, uint8_t count) {
        if (count != sequence_mapper_.total_active) {
            Serial.println("[MAPPING] Reorder failed: size mismatch");
            return;
        }

        uint8_t temp_sequences[Constants::MAXNUM_TOTAL_TASK_SEQUENCE];

        for (uint8_t i = 0; i < sequence_mapper_.total_active; i++) {
            temp_sequences[i] = sequence_mapper_.active_sequences[i];
        }

        for (uint8_t i = 0; i < sequence_mapper_.total_active; i++) {
            if (new_order[i] < sequence_mapper_.total_active) {
                sequence_mapper_.active_sequences[i] = temp_sequences[new_order[i]];
            }
        }

        Serial.println("[MAPPING] Sequences reordered");
        printSequenceMapping();
    }

    uint8_t BaseChannel::mapApiIndexToActualSequence(uint8_t api_index) {
        if (api_index >= sequence_mapper_.total_active) {
            return 0xFF;
        }
        return sequence_mapper_.active_sequences[api_index];
    }

    uint8_t BaseChannel::mapActualSequenceToApiIndex(uint8_t actual_sequence) {
        for (uint8_t i = 0; i < sequence_mapper_.total_active; i++) {
            if (sequence_mapper_.active_sequences[i] == actual_sequence) {
                return i;
            }
        }
        return 0xFF;
    }

    bool BaseChannel::isApiIndexValid(uint8_t api_index) {
        return api_index < sequence_mapper_.total_active;
    }

    void BaseChannel::printSequenceMapping() {
        if (!sequence_mapper_.is_mapping_enabled) {
            Serial.println("[MAPPING] Mapping disabled - using default behavior");
            return;
        }

        Serial.println("[MAPPING] Current sequence mapping:");
        for (uint8_t i = 0; i < sequence_mapper_.total_active; i++) {
            Serial.printf("  API Index %d \u2192 Sequence %d\n", i, sequence_mapper_.active_sequences[i]);
        }
        Serial.printf("[MAPPING] Total active sequences: %d\n", sequence_mapper_.total_active);
    }

    String BaseChannel::getActiveMappingString() {
        String result = "";
        for (uint8_t i = 0; i < sequence_mapper_.total_active; i++) {
            if (i > 0) result += ",";
            result += String(i) + "\u2192" + String(sequence_mapper_.active_sequences[i]);
        }
        return result;
    }

    void BaseChannel::enableWebServer(const char *device_name, bool auto_task) {
        if (web_manager_ == nullptr) {
            BaseConfig *config = nullptr;
            if (config_data_ptr_ != nullptr) {
                config = new BaseConfig();
                config->setChannel(config_data_ptr_->header.channel_);
                config->setVersion(config_data_ptr_->header.version_);
            }

            web_manager_ = new WebManager(this, config);
            if (config != nullptr) {
                web_manager_->setConfigOwnership(true);
            }
            web_manager_->enableWebServer(device_name, auto_task);
        }
    }

    void BaseChannel::enableWebServerManual(const char *device_name) {
        enableWebServer(device_name, false);
    }
}
/*
 *  BaseChannelSequence.cpp - AutoLightv4 Sequence Implementation (V3 Compatible)
 *
 *  Kastara Electronics Embedded Development
 *  Created on: 2024. 7. 23
 */

#include "BaseChannel.h"

namespace AutoLight {

    void BaseChannel::taskSequence2() {
        if (!config_ptr_) return;
        uint8_t* pin_ptr = config_ptr_->dynamic_config.channel_map;
        uint8_t pin_size = config_ptr_->getTotalChannels();
        uint32_t delay_time = channel_data_.custom_delay_;
        
        {
            for (int i = 0; i < 2; i++) {
                for (int j = 0; j < 15; ++j) {
                    for (int k = 0; k < pin_size; k++) {
                        set(k, HIGH);
                    }
                    sleep(delay_time);
                    for (int k = 0; k < pin_size; k++) {
                        set(k, LOW);
                    }
                    sleep(delay_time);
                }
                sleep(500);
            }
            clearAll();
        }
        {
            for (int i = 0; i < 4; ++i) {
                for (int j = 0; j < (pin_size / 2); j++) {
                    set(j, HIGH);
                }
                sleep(delay_time);
                for (int j = 0; j < (pin_size / 2); j++) {
                    set(j, LOW);
                }
                sleep(delay_time);
            }
            sleep(50);
            for (int i = 0; i < 4; ++i) {
                for (int j = (pin_size / 2); j < pin_size; j++) {
                    set(j, HIGH);
                }
                sleep(delay_time);
                for (int j = (pin_size / 2); j < pin_size; j++) {
                    set(j, LOW);
                }
                sleep(delay_time);
            }
            sleep(50);
            for (int i = 0; i < 4; ++i) {
                for (int j = 0; j < (pin_size / 2); j++) {
                    set(j, HIGH);
                }
                sleep(delay_time);
                for (int j = 0; j < (pin_size / 2); j++) {
                    set(j, LOW);
                }
                sleep(delay_time);
            }
            sleep(500);
            clearAll();
        }
        {
            for (int i = 0; i < 4; ++i) {
                for (int j = (pin_size / 2); j < pin_size; j++) {
                    set(j, HIGH);
                }
                sleep(delay_time);
                for (int j = (pin_size / 2); j < pin_size; j++) {
                    set(j, LOW);
                }
                sleep(delay_time);
            }
            sleep(50);
            for (int i = 0; i < 4; ++i) {
                for (int j = 0; j < (pin_size / 2); j++) {
                    set(j, HIGH);
                }
                sleep(delay_time);
                for (int j = 0; j < (pin_size / 2); j++) {
                    set(j, LOW);
                }
                sleep(delay_time);
            }
            sleep(50);
            for (int i = 0; i < 4; ++i) {
                for (int j = (pin_size / 2); j < pin_size; j++) {
                    set(j, HIGH);
                }
                sleep(delay_time);
                for (int j = (pin_size / 2); j < pin_size; j++) {
                    set(j, LOW);
                }
                sleep(delay_time);
            }
            clearAll();
            sleep(500);
        }
    }

    void BaseChannel::taskSequence3() {
        if (!config_ptr_) return;
        uint8_t pin_size = config_ptr_->getTotalChannels();
        uint32_t delay_time = channel_data_.custom_delay_;
        
        {
            for (int i = 0; i < pin_size / 2; i += 2) {
                for (int j = 0; j < 8; ++j) {
                    set(i, HIGH);
                    set(i + 1, HIGH);
                    sleep(delay_time);
                    set(i, LOW);
                    set(i + 1, LOW);
                    sleep(delay_time);
                }
                set(i, HIGH);
                set(i + 1, HIGH);
                sleep(300);
                if (i < (pin_size / 2) - 1) {
                    for (int j = 0; j < 8; ++j) {
                        set((pin_size - 1) - i, HIGH);
                        set((pin_size - 1) - (i + 1), HIGH);
                        sleep(delay_time);
                        set((pin_size - 1) - i, LOW);
                        set((pin_size - 1) - (i + 1), LOW);
                        sleep(delay_time);
                    }
                    set((pin_size - 1) - i, HIGH);
                    set((pin_size - 1) - (i + 1), HIGH);
                    sleep(300);
                }
            }
            for (int i = pin_size; i > 0; --i) {
                set(i - 1, LOW);
                sleep(delay_time * 2);
            }
            clearAll();
            sleep(500);
        }
    }

    void BaseChannel::taskSequence4() {
        if (!config_ptr_) return;
        uint8_t pin_size = config_ptr_->getTotalChannels();
        uint32_t delay_time = channel_data_.custom_delay_;
        
        {
            for (int i = pin_size; i > 0; --i) {
                for (int j = 0; j < i; j++) {
                    set(j, HIGH);
                    sleep(delay_time * 2);
                    set(j, LOW);
                }
                set(i - 1, HIGH);
            }
            for (int i = pin_size; i > 0; --i) {
                set(i - 1, LOW);
                sleep(delay_time * 2);
            }
            clearAll();
            sleep(500);
        }
    }

    void BaseChannel::taskSequence5() {
        if (!config_ptr_) return;
        uint8_t pin_size = config_ptr_->getTotalChannels();
        uint32_t delay_time = channel_data_.custom_delay_;
        
        {
            for (int j = 0; j < pin_size / 2; ++j) {
                for (int i = pin_size; i > pin_size / 2 + j; --i) {
                    set(i - 1, HIGH);
                    set(pin_size - i, HIGH);
                    sleep(delay_time * 2);
                    set(i - 1, LOW);
                    set(pin_size - i, LOW);
                }
                for (int i = j; i < pin_size / 2; ++i) {
                    set(i + pin_size / 2, HIGH);
                    set((pin_size / 2 - 1) - i, HIGH);
                    sleep(delay_time * 2);
                    set(i + pin_size / 2, LOW);
                    set((pin_size / 2 - 1) - i, LOW);
                }
                for (int i = pin_size - 1; i > pin_size / 2 + j; --i) {
                    set(i - 1, HIGH);
                    set(pin_size - i, HIGH);
                    sleep(delay_time * 2);
                    set(i - 1, LOW);
                    set(pin_size - i, LOW);
                }
                set(pin_size / 2 + j, HIGH);
                set((pin_size / 2 - 1) - j, HIGH);
            }
            for (int i = 0; i < pin_size / 2; ++i) {
                set(i + pin_size / 2, LOW);
                set((pin_size / 2 - 1) - i, LOW);
                sleep(delay_time * 2);
            }
            clearAll();
            sleep(500);
        }
    }

    void BaseChannel::taskSequence6() {
        if (!config_ptr_) return;
        uint8_t pin_size = config_ptr_->getTotalChannels();
        uint32_t delay_time = channel_data_.custom_delay_;
        
        {
            int halfSize = pin_size / 2;

            for (int i = pin_size; i > 0; --i) {
                int time = map(pin_size - i, 1, pin_size, delay_time + (delay_time / 2), delay_time / 2);
                for (int j = pin_size - 1; j >= pin_size - i; --j) {
                    if (j >= halfSize) {
                        set(halfSize + (pin_size - 1) - j, HIGH);
                        sleep(time);
                        set(halfSize + (pin_size - 1) - j, LOW);
                    } else {
                        set(j, HIGH);
                        sleep(time);
                        set(j, LOW);
                    }
                }
                if (i > halfSize) {
                    set(pin_size - i, HIGH);
                } else {
                    set(pin_size - (halfSize - i) - 1, HIGH);
                }
            }
            sleep(delay_time * 12);

            for (int i = pin_size; i > 0; --i) {
                if (i > halfSize) set(halfSize + (pin_size - i), LOW);
                else set(i - 1, LOW);
                sleep(delay_time);
            }

            for (int j = 0; j < 2; ++j) {
                for (int i = 0; i < pin_size; ++i) {
                    if (i < halfSize) set(i, HIGH);
                    else set(pin_size - (i - halfSize) - 1, HIGH);
                    sleep(delay_time);
                }
                for (int i = pin_size; i > 0; --i) {
                    if (i > halfSize) set(halfSize + (pin_size - i), LOW);
                    else set(i - 1, LOW);
                    sleep(delay_time);
                }
            }

            for (int i = 0; i < pin_size / 2; ++i) {
                set(i, HIGH);
                set(halfSize + i, HIGH);
                sleep(delay_time);
            }
            for (int i = 0; i < pin_size / 2; ++i) {
                set(i, LOW);
                set(halfSize + i, LOW);
                sleep(delay_time);
            }

            for (int j = 0; j < 2; ++j) {
                for (int i = 0; i < pin_size / 2; ++i) {
                    set(halfSize - i - 1, HIGH);
                    set(pin_size - i - 1, HIGH);
                    sleep(delay_time);
                }
                for (int i = 0; i < pin_size / 2; ++i) {
                    set(i, LOW);
                    set(halfSize + i, LOW);
                    sleep(delay_time);
                }
            }

            sleep(delay_time * 18);

            for (int i = 0; i < 2; ++i) {
                setAll(true);
                sleep(delay_time * 3);
                clearAll();
                sleep(delay_time * 3);
            }

            sleep(delay_time * 18);
        }
    }

    void BaseChannel::taskSequence7() {
        if (!config_ptr_) return;
        uint8_t pin_size = config_ptr_->getTotalChannels();
        uint32_t delay_time = channel_data_.custom_delay_;
        
        {
            for (int j = 0; j < pin_size / 2; ++j) {
                for (int i = pin_size / 2; i > j; --i) {
                    for (int k = 0; k < 4; ++k) {
                        set(i - 1, HIGH);
                        set(pin_size - i, HIGH);
                        sleep(delay_time);
                        set(i - 1, LOW);
                        set(pin_size - i, LOW);
                        sleep(delay_time);
                    }
                }
                set(j, HIGH);
                set((pin_size - 1) - j, HIGH);
            }
            clearAll();
            sleep(500);
        }
    }

    void BaseChannel::taskSequence8() {
        if (!config_ptr_) return;
        uint8_t pin_size = config_ptr_->getTotalChannels();
        uint32_t delay_time = channel_data_.custom_delay_;
        
        {
            clearAll();
            for (float k = delay_time * 2; k >= delay_time; k -= delay_time) {
                for (int i = 0; i < pin_size; i++) {
                    set(i, HIGH);
                    sleep(k);
                }
                for (int i = 0; i < pin_size; i++) {
                    set(i, LOW);
                    sleep(k);
                }
                for (int j = pin_size; j > 0; j--) {
                    set(j - 1, HIGH);
                    sleep(k);
                }
                for (int j = pin_size; j > 0; j--) {
                    set(j - 1, LOW);
                    sleep(k);
                }
            }
            sleep(500);
        }
    }

    void BaseChannel::taskSequence9() {
        if (!config_ptr_) return;
        uint8_t pin_size = config_ptr_->getTotalChannels();
        uint32_t delay_time = channel_data_.custom_delay_;
        
        auto setRandomLed = [&](int value) -> void {
            int data[pin_size];
            for (int i = 0; i < pin_size; ++i) {
                data[i] = !value;
            }
            for (int i = 0; i < pin_size; ++i) {
                int index;
                do {
                    index = random(pin_size);
                } while (data[index] == value);
                data[index] = value;
                for (int j = 0; j < pin_size; ++j) {
                    set(j, data[j]);
                }
                sleep(delay_time);
            }
        };
        for (int i = 0; i < 3; ++i) {
            setRandomLed(HIGH);
            setRandomLed(LOW);
        }
    }

    void BaseChannel::taskSequence10() {
        if (!config_ptr_) return;
        uint8_t pin_size = config_ptr_->getTotalChannels();
        uint32_t delay_time = channel_data_.custom_delay_;
        
        if ((pin_size / 2) % 2 == 1) {
            taskSequence7();
        } else {
            auto setWaveLed = [&](int value) -> void {
                for (int i = 0; i < (pin_size / 2); ++i) {
                    if (i < ceil((pin_size / 2) / 2)) {
                        set(i, value);
                        set((pin_size / 2) - (i + 1), value);
                    } else {
                        set(i + (pin_size / 2), value);
                        set((pin_size) - i - 1, value);
                    }
                    sleep(delay_time);
                }
            };

            auto setWaveLedReverse = [&](int value) -> void {
                for (int i = (pin_size / 2) - 1; i >= 0; --i) {
                    if (i >= ceil((pin_size / 2) / 2)) {
                        set(i + (pin_size / 2), value);
                        set((pin_size) - i - 1, value);
                    } else {
                        set(i, value);
                        set((pin_size / 2) - (i + 1), value);
                    }
                    sleep(delay_time);
                }
            };

            for (int i = 0; i < 2; ++i) {
                setWaveLed(HIGH);
                setWaveLed(LOW);
            }
            for (int i = 0; i < 2; ++i) {
                setWaveLedReverse(HIGH);
                setWaveLedReverse(LOW);
            }
        }
    }

    void BaseChannel::taskSequence11() {
        if (!config_ptr_) return;
        uint8_t pin_size = config_ptr_->getTotalChannels();
        uint32_t delay_time = channel_data_.custom_delay_;
        
        for (int j = 0; j < 2; j++) {
            for (int i = 0; i < pin_size / 2 + 2; i++) {
                if (i < pin_size / 2) set(i, HIGH);
                if (i < pin_size / 2) set(i + (pin_size / 2), HIGH);
                if (i > 1 && i < pin_size / 2 + 2) set(i - 2, HIGH);
                if (i > 1 && i < pin_size / 2 + 1) set(i - 1, HIGH);
                if (i > 1 && i < pin_size / 2 + 2) set(i + (pin_size / 2) - 2, HIGH);
                if (i > 1 && i < pin_size / 2 + 1) set(i + (pin_size / 2) - 1, HIGH);
                sleep(delay_time * 2);
                if (i < pin_size / 2) set(i, LOW);
                if (i < pin_size / 2) set(i + (pin_size / 2), LOW);
                if (i > 1 && i < pin_size / 2 + 2) set(i - 2, LOW);
                if (i > 1 && i < pin_size / 2 + 1) set(i - 1, LOW);
                if (i > 1 && i < pin_size / 2 + 2) set(i + (pin_size / 2) - 2, LOW);
                if (i > 1 && i < pin_size / 2 + 1) set(i + (pin_size / 2) - 1, LOW);
            }
        }
        for (int j = 0; j < 2; j++) {
            for (int i = 0; i < pin_size / 2 + 2; i++) {
                if (i < pin_size / 2) set((pin_size / 2) - 1 - i, HIGH);
                if (i < pin_size / 2) set(pin_size - 1 - i, HIGH);
                if (i > 1 && i < pin_size / 2 + 2) set((pin_size / 2) - 1 - i + 2, HIGH);
                if (i > 1 && i < pin_size / 2 + 1) set((pin_size / 2) - 1 - i + 1, HIGH);
                if (i > 1 && i < pin_size / 2 + 2) set(pin_size - 1 - i + 2, HIGH);
                if (i > 1 && i < pin_size / 2 + 1) set(pin_size - 1 - i + 1, HIGH);
                sleep(delay_time * 2);
                if (i < pin_size / 2) set((pin_size / 2) - 1 - i, LOW);
                if (i < pin_size / 2) set(pin_size - 1 - i, LOW);
                if (i > 1 && i < pin_size / 2 + 2) set((pin_size / 2) - 1 - i + 2, LOW);
                if (i > 1 && i < pin_size / 2 + 1) set((pin_size / 2) - 1 - i + 1, LOW);
                if (i > 1 && i < pin_size / 2 + 2) set(pin_size - 1 - i + 2, LOW);
                if (i > 1 && i < pin_size / 2 + 1) set(pin_size - 1 - i + 1, LOW);
            }
        }
        sleep(delay_time * 2);
        
        for (int j = 0; j < 2; j++) {
            for (int i = 0; i < pin_size / 2; i++) {
                set((pin_size / 2) - 1 - i, HIGH);
                set(i + (pin_size / 2), HIGH);
                sleep(delay_time + (delay_time / 2));
            }
            for (int i = 0; i < pin_size / 2; i++) {
                set((pin_size / 2) - 1 - i, LOW);
                set(i + (pin_size / 2), LOW);
                sleep(delay_time + (delay_time / 2));
            }
        }
    }

    void BaseChannel::taskSequence12() {
        if (!config_ptr_) return;
        uint8_t pin_size = config_ptr_->getTotalChannels();
        uint32_t delay_time = channel_data_.custom_delay_;
        
        if (pin_size < 8) return;
        
        taskSequence11();
    }

    void BaseChannel::taskSequence13() {
        if (!config_ptr_) return;
        uint8_t pin_size = config_ptr_->getTotalChannels();
        uint32_t delay_time = channel_data_.custom_delay_;
        
        if (pin_size < 8) return;
        
        for (int k = 0; k < 3; k++) {
            for (int j = 0; j < 3; j++) {
                for (int i = 0; i < pin_size; i++) {
                    set(i, HIGH);
                }
                sleep(delay_time * 2);
                for (int i = 0; i < pin_size; i++) {
                    set(i, LOW);
                }
                sleep(delay_time * 2);
            }
            sleep(delay_time * 2);
        }
    }

    void BaseChannel::taskSequence14() {
        if (!config_ptr_) return;
        uint8_t pin_size = config_ptr_->getTotalChannels();
        uint32_t delay_time = channel_data_.custom_delay_;
        
        if (pin_size < 8) return;
        
        for (int i = 0; i < pin_size / 2; i++) {
            for (int j = 0; j < pin_size / 2; j++) {
                set(j, HIGH);
                set(pin_size - 1 - j, HIGH);
                sleep(delay_time);
            }
            for (int j = 0; j < i; j++) {
                set(j, HIGH);
                set(pin_size - 1 - j, HIGH);
                sleep(delay_time * 2);
                set(j, LOW);
                set(pin_size - 1 - j, LOW);
                sleep(delay_time * 2);
            }
            set(i, LOW);
            set(pin_size - 1 - i, LOW);
            sleep(delay_time * 2);
        }
    }

    void BaseChannel::taskSequence15() {
        taskSequence2();
        taskSequence3();
        taskSequence4();
        taskSequence5();
        taskSequence6();
        taskSequence7();
        taskSequence8();
        taskSequence9();
        taskSequence10();
        taskSequence11();
        taskSequence12();
        taskSequence13();
        taskSequence14();
    }

    void BaseChannel::sleep(uint32_t ms) {
        delay(ms);
    }
}  // namespace AutoLight
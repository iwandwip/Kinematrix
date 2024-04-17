/*
 *  BaseChannelSequence.cpp
 *
 *  Kastara Electronics Embedded Development
 *  Created on: 2023. 4. 3
 */

#include "BaseChannel.h"

namespace AutoLight {
    void BaseChannel::taskSequence2() {
//        Serial.println("BaseChannel::taskSequence1() start");
        // blink ////////////////////////////////////////
        {
            for (int i = 0; i < 2; i++) {
                for (int j = 0; j < 15; ++j) {
                    for (int k = 0; k < config_data_ptr_->header.pin_size_; k++) {
                        set(config_data_ptr_->header.pin_ptr_[k], HIGH);
                    }
                    sleep(channel_data_.delay_time_);
                    for (int k = 0; k < config_data_ptr_->header.pin_size_; k++) {
                        set(config_data_ptr_->header.pin_ptr_[k], LOW);
                    }
                    sleep(channel_data_.delay_time_);
                }
                sleep(500);
            }
            off();
        }
        // half blink ////////////////////////////////////////
        {
            for (int i = 0; i < 4; ++i) {
                for (int j = 0; j < (config_data_ptr_->header.pin_size_ / 2); j++) {
                    set(config_data_ptr_->header.pin_ptr_[j], HIGH);
                }
                sleep(channel_data_.delay_time_);
                for (int j = 0; j < (config_data_ptr_->header.pin_size_ / 2); j++) {
                    set(config_data_ptr_->header.pin_ptr_[j], LOW);
                }
                sleep(channel_data_.delay_time_);
            }
            sleep(50);
            for (int i = 0; i < 4; ++i) {
                for (int j = (config_data_ptr_->header.pin_size_ / 2); j < config_data_ptr_->header.pin_size_; j++) {
                    set(config_data_ptr_->header.pin_ptr_[j], HIGH);
                }
                sleep(channel_data_.delay_time_);
                for (int j = (config_data_ptr_->header.pin_size_ / 2); j < config_data_ptr_->header.pin_size_; j++) {
                    set(config_data_ptr_->header.pin_ptr_[j], LOW);
                }
                sleep(channel_data_.delay_time_);
            }
            sleep(50);
            for (int i = 0; i < 4; ++i) {
                for (int j = 0; j < (config_data_ptr_->header.pin_size_ / 2); j++) {
                    set(config_data_ptr_->header.pin_ptr_[j], HIGH);
                }
                sleep(channel_data_.delay_time_);
                for (int j = 0; j < (config_data_ptr_->header.pin_size_ / 2); j++) {
                    set(config_data_ptr_->header.pin_ptr_[j], LOW);
                }
                sleep(channel_data_.delay_time_);
            }
            sleep(500);
            off();
        }
        // half blink ////////////////////////////////////////
        {
            for (int i = 0; i < 4; ++i) {
                for (int j = (config_data_ptr_->header.pin_size_ / 2); j < config_data_ptr_->header.pin_size_; j++) {
                    set(config_data_ptr_->header.pin_ptr_[j], HIGH);
                }
                sleep(channel_data_.delay_time_);
                for (int j = (config_data_ptr_->header.pin_size_ / 2); j < config_data_ptr_->header.pin_size_; j++) {
                    set(config_data_ptr_->header.pin_ptr_[j], LOW);
                }
                sleep(channel_data_.delay_time_);
            }
            sleep(50);
            for (int i = 0; i < 4; ++i) {
                for (int j = 0; j < (config_data_ptr_->header.pin_size_ / 2); j++) {
                    set(config_data_ptr_->header.pin_ptr_[j], HIGH);
                }
                sleep(channel_data_.delay_time_);
                for (int j = 0; j < (config_data_ptr_->header.pin_size_ / 2); j++) {
                    set(config_data_ptr_->header.pin_ptr_[j], LOW);
                }
                sleep(channel_data_.delay_time_);
            }
            sleep(50);
            for (int i = 0; i < 4; ++i) {
                for (int j = (config_data_ptr_->header.pin_size_ / 2); j < config_data_ptr_->header.pin_size_; j++) {
                    set(config_data_ptr_->header.pin_ptr_[j], HIGH);
                }
                sleep(channel_data_.delay_time_);
                for (int j = (config_data_ptr_->header.pin_size_ / 2); j < config_data_ptr_->header.pin_size_; j++) {
                    set(config_data_ptr_->header.pin_ptr_[j], LOW);
                }
                sleep(channel_data_.delay_time_);
            }
            off();
            sleep(500);
        }
//        Serial.println("BaseChannel::taskSequence1() end");
    }

    void BaseChannel::taskSequence3() {
//        Serial.println("BaseChannel::taskSequence2() start");
        // fill 2 point ////////////////////////////////////////
        {
            for (int i = 0; i < config_data_ptr_->header.pin_size_ / 2; i += 2) {
                for (int j = 0; j < 8; ++j) {
                    set(config_data_ptr_->header.pin_ptr_[i], HIGH);
                    set(config_data_ptr_->header.pin_ptr_[i + 1], HIGH);
                    sleep(channel_data_.delay_time_);
                    set(config_data_ptr_->header.pin_ptr_[i], LOW);
                    set(config_data_ptr_->header.pin_ptr_[i + 1], LOW);
                    sleep(channel_data_.delay_time_);
                }
                set(config_data_ptr_->header.pin_ptr_[i], HIGH);
                set(config_data_ptr_->header.pin_ptr_[i + 1], HIGH);
                sleep(300);
                if (i < (config_data_ptr_->header.pin_size_ / 2) - 1) {
                    for (int j = 0; j < 8; ++j) {
                        set(config_data_ptr_->header.pin_ptr_[(config_data_ptr_->header.pin_size_ - 1) - i], HIGH);
                        set(config_data_ptr_->header.pin_ptr_[(config_data_ptr_->header.pin_size_ - 1) - (i + 1)],
                            HIGH);
                        sleep(channel_data_.delay_time_);
                        set(config_data_ptr_->header.pin_ptr_[(config_data_ptr_->header.pin_size_ - 1) - i], LOW);
                        set(config_data_ptr_->header.pin_ptr_[(config_data_ptr_->header.pin_size_ - 1) - (i + 1)], LOW);
                        sleep(channel_data_.delay_time_);
                    }
                    set(config_data_ptr_->header.pin_ptr_[(config_data_ptr_->header.pin_size_ - 1) - i], HIGH);
                    set(config_data_ptr_->header.pin_ptr_[(config_data_ptr_->header.pin_size_ - 1) - (i + 1)], HIGH);
                    sleep(300);
                }
            }
            for (int i = config_data_ptr_->header.pin_size_; i > 0; --i) {
                set(config_data_ptr_->header.pin_ptr_[i - 1], LOW);
                sleep(channel_data_.delay_time_ * 2);
            }
            off();
            sleep(500);
        }
//        Serial.println("BaseChannel::taskSequence2() end");
    }

    void BaseChannel::taskSequence4() {
//        Serial.println("BaseChannel::taskSequence3() start");
        // fill right ////////////////////////////////////////
        {
            for (int i = config_data_ptr_->header.pin_size_; i > 0; --i) {
                for (int j = 0; j < i; j++) {
                    set(config_data_ptr_->header.pin_ptr_[j], HIGH);
                    sleep(channel_data_.delay_time_ * 2);
                    set(config_data_ptr_->header.pin_ptr_[j], LOW);
                }
                set(config_data_ptr_->header.pin_ptr_[i - 1], HIGH);
            }
            for (int i = config_data_ptr_->header.pin_size_; i > 0; --i) {
                set(config_data_ptr_->header.pin_ptr_[i - 1], LOW);
                sleep(channel_data_.delay_time_ * 2);
            }
            off();
            sleep(500);
        }
//        Serial.println("BaseChannel::taskSequence3() end");
    }

    void BaseChannel::taskSequence5() {
//        Serial.println("BaseChannel::taskSequence4() start");
        // fill in ////////////////////////////////////////
        {
            for (int j = 0; j < config_data_ptr_->header.pin_size_ / 2; ++j) {
                for (int i = config_data_ptr_->header.pin_size_; i > config_data_ptr_->header.pin_size_ / 2 + j; --i) {
                    set(config_data_ptr_->header.pin_ptr_[i - 1], HIGH);
                    set(config_data_ptr_->header.pin_ptr_[config_data_ptr_->header.pin_size_ - i], HIGH);
                    sleep(channel_data_.delay_time_ * 2);
                    set(config_data_ptr_->header.pin_ptr_[i - 1], LOW);
                    set(config_data_ptr_->header.pin_ptr_[config_data_ptr_->header.pin_size_ - i], LOW);
                }
                for (int i = j; i < config_data_ptr_->header.pin_size_ / 2; ++i) {
                    set(config_data_ptr_->header.pin_ptr_[i + config_data_ptr_->header.pin_size_ / 2], HIGH);
                    set(config_data_ptr_->header.pin_ptr_[(config_data_ptr_->header.pin_size_ / 2 - 1) - i], HIGH);
                    sleep(channel_data_.delay_time_ * 2);
                    set(config_data_ptr_->header.pin_ptr_[i + config_data_ptr_->header.pin_size_ / 2], LOW);
                    set(config_data_ptr_->header.pin_ptr_[(config_data_ptr_->header.pin_size_ / 2 - 1) - i], LOW);
                }
                for (int i = config_data_ptr_->header.pin_size_ - 1;
                     i > config_data_ptr_->header.pin_size_ / 2 + j; --i) {
                    set(config_data_ptr_->header.pin_ptr_[i - 1], HIGH);
                    set(config_data_ptr_->header.pin_ptr_[config_data_ptr_->header.pin_size_ - i], HIGH);
                    sleep(channel_data_.delay_time_ * 2);
                    set(config_data_ptr_->header.pin_ptr_[i - 1], LOW);
                    set(config_data_ptr_->header.pin_ptr_[config_data_ptr_->header.pin_size_ - i], LOW);
                }
                set(config_data_ptr_->header.pin_ptr_[config_data_ptr_->header.pin_size_ / 2 + j], HIGH);
                set(config_data_ptr_->header.pin_ptr_[(config_data_ptr_->header.pin_size_ / 2 - 1) - j], HIGH);
            }
            for (int i = 0; i < config_data_ptr_->header.pin_size_ / 2; ++i) {
                set(config_data_ptr_->header.pin_ptr_[i + config_data_ptr_->header.pin_size_ / 2], LOW);
                set(config_data_ptr_->header.pin_ptr_[(config_data_ptr_->header.pin_size_ / 2 - 1) - i], LOW);
                sleep(channel_data_.delay_time_ * 2);
            }
            off();
            sleep(500);
        }
//        Serial.println("BaseChannel::taskSequence4() end");
    }

    void BaseChannel::taskSequence6() {
//        Serial.println("BaseChannel::taskSequence5() start");
        // blink 1 by 1 ////////////////////////////////////////
        {
            for (int i = 0; i < config_data_ptr_->header.pin_size_; ++i) {
                for (int j = 0; j < 4; ++j) {
                    set(config_data_ptr_->header.pin_ptr_[i], HIGH);
                    sleep(channel_data_.delay_time_);
                    set(config_data_ptr_->header.pin_ptr_[i], LOW);
                    sleep(channel_data_.delay_time_);
                }
                sleep(channel_data_.delay_time_);
            }
            for (int i = config_data_ptr_->header.pin_size_; i > 0; --i) {
                for (int j = 0; j < 4; ++j) {
                    set(config_data_ptr_->header.pin_ptr_[i - 1], HIGH);
                    sleep(channel_data_.delay_time_);
                    set(config_data_ptr_->header.pin_ptr_[i - 1], LOW);
                    sleep(channel_data_.delay_time_);
                }
                sleep(channel_data_.delay_time_);
            }
            off();
            sleep(500);
        }
//        Serial.println("BaseChannel::taskSequence5() end");
    }

    void BaseChannel::taskSequence7() {
//        Serial.println("BaseChannel::taskSequence6() start");
        // blink 2 fill ////////////////////////////////////////
        {
            for (int j = 0; j < config_data_ptr_->header.pin_size_ / 2; ++j) {
                for (int i = config_data_ptr_->header.pin_size_ / 2; i > j; --i) {
                    for (int k = 0; k < 4; ++k) {
                        set(config_data_ptr_->header.pin_ptr_[i - 1], HIGH);
                        set(config_data_ptr_->header.pin_ptr_[config_data_ptr_->header.pin_size_ - i], HIGH);
                        sleep(channel_data_.delay_time_);
                        set(config_data_ptr_->header.pin_ptr_[i - 1], LOW);
                        set(config_data_ptr_->header.pin_ptr_[config_data_ptr_->header.pin_size_ - i], LOW);
                        sleep(channel_data_.delay_time_);
                    }
                }
                set(config_data_ptr_->header.pin_ptr_[j], HIGH);
                set(config_data_ptr_->header.pin_ptr_[(config_data_ptr_->header.pin_size_ - 1) - j], HIGH);
            }
            off();
            sleep(500);
        }
//        Serial.println("BaseChannel::taskSequence6() end");
    }

    void BaseChannel::taskSequence8() {
//        Serial.println("BaseChannel::taskSequence7() start");
        // snake and snake reverse ////////////////////////////////////////
        {
            off();
            for (float k = channel_data_.delay_time_ * 2;
                 k >= channel_data_.delay_time_; k -= channel_data_.delay_time_) {
                for (int i = 0; i < config_data_ptr_->header.pin_size_; i++) {
                    set(config_data_ptr_->header.pin_ptr_[i], HIGH);
                    sleep(k);
                }
                for (int i = 0; i < config_data_ptr_->header.pin_size_; i++) {
                    set(config_data_ptr_->header.pin_ptr_[i], LOW);
                    sleep(k);
                }
                for (int j = config_data_ptr_->header.pin_size_; j > 0; j--) {
                    set(config_data_ptr_->header.pin_ptr_[j - 1], HIGH);
                    sleep(k);
                }
                for (int j = config_data_ptr_->header.pin_size_; j > 0; j--) {
                    set(config_data_ptr_->header.pin_ptr_[j - 1], LOW);
                    sleep(k);
                }
            }
            sleep(500);
        }
//        Serial.println("BaseChannel::taskSequence7() end");
    }

    void BaseChannel::taskSequence9() {
//        Serial.println("BaseChannel::taskSequence8() start");
        auto setRandomLed = [&](int value) -> void {
            int data[config_data_ptr_->header.pin_size_];
            for (int i = 0; i < config_data_ptr_->header.pin_size_; ++i) {
                data[i] = !value;
            }
            for (int i = 0; i < config_data_ptr_->header.pin_size_; ++i) {
                int index;
                do {
                    index = random(config_data_ptr_->header.pin_size_);
                } while (data[index] == value);
                data[index] = value;
                for (int j = 0; j < config_data_ptr_->header.pin_size_; ++j) {
                    set(config_data_ptr_->header.pin_ptr_[j], data[j]);
                }
                sleep(channel_data_.delay_time_);
            }
        };

        setRandomLed(HIGH);
        setRandomLed(LOW);
//        Serial.println("BaseChannel::taskSequence8() end");
    }

    void BaseChannel::taskSequence10() {
//        Serial.println("BaseChannel::taskSequence9() start");
        if ((config_data_ptr_->header.pin_size_ / 2) % 2 == 1) {
            taskSequence7();
        } else {
            auto setWaveLed = [&](int value) -> void {
                for (int i = 0; i < (config_data_ptr_->header.pin_size_ / 2); ++i) {
                    if (i < ceil((config_data_ptr_->header.pin_size_ / 2) / 2)) {
                        set(config_data_ptr_->header.pin_ptr_[i], value);
                        set(config_data_ptr_->header.pin_ptr_[(config_data_ptr_->header.pin_size_ / 2) - (i + 1)],
                            value);
                    } else {
                        set(config_data_ptr_->header.pin_ptr_[i + (config_data_ptr_->header.pin_size_ / 2)], value);
                        set(config_data_ptr_->header.pin_ptr_[(config_data_ptr_->header.pin_size_) - i - 1], value);
                    }
                    sleep(channel_data_.delay_time_);
                }
            };
            setWaveLed(HIGH);
            setWaveLed(LOW);
        }
//        Serial.println("BaseChannel::taskSequence9() end");
    }
}
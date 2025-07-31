/*
 *  BaseChannelSequence.cpp
 *
 *  Kastara Electronics Embedded Development
 *  Created on: 2023. 4. 3
 */

#include "BaseChannel.h"

namespace AutoLight {
    void BaseChannel::taskSequence2BlinkAll() {
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

    void BaseChannel::taskSequence3FillTwoPoint() {
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

    void BaseChannel::taskSequence4FillRight() {
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

    void BaseChannel::taskSequence5FillIn() {
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

    void BaseChannel::taskSequence6BlinkOneByOne() {
//        Serial.println("BaseChannel::taskSequence5() start");
        // blink 1 by 1 ////////////////////////////////////////
//        {
//            for (int i = 0; i < config_data_ptr_->header.pin_size_; ++i) {
//                for (int j = 0; j < 4; ++j) {
//                    set(config_data_ptr_->header.pin_ptr_[i], HIGH);
//                    sleep(channel_data_.delay_time_);
//                    set(config_data_ptr_->header.pin_ptr_[i], LOW);
//                    sleep(channel_data_.delay_time_);
//                }
//                sleep(channel_data_.delay_time_);
//            }
//            for (int i = config_data_ptr_->header.pin_size_; i > 0; --i) {
//                for (int j = 0; j < 4; ++j) {
//                    set(config_data_ptr_->header.pin_ptr_[i - 1], HIGH);
//                    sleep(channel_data_.delay_time_);
//                    set(config_data_ptr_->header.pin_ptr_[i - 1], LOW);
//                    sleep(channel_data_.delay_time_);
//                }
//                sleep(channel_data_.delay_time_);
//            }
//            off();
//            sleep(500);
//        }
//        Serial.println("BaseChannel::taskSequence5() end");

        {
            int halfSize = config_data_ptr_->header.pin_size_ / 2;

            for (int i = config_data_ptr_->header.pin_size_; i > 0; --i) {
                int time = map(config_data_ptr_->header.pin_size_ - i, 1, config_data_ptr_->header.pin_size_, channel_data_.delay_time_ + (channel_data_.delay_time_ / 2), channel_data_.delay_time_ / 2);
                for (int j = config_data_ptr_->header.pin_size_ - 1; j >= config_data_ptr_->header.pin_size_ - i; --j) {
                    if (j >= halfSize) {
                        set(config_data_ptr_->header.pin_ptr_[halfSize + (config_data_ptr_->header.pin_size_ - 1) - j], HIGH);
                        sleep(time);
                        set(config_data_ptr_->header.pin_ptr_[halfSize + (config_data_ptr_->header.pin_size_ - 1) - j], LOW);
                    } else {
                        set(config_data_ptr_->header.pin_ptr_[j], HIGH);
                        sleep(time);
                        set(config_data_ptr_->header.pin_ptr_[j], LOW);
                    }
                }
                if (i > halfSize) {
                    set(config_data_ptr_->header.pin_ptr_[config_data_ptr_->header.pin_size_ - i], HIGH);
                } else {
                    set(config_data_ptr_->header.pin_ptr_[config_data_ptr_->header.pin_size_ - (halfSize - i) - 1], HIGH);
                }
            }
            sleep(channel_data_.delay_time_ * 12);

            for (int i = config_data_ptr_->header.pin_size_; i > 0; --i) {
                if (i > halfSize) set(config_data_ptr_->header.pin_ptr_[halfSize + (config_data_ptr_->header.pin_size_ - i)], LOW);
                else set(config_data_ptr_->header.pin_ptr_[i - 1], LOW);
                sleep(channel_data_.delay_time_);
            }

            for (int j = 0; j < 2; ++j) {
                for (int i = 0; i < config_data_ptr_->header.pin_size_; ++i) {
                    if (i < halfSize) set(config_data_ptr_->header.pin_ptr_[i], HIGH);
                    else set(config_data_ptr_->header.pin_ptr_[config_data_ptr_->header.pin_size_ - (i - halfSize) - 1], HIGH);
                    sleep(channel_data_.delay_time_);
                }
                for (int i = config_data_ptr_->header.pin_size_; i > 0; --i) {
                    if (i > halfSize) set(config_data_ptr_->header.pin_ptr_[halfSize + (config_data_ptr_->header.pin_size_ - i)], LOW);
                    else set(config_data_ptr_->header.pin_ptr_[i - 1], LOW);
                    sleep(channel_data_.delay_time_);
                }
            }

            for (int i = 0; i < config_data_ptr_->header.pin_size_ / 2; ++i) {
                set(config_data_ptr_->header.pin_ptr_[i], HIGH);
//            set(config_data_ptr_->header.pin_ptr_[halfSize + i], HIGH);
                set(config_data_ptr_->header.pin_ptr_[halfSize + i], HIGH);
                sleep(channel_data_.delay_time_);
            }
            for (int i = 0; i < config_data_ptr_->header.pin_size_ / 2; ++i) {
                set(config_data_ptr_->header.pin_ptr_[i], LOW);
//            set(config_data_ptr_->header.pin_ptr_[halfSize + i], LOW);
                set(config_data_ptr_->header.pin_ptr_[halfSize + i], LOW);
                sleep(channel_data_.delay_time_);
            }

            for (int j = 0; j < 2; ++j) {
                for (int i = 0; i < config_data_ptr_->header.pin_size_ / 2; ++i) {
                    set(config_data_ptr_->header.pin_ptr_[halfSize - i - 1], HIGH);
//            set(config_data_ptr_->header.pin_ptr_[halfSize + i], HIGH);
                    set(config_data_ptr_->header.pin_ptr_[config_data_ptr_->header.pin_size_ - i - 1], HIGH);
                    sleep(channel_data_.delay_time_);
                }
                for (int i = 0; i < config_data_ptr_->header.pin_size_ / 2; ++i) {
                    set(config_data_ptr_->header.pin_ptr_[i], LOW);
//            set(config_data_ptr_->header.pin_ptr_[halfSize + i], LOW);
                    set(config_data_ptr_->header.pin_ptr_[halfSize + i], LOW);
                    sleep(channel_data_.delay_time_);
                }
            }

            sleep(channel_data_.delay_time_ * 18);

            for (int i = 0; i < 2; ++i) {
                on();
                sleep(channel_data_.delay_time_ * 3);
                off();
                sleep(channel_data_.delay_time_ * 3);
            }

            sleep(channel_data_.delay_time_ * 18);
        }
    }

    void BaseChannel::taskSequence7BlinkTwoFill() {
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

    void BaseChannel::taskSequence8SnakeAndReverse() {
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

    void BaseChannel::taskSequence9Random() {
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
        for (int i = 0; i < 3; ++i) {
            setRandomLed(HIGH);
            setRandomLed(LOW);
        }
//        Serial.println("BaseChannel::taskSequence8() end");
    }

    void BaseChannel::taskSequence10Wave() {
//        Serial.println("BaseChannel::taskSequence9() start");
        if ((config_data_ptr_->header.pin_size_ / 2) % 2 == 1) {
            taskSequence7BlinkTwoFill();
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

            auto setWaveLedReverse = [&](int value) -> void {
                for (int i = (config_data_ptr_->header.pin_size_ / 2) - 1; i >= 0; --i) {
                    if (i >= ceil((config_data_ptr_->header.pin_size_ / 2) / 2)) {
                        set(config_data_ptr_->header.pin_ptr_[i + (config_data_ptr_->header.pin_size_ / 2)], value);
                        set(config_data_ptr_->header.pin_ptr_[(config_data_ptr_->header.pin_size_) - i - 1], value);
                    } else {
                        set(config_data_ptr_->header.pin_ptr_[i], value);
                        set(config_data_ptr_->header.pin_ptr_[(config_data_ptr_->header.pin_size_ / 2) - (i + 1)], value);
                    }
                    sleep(channel_data_.delay_time_);
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
//        Serial.println("BaseChannel::taskSequence9() end");

    }

    void BaseChannel::taskSequence11Complex() {
        for (int j = 0; j < 2; j++) {
            for (int i = 0; i < config_data_ptr_->header.pin_size_ / 2 + 2; i++) {
                if (i < config_data_ptr_->header.pin_size_ / 2) set(config_data_ptr_->header.pin_ptr_[i], HIGH);
                if (i < config_data_ptr_->header.pin_size_ / 2) set(config_data_ptr_->header.pin_ptr_[i + (config_data_ptr_->header.pin_size_ / 2)], HIGH);
                if (i > 1 && i < config_data_ptr_->header.pin_size_ / 2 + 2) set(config_data_ptr_->header.pin_ptr_[i - 2], HIGH);
                if (i > 1 && i < config_data_ptr_->header.pin_size_ / 2 + 1) set(config_data_ptr_->header.pin_ptr_[i - 1], HIGH);
                if (i > 1 && i < config_data_ptr_->header.pin_size_ / 2 + 2) set(config_data_ptr_->header.pin_ptr_[i + (config_data_ptr_->header.pin_size_ / 2) - 2], HIGH);
                if (i > 1 && i < config_data_ptr_->header.pin_size_ / 2 + 1) set(config_data_ptr_->header.pin_ptr_[i + (config_data_ptr_->header.pin_size_ / 2) - 1], HIGH);
                sleep(channel_data_.delay_time_ * 2);
                if (i < config_data_ptr_->header.pin_size_ / 2) set(config_data_ptr_->header.pin_ptr_[i], LOW);
                if (i < config_data_ptr_->header.pin_size_ / 2) set(config_data_ptr_->header.pin_ptr_[i + (config_data_ptr_->header.pin_size_ / 2)], LOW);
                if (i > 1 && i < config_data_ptr_->header.pin_size_ / 2 + 2) set(config_data_ptr_->header.pin_ptr_[i - 2], LOW);
                if (i > 1 && i < config_data_ptr_->header.pin_size_ / 2 + 1) set(config_data_ptr_->header.pin_ptr_[i - 1], LOW);
                if (i > 1 && i < config_data_ptr_->header.pin_size_ / 2 + 2) set(config_data_ptr_->header.pin_ptr_[i + (config_data_ptr_->header.pin_size_ / 2) - 2], LOW);
                if (i > 1 && i < config_data_ptr_->header.pin_size_ / 2 + 1) set(config_data_ptr_->header.pin_ptr_[i + (config_data_ptr_->header.pin_size_ / 2) - 1], LOW);
            }
        }
        for (int j = 0; j < 2; j++) {
            for (int i = 0; i < config_data_ptr_->header.pin_size_ / 2 + 2; i++) {
                if (i < config_data_ptr_->header.pin_size_ / 2) set(config_data_ptr_->header.pin_ptr_[(config_data_ptr_->header.pin_size_ / 2) - 1 - i], HIGH);
                if (i < config_data_ptr_->header.pin_size_ / 2) set(config_data_ptr_->header.pin_ptr_[config_data_ptr_->header.pin_size_ - 1 - i], HIGH);
                if (i > 1 && i < config_data_ptr_->header.pin_size_ / 2 + 2) set(config_data_ptr_->header.pin_ptr_[(config_data_ptr_->header.pin_size_ / 2) - 1 - i + 2], HIGH);
                if (i > 1 && i < config_data_ptr_->header.pin_size_ / 2 + 1) set(config_data_ptr_->header.pin_ptr_[(config_data_ptr_->header.pin_size_ / 2) - 1 - i + 1], HIGH);
                if (i > 1 && i < config_data_ptr_->header.pin_size_ / 2 + 2) set(config_data_ptr_->header.pin_ptr_[config_data_ptr_->header.pin_size_ - 1 - i + 2], HIGH);
                if (i > 1 && i < config_data_ptr_->header.pin_size_ / 2 + 1) set(config_data_ptr_->header.pin_ptr_[config_data_ptr_->header.pin_size_ - 1 - i + 1], HIGH);
                sleep(channel_data_.delay_time_ * 2);
                if (i < config_data_ptr_->header.pin_size_ / 2) set(config_data_ptr_->header.pin_ptr_[(config_data_ptr_->header.pin_size_ / 2) - 1 - i], LOW);
                if (i < config_data_ptr_->header.pin_size_ / 2) set(config_data_ptr_->header.pin_ptr_[config_data_ptr_->header.pin_size_ - 1 - i], LOW);
                if (i > 1 && i < config_data_ptr_->header.pin_size_ / 2 + 2) set(config_data_ptr_->header.pin_ptr_[(config_data_ptr_->header.pin_size_ / 2) - 1 - i + 2], LOW);
                if (i > 1 && i < config_data_ptr_->header.pin_size_ / 2 + 1) set(config_data_ptr_->header.pin_ptr_[(config_data_ptr_->header.pin_size_ / 2) - 1 - i + 1], LOW);
                if (i > 1 && i < config_data_ptr_->header.pin_size_ / 2 + 2) set(config_data_ptr_->header.pin_ptr_[config_data_ptr_->header.pin_size_ - 1 - i + 2], LOW);
                if (i > 1 && i < config_data_ptr_->header.pin_size_ / 2 + 1) set(config_data_ptr_->header.pin_ptr_[config_data_ptr_->header.pin_size_ - 1 - i + 1], LOW);
            }
        }
        sleep(channel_data_.delay_time_ * 2);
        //////////////////////////////////////
        for (int j = 0; j < 2; j++) {
            for (int i = 0; i < config_data_ptr_->header.pin_size_ / 2; i++) {
                set(config_data_ptr_->header.pin_ptr_[(config_data_ptr_->header.pin_size_ / 2) - 1 - i], HIGH);
                set(config_data_ptr_->header.pin_ptr_[i + (config_data_ptr_->header.pin_size_ / 2)], HIGH);
                sleep(channel_data_.delay_time_ + (channel_data_.delay_time_ / 2));
            }
            for (int i = 0; i < config_data_ptr_->header.pin_size_ / 2; i++) {
                set(config_data_ptr_->header.pin_ptr_[(config_data_ptr_->header.pin_size_ / 2) - 1 - i], LOW);
                set(config_data_ptr_->header.pin_ptr_[i + (config_data_ptr_->header.pin_size_ / 2)], LOW);
                sleep(channel_data_.delay_time_ + (channel_data_.delay_time_ / 2));
            }
        }
        //////////////////////////////////////
        for (int j = 0; j < 2; j++) {
            for (int i = 0; i < config_data_ptr_->header.pin_size_ / 2 + 3; i++) {
                if (i < config_data_ptr_->header.pin_size_ / 2) set(config_data_ptr_->header.pin_ptr_[i], HIGH);
                if (i < config_data_ptr_->header.pin_size_ / 2) set(config_data_ptr_->header.pin_ptr_[i + (config_data_ptr_->header.pin_size_ / 2)], HIGH);
                if (i > 2 && i < config_data_ptr_->header.pin_size_ / 2 + 3) set(config_data_ptr_->header.pin_ptr_[i - 3], HIGH);
                if (i > 2 && i < config_data_ptr_->header.pin_size_ / 2 + 3) set(config_data_ptr_->header.pin_ptr_[i + (config_data_ptr_->header.pin_size_ / 2) - 3], HIGH);
                if (i > 1 && i < config_data_ptr_->header.pin_size_ / 2 + 2) set(config_data_ptr_->header.pin_ptr_[i - 2], HIGH);
                if (i > 1 && i < config_data_ptr_->header.pin_size_ / 2 + 2) set(config_data_ptr_->header.pin_ptr_[i + (config_data_ptr_->header.pin_size_ / 2) - 2], HIGH);
                if (i > 0 && i < config_data_ptr_->header.pin_size_ / 2 + 1) set(config_data_ptr_->header.pin_ptr_[i - 1], HIGH);
                if (i > 0 && i < config_data_ptr_->header.pin_size_ / 2 + 1) set(config_data_ptr_->header.pin_ptr_[i + (config_data_ptr_->header.pin_size_ / 2) - 1], HIGH);
                sleep(channel_data_.delay_time_ * 2);
                if (i < config_data_ptr_->header.pin_size_ / 2) set(config_data_ptr_->header.pin_ptr_[i], LOW);
                if (i < config_data_ptr_->header.pin_size_ / 2) set(config_data_ptr_->header.pin_ptr_[i + (config_data_ptr_->header.pin_size_ / 2)], LOW);
                if (i > 2 && i < config_data_ptr_->header.pin_size_ / 2 + 3) set(config_data_ptr_->header.pin_ptr_[i - 3], LOW);
                if (i > 2 && i < config_data_ptr_->header.pin_size_ / 2 + 3) set(config_data_ptr_->header.pin_ptr_[i + (config_data_ptr_->header.pin_size_ / 2) - 3], LOW);
                if (i > 1 && i < config_data_ptr_->header.pin_size_ / 2 + 2) set(config_data_ptr_->header.pin_ptr_[i - 2], LOW);
                if (i > 1 && i < config_data_ptr_->header.pin_size_ / 2 + 2) set(config_data_ptr_->header.pin_ptr_[i + (config_data_ptr_->header.pin_size_ / 2) - 2], LOW);
                if (i > 0 && i < config_data_ptr_->header.pin_size_ / 2 + 1) set(config_data_ptr_->header.pin_ptr_[i - 1], LOW);
                if (i > 0 && i < config_data_ptr_->header.pin_size_ / 2 + 1) set(config_data_ptr_->header.pin_ptr_[i + (config_data_ptr_->header.pin_size_ / 2) - 1], LOW);
            }
            for (int i = 0; i < config_data_ptr_->header.pin_size_; i++) {
                set(config_data_ptr_->header.pin_ptr_[i], HIGH);
            }
            sleep(channel_data_.delay_time_);
            for (int i = 0; i < config_data_ptr_->header.pin_size_; i++) {
                set(config_data_ptr_->header.pin_ptr_[i], LOW);
            }
            sleep(channel_data_.delay_time_ * 3);

            //////////////////////////////////////
//        for (int i = 0; i < arrSize + 2; i++) {
//            if (i < arrSize) set(arrData[i], HIGH);
//            if (i > 1 && i < arrSize + 2) set(arrData[i - 2], HIGH);
//            if (i > 0 && i < arrSize + 1) set(arrData[i - 1], HIGH);
//            sleep(channel_data_.delay_time_ * 2);
//
//            if (i < arrSize) set(arrData[i], LOW);
//            if (i > 1 && i < arrSize + 2) set(arrData[i - 2], LOW);
//            if (i > 0 && i < arrSize + 1) set(arrData[i - 1], LOW);
//        }
            for (int i = 0; i < config_data_ptr_->header.pin_size_ / 2 + 3; i++) {
                if (i < config_data_ptr_->header.pin_size_ / 2) set(config_data_ptr_->header.pin_ptr_[(config_data_ptr_->header.pin_size_ / 2) - 1 - i], HIGH);
                if (i < config_data_ptr_->header.pin_size_ / 2) set(config_data_ptr_->header.pin_ptr_[config_data_ptr_->header.pin_size_ - 1 - i], HIGH);
                if (i > 2 && i < config_data_ptr_->header.pin_size_ / 2 + 3) set(config_data_ptr_->header.pin_ptr_[(config_data_ptr_->header.pin_size_ / 2) - 1 - i + 3], HIGH);
                if (i > 2 && i < config_data_ptr_->header.pin_size_ / 2 + 3) set(config_data_ptr_->header.pin_ptr_[config_data_ptr_->header.pin_size_ - 1 - i + 3], HIGH);
                if (i > 1 && i < config_data_ptr_->header.pin_size_ / 2 + 2) set(config_data_ptr_->header.pin_ptr_[(config_data_ptr_->header.pin_size_ / 2) - 1 - i + 2], HIGH);
                if (i > 1 && i < config_data_ptr_->header.pin_size_ / 2 + 2) set(config_data_ptr_->header.pin_ptr_[config_data_ptr_->header.pin_size_ - 1 - i + 2], HIGH);
                if (i > 0 && i < config_data_ptr_->header.pin_size_ / 2 + 1) set(config_data_ptr_->header.pin_ptr_[(config_data_ptr_->header.pin_size_ / 2) - 1 - i + 1], HIGH);
                if (i > 0 && i < config_data_ptr_->header.pin_size_ / 2 + 1) set(config_data_ptr_->header.pin_ptr_[config_data_ptr_->header.pin_size_ - 1 - i + 1], HIGH);
                sleep(channel_data_.delay_time_ * 2);
                if (i < config_data_ptr_->header.pin_size_ / 2) set(config_data_ptr_->header.pin_ptr_[(config_data_ptr_->header.pin_size_ / 2) - 1 - i], LOW);
                if (i < config_data_ptr_->header.pin_size_ / 2) set(config_data_ptr_->header.pin_ptr_[config_data_ptr_->header.pin_size_ - 1 - i], LOW);
                if (i > 2 && i < config_data_ptr_->header.pin_size_ / 2 + 3) set(config_data_ptr_->header.pin_ptr_[(config_data_ptr_->header.pin_size_ / 2) - 1 - i + 3], LOW);
                if (i > 2 && i < config_data_ptr_->header.pin_size_ / 2 + 3) set(config_data_ptr_->header.pin_ptr_[config_data_ptr_->header.pin_size_ - 1 - i + 3], LOW);
                if (i > 1 && i < config_data_ptr_->header.pin_size_ / 2 + 2) set(config_data_ptr_->header.pin_ptr_[(config_data_ptr_->header.pin_size_ / 2) - 1 - i + 2], LOW);
                if (i > 1 && i < config_data_ptr_->header.pin_size_ / 2 + 2) set(config_data_ptr_->header.pin_ptr_[config_data_ptr_->header.pin_size_ - 1 - i + 2], LOW);
                if (i > 0 && i < config_data_ptr_->header.pin_size_ / 2 + 1) set(config_data_ptr_->header.pin_ptr_[(config_data_ptr_->header.pin_size_ / 2) - 1 - i + 1], LOW);
                if (i > 0 && i < config_data_ptr_->header.pin_size_ / 2 + 1) set(config_data_ptr_->header.pin_ptr_[config_data_ptr_->header.pin_size_ - 1 - i + 1], LOW);
            }
            for (int i = 0; i < config_data_ptr_->header.pin_size_; i++) {
                set(config_data_ptr_->header.pin_ptr_[i], HIGH);
            }
            sleep(channel_data_.delay_time_);
            for (int i = 0; i < config_data_ptr_->header.pin_size_; i++) {
                set(config_data_ptr_->header.pin_ptr_[i], LOW);
            }
            sleep(channel_data_.delay_time_ * 3);
        }
    }

    void BaseChannel::taskSequence12PatternMatrix() {
        if (config_data_ptr_->header.pin_size_ < 8) return;
        const int patternMode[9][8] = {
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

        int arrData[config_data_ptr_->header.pin_size_];
        int arrSize = 0;
        int arrLastIndex = 0;

        resetArray(arrData, arrSize, arrLastIndex);
        arrSize += shiftArrayAccordingToOrder(patternMode, 9, config_data_ptr_->header.pin_ptr_, config_data_ptr_->header.pin_size_, arrData, arrLastIndex, 0);
        arrSize += shiftArrayAccordingToOrder(patternMode, 9, config_data_ptr_->header.pin_ptr_, config_data_ptr_->header.pin_size_, arrData, arrLastIndex, 7);
        for (int j = 0; j < 4; j++) {
            for (int i = 0; i < arrSize; i++) {
                set(arrData[i], HIGH);
            }
            sleep(channel_data_.delay_time_ + (channel_data_.delay_time_ / 2));
            for (int i = 0; i < arrSize; i++) {
                set(arrData[i], LOW);
            }
            sleep(channel_data_.delay_time_ + (channel_data_.delay_time_ / 2));
        }
        sleep(channel_data_.delay_time_ * 4);

        resetArray(arrData, arrSize, arrLastIndex);
        arrSize += shiftArrayAccordingToOrder(patternMode, 9, config_data_ptr_->header.pin_ptr_, config_data_ptr_->header.pin_size_, arrData, arrLastIndex, 1);
        arrSize += shiftArrayAccordingToOrder(patternMode, 9, config_data_ptr_->header.pin_ptr_, config_data_ptr_->header.pin_size_, arrData, arrLastIndex, 6);
        for (int j = 0; j < 4; j++) {
            for (int i = 0; i < arrSize; i++) {
                set(arrData[i], HIGH);
            }
            sleep(channel_data_.delay_time_ + (channel_data_.delay_time_ / 2));
            for (int i = 0; i < arrSize; i++) {
                set(arrData[i], LOW);
            }
            sleep(channel_data_.delay_time_ + (channel_data_.delay_time_ / 2));
        }
        sleep(channel_data_.delay_time_ * 4);

        resetArray(arrData, arrSize, arrLastIndex);
        arrSize += shiftArrayAccordingToOrder(patternMode, 9, config_data_ptr_->header.pin_ptr_, config_data_ptr_->header.pin_size_, arrData, arrLastIndex, 2);
        arrSize += shiftArrayAccordingToOrder(patternMode, 9, config_data_ptr_->header.pin_ptr_, config_data_ptr_->header.pin_size_, arrData, arrLastIndex, 3);
        arrSize += shiftArrayAccordingToOrder(patternMode, 9, config_data_ptr_->header.pin_ptr_, config_data_ptr_->header.pin_size_, arrData, arrLastIndex, 4);
        arrSize += shiftArrayAccordingToOrder(patternMode, 9, config_data_ptr_->header.pin_ptr_, config_data_ptr_->header.pin_size_, arrData, arrLastIndex, 5);
        for (int j = 0; j < 4; j++) {
            for (int i = 0; i < arrSize; i++) {
                set(arrData[i], HIGH);
            }
            sleep(channel_data_.delay_time_ + (channel_data_.delay_time_ / 2));
            for (int i = 0; i < arrSize; i++) {
                set(arrData[i], LOW);
            }
            sleep(channel_data_.delay_time_ + (channel_data_.delay_time_ / 2));
        }
        sleep(channel_data_.delay_time_ * 4);

        ////////////////////////////////////////////////////////////////////////

        resetArray(arrData, arrSize, arrLastIndex);
        arrSize += shiftArrayAccordingToOrder(patternMode, 9, config_data_ptr_->header.pin_ptr_, config_data_ptr_->header.pin_size_, arrData, arrLastIndex, 2);
        arrSize += shiftArrayAccordingToOrder(patternMode, 9, config_data_ptr_->header.pin_ptr_, config_data_ptr_->header.pin_size_, arrData, arrLastIndex, 3);
        arrSize += shiftArrayAccordingToOrder(patternMode, 9, config_data_ptr_->header.pin_ptr_, config_data_ptr_->header.pin_size_, arrData, arrLastIndex, 4);
        arrSize += shiftArrayAccordingToOrder(patternMode, 9, config_data_ptr_->header.pin_ptr_, config_data_ptr_->header.pin_size_, arrData, arrLastIndex, 5);
        for (int j = 0; j < 4; j++) {
            for (int i = 0; i < arrSize; i++) {
                set(arrData[i], HIGH);
            }
            sleep(channel_data_.delay_time_ + (channel_data_.delay_time_ / 2));
            for (int i = 0; i < arrSize; i++) {
                set(arrData[i], LOW);
            }
            sleep(channel_data_.delay_time_ + (channel_data_.delay_time_ / 2));
        }
        sleep(channel_data_.delay_time_ * 4);

        resetArray(arrData, arrSize, arrLastIndex);
        arrSize += shiftArrayAccordingToOrder(patternMode, 9, config_data_ptr_->header.pin_ptr_, config_data_ptr_->header.pin_size_, arrData, arrLastIndex, 1);
        arrSize += shiftArrayAccordingToOrder(patternMode, 9, config_data_ptr_->header.pin_ptr_, config_data_ptr_->header.pin_size_, arrData, arrLastIndex, 6);
        for (int j = 0; j < 4; j++) {
            for (int i = 0; i < arrSize; i++) {
                set(arrData[i], HIGH);
            }
            sleep(channel_data_.delay_time_ + (channel_data_.delay_time_ / 2));
            for (int i = 0; i < arrSize; i++) {
                set(arrData[i], LOW);
            }
            sleep(channel_data_.delay_time_ + (channel_data_.delay_time_ / 2));
        }
        sleep(channel_data_.delay_time_ * 4);

        resetArray(arrData, arrSize, arrLastIndex);
        arrSize += shiftArrayAccordingToOrder(patternMode, 9, config_data_ptr_->header.pin_ptr_, config_data_ptr_->header.pin_size_, arrData, arrLastIndex, 0);
        arrSize += shiftArrayAccordingToOrder(patternMode, 9, config_data_ptr_->header.pin_ptr_, config_data_ptr_->header.pin_size_, arrData, arrLastIndex, 7);
        for (int j = 0; j < 4; j++) {
            for (int i = 0; i < arrSize; i++) {
                set(arrData[i], HIGH);
            }
            sleep(channel_data_.delay_time_ + (channel_data_.delay_time_ / 2));
            for (int i = 0; i < arrSize; i++) {
                set(arrData[i], LOW);
            }
            sleep(channel_data_.delay_time_ + (channel_data_.delay_time_ / 2));
        }
        sleep(channel_data_.delay_time_ * 4);

        ////////////////////////////////////////////////////////////////////////
        resetArray(arrData, arrSize, arrLastIndex);
        arrSize += shiftArrayAccordingToOrder(patternMode, 9, config_data_ptr_->header.pin_ptr_, config_data_ptr_->header.pin_size_, arrData, arrLastIndex, 0);
        arrSize += shiftArrayAccordingToOrder(patternMode, 9, config_data_ptr_->header.pin_ptr_, config_data_ptr_->header.pin_size_, arrData, arrLastIndex, 7);

        for (int j = 0; j < 3; j++) {
            for (int i = 0; i < arrSize / 2; i++) {
                set(arrData[i], HIGH);
                set(arrData[arrSize - 1 - i], HIGH);
                sleep(channel_data_.delay_time_ * 2);
                set(arrData[i], LOW);
                set(arrData[arrSize - 1 - i], LOW);
//                sleep(channel_data_.delay_time_ + (channel_data_.delay_time_ / 2));
            }
            for (int i = 1; i < (arrSize / 2) - 1; i++) {
                set(arrData[(arrSize / 2) - 1 - i], HIGH);
                set(arrData[(arrSize / 2) + i], HIGH);
                sleep(channel_data_.delay_time_ * 2);
                set(arrData[(arrSize / 2) - 1 - i], LOW);
                set(arrData[(arrSize / 2) + i], LOW);
//                sleep(channel_data_.delay_time_ + (channel_data_.delay_time_ / 2));
            }
        }

        ////////////////////////////////////////////////////////////////////////
        resetArray(arrData, arrSize, arrLastIndex);
        arrSize += shiftArrayAccordingToOrder(patternMode, 9, config_data_ptr_->header.pin_ptr_, config_data_ptr_->header.pin_size_, arrData, arrLastIndex, 1);
        arrSize += shiftArrayAccordingToOrder(patternMode, 9, config_data_ptr_->header.pin_ptr_, config_data_ptr_->header.pin_size_, arrData, arrLastIndex, 6);

        for (int j = 0; j < 3; j++) {
            for (int i = 0; i < arrSize / 2; i++) {
                set(arrData[i], HIGH);
                set(arrData[arrSize - 1 - i], HIGH);
                sleep(channel_data_.delay_time_ * 2);
                set(arrData[i], LOW);
                set(arrData[arrSize - 1 - i], LOW);
//                sleep(channel_data_.delay_time_ + (channel_data_.delay_time_ / 2));
            }
            for (int i = 1; i < (arrSize / 2) - 1; i++) {
                set(arrData[(arrSize / 2) - 1 - i], HIGH);
                set(arrData[(arrSize / 2) + i], HIGH);
                sleep(channel_data_.delay_time_ * 2);
                set(arrData[(arrSize / 2) - 1 - i], LOW);
                set(arrData[(arrSize / 2) + i], LOW);
//                sleep(channel_data_.delay_time_ + (channel_data_.delay_time_ / 2));
            }
        }

        ////////////////////////////////////////////////////////////////////////
        resetArray(arrData, arrSize, arrLastIndex);
        arrSize += shiftArrayAccordingToOrder(patternMode, 9, config_data_ptr_->header.pin_ptr_, config_data_ptr_->header.pin_size_, arrData, arrLastIndex, 2);
        arrSize += shiftArrayAccordingToOrder(patternMode, 9, config_data_ptr_->header.pin_ptr_, config_data_ptr_->header.pin_size_, arrData, arrLastIndex, 3);
        arrSize += shiftArrayAccordingToOrder(patternMode, 9, config_data_ptr_->header.pin_ptr_, config_data_ptr_->header.pin_size_, arrData, arrLastIndex, 4);
        arrSize += shiftArrayAccordingToOrder(patternMode, 9, config_data_ptr_->header.pin_ptr_, config_data_ptr_->header.pin_size_, arrData, arrLastIndex, 5);

        for (int j = 0; j < 3; j++) {
            for (int i = 0; i < arrSize / 2; i++) {
                set(arrData[i], HIGH);
                set(arrData[arrSize - 1 - i], HIGH);
                sleep(channel_data_.delay_time_ * 2);
                set(arrData[i], LOW);
                set(arrData[arrSize - 1 - i], LOW);
//                sleep(channel_data_.delay_time_ + (channel_data_.delay_time_ / 2));
            }
            for (int i = 1; i < (arrSize / 2) - 1; i++) {
                set(arrData[(arrSize / 2) - 1 - i], HIGH);
                set(arrData[(arrSize / 2) + i], HIGH);
                sleep(channel_data_.delay_time_ * 2);
                set(arrData[(arrSize / 2) - 1 - i], LOW);
                set(arrData[(arrSize / 2) + i], LOW);
//                sleep(channel_data_.delay_time_ + (channel_data_.delay_time_ / 2));
            }
        }

        ////////////////////////////////////////////////////////////////////////
        for (int j = 0; j < 2; j++) {
            resetArray(arrData, arrSize, arrLastIndex);
            arrSize += shiftArrayAccordingToOrder(patternMode, 9, config_data_ptr_->header.pin_ptr_, config_data_ptr_->header.pin_size_, arrData, arrLastIndex, 0);
            arrSize += shiftArrayAccordingToOrder(patternMode, 9, config_data_ptr_->header.pin_ptr_, config_data_ptr_->header.pin_size_, arrData, arrLastIndex, 1);
            arrSize += shiftArrayAccordingToOrder(patternMode, 9, config_data_ptr_->header.pin_ptr_, config_data_ptr_->header.pin_size_, arrData, arrLastIndex, 6);
            arrSize += shiftArrayAccordingToOrder(patternMode, 9, config_data_ptr_->header.pin_ptr_, config_data_ptr_->header.pin_size_, arrData, arrLastIndex, 7);

            for (int i = arrSize + 1; i >= 0; i--) {
                if (i < arrSize) set(arrData[i], HIGH);
                if (i < arrSize + 1 && i > 0) set(arrData[i - 1], HIGH);
                if (i < arrSize + 2 && i > 1) set(arrData[i - 2], HIGH);
                sleep(channel_data_.delay_time_ * 2);

                if (i < arrSize) set(arrData[i], LOW);
                if (i < arrSize + 1 && i > 0) set(arrData[i - 1], LOW);
                if (i < arrSize + 2 && i > 1) set(arrData[i - 2], LOW);
//                sleep(channel_data_.delay_time_);
            }

            resetArray(arrData, arrSize, arrLastIndex);
            arrSize += shiftArrayAccordingToOrder(patternMode, 9, config_data_ptr_->header.pin_ptr_, config_data_ptr_->header.pin_size_, arrData, arrLastIndex, 0);
            arrSize += shiftArrayAccordingToOrder(patternMode, 9, config_data_ptr_->header.pin_ptr_, config_data_ptr_->header.pin_size_, arrData, arrLastIndex, 1);
            arrSize += shiftArrayAccordingToOrder(patternMode, 9, config_data_ptr_->header.pin_ptr_, config_data_ptr_->header.pin_size_, arrData, arrLastIndex, 6);
            arrSize += shiftArrayAccordingToOrder(patternMode, 9, config_data_ptr_->header.pin_ptr_, config_data_ptr_->header.pin_size_, arrData, arrLastIndex, 7);

            for (int i = 0; i < arrSize + 2; i++) {
                if (i < arrSize) set(arrData[i], HIGH);
                if (i > 1 && i < arrSize + 2) set(arrData[i - 2], HIGH);
                if (i > 0 && i < arrSize + 1) set(arrData[i - 1], HIGH);
                sleep(channel_data_.delay_time_ * 2);

                if (i < arrSize) set(arrData[i], LOW);
                if (i > 1 && i < arrSize + 2) set(arrData[i - 2], LOW);
                if (i > 0 && i < arrSize + 1) set(arrData[i - 1], LOW);
//                sleep(channel_data_.delay_time_);
            }
        }

        ////////////////////////////////////////////////////////////////////////
        resetArray(arrData, arrSize, arrLastIndex);
        arrSize += shiftArrayAccordingToOrder(patternMode, 9, config_data_ptr_->header.pin_ptr_, config_data_ptr_->header.pin_size_, arrData, arrLastIndex, 3);
        arrSize += shiftArrayAccordingToOrder(patternMode, 9, config_data_ptr_->header.pin_ptr_, config_data_ptr_->header.pin_size_, arrData, arrLastIndex, 2, true);
        arrSize += shiftArrayAccordingToOrder(patternMode, 9, config_data_ptr_->header.pin_ptr_, config_data_ptr_->header.pin_size_, arrData, arrLastIndex, 6);
        arrSize += shiftArrayAccordingToOrder(patternMode, 9, config_data_ptr_->header.pin_ptr_, config_data_ptr_->header.pin_size_, arrData, arrLastIndex, 7);
        for (int i = arrSize + 1; i >= 0; i--) {
            if (i < arrSize) set(arrData[i], HIGH);
            if (i < arrSize + 1 && i > 0) set(arrData[i - 1], HIGH);
            if (i < arrSize + 2 && i > 1) set(arrData[i - 2], HIGH);
            sleep(channel_data_.delay_time_ * 2);

            if (i < arrSize) set(arrData[i], LOW);
            if (i < arrSize + 1 && i > 0) set(arrData[i - 1], LOW);
            if (i < arrSize + 2 && i > 1) set(arrData[i - 2], LOW);
//            sleep(channel_data_.delay_time_);
        }

        resetArray(arrData, arrSize, arrLastIndex);
        arrSize += shiftArrayAccordingToOrder(patternMode, 9, config_data_ptr_->header.pin_ptr_, config_data_ptr_->header.pin_size_, arrData, arrLastIndex, 0);
        arrSize += shiftArrayAccordingToOrder(patternMode, 9, config_data_ptr_->header.pin_ptr_, config_data_ptr_->header.pin_size_, arrData, arrLastIndex, 1);
        arrSize += shiftArrayAccordingToOrder(patternMode, 9, config_data_ptr_->header.pin_ptr_, config_data_ptr_->header.pin_size_, arrData, arrLastIndex, 5, true);
        arrSize += shiftArrayAccordingToOrder(patternMode, 9, config_data_ptr_->header.pin_ptr_, config_data_ptr_->header.pin_size_, arrData, arrLastIndex, 4);
        for (int i = 0; i < arrSize + 2; i++) {
            if (i < arrSize) set(arrData[i], HIGH);
            if (i > 1 && i < arrSize + 2) set(arrData[i - 2], HIGH);
            if (i > 0 && i < arrSize + 1) set(arrData[i - 1], HIGH);
            sleep(channel_data_.delay_time_ * 2);

            if (i < arrSize) set(arrData[i], LOW);
            if (i > 1 && i < arrSize + 2) set(arrData[i - 2], LOW);
            if (i > 0 && i < arrSize + 1) set(arrData[i - 1], LOW);
//            sleep(channel_data_.delay_time_);
        }

        //////////////////////////////////////////////////////////////////////
        for (int j = 0; j < 2; j++) {
            resetArray(arrData, arrSize, arrLastIndex);
            arrSize += shiftArrayAccordingToOrder(patternMode, 9, config_data_ptr_->header.pin_ptr_, config_data_ptr_->header.pin_size_, arrData, arrLastIndex, 7, true);
            arrSize += shiftArrayAccordingToOrder(patternMode, 9, config_data_ptr_->header.pin_ptr_, config_data_ptr_->header.pin_size_, arrData, arrLastIndex, 6, true);
            for (int i = 0; i < arrSize; i++) {
                set(arrData[i], HIGH);
                sleep(channel_data_.delay_time_ * 2);
                set(arrData[i], LOW);
//                sleep(channel_data_.delay_time_);
            }
            resetArray(arrData, arrSize, arrLastIndex);
            arrSize += shiftArrayAccordingToOrder(patternMode, 9, config_data_ptr_->header.pin_ptr_, config_data_ptr_->header.pin_size_, arrData, arrLastIndex, 5, true);
            arrSize += shiftArrayAccordingToOrder(patternMode, 9, config_data_ptr_->header.pin_ptr_, config_data_ptr_->header.pin_size_, arrData, arrLastIndex, 4, true);
            for (int i = 0; i < arrSize + 1; i++) {
                if (i < arrSize) set(arrData[i], HIGH);
                if (i > 0 && i < arrSize + 1) set(arrData[i - 1], HIGH);
                sleep(channel_data_.delay_time_ * 2);

                if (i < arrSize) set(arrData[i], LOW);
                if (i > 0 && i < arrSize + 1) set(arrData[i - 1], LOW);
//                sleep(channel_data_.delay_time_);
            }
            resetArray(arrData, arrSize, arrLastIndex);
            arrSize += shiftArrayAccordingToOrder(patternMode, 9, config_data_ptr_->header.pin_ptr_, config_data_ptr_->header.pin_size_, arrData, arrLastIndex, 3, true);
            arrSize += shiftArrayAccordingToOrder(patternMode, 9, config_data_ptr_->header.pin_ptr_, config_data_ptr_->header.pin_size_, arrData, arrLastIndex, 2, true);
            for (int i = 0; i < arrSize + 1; i++) {
                if (i < arrSize) set(arrData[i], HIGH);
                if (i > 0 && i < arrSize + 1) set(arrData[i - 1], HIGH);
                sleep(channel_data_.delay_time_ * 2);

                if (i < arrSize) set(arrData[i], LOW);
                if (i > 0 && i < arrSize + 1) set(arrData[i - 1], LOW);
//                sleep(channel_data_.delay_time_);
            }
            resetArray(arrData, arrSize, arrLastIndex);
            arrSize += shiftArrayAccordingToOrder(patternMode, 9, config_data_ptr_->header.pin_ptr_, config_data_ptr_->header.pin_size_, arrData, arrLastIndex, 1, true);
            arrSize += shiftArrayAccordingToOrder(patternMode, 9, config_data_ptr_->header.pin_ptr_, config_data_ptr_->header.pin_size_, arrData, arrLastIndex, 0, true);
            for (int i = 0; i < arrSize; i++) {
                set(arrData[i], HIGH);
                sleep(channel_data_.delay_time_ * 2);
                set(arrData[i], LOW);
//                sleep(channel_data_.delay_time_);
            }
        }

        ////////////////////////////////////////////////////////////////////////
        for (int j = 0; j < 2; j++) {
            resetArray(arrData, arrSize, arrLastIndex);
            arrSize += shiftArrayAccordingToOrder(patternMode, 9, config_data_ptr_->header.pin_ptr_, config_data_ptr_->header.pin_size_, arrData, arrLastIndex, 7, true);
            arrSize += shiftArrayAccordingToOrder(patternMode, 9, config_data_ptr_->header.pin_ptr_, config_data_ptr_->header.pin_size_, arrData, arrLastIndex, 6, true);
            arrSize += shiftArrayAccordingToOrder(patternMode, 9, config_data_ptr_->header.pin_ptr_, config_data_ptr_->header.pin_size_, arrData, arrLastIndex, 5, true);
            arrSize += shiftArrayAccordingToOrder(patternMode, 9, config_data_ptr_->header.pin_ptr_, config_data_ptr_->header.pin_size_, arrData, arrLastIndex, 4, true);

            arrSize += shiftArrayAccordingToOrder(patternMode, 9, config_data_ptr_->header.pin_ptr_, config_data_ptr_->header.pin_size_, arrData, arrLastIndex, 0);
            arrSize += shiftArrayAccordingToOrder(patternMode, 9, config_data_ptr_->header.pin_ptr_, config_data_ptr_->header.pin_size_, arrData, arrLastIndex, 1);
            arrSize += shiftArrayAccordingToOrder(patternMode, 9, config_data_ptr_->header.pin_ptr_, config_data_ptr_->header.pin_size_, arrData, arrLastIndex, 2);
            arrSize += shiftArrayAccordingToOrder(patternMode, 9, config_data_ptr_->header.pin_ptr_, config_data_ptr_->header.pin_size_, arrData, arrLastIndex, 3);

            for (int i = 0; i < (arrSize / 2) + 2; i++) {
                if (i < arrSize && i < arrSize / 2) {
                    set(arrData[i], HIGH);
                    set(arrData[i + (arrSize / 2)], HIGH);
                }
                if (i > 1 && i < arrSize + 2 && i < arrSize / 2 + 2) {
                    set(arrData[i - 2], HIGH);
                    set(arrData[i + (arrSize / 2) - 2], HIGH);
                }
                if (i > 0 && i < arrSize + 1 && i < arrSize / 2 + 1) {
                    set(arrData[i - 1], HIGH);
                    set(arrData[i + (arrSize / 2) - 1], HIGH);
                }
                sleep(channel_data_.delay_time_ * 2);

                if (i < arrSize && i < arrSize / 2) {
                    set(arrData[i], LOW);
                    set(arrData[i + (arrSize / 2)], LOW);
                }
                if (i > 1 && i < arrSize + 2 && i < arrSize / 2 + 2) {
                    set(arrData[i - 2], LOW);
                    set(arrData[i + (arrSize / 2) - 2], LOW);
                }
                if (i > 0 && i < arrSize + 1 && i < arrSize / 2 + 1) {
                    set(arrData[i - 1], LOW);
                    set(arrData[i + (arrSize / 2) - 1], LOW);
                }
//                sleep(channel_data_.delay_time_);
            }

            resetArray(arrData, arrSize, arrLastIndex);
            arrSize += shiftArrayAccordingToOrder(patternMode, 9, config_data_ptr_->header.pin_ptr_, config_data_ptr_->header.pin_size_, arrData, arrLastIndex, 4);
            arrSize += shiftArrayAccordingToOrder(patternMode, 9, config_data_ptr_->header.pin_ptr_, config_data_ptr_->header.pin_size_, arrData, arrLastIndex, 5);
            arrSize += shiftArrayAccordingToOrder(patternMode, 9, config_data_ptr_->header.pin_ptr_, config_data_ptr_->header.pin_size_, arrData, arrLastIndex, 6);
            arrSize += shiftArrayAccordingToOrder(patternMode, 9, config_data_ptr_->header.pin_ptr_, config_data_ptr_->header.pin_size_, arrData, arrLastIndex, 7);

            arrSize += shiftArrayAccordingToOrder(patternMode, 9, config_data_ptr_->header.pin_ptr_, config_data_ptr_->header.pin_size_, arrData, arrLastIndex, 3, true);
            arrSize += shiftArrayAccordingToOrder(patternMode, 9, config_data_ptr_->header.pin_ptr_, config_data_ptr_->header.pin_size_, arrData, arrLastIndex, 2, true);
            arrSize += shiftArrayAccordingToOrder(patternMode, 9, config_data_ptr_->header.pin_ptr_, config_data_ptr_->header.pin_size_, arrData, arrLastIndex, 1, true);
            arrSize += shiftArrayAccordingToOrder(patternMode, 9, config_data_ptr_->header.pin_ptr_, config_data_ptr_->header.pin_size_, arrData, arrLastIndex, 0, true);

            for (int i = 0; i < (arrSize / 2) + 2; i++) {
                if (i < arrSize && i < arrSize / 2) {
                    set(arrData[i], HIGH);
                    set(arrData[i + (arrSize / 2)], HIGH);
                }
                if (i > 1 && i < arrSize + 2 && i < arrSize / 2 + 2) {
                    set(arrData[i - 2], HIGH);
                    set(arrData[i + (arrSize / 2) - 2], HIGH);
                }
                if (i > 0 && i < arrSize + 1 && i < arrSize / 2 + 1) {
                    set(arrData[i - 1], HIGH);
                    set(arrData[i + (arrSize / 2) - 1], HIGH);
                }
                sleep(channel_data_.delay_time_ * 2);

                if (i < arrSize && i < arrSize / 2) {
                    set(arrData[i], LOW);
                    set(arrData[i + (arrSize / 2)], LOW);
                }
                if (i > 1 && i < arrSize + 2 && i < arrSize / 2 + 2) {
                    set(arrData[i - 2], LOW);
                    set(arrData[i + (arrSize / 2) - 2], LOW);
                }
                if (i > 0 && i < arrSize + 1 && i < arrSize / 2 + 1) {
                    set(arrData[i - 1], LOW);
                    set(arrData[i + (arrSize / 2) - 1], LOW);
                }
//                sleep(channel_data_.delay_time_);
            }
        }
    }

    void BaseChannel::taskSequence13BlinkPattern() {
        if (config_data_ptr_->header.pin_size_ < 8) return;

        const int patternMode[9][8] = {
                {3, 3, 3, 3, 3, 3, 3, 3},  // Untuk size 24
                {3, 2, 3, 3, 3, 3, 2, 3},  // Untuk size 22
                {3, 2, 2, 3, 3, 2, 2, 3},  // Untuk size 20
                {3, 2, 2, 2, 2, 2, 2, 3},  // Untuk size 18
                {3, 1, 2, 2, 2, 2, 1, 3},  // Untuk size 16
                {3, 1, 1, 2, 2, 1, 1, 3},  // Untuk size 14
                {3, 1, 1, 1, 1, 1, 1, 3},  // Untuk size 12
                {3, 0, 1, 1, 1, 1, 0, 3},  // Untuk size 10
                {3, 0, 0, 1, 1, 0, 0, 3}   // Untuk size 8
        };

        int arrData[config_data_ptr_->header.pin_size_];
        int arrSize = 0;
        int arrLastIndex = 0;

        for (int k = 0; k < 3; k++) {
            resetArray(arrData, arrSize, arrLastIndex);
            arrSize += shiftArrayAccordingToOrder(patternMode, 9, config_data_ptr_->header.pin_ptr_, config_data_ptr_->header.pin_size_, arrData, arrLastIndex, 3);
            arrSize += shiftArrayAccordingToOrder(patternMode, 9, config_data_ptr_->header.pin_ptr_, config_data_ptr_->header.pin_size_, arrData, arrLastIndex, 7);

            for (int j = 0; j < 3; j++) {
                for (int i = 0; i < arrSize; i++) {
                    set(arrData[i], HIGH);
                }
                sleep(channel_data_.delay_time_ * 2);
                for (int i = 0; i < arrSize; i++) {
                    set(arrData[i], LOW);
                }
                sleep(channel_data_.delay_time_ * 2);
            }
            sleep(channel_data_.delay_time_ * 2);

            resetArray(arrData, arrSize, arrLastIndex);
            arrSize += shiftArrayAccordingToOrder(patternMode, 9, config_data_ptr_->header.pin_ptr_, config_data_ptr_->header.pin_size_, arrData, arrLastIndex, 0);
            arrSize += shiftArrayAccordingToOrder(patternMode, 9, config_data_ptr_->header.pin_ptr_, config_data_ptr_->header.pin_size_, arrData, arrLastIndex, 4);

            for (int j = 0; j < 3; j++) {
                for (int i = 0; i < arrSize; i++) {
                    set(arrData[i], HIGH);
                }
                sleep(channel_data_.delay_time_ * 2);
                for (int i = 0; i < arrSize; i++) {
                    set(arrData[i], LOW);
                }
                sleep(channel_data_.delay_time_ * 2);
            }
            sleep(channel_data_.delay_time_ * 2);
        }

        ////////////////////////////////////////////////////////////////////////
        for (int k = 0; k < 3; k++) {
            resetArray(arrData, arrSize, arrLastIndex);
            arrSize += shiftArrayAccordingToOrder(patternMode, 9, config_data_ptr_->header.pin_ptr_, config_data_ptr_->header.pin_size_, arrData, arrLastIndex, 0);
            arrSize += shiftArrayAccordingToOrder(patternMode, 9, config_data_ptr_->header.pin_ptr_, config_data_ptr_->header.pin_size_, arrData, arrLastIndex, 3);
            arrSize += shiftArrayAccordingToOrder(patternMode, 9, config_data_ptr_->header.pin_ptr_, config_data_ptr_->header.pin_size_, arrData, arrLastIndex, 4);
            arrSize += shiftArrayAccordingToOrder(patternMode, 9, config_data_ptr_->header.pin_ptr_, config_data_ptr_->header.pin_size_, arrData, arrLastIndex, 7);

            for (int j = 0; j < 3; j++) {
                for (int i = 0; i < arrSize; i++) {
                    set(arrData[i], HIGH);
                }
                sleep(channel_data_.delay_time_ * 2);
                for (int i = 0; i < arrSize; i++) {
                    set(arrData[i], LOW);
                }
                sleep(channel_data_.delay_time_ * 2);
            }
            sleep(channel_data_.delay_time_ * 2);

            resetArray(arrData, arrSize, arrLastIndex);
            arrSize += shiftArrayAccordingToOrder(patternMode, 9, config_data_ptr_->header.pin_ptr_, config_data_ptr_->header.pin_size_, arrData, arrLastIndex, 1);
            arrSize += shiftArrayAccordingToOrder(patternMode, 9, config_data_ptr_->header.pin_ptr_, config_data_ptr_->header.pin_size_, arrData, arrLastIndex, 2);
            arrSize += shiftArrayAccordingToOrder(patternMode, 9, config_data_ptr_->header.pin_ptr_, config_data_ptr_->header.pin_size_, arrData, arrLastIndex, 5);
            arrSize += shiftArrayAccordingToOrder(patternMode, 9, config_data_ptr_->header.pin_ptr_, config_data_ptr_->header.pin_size_, arrData, arrLastIndex, 6);

            for (int j = 0; j < 3; j++) {
                for (int i = 0; i < arrSize; i++) {
                    set(arrData[i], HIGH);
                }
                sleep(channel_data_.delay_time_ * 2);
                for (int i = 0; i < arrSize; i++) {
                    set(arrData[i], LOW);
                }
                sleep(channel_data_.delay_time_ * 2);
            }
            sleep(channel_data_.delay_time_ * 2);
        }

        ////////////////////////////////////////////////////////////////////////
        int subArrData1[config_data_ptr_->header.pin_size_];
        int subArrSize1 = 0;
        int subArrLastIndex1 = 0;

        int subArrData2[config_data_ptr_->header.pin_size_];
        int subArrSize2 = 0;
        int subArrLastIndex2 = 0;

        int subArrData3[config_data_ptr_->header.pin_size_];
        int subArrSize3 = 0;
        int subArrLastIndex3 = 0;

        int subArrData4[config_data_ptr_->header.pin_size_];
        int subArrSize4 = 0;
        int subArrLastIndex4 = 0;

        resetArray(arrData, arrSize, arrLastIndex);
        resetArray(subArrData1, subArrSize1, subArrLastIndex1);
        resetArray(subArrData2, subArrSize2, subArrLastIndex2);
        resetArray(subArrData3, subArrSize3, subArrLastIndex3);
        resetArray(subArrData4, subArrSize4, subArrLastIndex4);

        subArrSize1 += shiftArrayAccordingToOrder(patternMode, 9, config_data_ptr_->header.pin_ptr_, config_data_ptr_->header.pin_size_, subArrData1, subArrLastIndex1, 0);
        subArrSize1 += shiftArrayAccordingToOrder(patternMode, 9, config_data_ptr_->header.pin_ptr_, config_data_ptr_->header.pin_size_, subArrData1, subArrLastIndex1, 1);

        subArrSize2 += shiftArrayAccordingToOrder(patternMode, 9, config_data_ptr_->header.pin_ptr_, config_data_ptr_->header.pin_size_, subArrData2, subArrLastIndex2, 3, true);
        subArrSize2 += shiftArrayAccordingToOrder(patternMode, 9, config_data_ptr_->header.pin_ptr_, config_data_ptr_->header.pin_size_, subArrData2, subArrLastIndex2, 2, true);

        subArrSize3 += shiftArrayAccordingToOrder(patternMode, 9, config_data_ptr_->header.pin_ptr_, config_data_ptr_->header.pin_size_, subArrData3, subArrLastIndex3, 4);
        subArrSize3 += shiftArrayAccordingToOrder(patternMode, 9, config_data_ptr_->header.pin_ptr_, config_data_ptr_->header.pin_size_, subArrData3, subArrLastIndex3, 5);

        subArrSize4 += shiftArrayAccordingToOrder(patternMode, 9, config_data_ptr_->header.pin_ptr_, config_data_ptr_->header.pin_size_, subArrData4, subArrLastIndex4, 7, true);
        subArrSize4 += shiftArrayAccordingToOrder(patternMode, 9, config_data_ptr_->header.pin_ptr_, config_data_ptr_->header.pin_size_, subArrData4, subArrLastIndex4, 6, true);

        arrSize = findMax(subArrSize1, subArrSize2, subArrSize3, subArrSize4, -1);

        for (int i = 0; i < arrSize; i++) {
            if (i < subArrSize1) set(subArrData1[i], HIGH);
            if (i < subArrSize2) set(subArrData2[i], HIGH);
            if (i < subArrSize3) set(subArrData3[i], HIGH);
            if (i < subArrSize4) set(subArrData4[i], HIGH);
            sleep(channel_data_.delay_time_ * 2);
            if (i < subArrSize1) set(subArrData1[i], LOW);
            if (i < subArrSize2) set(subArrData2[i], LOW);
            if (i < subArrSize3) set(subArrData3[i], LOW);
            if (i < subArrSize4) set(subArrData4[i], LOW);
//            sleep(channel_data_.delay_time_);
        }

        for (int j = 0; j < 2; j++) {
            for (int i = arrSize - 1; i >= 0; i--) {
                if (i < subArrSize1) set(subArrData1[i], HIGH);
                if (i < subArrSize2) set(subArrData2[i], HIGH);
                if (i < subArrSize3) set(subArrData3[i], HIGH);
                if (i < subArrSize4) set(subArrData4[i], HIGH);
                sleep(channel_data_.delay_time_ * 2);
            }
            for (int i = 0; i < arrSize; i++) {
                if (i < subArrSize1) set(subArrData1[i], LOW);
                if (i < subArrSize2) set(subArrData2[i], LOW);
                if (i < subArrSize3) set(subArrData3[i], LOW);
                if (i < subArrSize4) set(subArrData4[i], LOW);
                sleep(channel_data_.delay_time_ * 2);
            }
            sleep(channel_data_.delay_time_ * 3);
        }

        ////////////////////////////////////////////////////////////////////////
        for (int k = 0; k < 4; k++) {
            for (int j = 0; j < 8; j += 2) {
                resetArray(arrData, arrSize, arrLastIndex);
                arrSize += shiftArrayAccordingToOrder(patternMode, 9, config_data_ptr_->header.pin_ptr_, config_data_ptr_->header.pin_size_, arrData, arrLastIndex, j);
                arrSize += shiftArrayAccordingToOrder(patternMode, 9, config_data_ptr_->header.pin_ptr_, config_data_ptr_->header.pin_size_, arrData, arrLastIndex, j + 1);

                for (int i = 0; i < arrSize; i++) {
                    set(arrData[i], HIGH);
                }
                sleep(channel_data_.delay_time_ * 2);
                for (int i = 0; i < arrSize; i++) {
                    set(arrData[i], LOW);
                }
                sleep(channel_data_.delay_time_ / 2);
            }
            sleep(channel_data_.delay_time_ * 2);
        }

        ////////////////////////////////////////////////////////////////////////
        for (int i = 0; i < arrSize; i++) {
            for (int j = 0; j < 2; j++) {
                if (i < subArrSize1) set(subArrData1[i], HIGH);
                if (i < subArrSize2) set(subArrData2[i], HIGH);
                if (i < subArrSize3) set(subArrData3[i], HIGH);
                if (i < subArrSize4) set(subArrData4[i], HIGH);
                sleep(channel_data_.delay_time_);
                if (i < subArrSize1) set(subArrData1[i], LOW);
                if (i < subArrSize2) set(subArrData2[i], LOW);
                if (i < subArrSize3) set(subArrData3[i], LOW);
                if (i < subArrSize4) set(subArrData4[i], LOW);
                sleep(channel_data_.delay_time_);
            }
        }
        for (int i = arrSize - 1; i >= 0; i--) {
            for (int j = 0; j < 2; j++) {
                if (i < subArrSize1) set(subArrData1[i], HIGH);
                if (i < subArrSize2) set(subArrData2[i], HIGH);
                if (i < subArrSize3) set(subArrData3[i], HIGH);
                if (i < subArrSize4) set(subArrData4[i], HIGH);
                sleep(channel_data_.delay_time_);
                if (i < subArrSize1) set(subArrData1[i], LOW);
                if (i < subArrSize2) set(subArrData2[i], LOW);
                if (i < subArrSize3) set(subArrData3[i], LOW);
                if (i < subArrSize4) set(subArrData4[i], LOW);
                sleep(channel_data_.delay_time_);
            }
        }

        ////////////////////////////////////////////////////////////////////////
        resetArray(arrData, arrSize, arrLastIndex);
        resetArray(subArrData1, subArrSize1, subArrLastIndex1);
        resetArray(subArrData2, subArrSize2, subArrLastIndex2);

        subArrSize1 += shiftArrayAccordingToOrder(patternMode, 9, config_data_ptr_->header.pin_ptr_, config_data_ptr_->header.pin_size_, subArrData1, subArrLastIndex1, 7, true);
        subArrSize1 += shiftArrayAccordingToOrder(patternMode, 9, config_data_ptr_->header.pin_ptr_, config_data_ptr_->header.pin_size_, subArrData1, subArrLastIndex1, 6, true);

        subArrSize1 += shiftArrayAccordingToOrder(patternMode, 9, config_data_ptr_->header.pin_ptr_, config_data_ptr_->header.pin_size_, subArrData1, subArrLastIndex1, 1, true);
        subArrSize1 += shiftArrayAccordingToOrder(patternMode, 9, config_data_ptr_->header.pin_ptr_, config_data_ptr_->header.pin_size_, subArrData1, subArrLastIndex1, 0, true);

        subArrSize2 += shiftArrayAccordingToOrder(patternMode, 9, config_data_ptr_->header.pin_ptr_, config_data_ptr_->header.pin_size_, subArrData2, subArrLastIndex2, 4);
        subArrSize2 += shiftArrayAccordingToOrder(patternMode, 9, config_data_ptr_->header.pin_ptr_, config_data_ptr_->header.pin_size_, subArrData2, subArrLastIndex2, 5);

        subArrSize2 += shiftArrayAccordingToOrder(patternMode, 9, config_data_ptr_->header.pin_ptr_, config_data_ptr_->header.pin_size_, subArrData2, subArrLastIndex2, 2);
        subArrSize2 += shiftArrayAccordingToOrder(patternMode, 9, config_data_ptr_->header.pin_ptr_, config_data_ptr_->header.pin_size_, subArrData2, subArrLastIndex2, 3);

        arrSize = findMax(subArrSize1, subArrSize2, -1);

        for (int i = 0; i < arrSize; i++) {
            if (i < subArrSize1) set(subArrData1[i], HIGH);
            if (i < subArrSize2) set(subArrData2[i], HIGH);
            sleep(channel_data_.delay_time_ * 2);
        }
        for (int i = 0; i < arrSize; i++) {
            if (i < subArrSize1) set(subArrData1[i], LOW);
            if (i < subArrSize2) set(subArrData2[i], LOW);
            sleep(channel_data_.delay_time_ * 2);
        }

        ////////////////////////////////////////////////////////////////////////
        for (int k = 0; k < 3; k++) {
            resetArray(arrData, arrSize, arrLastIndex);
            arrSize += shiftArrayAccordingToOrder(patternMode, 9, config_data_ptr_->header.pin_ptr_, config_data_ptr_->header.pin_size_, arrData, arrLastIndex, 2);
            arrSize += shiftArrayAccordingToOrder(patternMode, 9, config_data_ptr_->header.pin_ptr_, config_data_ptr_->header.pin_size_, arrData, arrLastIndex, 3);
            arrSize += shiftArrayAccordingToOrder(patternMode, 9, config_data_ptr_->header.pin_ptr_, config_data_ptr_->header.pin_size_, arrData, arrLastIndex, 6);
            arrSize += shiftArrayAccordingToOrder(patternMode, 9, config_data_ptr_->header.pin_ptr_, config_data_ptr_->header.pin_size_, arrData, arrLastIndex, 7);

            for (int j = 0; j < 3; j++) {
                for (int i = 0; i < arrSize; i++) {
                    set(arrData[i], HIGH);
                }
                sleep(channel_data_.delay_time_ * 2);
                for (int i = 0; i < arrSize; i++) {
                    set(arrData[i], LOW);
                }
                sleep(channel_data_.delay_time_ * 2);
            }
            sleep(channel_data_.delay_time_ * 2);

            resetArray(arrData, arrSize, arrLastIndex);
            arrSize += shiftArrayAccordingToOrder(patternMode, 9, config_data_ptr_->header.pin_ptr_, config_data_ptr_->header.pin_size_, arrData, arrLastIndex, 0);
            arrSize += shiftArrayAccordingToOrder(patternMode, 9, config_data_ptr_->header.pin_ptr_, config_data_ptr_->header.pin_size_, arrData, arrLastIndex, 1);
            arrSize += shiftArrayAccordingToOrder(patternMode, 9, config_data_ptr_->header.pin_ptr_, config_data_ptr_->header.pin_size_, arrData, arrLastIndex, 4);
            arrSize += shiftArrayAccordingToOrder(patternMode, 9, config_data_ptr_->header.pin_ptr_, config_data_ptr_->header.pin_size_, arrData, arrLastIndex, 5);

            for (int j = 0; j < 3; j++) {
                for (int i = 0; i < arrSize; i++) {
                    set(arrData[i], HIGH);
                }
                sleep(channel_data_.delay_time_ * 2);
                for (int i = 0; i < arrSize; i++) {
                    set(arrData[i], LOW);
                }
                sleep(channel_data_.delay_time_ * 2);
            }
            sleep(channel_data_.delay_time_ * 2);
        }

        ////////////////////////////////////////////////////////////////////////
        resetArray(arrData, arrSize, arrLastIndex);
        arrSize += shiftArrayAccordingToOrder(patternMode, 9, config_data_ptr_->header.pin_ptr_, config_data_ptr_->header.pin_size_, arrData, arrLastIndex, 0);
        arrSize += shiftArrayAccordingToOrder(patternMode, 9, config_data_ptr_->header.pin_ptr_, config_data_ptr_->header.pin_size_, arrData, arrLastIndex, 1);
        arrSize += shiftArrayAccordingToOrder(patternMode, 9, config_data_ptr_->header.pin_ptr_, config_data_ptr_->header.pin_size_, arrData, arrLastIndex, 5, true);
        arrSize += shiftArrayAccordingToOrder(patternMode, 9, config_data_ptr_->header.pin_ptr_, config_data_ptr_->header.pin_size_, arrData, arrLastIndex, 4, true);

        for (int i = 0; i < arrSize + 2; i++) {
            if (i < arrSize) set(arrData[i], HIGH);
            if (i > 1 && i < arrSize + 2) set(arrData[i - 2], HIGH);
            if (i > 0 && i < arrSize + 1) set(arrData[i - 1], HIGH);
            sleep(channel_data_.delay_time_ * 3);

            if (i < arrSize) set(arrData[i], LOW);
            if (i > 1 && i < arrSize + 2) set(arrData[i - 2], LOW);
            if (i > 0 && i < arrSize + 1) set(arrData[i - 1], LOW);
//            sleep(channel_data_.delay_time_);
        }
        resetArray(arrData, arrSize, arrLastIndex);
        arrSize += shiftArrayAccordingToOrder(patternMode, 9, config_data_ptr_->header.pin_ptr_, config_data_ptr_->header.pin_size_, arrData, arrLastIndex, 7, true);
        arrSize += shiftArrayAccordingToOrder(patternMode, 9, config_data_ptr_->header.pin_ptr_, config_data_ptr_->header.pin_size_, arrData, arrLastIndex, 6, true);
        arrSize += shiftArrayAccordingToOrder(patternMode, 9, config_data_ptr_->header.pin_ptr_, config_data_ptr_->header.pin_size_, arrData, arrLastIndex, 2);
        arrSize += shiftArrayAccordingToOrder(patternMode, 9, config_data_ptr_->header.pin_ptr_, config_data_ptr_->header.pin_size_, arrData, arrLastIndex, 3);

        for (int i = 0; i < arrSize + 2; i++) {
            if (i < arrSize) set(arrData[i], HIGH);
            if (i > 1 && i < arrSize + 2) set(arrData[i - 2], HIGH);
            if (i > 0 && i < arrSize + 1) set(arrData[i - 1], HIGH);
            sleep(channel_data_.delay_time_ * 3);

            if (i < arrSize) set(arrData[i], LOW);
            if (i > 1 && i < arrSize + 2) set(arrData[i - 2], LOW);
            if (i > 0 && i < arrSize + 1) set(arrData[i - 1], LOW);
//            sleep(channel_data_.delay_time_);
        }

        ////////////////////////////////////////////////////////////////////////
        resetArray(arrData, arrSize, arrLastIndex);
        resetArray(subArrData1, subArrSize1, subArrLastIndex1);
        resetArray(subArrData2, subArrSize2, subArrLastIndex2);

        subArrSize1 += shiftArrayAccordingToOrder(patternMode, 9, config_data_ptr_->header.pin_ptr_, config_data_ptr_->header.pin_size_, subArrData1, subArrLastIndex1, 0);
        subArrSize1 += shiftArrayAccordingToOrder(patternMode, 9, config_data_ptr_->header.pin_ptr_, config_data_ptr_->header.pin_size_, subArrData1, subArrLastIndex1, 1);

        subArrSize2 += shiftArrayAccordingToOrder(patternMode, 9, config_data_ptr_->header.pin_ptr_, config_data_ptr_->header.pin_size_, subArrData2, subArrLastIndex2, 6);
        subArrSize2 += shiftArrayAccordingToOrder(patternMode, 9, config_data_ptr_->header.pin_ptr_, config_data_ptr_->header.pin_size_, subArrData2, subArrLastIndex2, 7);

        arrSize = findMax(subArrSize1, subArrSize2, -1);

        for (int j = 0; j < 2; j++) {
            for (int i = 0; i < arrSize; i++) {
                if (i < subArrSize1) set(subArrData1[i], HIGH);
                if (i < subArrSize2) set(subArrData2[i], HIGH);
                sleep(channel_data_.delay_time_ * 3);
            }
            for (int i = arrSize - 1; i >= 0; i--) {
                if (i < subArrSize1) set(subArrData1[i], LOW);
                if (i < subArrSize2) set(subArrData2[i], LOW);
                sleep(channel_data_.delay_time_ * 3);
            }
            for (int i = arrSize - 1; i >= 0; i--) {
                if (i < subArrSize1) set(subArrData1[i], HIGH);
                if (i < subArrSize2) set(subArrData2[i], HIGH);
                sleep(channel_data_.delay_time_ * 3);
            }
            for (int i = 0; i < arrSize; i++) {
                if (i < subArrSize1) set(subArrData1[i], LOW);
                if (i < subArrSize2) set(subArrData2[i], LOW);
                sleep(channel_data_.delay_time_ * 3);
            }
        }
    }

    void BaseChannel::taskSequence14AdvancedPattern() {
        const int size = config_data_ptr_->header.pin_size_;
        if (config_data_ptr_->header.pin_size_ < 8) return;

        const int patternMode[9][8] = {
                {3, 4, 4, 1, 1, 4, 4, 3},  // for size 24
                {3, 3, 4, 1, 1, 4, 3, 3},  // for size 22
                {3, 3, 3, 1, 1, 3, 3, 3},  // for size 20
                {3, 2, 3, 1, 1, 3, 2, 3},  // for size 18
                {3, 2, 2, 1, 1, 2, 2, 3},  // for size 16
                {3, 1, 2, 1, 1, 1, 2, 3},  // for size 14
                {3, 1, 1, 1, 1, 1, 1, 3},  // for size 12
                {3, 0, 1, 1, 1, 1, 0, 3},  // for size 10
                {3, 0, 0, 1, 1, 0, 0, 3}   // for size 8
        };

        int arrData[size];
        int arrSize = 0;
        int arrLastIndex = 0;

        for (int i = 0; i < size / 2; i++) {
            for (int j = 0; j < size / 2; j++) {
                set(config_data_ptr_->header.pin_ptr_[j], HIGH);
                set(config_data_ptr_->header.pin_ptr_[size - 1 - j], HIGH);
//                set(config_data_ptr_->header.pin_ptr_[j + (size / 2)], HIGH);
                sleep(channel_data_.delay_time_);
            }
            for (int j = 0; j < i; j++) {
                set(config_data_ptr_->header.pin_ptr_[j], HIGH);
                set(config_data_ptr_->header.pin_ptr_[size - 1 - j], HIGH);
                sleep(channel_data_.delay_time_ * 2);
                set(config_data_ptr_->header.pin_ptr_[j], LOW);
                set(config_data_ptr_->header.pin_ptr_[size - 1 - j], LOW);
                sleep(channel_data_.delay_time_ * 2);
            }
            set(config_data_ptr_->header.pin_ptr_[i], LOW);
            set(config_data_ptr_->header.pin_ptr_[size - 1 - i], LOW);
            sleep(channel_data_.delay_time_ * 2);
        }

        /////////////////////////////////////////////////////////////////////////

        for (int k = 0; k < 2; k++) {
            for (int i = 0; i < size / 2 + 3; i++) {
                if (i < size / 2) set(config_data_ptr_->header.pin_ptr_[i], HIGH);
                if (i > 2 && i < size / 2 + 3) set(config_data_ptr_->header.pin_ptr_[i - 3], HIGH);
                if (i > 1 && i < size / 2 + 2) set(config_data_ptr_->header.pin_ptr_[i - 2], HIGH);
                if (i > 0 && i < size / 2 + 1) set(config_data_ptr_->header.pin_ptr_[i - 1], HIGH);

                if (i < size / 2) set(config_data_ptr_->header.pin_ptr_[size - 1 - i], HIGH);
                if (i > 2 && i < size / 2 + 3) set(config_data_ptr_->header.pin_ptr_[size - 1 - i + 3], HIGH);
                if (i > 1 && i < size / 2 + 2) set(config_data_ptr_->header.pin_ptr_[size - 1 - i + 2], HIGH);
                if (i > 0 && i < size / 2 + 1) set(config_data_ptr_->header.pin_ptr_[size - 1 - i + 1], HIGH);
                sleep(channel_data_.delay_time_ * 2);
                if (i < size / 2) set(config_data_ptr_->header.pin_ptr_[i], LOW);
                if (i > 2 && i < size / 2 + 3) set(config_data_ptr_->header.pin_ptr_[i - 3], LOW);
                if (i > 1 && i < size / 2 + 2) set(config_data_ptr_->header.pin_ptr_[i - 2], LOW);
                if (i > 0 && i < size / 2 + 1) set(config_data_ptr_->header.pin_ptr_[i - 1], LOW);

                if (i < size / 2) set(config_data_ptr_->header.pin_ptr_[size - 1 - i], LOW);
                if (i > 2 && i < size / 2 + 3) set(config_data_ptr_->header.pin_ptr_[size - 1 - i + 3], LOW);
                if (i > 1 && i < size / 2 + 2) set(config_data_ptr_->header.pin_ptr_[size - 1 - i + 2], LOW);
                if (i > 0 && i < size / 2 + 1) set(config_data_ptr_->header.pin_ptr_[size - 1 - i + 1], LOW);
            }

            for (int i = 0; i < size / 2 + 3; i++) {
                if (i < size / 2) set(config_data_ptr_->header.pin_ptr_[(size / 2) - 1 - i], HIGH);
                if (i > 2 && i < size / 2 + 3) set(config_data_ptr_->header.pin_ptr_[(size / 2) - 1 - i + 3], HIGH);
                if (i > 1 && i < size / 2 + 2) set(config_data_ptr_->header.pin_ptr_[(size / 2) - 1 - i + 2], HIGH);
                if (i > 0 && i < size / 2 + 1) set(config_data_ptr_->header.pin_ptr_[(size / 2) - 1 - i + 1], HIGH);

                if (i < size / 2) set(config_data_ptr_->header.pin_ptr_[i + (size / 2)], HIGH);
                if (i > 2 && i < size / 2 + 3) set(config_data_ptr_->header.pin_ptr_[i + (size / 2) - 3], HIGH);
                if (i > 1 && i < size / 2 + 2) set(config_data_ptr_->header.pin_ptr_[i + (size / 2) - 2], HIGH);
                if (i > 0 && i < size / 2 + 1) set(config_data_ptr_->header.pin_ptr_[i + (size / 2) - 1], HIGH);
                sleep(channel_data_.delay_time_ * 2);
                if (i < size / 2) set(config_data_ptr_->header.pin_ptr_[(size / 2) - 1 - i], LOW);
                if (i > 2 && i < size / 2 + 3) set(config_data_ptr_->header.pin_ptr_[(size / 2) - 1 - i + 3], LOW);
                if (i > 1 && i < size / 2 + 2) set(config_data_ptr_->header.pin_ptr_[(size / 2) - 1 - i + 2], LOW);
                if (i > 0 && i < size / 2 + 1) set(config_data_ptr_->header.pin_ptr_[(size / 2) - 1 - i + 1], LOW);

                if (i < size / 2) set(config_data_ptr_->header.pin_ptr_[i + (size / 2)], LOW);
                if (i > 2 && i < size / 2 + 3) set(config_data_ptr_->header.pin_ptr_[i + (size / 2) - 3], LOW);
                if (i > 1 && i < size / 2 + 2) set(config_data_ptr_->header.pin_ptr_[i + (size / 2) - 2], LOW);
                if (i > 0 && i < size / 2 + 1) set(config_data_ptr_->header.pin_ptr_[i + (size / 2) - 1], LOW);
            }
        }

        /////////////////////////////////////////////////////////////////////////

        for (int j = size / 2 - 1; j >= 0; j--) {
            for (int i = 0; i < j + 3; i++) {
                if (i <= j) set(config_data_ptr_->header.pin_ptr_[i], HIGH);
                if (i > 2 && i <= j + 3) set(config_data_ptr_->header.pin_ptr_[i - 3], HIGH);
                if (i > 1 && i <= j + 2) set(config_data_ptr_->header.pin_ptr_[i - 2], HIGH);
                if (i > 0 && i <= j + 1) set(config_data_ptr_->header.pin_ptr_[i - 1], HIGH);

                if (i <= j) set(config_data_ptr_->header.pin_ptr_[size - 1 - i], HIGH);
                if (i > 2 && i <= j + 3) set(config_data_ptr_->header.pin_ptr_[size - 1 - i + 3], HIGH);
                if (i > 1 && i <= j + 2) set(config_data_ptr_->header.pin_ptr_[size - 1 - i + 2], HIGH);
                if (i > 0 && i <= j + 1) set(config_data_ptr_->header.pin_ptr_[size - 1 - i + 1], HIGH);
                sleep(channel_data_.delay_time_ * 2);
                if (i <= j) set(config_data_ptr_->header.pin_ptr_[i], LOW);
                if (i > 2 && i <= j + 3) set(config_data_ptr_->header.pin_ptr_[i - 3], LOW);
                if (i > 1 && i <= j + 2) set(config_data_ptr_->header.pin_ptr_[i - 2], LOW);
                if (i > 0 && i <= j + 1) set(config_data_ptr_->header.pin_ptr_[i - 1], LOW);

                if (i <= j) set(config_data_ptr_->header.pin_ptr_[size - 1 - i], LOW);
                if (i > 2 && i <= j + 3) set(config_data_ptr_->header.pin_ptr_[size - 1 - i + 3], LOW);
                if (i > 1 && i <= j + 2) set(config_data_ptr_->header.pin_ptr_[size - 1 - i + 2], LOW);
                if (i > 0 && i <= j + 1) set(config_data_ptr_->header.pin_ptr_[size - 1 - i + 1], LOW);
            }
            set(config_data_ptr_->header.pin_ptr_[j], HIGH);
            set(config_data_ptr_->header.pin_ptr_[size - 1 - j], HIGH);
        }

        for (int i = 0; i < size / 2 + 3; i++) {
            sleep(channel_data_.delay_time_ * 2);
            if (i < size / 2) set(config_data_ptr_->header.pin_ptr_[(size / 2) - 1 - i], LOW);
            if (i > 2 && i < size / 2 + 3) set(config_data_ptr_->header.pin_ptr_[(size / 2) - 1 - i + 3], LOW);
            if (i > 1 && i < size / 2 + 2) set(config_data_ptr_->header.pin_ptr_[(size / 2) - 1 - i + 2], LOW);
            if (i > 0 && i < size / 2 + 1) set(config_data_ptr_->header.pin_ptr_[(size / 2) - 1 - i + 1], LOW);

            if (i < size / 2) set(config_data_ptr_->header.pin_ptr_[i + (size / 2)], LOW);
            if (i > 2 && i < size / 2 + 3) set(config_data_ptr_->header.pin_ptr_[i + (size / 2) - 3], LOW);
            if (i > 1 && i < size / 2 + 2) set(config_data_ptr_->header.pin_ptr_[i + (size / 2) - 2], LOW);
            if (i > 0 && i < size / 2 + 1) set(config_data_ptr_->header.pin_ptr_[i + (size / 2) - 1], LOW);
        }

        ///////////////////////////////////////////////////////////////////////

        resetArray(arrData, arrSize, arrLastIndex);
        arrSize += shiftArrayAccordingToOrder(patternMode, 9, config_data_ptr_->header.pin_ptr_, size, arrData, arrLastIndex, 1, true);
        arrSize += shiftArrayAccordingToOrder(patternMode, 9, config_data_ptr_->header.pin_ptr_, size, arrData, arrLastIndex, 6);

        for (int i = 0; i < arrSize / 2; i++) {
            set(arrData[i], HIGH);
            set(arrData[arrSize / 2 + i], HIGH);
            sleep(channel_data_.delay_time_ * 3);
        }

        for (int i = 0; i < arrSize / 2; i++) {
            set(arrData[i], LOW);
            set(arrData[arrSize / 2 + i], LOW);
            sleep(channel_data_.delay_time_ * 3);
        }

        resetArray(arrData, arrSize, arrLastIndex);
        arrSize += shiftArrayAccordingToOrder(patternMode, 9, config_data_ptr_->header.pin_ptr_, size, arrData, arrLastIndex, 1);
        arrSize += shiftArrayAccordingToOrder(patternMode, 9, config_data_ptr_->header.pin_ptr_, size, arrData, arrLastIndex, 6, true);

        for (int i = 0; i < arrSize / 2; i++) {
            set(arrData[i], HIGH);
            set(arrData[arrSize / 2 + i], HIGH);
            sleep(channel_data_.delay_time_ * 3);
        }

        for (int i = 0; i < arrSize / 2; i++) {
            set(arrData[i], LOW);
            set(arrData[arrSize / 2 + i], LOW);
            sleep(channel_data_.delay_time_ * 3);
        }

        ///////////////////////////////////////////////////////////////////////

        resetArray(arrData, arrSize, arrLastIndex);
        arrSize += shiftArrayAccordingToOrder(patternMode, 9, config_data_ptr_->header.pin_ptr_, size, arrData, arrLastIndex, 7, true);
        arrSize += shiftArrayAccordingToOrder(patternMode, 9, config_data_ptr_->header.pin_ptr_, size, arrData, arrLastIndex, 6, true);
        arrSize += shiftArrayAccordingToOrder(patternMode, 9, config_data_ptr_->header.pin_ptr_, size, arrData, arrLastIndex, 5, true);
        arrSize += shiftArrayAccordingToOrder(patternMode, 9, config_data_ptr_->header.pin_ptr_, size, arrData, arrLastIndex, 4, true);
        arrSize += shiftArrayAccordingToOrder(patternMode, 9, config_data_ptr_->header.pin_ptr_, size, arrData, arrLastIndex, 3, true);
        arrSize += shiftArrayAccordingToOrder(patternMode, 9, config_data_ptr_->header.pin_ptr_, size, arrData, arrLastIndex, 2, true);
        arrSize += shiftArrayAccordingToOrder(patternMode, 9, config_data_ptr_->header.pin_ptr_, size, arrData, arrLastIndex, 1, true);
        arrSize += shiftArrayAccordingToOrder(patternMode, 9, config_data_ptr_->header.pin_ptr_, size, arrData, arrLastIndex, 0, true);

        for (int i = 0; i < arrSize + 3; i++) {
            if (i < arrSize) set(arrData[i], HIGH);
            if (i > 2 && i < arrSize + 3) set(arrData[i - 3], HIGH);
            if (i > 1 && i < arrSize + 2) set(arrData[i - 2], HIGH);
            if (i > 0 && i < arrSize + 1) set(arrData[i - 1], HIGH);
            sleep(channel_data_.delay_time_ * 2);
            if (i < arrSize) set(arrData[i], LOW);
            if (i > 2 && i < arrSize + 3) set(arrData[i - 3], LOW);
            if (i > 1 && i < arrSize + 2) set(arrData[i - 2], LOW);
            if (i > 0 && i < arrSize + 1) set(arrData[i - 1], LOW);
        }

        resetArray(arrData, arrSize, arrLastIndex);
        arrSize += shiftArrayAccordingToOrder(patternMode, 9, config_data_ptr_->header.pin_ptr_, size, arrData, arrLastIndex, 0);
        arrSize += shiftArrayAccordingToOrder(patternMode, 9, config_data_ptr_->header.pin_ptr_, size, arrData, arrLastIndex, 1);
        arrSize += shiftArrayAccordingToOrder(patternMode, 9, config_data_ptr_->header.pin_ptr_, size, arrData, arrLastIndex, 2);
        arrSize += shiftArrayAccordingToOrder(patternMode, 9, config_data_ptr_->header.pin_ptr_, size, arrData, arrLastIndex, 3);
        arrSize += shiftArrayAccordingToOrder(patternMode, 9, config_data_ptr_->header.pin_ptr_, size, arrData, arrLastIndex, 4);
        arrSize += shiftArrayAccordingToOrder(patternMode, 9, config_data_ptr_->header.pin_ptr_, size, arrData, arrLastIndex, 5);
        arrSize += shiftArrayAccordingToOrder(patternMode, 9, config_data_ptr_->header.pin_ptr_, size, arrData, arrLastIndex, 6);
        arrSize += shiftArrayAccordingToOrder(patternMode, 9, config_data_ptr_->header.pin_ptr_, size, arrData, arrLastIndex, 7);

        for (int i = 0; i < arrSize + 3; i++) {
            if (i < arrSize) set(arrData[i], HIGH);
            if (i > 2 && i < arrSize + 3) set(arrData[i - 3], HIGH);
            if (i > 1 && i < arrSize + 2) set(arrData[i - 2], HIGH);
            if (i > 0 && i < arrSize + 1) set(arrData[i - 1], HIGH);
            sleep(channel_data_.delay_time_ * 2);
            if (i < arrSize) set(arrData[i], LOW);
            if (i > 2 && i < arrSize + 3) set(arrData[i - 3], LOW);
            if (i > 1 && i < arrSize + 2) set(arrData[i - 2], LOW);
            if (i > 0 && i < arrSize + 1) set(arrData[i - 1], LOW);
        }

        /////////////////////////////////////////////////////////////////////////

        for (int i = 0; i < size / 2 + 3; i++) {
            if (i < size / 2) set(config_data_ptr_->header.pin_ptr_[i], HIGH);
            if (i > 2 && i < size / 2 + 3) set(config_data_ptr_->header.pin_ptr_[i - 3], HIGH);
            if (i > 1 && i < size / 2 + 2) set(config_data_ptr_->header.pin_ptr_[i - 2], HIGH);
            if (i > 0 && i < size / 2 + 1) set(config_data_ptr_->header.pin_ptr_[i - 1], HIGH);

            if (i < size / 2) set(config_data_ptr_->header.pin_ptr_[size - 1 - i], HIGH);
            if (i > 2 && i < size / 2 + 3) set(config_data_ptr_->header.pin_ptr_[size - 1 - i + 3], HIGH);
            if (i > 1 && i < size / 2 + 2) set(config_data_ptr_->header.pin_ptr_[size - 1 - i + 2], HIGH);
            if (i > 0 && i < size / 2 + 1) set(config_data_ptr_->header.pin_ptr_[size - 1 - i + 1], HIGH);
            sleep(channel_data_.delay_time_ * 2);
            if (i < size / 2) set(config_data_ptr_->header.pin_ptr_[i], LOW);
            if (i > 2 && i < size / 2 + 3) set(config_data_ptr_->header.pin_ptr_[i - 3], LOW);
            if (i > 1 && i < size / 2 + 2) set(config_data_ptr_->header.pin_ptr_[i - 2], LOW);
            if (i > 0 && i < size / 2 + 1) set(config_data_ptr_->header.pin_ptr_[i - 1], LOW);

            if (i < size / 2) set(config_data_ptr_->header.pin_ptr_[size - 1 - i], LOW);
            if (i > 2 && i < size / 2 + 3) set(config_data_ptr_->header.pin_ptr_[size - 1 - i + 3], LOW);
            if (i > 1 && i < size / 2 + 2) set(config_data_ptr_->header.pin_ptr_[size - 1 - i + 2], LOW);
            if (i > 0 && i < size / 2 + 1) set(config_data_ptr_->header.pin_ptr_[size - 1 - i + 1], LOW);
        }

        for (int i = 0; i < size / 2 + 3; i++) {
            if (i < size / 2) set(config_data_ptr_->header.pin_ptr_[(size / 2) - 1 - i], HIGH);
            if (i > 2 && i < size / 2 + 3) set(config_data_ptr_->header.pin_ptr_[(size / 2) - 1 - i + 3], HIGH);
            if (i > 1 && i < size / 2 + 2) set(config_data_ptr_->header.pin_ptr_[(size / 2) - 1 - i + 2], HIGH);
            if (i > 0 && i < size / 2 + 1) set(config_data_ptr_->header.pin_ptr_[(size / 2) - 1 - i + 1], HIGH);

            if (i < size / 2) set(config_data_ptr_->header.pin_ptr_[i + (size / 2)], HIGH);
            if (i > 2 && i < size / 2 + 3) set(config_data_ptr_->header.pin_ptr_[i + (size / 2) - 3], HIGH);
            if (i > 1 && i < size / 2 + 2) set(config_data_ptr_->header.pin_ptr_[i + (size / 2) - 2], HIGH);
            if (i > 0 && i < size / 2 + 1) set(config_data_ptr_->header.pin_ptr_[i + (size / 2) - 1], HIGH);
            sleep(channel_data_.delay_time_ * 2);
            if (i < size / 2) set(config_data_ptr_->header.pin_ptr_[(size / 2) - 1 - i], LOW);
            if (i > 2 && i < size / 2 + 3) set(config_data_ptr_->header.pin_ptr_[(size / 2) - 1 - i + 3], LOW);
            if (i > 1 && i < size / 2 + 2) set(config_data_ptr_->header.pin_ptr_[(size / 2) - 1 - i + 2], LOW);
            if (i > 0 && i < size / 2 + 1) set(config_data_ptr_->header.pin_ptr_[(size / 2) - 1 - i + 1], LOW);

            if (i < size / 2) set(config_data_ptr_->header.pin_ptr_[i + (size / 2)], LOW);
            if (i > 2 && i < size / 2 + 3) set(config_data_ptr_->header.pin_ptr_[i + (size / 2) - 3], LOW);
            if (i > 1 && i < size / 2 + 2) set(config_data_ptr_->header.pin_ptr_[i + (size / 2) - 2], LOW);
            if (i > 0 && i < size / 2 + 1) set(config_data_ptr_->header.pin_ptr_[i + (size / 2) - 1], LOW);
        }

        ////////////////////////////////////////////////////////////////////////
        int subArrData1[size];
        int subArrSize1 = 0;
        int subArrLastIndex1 = 0;

        int subArrData2[size];
        int subArrSize2 = 0;
        int subArrLastIndex2 = 0;

        resetArray(subArrData1, subArrSize1, subArrLastIndex1);
        subArrSize1 += shiftArrayAccordingToOrder(patternMode, 9, config_data_ptr_->header.pin_ptr_, size, subArrData1, subArrLastIndex1, 0);
        subArrSize1 += shiftArrayAccordingToOrder(patternMode, 9, config_data_ptr_->header.pin_ptr_, size, subArrData1, subArrLastIndex1, 1);

        resetArray(subArrData2, subArrSize2, subArrLastIndex2);
        subArrSize2 += shiftArrayAccordingToOrder(patternMode, 9, config_data_ptr_->header.pin_ptr_, size, subArrData2, subArrLastIndex2, 4);
        subArrSize2 += shiftArrayAccordingToOrder(patternMode, 9, config_data_ptr_->header.pin_ptr_, size, subArrData2, subArrLastIndex2, 5);

        for (int i = 0; i < subArrSize1 + 2; i++) {
            if (i < subArrSize1) set(subArrData1[i], HIGH);
            if (i > 1 && i < subArrSize1 + 2) set(subArrData1[i - 2], HIGH);
            if (i > 0 && i < subArrSize1 + 1) set(subArrData1[i - 1], HIGH);

            if (i < subArrSize2) set(subArrData2[i], HIGH);
            if (i > 1 && i < subArrSize2 + 2) set(subArrData2[i - 2], HIGH);
            if (i > 0 && i < subArrSize2 + 1) set(subArrData2[i - 1], HIGH);
            sleep(channel_data_.delay_time_ * 2);
            if (i < subArrSize1) set(subArrData1[i], LOW);
            if (i > 1 && i < subArrSize1 + 2) set(subArrData1[i - 2], LOW);
            if (i > 0 && i < subArrSize1 + 1) set(subArrData1[i - 1], LOW);

            if (i < subArrSize2) set(subArrData2[i], LOW);
            if (i > 1 && i < subArrSize2 + 2) set(subArrData2[i - 2], LOW);
            if (i > 0 && i < subArrSize2 + 1) set(subArrData2[i - 1], LOW);
        }

        resetArray(subArrData1, subArrSize1, subArrLastIndex1);
        subArrSize1 += shiftArrayAccordingToOrder(patternMode, 9, config_data_ptr_->header.pin_ptr_, size, subArrData1, subArrLastIndex1, 6);
        subArrSize1 += shiftArrayAccordingToOrder(patternMode, 9, config_data_ptr_->header.pin_ptr_, size, subArrData1, subArrLastIndex1, 7);

        resetArray(subArrData2, subArrSize2, subArrLastIndex2);
        subArrSize2 += shiftArrayAccordingToOrder(patternMode, 9, config_data_ptr_->header.pin_ptr_, size, subArrData2, subArrLastIndex2, 2);
        subArrSize2 += shiftArrayAccordingToOrder(patternMode, 9, config_data_ptr_->header.pin_ptr_, size, subArrData2, subArrLastIndex2, 3);

        for (int i = 0; i < subArrSize1 + 2; i++) {
            if (i < subArrSize1) set(subArrData1[i], HIGH);
            if (i > 1 && i < subArrSize1 + 2) set(subArrData1[i - 2], HIGH);
            if (i > 0 && i < subArrSize1 + 1) set(subArrData1[i - 1], HIGH);

            if (i < subArrSize2) set(subArrData2[i], HIGH);
            if (i > 1 && i < subArrSize2 + 2) set(subArrData2[i - 2], HIGH);
            if (i > 0 && i < subArrSize2 + 1) set(subArrData2[i - 1], HIGH);
            sleep(channel_data_.delay_time_ * 2);
            if (i < subArrSize1) set(subArrData1[i], LOW);
            if (i > 1 && i < subArrSize1 + 2) set(subArrData1[i - 2], LOW);
            if (i > 0 && i < subArrSize1 + 1) set(subArrData1[i - 1], LOW);

            if (i < subArrSize2) set(subArrData2[i], LOW);
            if (i > 1 && i < subArrSize2 + 2) set(subArrData2[i - 2], LOW);
            if (i > 0 && i < subArrSize2 + 1) set(subArrData2[i - 1], LOW);
        }

        ////////////////////////////////////////////////////////////////////////
        resetArray(subArrData1, subArrSize1, subArrLastIndex1);
        subArrSize1 += shiftArrayAccordingToOrder(patternMode, 9, config_data_ptr_->header.pin_ptr_, size, subArrData1, subArrLastIndex1, 7, true);
        subArrSize1 += shiftArrayAccordingToOrder(patternMode, 9, config_data_ptr_->header.pin_ptr_, size, subArrData1, subArrLastIndex1, 6, true);

        resetArray(subArrData2, subArrSize2, subArrLastIndex2);
        subArrSize2 += shiftArrayAccordingToOrder(patternMode, 9, config_data_ptr_->header.pin_ptr_, size, subArrData2, subArrLastIndex2, 3, true);
        subArrSize2 += shiftArrayAccordingToOrder(patternMode, 9, config_data_ptr_->header.pin_ptr_, size, subArrData2, subArrLastIndex2, 2, true);

        for (int i = 0; i < subArrSize1 + 2; i++) {
            if (i < subArrSize1) set(subArrData1[i], HIGH);
            if (i > 1 && i < subArrSize1 + 2) set(subArrData1[i - 2], HIGH);
            if (i > 0 && i < subArrSize1 + 1) set(subArrData1[i - 1], HIGH);

            if (i < subArrSize2) set(subArrData2[i], HIGH);
            if (i > 1 && i < subArrSize2 + 2) set(subArrData2[i - 2], HIGH);
            if (i > 0 && i < subArrSize2 + 1) set(subArrData2[i - 1], HIGH);
            sleep(channel_data_.delay_time_ * 2);
            if (i < subArrSize1) set(subArrData1[i], LOW);
            if (i > 1 && i < subArrSize1 + 2) set(subArrData1[i - 2], LOW);
            if (i > 0 && i < subArrSize1 + 1) set(subArrData1[i - 1], LOW);

            if (i < subArrSize2) set(subArrData2[i], LOW);
            if (i > 1 && i < subArrSize2 + 2) set(subArrData2[i - 2], LOW);
            if (i > 0 && i < subArrSize2 + 1) set(subArrData2[i - 1], LOW);
        }

        resetArray(subArrData1, subArrSize1, subArrLastIndex1);
        subArrSize1 += shiftArrayAccordingToOrder(patternMode, 9, config_data_ptr_->header.pin_ptr_, size, subArrData1, subArrLastIndex1, 1, true);
        subArrSize1 += shiftArrayAccordingToOrder(patternMode, 9, config_data_ptr_->header.pin_ptr_, size, subArrData1, subArrLastIndex1, 0, true);

        resetArray(subArrData2, subArrSize2, subArrLastIndex2);
        subArrSize2 += shiftArrayAccordingToOrder(patternMode, 9, config_data_ptr_->header.pin_ptr_, size, subArrData2, subArrLastIndex2, 5, true);
        subArrSize2 += shiftArrayAccordingToOrder(patternMode, 9, config_data_ptr_->header.pin_ptr_, size, subArrData2, subArrLastIndex2, 4, true);

        for (int i = 0; i < subArrSize1 + 2; i++) {
            if (i < subArrSize1) set(subArrData1[i], HIGH);
            if (i > 1 && i < subArrSize1 + 2) set(subArrData1[i - 2], HIGH);
            if (i > 0 && i < subArrSize1 + 1) set(subArrData1[i - 1], HIGH);

            if (i < subArrSize2) set(subArrData2[i], HIGH);
            if (i > 1 && i < subArrSize2 + 2) set(subArrData2[i - 2], HIGH);
            if (i > 0 && i < subArrSize2 + 1) set(subArrData2[i - 1], HIGH);
            sleep(channel_data_.delay_time_ * 2);
            if (i < subArrSize1) set(subArrData1[i], LOW);
            if (i > 1 && i < subArrSize1 + 2) set(subArrData1[i - 2], LOW);
            if (i > 0 && i < subArrSize1 + 1) set(subArrData1[i - 1], LOW);

            if (i < subArrSize2) set(subArrData2[i], LOW);
            if (i > 1 && i < subArrSize2 + 2) set(subArrData2[i - 2], LOW);
            if (i > 0 && i < subArrSize2 + 1) set(subArrData2[i - 1], LOW);
        }

        ////////////////////////////////////////////////////////////////////////
        for (int i = 0; i < size / 2 + 3; i++) {
            if (i < size / 2) set(config_data_ptr_->header.pin_ptr_[i], HIGH);
            if (i > 2 && i < size / 2 + 3) set(config_data_ptr_->header.pin_ptr_[i - 3], HIGH);
            if (i > 1 && i < size / 2 + 2) set(config_data_ptr_->header.pin_ptr_[i - 2], HIGH);
            if (i > 0 && i < size / 2 + 1) set(config_data_ptr_->header.pin_ptr_[i - 1], HIGH);

            if (i < size / 2) set(config_data_ptr_->header.pin_ptr_[size - 1 - i], HIGH);
            if (i > 2 && i < size / 2 + 3) set(config_data_ptr_->header.pin_ptr_[size - 1 - i + 3], HIGH);
            if (i > 1 && i < size / 2 + 2) set(config_data_ptr_->header.pin_ptr_[size - 1 - i + 2], HIGH);
            if (i > 0 && i < size / 2 + 1) set(config_data_ptr_->header.pin_ptr_[size - 1 - i + 1], HIGH);
            sleep(channel_data_.delay_time_ * 2);
            if (i < size / 2) set(config_data_ptr_->header.pin_ptr_[i], LOW);
            if (i > 2 && i < size / 2 + 3) set(config_data_ptr_->header.pin_ptr_[i - 3], LOW);
            if (i > 1 && i < size / 2 + 2) set(config_data_ptr_->header.pin_ptr_[i - 2], LOW);
            if (i > 0 && i < size / 2 + 1) set(config_data_ptr_->header.pin_ptr_[i - 1], LOW);

            if (i < size / 2) set(config_data_ptr_->header.pin_ptr_[size - 1 - i], LOW);
            if (i > 2 && i < size / 2 + 3) set(config_data_ptr_->header.pin_ptr_[size - 1 - i + 3], LOW);
            if (i > 1 && i < size / 2 + 2) set(config_data_ptr_->header.pin_ptr_[size - 1 - i + 2], LOW);
            if (i > 0 && i < size / 2 + 1) set(config_data_ptr_->header.pin_ptr_[size - 1 - i + 1], LOW);
        }

        for (int i = 0; i < size / 2 + 3; i++) {
            if (i < size / 2) set(config_data_ptr_->header.pin_ptr_[(size / 2) - 1 - i], HIGH);
            if (i > 2 && i < size / 2 + 3) set(config_data_ptr_->header.pin_ptr_[(size / 2) - 1 - i + 3], HIGH);
            if (i > 1 && i < size / 2 + 2) set(config_data_ptr_->header.pin_ptr_[(size / 2) - 1 - i + 2], HIGH);
            if (i > 0 && i < size / 2 + 1) set(config_data_ptr_->header.pin_ptr_[(size / 2) - 1 - i + 1], HIGH);

            if (i < size / 2) set(config_data_ptr_->header.pin_ptr_[i + (size / 2)], HIGH);
            if (i > 2 && i < size / 2 + 3) set(config_data_ptr_->header.pin_ptr_[i + (size / 2) - 3], HIGH);
            if (i > 1 && i < size / 2 + 2) set(config_data_ptr_->header.pin_ptr_[i + (size / 2) - 2], HIGH);
            if (i > 0 && i < size / 2 + 1) set(config_data_ptr_->header.pin_ptr_[i + (size / 2) - 1], HIGH);
            sleep(channel_data_.delay_time_ * 2);
            if (i < size / 2) set(config_data_ptr_->header.pin_ptr_[(size / 2) - 1 - i], LOW);
            if (i > 2 && i < size / 2 + 3) set(config_data_ptr_->header.pin_ptr_[(size / 2) - 1 - i + 3], LOW);
            if (i > 1 && i < size / 2 + 2) set(config_data_ptr_->header.pin_ptr_[(size / 2) - 1 - i + 2], LOW);
            if (i > 0 && i < size / 2 + 1) set(config_data_ptr_->header.pin_ptr_[(size / 2) - 1 - i + 1], LOW);

            if (i < size / 2) set(config_data_ptr_->header.pin_ptr_[i + (size / 2)], LOW);
            if (i > 2 && i < size / 2 + 3) set(config_data_ptr_->header.pin_ptr_[i + (size / 2) - 3], LOW);
            if (i > 1 && i < size / 2 + 2) set(config_data_ptr_->header.pin_ptr_[i + (size / 2) - 2], LOW);
            if (i > 0 && i < size / 2 + 1) set(config_data_ptr_->header.pin_ptr_[i + (size / 2) - 1], LOW);
        }
    }

    void BaseChannel::taskSequence15AllSequences() {
        taskSequence2BlinkAll();
        taskSequence3FillTwoPoint();
        taskSequence4FillRight();
        taskSequence5FillIn();
        taskSequence6BlinkOneByOne();
        taskSequence7BlinkTwoFill();
        taskSequence8SnakeAndReverse();
        taskSequence9Random();
        taskSequence10Wave();
        taskSequence11Complex();
        taskSequence12PatternMatrix();
        taskSequence13BlinkPattern();
        taskSequence14AdvancedPattern();
    }
}
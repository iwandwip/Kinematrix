/*
 *  BaseChannelSequence.cpp
 *
 *  Kastara Electronics Embedded Development
 *  Created on: 2023. 4. 3
 */

#include "BaseChannel.h"

namespace AutoLight {
    void BaseChannel::taskSequence0() {
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
    }

    void BaseChannel::taskSequence1() {
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
    }

    void BaseChannel::taskSequence2() {
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
    }

    void BaseChannel::taskSequence3() {
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
    }

    void BaseChannel::taskSequence4() {
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
    }

    void BaseChannel::taskSequence5() {
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
    }

    void BaseChannel::taskSequence6() {
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
    }

    void BaseChannel::taskSequence7() {
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
    }

    void BaseChannel::taskSequence8() {
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
    }

//    void BaseChannel::taskSequence9() {
//        static const int greenMaxAreaSize = 8;
//        static int size = config_data_ptr_->header.pin_size_;
//        static uint8_t *pin = config_data_ptr_->header.pin_ptr_;
//        static void (BaseChannel::*setPtr)(uint8_t, uint8_t) = &BaseChannel::set;
//
//        static TaskHandle task;
//        static auto taskLed1 = task.createTask(1024, [](void *pvParameter) -> void {
//            for (;;) {
//                task.delay(100);
//            }
//        });

//        static int greenStart = (size - 8) / 2;
//        static int greenEnd = greenStart + 8;
//        Serial.print("[");
//        for (int i = greenStart; i < greenEnd; ++i) {
//            Serial.print(pin[i]);
//            Serial.print(" ");
//        }
//        Serial.print("]");
//
//        static int blueStart = ((size - 8) / 2) - 1;
//        static int blueEnd = (blueStart + 8) + 1;
//        static int blueIndex0 = blueStart - 1;
//        static int blueIndex1 = blueStart;
//        static int blueIndex2 = blueEnd;
//        static int blueIndex3 = blueEnd + 1;
//        Serial.print("[");
//        Serial.print(pin[blueIndex0]);
//        Serial.print(" ");
//        Serial.print(pin[blueIndex1]);
//        Serial.print(" ");
//        Serial.print(pin[blueIndex2]);
//        Serial.print(" ");
//        Serial.print(pin[blueIndex3]);
//        Serial.print(" ");
//        Serial.print("]");
//        Serial.println();

//        int zero = 0;
//        int full = config_data_ptr_->header.pin_size_;
//        int half = config_data_ptr_->header.pin_size_ / 2;
//        int quarter = half / 2;
//        int delayHalf = channel_data_.delay_time_ / 2;
//
//        int delay = channel_data_.delay_time_;
//        bool isEven = (half % 2 == 0);
//
//        static bool isReverse = false;
//        static uint8_t count_1 = 0;
//
//        isReverse ^= 1;
//        if (count_1 >= 4) count_1 = 0;
//        else count_1 += 1;
//
//        Serial.print(count_1);
//        Serial.println();
//
//        int startPinIndex = (count_1 >= 1 && count_1 <= 2) ? zero : zero + 1;
//        int endPinIndex = (count_1 >= 1 && count_1 <= 2) ? full - 1 : full - 2;
//        int thirdState = (isReverse);
//
//        for (int i = 0; i < quarter - 1; ++i) {
//            for (int j = 0; j < 2; ++j) {
//                switch (i) {
//                    case 0:
//                    case 1:
//                        set(config_data_ptr_->header.pin_ptr_[startPinIndex], LOW);
//                        set(config_data_ptr_->header.pin_ptr_[endPinIndex], LOW);
//                        break;
//                    case 2:
//                    case 3:
//                        set(config_data_ptr_->header.pin_ptr_[startPinIndex], HIGH);
//                        set(config_data_ptr_->header.pin_ptr_[endPinIndex], HIGH);
//                        break;
//                    case 4:
//                    case 5:
//                        set(config_data_ptr_->header.pin_ptr_[startPinIndex], LOW);
//                        set(config_data_ptr_->header.pin_ptr_[endPinIndex], LOW);
//                        break;
//                }
//
//                if (thirdState) {
//                    set(config_data_ptr_->header.pin_ptr_[zero + 2], HIGH);
//                    set(config_data_ptr_->header.pin_ptr_[full - 3], HIGH);
//                } else {
//                    set(config_data_ptr_->header.pin_ptr_[zero + 2], LOW);
//                    set(config_data_ptr_->header.pin_ptr_[full - 3], LOW);
//                }
//
//                set(config_data_ptr_->header.pin_ptr_[(quarter - 1) + i], HIGH);
//                set(config_data_ptr_->header.pin_ptr_[(full) - quarter - i], HIGH);
//                sleep(delayHalf / 2);
//
//                set(config_data_ptr_->header.pin_ptr_[(quarter - 1) + i], LOW);
//                set(config_data_ptr_->header.pin_ptr_[(full) - quarter - i], LOW);
//                sleep(delayHalf / 2);
//            }
//        }
//
//        for (int i = 0; i < quarter - 1; ++i) {
//            for (int j = 0; j < 2; ++j) {
//                switch (i) {
//                    case 0:
//                    case 1:
//                        set(config_data_ptr_->header.pin_ptr_[startPinIndex], LOW);
//                        set(config_data_ptr_->header.pin_ptr_[endPinIndex], LOW);
//                        break;
//                    case 2:
//                    case 3:
//                        set(config_data_ptr_->header.pin_ptr_[startPinIndex], HIGH);
//                        set(config_data_ptr_->header.pin_ptr_[endPinIndex], HIGH);
//                        break;
//                    case 4:
//                    case 5:
//                        set(config_data_ptr_->header.pin_ptr_[startPinIndex], LOW);
//                        set(config_data_ptr_->header.pin_ptr_[endPinIndex], LOW);
//                        break;
//                }
//
//                if (thirdState) {
//                    set(config_data_ptr_->header.pin_ptr_[zero + 2], HIGH);
//                    set(config_data_ptr_->header.pin_ptr_[full - 3], HIGH);
//                } else {
//                    set(config_data_ptr_->header.pin_ptr_[zero + 2], LOW);
//                    set(config_data_ptr_->header.pin_ptr_[full - 3], LOW);
//                }
//
//                set(config_data_ptr_->header.pin_ptr_[(quarter + 3) - i], HIGH);
//                set(config_data_ptr_->header.pin_ptr_[(half + 2) + i], HIGH);
//                sleep(delayHalf / 2);
//
//                set(config_data_ptr_->header.pin_ptr_[(quarter + 3) - i], LOW);
//                set(config_data_ptr_->header.pin_ptr_[(half + 2) + i], LOW);
//                sleep(delayHalf / 2);
//            }
//        }
//    }

}
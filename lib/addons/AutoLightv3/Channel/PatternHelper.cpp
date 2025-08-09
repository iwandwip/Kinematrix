#include "PatternHelper.h"

namespace AutoLight {

PatternHelper::PatternHelper(BaseChannel* ch) : channel_(ch) {
    size_ = ch->config_data_ptr_->header.pin_size_;
    pins_ = ch->config_data_ptr_->header.pin_ptr_;
    delay_ = ch->channel_data_.delay_time_;
}

void PatternHelper::setRange(int start, int end, bool state) {
    for (int i = start; i <= end; i++) {
        channel_->set(pins_[i], state);
    }
}

void PatternHelper::setSymmetric(int index, bool state) {
    channel_->set(pins_[index], state);
    channel_->set(pins_[size_ - 1 - index], state);
}

void PatternHelper::setMirror(int left, int right, bool state) {
    channel_->set(pins_[left], state);
    channel_->set(pins_[right], state);
}

void PatternHelper::blinkRange(int start, int end, int times) {
    for (int t = 0; t < times; t++) {
        setRange(start, end, HIGH);
        channel_->sleep(delay_);
        setRange(start, end, LOW);
        channel_->sleep(delay_);
    }
}

void PatternHelper::wave(int start, int end, int step_delay) {
    int actual_delay = step_delay > 0 ? step_delay : delay_;
    if (start <= end) {
        for (int i = start; i <= end; i++) {
            channel_->set(pins_[i], HIGH);
            channel_->sleep(actual_delay);
        }
        for (int i = start; i <= end; i++) {
            channel_->set(pins_[i], LOW);
            channel_->sleep(actual_delay);
        }
    } else {
        for (int i = start; i >= end; i--) {
            channel_->set(pins_[i], HIGH);
            channel_->sleep(actual_delay);
        }
        for (int i = start; i >= end; i--) {
            channel_->set(pins_[i], LOW);
            channel_->sleep(actual_delay);
        }
    }
}

void PatternHelper::ripple(int center, int speed) {
    int actual_speed = speed > 0 ? speed : delay_;
    for (int radius = 0; radius < size_/2; radius++) {
        if (center - radius >= 0) channel_->set(pins_[center - radius], HIGH);
        if (center + radius < size_) channel_->set(pins_[center + radius], HIGH);
        channel_->sleep(actual_speed);
    }
    for (int radius = size_/2; radius >= 0; radius--) {
        if (center - radius >= 0) channel_->set(pins_[center - radius], LOW);
        if (center + radius < size_) channel_->set(pins_[center + radius], LOW);
        channel_->sleep(actual_speed);
    }
}

void PatternHelper::fillSymmetric(int step_delay) {
    int actual_delay = step_delay > 0 ? step_delay : delay_;
    for (int i = 0; i < size_/2; i++) {
        setSymmetric(i, HIGH);
        channel_->sleep(actual_delay);
    }
}

void PatternHelper::unfillSymmetric(int step_delay) {
    int actual_delay = step_delay > 0 ? step_delay : delay_;
    for (int i = size_/2 - 1; i >= 0; i--) {
        setSymmetric(i, LOW);
        channel_->sleep(actual_delay);
    }
}

void PatternHelper::chase(int length, int speed) {
    int actual_speed = speed > 0 ? speed : delay_;
    for (int pos = 0; pos < size_; pos++) {
        for (int i = 0; i < length && (pos + i) < size_; i++) {
            channel_->set(pins_[pos + i], HIGH);
        }
        channel_->sleep(actual_speed);
        for (int i = 0; i < length && (pos + i) < size_; i++) {
            channel_->set(pins_[pos + i], LOW);
        }
    }
}

void PatternHelper::bounce(int ball_size, int cycles) {
    for (int c = 0; c < cycles; c++) {
        for (int pos = 0; pos <= size_ - ball_size; pos++) {
            for (int i = 0; i < ball_size; i++) {
                channel_->set(pins_[pos + i], HIGH);
            }
            channel_->sleep(delay_);
            for (int i = 0; i < ball_size; i++) {
                channel_->set(pins_[pos + i], LOW);
            }
        }
        for (int pos = size_ - ball_size - 1; pos >= 0; pos--) {
            for (int i = 0; i < ball_size; i++) {
                channel_->set(pins_[pos + i], HIGH);
            }
            channel_->sleep(delay_);
            for (int i = 0; i < ball_size; i++) {
                channel_->set(pins_[pos + i], LOW);
            }
        }
    }
}

void PatternHelper::alternate(int group_size, int times) {
    for (int t = 0; t < times; t++) {
        for (int i = 0; i < size_; i += group_size * 2) {
            for (int j = 0; j < group_size && (i + j) < size_; j++) {
                channel_->set(pins_[i + j], HIGH);
            }
        }
        channel_->sleep(delay_);
        for (int i = 0; i < size_; i += group_size * 2) {
            for (int j = 0; j < group_size && (i + j) < size_; j++) {
                channel_->set(pins_[i + j], LOW);
            }
        }
        
        for (int i = group_size; i < size_; i += group_size * 2) {
            for (int j = 0; j < group_size && (i + j) < size_; j++) {
                channel_->set(pins_[i + j], HIGH);
            }
        }
        channel_->sleep(delay_);
        for (int i = group_size; i < size_; i += group_size * 2) {
            for (int j = 0; j < group_size && (i + j) < size_; j++) {
                channel_->set(pins_[i + j], LOW);
            }
        }
    }
}

int PatternHelper::size() const {
    return size_;
}

void PatternHelper::set(int index, bool state) {
    if (index >= 0 && index < size_) {
        channel_->set(pins_[index], state);
    }
}

void PatternHelper::delay(int ms) {
    channel_->sleep(ms > 0 ? ms : delay_);
}

void PatternHelper::allOff() {
    channel_->off();
}

void PatternHelper::allOn() {
    channel_->on();
}

int PatternHelper::getDelay() const {
    return delay_;
}

void PatternHelper::setDelay(int new_delay) {
    delay_ = new_delay;
}

}
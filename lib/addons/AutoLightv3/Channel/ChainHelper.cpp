#include "ChainHelper.h"

namespace AutoLight {

ChainHelper::ChainHelper(PatternHelper& h) : helper_(h) {}

ChainHelper& ChainHelper::wave(int start, int end, int speed) {
    int actual_start = start < 0 ? 0 : start;
    int actual_end = end < 0 ? helper_.size() - 1 : end;
    helper_.wave(actual_start, actual_end, speed);
    return *this;
}

ChainHelper& ChainHelper::waveReverse(int speed) {
    helper_.wave(helper_.size() - 1, 0, speed);
    return *this;
}

ChainHelper& ChainHelper::ripple(int center, int speed) {
    int actual_center = center < 0 ? helper_.size() / 2 : center;
    helper_.ripple(actual_center, speed);
    return *this;
}

ChainHelper& ChainHelper::blink(int times) {
    helper_.blinkRange(0, helper_.size() - 1, times);
    return *this;
}

ChainHelper& ChainHelper::blinkRange(int start, int end, int times) {
    helper_.blinkRange(start, end, times);
    return *this;
}

ChainHelper& ChainHelper::fillSymmetric(int speed) {
    helper_.fillSymmetric(speed);
    return *this;
}

ChainHelper& ChainHelper::unfillSymmetric(int speed) {
    helper_.unfillSymmetric(speed);
    return *this;
}

ChainHelper& ChainHelper::chase(int length, int speed) {
    helper_.chase(length, speed);
    return *this;
}

ChainHelper& ChainHelper::bounce(int ball_size, int cycles) {
    helper_.bounce(ball_size, cycles);
    return *this;
}

ChainHelper& ChainHelper::alternate(int group_size, int times) {
    helper_.alternate(group_size, times);
    return *this;
}

ChainHelper& ChainHelper::wait(int ms) {
    helper_.delay(ms);
    return *this;
}

ChainHelper& ChainHelper::off() {
    helper_.allOff();
    return *this;
}

ChainHelper& ChainHelper::on() {
    helper_.allOn();
    return *this;
}

ChainHelper& ChainHelper::setSpeed(int new_delay) {
    helper_.setDelay(new_delay);
    return *this;
}

ChainHelper& ChainHelper::custom(PatternHelperCallback callback) {
    callback(helper_);
    return *this;
}

ChainHelper& ChainHelper::repeat(int times, ChainHelperCallback pattern) {
    for (int i = 0; i < times; i++) {
        pattern(*this);
    }
    return *this;
}

}
#ifndef PATTERN_HELPER_H
#define PATTERN_HELPER_H

#include "BaseChannel.h"

namespace AutoLight {

class PatternHelper {
private:
    BaseChannel* channel_;
    int size_;
    uint8_t* pins_;
    int delay_;
    
public:
    PatternHelper(BaseChannel* ch);
    
    void setRange(int start, int end, bool state);
    void setSymmetric(int index, bool state);
    void setMirror(int left, int right, bool state);
    void blinkRange(int start, int end, int times = 1);
    void wave(int start, int end, int step_delay = 0);
    void ripple(int center, int speed = 0);
    void fillSymmetric(int step_delay = 0);
    void unfillSymmetric(int step_delay = 0);
    void chase(int length, int speed = 0);
    void bounce(int ball_size = 1, int cycles = 1);
    void alternate(int group_size = 1, int times = 1);
    
    int size() const;
    void set(int index, bool state);
    void delay(int ms = 0);
    void allOff();
    void allOn();
    int getDelay() const;
    void setDelay(int new_delay);
};

}

#endif
#ifndef CHAIN_HELPER_H
#define CHAIN_HELPER_H

#include "PatternHelper.h"

namespace AutoLight {

class ChainHelper;
class PatternHelper;

typedef void (*PatternHelperCallback)(PatternHelper&);
typedef void (*ChainHelperCallback)(ChainHelper&);

class ChainHelper {
private:
    PatternHelper& helper_;
    
public:
    ChainHelper(PatternHelper& h);
    
    ChainHelper& wave(int start = -1, int end = -1, int speed = 0);
    ChainHelper& waveReverse(int speed = 0);
    ChainHelper& ripple(int center = -1, int speed = 0);
    ChainHelper& blink(int times = 1);
    ChainHelper& blinkRange(int start, int end, int times = 1);
    ChainHelper& fillSymmetric(int speed = 0);
    ChainHelper& unfillSymmetric(int speed = 0);
    ChainHelper& chase(int length = 3, int speed = 0);
    ChainHelper& bounce(int ball_size = 1, int cycles = 1);
    ChainHelper& alternate(int group_size = 1, int times = 1);
    ChainHelper& wait(int ms = 0);
    ChainHelper& off();
    ChainHelper& on();
    ChainHelper& setSpeed(int new_delay);
    ChainHelper& custom(PatternHelperCallback callback);
    ChainHelper& repeat(int times, ChainHelperCallback pattern);
};

}

#endif
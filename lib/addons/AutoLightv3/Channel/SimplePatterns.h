#ifndef SIMPLE_PATTERNS_H
#define SIMPLE_PATTERNS_H

#include "PatternHelper.h"

namespace AutoLight {

class SimplePatterns {
public:
    static void knightRider(PatternHelper& h, int speed = 0, int cycles = 1);
    static void breathe(PatternHelper& h, int cycles = 3, int fade_speed = 50);
    static void randomBlink(PatternHelper& h, int duration = 5000);
    static void scanner(PatternHelper& h, int width = 3, int cycles = 2);
    static void strobe(PatternHelper& h, int times = 10, int strobe_delay = 50);
    static void twinkle(PatternHelper& h, int duration = 3000, int intensity = 3);
    static void meteor(PatternHelper& h, int tail_length = 5, int cycles = 3, int speed = 0);
    static void rainbow(PatternHelper& h, int cycles = 2, int speed = 100);
    static void heartbeat(PatternHelper& h, int beats = 3);
    static void explosion(PatternHelper& h, int center = -1, int speed = 50);
    static void spiral(PatternHelper& h, int turns = 2, int speed = 0);
    static void police(PatternHelper& h, int cycles = 5);
};

}

#endif
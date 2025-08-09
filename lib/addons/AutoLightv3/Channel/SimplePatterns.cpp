#include "SimplePatterns.h"

namespace AutoLight {

void SimplePatterns::knightRider(PatternHelper& h, int speed, int cycles) {
    int step_delay = speed > 0 ? speed : h.getDelay();
    for (int c = 0; c < cycles; c++) {
        for (int i = 0; i < h.size(); i++) {
            h.set(i, HIGH);
            h.delay(step_delay);
            h.set(i, LOW);
        }
        for (int i = h.size() - 1; i >= 0; i--) {
            h.set(i, HIGH);
            h.delay(step_delay);
            h.set(i, LOW);
        }
    }
}

void SimplePatterns::breathe(PatternHelper& h, int cycles, int fade_speed) {
    for (int c = 0; c < cycles; c++) {
        for (int i = 0; i < h.size(); i++) {
            h.set(i, HIGH);
            h.delay(fade_speed);
        }
        h.delay(200);
        for (int i = h.size() - 1; i >= 0; i--) {
            h.set(i, LOW);
            h.delay(fade_speed);
        }
        h.delay(200);
    }
}

void SimplePatterns::randomBlink(PatternHelper& h, int duration) {
    unsigned long start = millis();
    while (millis() - start < duration) {
        int random_pin = random(h.size());
        h.set(random_pin, HIGH);
        h.delay(50);
        h.set(random_pin, LOW);
        h.delay(random(100, 300));
    }
}

void SimplePatterns::scanner(PatternHelper& h, int width, int cycles) {
    for (int c = 0; c < cycles; c++) {
        for (int pos = 0; pos <= h.size() - width; pos++) {
            for (int i = 0; i < width; i++) {
                h.set(pos + i, HIGH);
            }
            h.delay();
            for (int i = 0; i < width; i++) {
                h.set(pos + i, LOW);
            }
        }
        for (int pos = h.size() - width - 1; pos >= 0; pos--) {
            for (int i = 0; i < width; i++) {
                h.set(pos + i, HIGH);
            }
            h.delay();
            for (int i = 0; i < width; i++) {
                h.set(pos + i, LOW);
            }
        }
    }
}

void SimplePatterns::strobe(PatternHelper& h, int times, int strobe_delay) {
    for (int t = 0; t < times; t++) {
        h.allOn();
        h.delay(strobe_delay);
        h.allOff();
        h.delay(strobe_delay);
    }
}

void SimplePatterns::twinkle(PatternHelper& h, int duration, int intensity) {
    unsigned long start = millis();
    while (millis() - start < duration) {
        for (int i = 0; i < intensity; i++) {
            int pin = random(h.size());
            h.set(pin, HIGH);
        }
        h.delay(random(100, 500));
        h.allOff();
        h.delay(random(50, 200));
    }
}

void SimplePatterns::meteor(PatternHelper& h, int tail_length, int cycles, int speed) {
    int step_delay = speed > 0 ? speed : h.getDelay();
    for (int c = 0; c < cycles; c++) {
        for (int pos = 0; pos < h.size() + tail_length; pos++) {
            h.allOff();
            for (int i = 0; i < tail_length; i++) {
                int pixel_pos = pos - i;
                if (pixel_pos >= 0 && pixel_pos < h.size()) {
                    h.set(pixel_pos, HIGH);
                }
            }
            h.delay(step_delay);
        }
    }
}

void SimplePatterns::rainbow(PatternHelper& h, int cycles, int speed) {
    for (int c = 0; c < cycles; c++) {
        for (int offset = 0; offset < h.size(); offset++) {
            h.allOff();
            for (int i = 0; i < h.size(); i++) {
                if ((i + offset) % 3 == 0) {
                    h.set(i, HIGH);
                }
            }
            h.delay(speed);
        }
    }
}

void SimplePatterns::heartbeat(PatternHelper& h, int beats) {
    for (int b = 0; b < beats; b++) {
        h.allOn();
        h.delay(100);
        h.allOff();
        h.delay(100);
        h.allOn();
        h.delay(100);
        h.allOff();
        h.delay(500);
    }
}

void SimplePatterns::explosion(PatternHelper& h, int center, int speed) {
    int actual_center = center < 0 ? h.size() / 2 : center;
    
    for (int radius = 0; radius <= h.size() / 2; radius++) {
        for (int i = 0; i < h.size(); i++) {
            int distance = abs(i - actual_center);
            if (distance == radius) {
                h.set(i, HIGH);
            }
        }
        h.delay(speed);
    }
    h.delay(200);
    h.allOff();
}

void SimplePatterns::spiral(PatternHelper& h, int turns, int speed) {
    int step_delay = speed > 0 ? speed : h.getDelay();
    int half = h.size() / 2;
    
    for (int t = 0; t < turns; t++) {
        for (int i = 0; i < half; i++) {
            h.set(i, HIGH);
            h.set(h.size() - 1 - i, HIGH);
            h.delay(step_delay);
            h.set(i, LOW);
            h.set(h.size() - 1 - i, LOW);
        }
    }
}

void SimplePatterns::police(PatternHelper& h, int cycles) {
    int half = h.size() / 2;
    for (int c = 0; c < cycles; c++) {
        h.setRange(0, half - 1, HIGH);
        h.delay(200);
        h.setRange(0, half - 1, LOW);
        h.setRange(half, h.size() - 1, HIGH);
        h.delay(200);
        h.setRange(half, h.size() - 1, LOW);
    }
}

}
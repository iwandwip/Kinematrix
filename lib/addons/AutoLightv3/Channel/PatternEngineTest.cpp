#include "PatternHelper.h"
#include "ChainHelper.h"
#include "SimplePatterns.h"

namespace AutoLight {

void customTestPattern(PatternHelper& h) {
    for (int i = 0; i < h.size(); i += 2) {
        h.set(i, HIGH);
        h.delay(50);
    }
    h.delay(500);
    h.allOff();
}

void chainTestPattern(ChainHelper& c) {
    c.wave().wait(200).waveReverse();
}

void BaseChannel::taskSequencePatternEngineTest() {
    PatternHelper h(this);
    
    h.wave(0, h.size() - 1);
    h.delay(500);
    
    h.ripple(h.size() / 2);
    h.delay(500);
    
    ChainHelper(h)
        .blink(3)
        .wait(200)
        .fillSymmetric()
        .wait(500)
        .unfillSymmetric()
        .custom(customTestPattern)
        .repeat(2, chainTestPattern)
        .off();
    
    SimplePatterns::knightRider(h, 100, 1);
    SimplePatterns::breathe(h, 2);
    SimplePatterns::explosion(h);
    
    for (int i = 0; i < h.size(); i++) {
        if (i % 3 == 0) {
            h.setSymmetric(i, HIGH);
            h.delay(100);
        }
    }
    h.delay(1000);
    h.allOff();
}

}
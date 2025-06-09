#include "RTCSensV2.h"

RTCDS1307Sens::RTCDS1307Sens(const DateTime &dt, TwoWire *_twoWirePtr)
        : twoWirePtr(_twoWirePtr),
          _updateTimer(0),
          _updateInterval(2000) {
}

RTCDS1307Sens::~RTCDS1307Sens() = default;

bool RTCDS1307Sens::init() {
    return true;
}

bool RTCDS1307Sens::update() {
    if (millis() - _updateTimer >= _updateInterval) {

        _updateTimer = millis();
        return true;
    }
    return false;
}

void RTCDS1307Sens::setUpdateInterval(uint32_t interval) {
    _updateInterval = interval;
}
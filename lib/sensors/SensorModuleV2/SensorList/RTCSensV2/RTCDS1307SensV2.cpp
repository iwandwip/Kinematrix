#include "RTCDS1307SensV2.h"

RTCDS1307SensV2::RTCDS1307SensV2(TwoWire *twoWirePtr)
    : BaseRTCSensV2(twoWirePtr),
      _useSquareWave(false),
      _squareWaveRate(DS1307_SquareWave1HZ) {
}

RTCDS1307SensV2::~RTCDS1307SensV2() = default;

bool RTCDS1307SensV2::init() {
    if (!begin(_twoWirePtr)) {
        return false;
    }
    
    if (!isrunning()) {
        adjust(DateTime(F(__DATE__), F(__TIME__)));
    }
    
    if (_useSquareWave) {
        writeSqwPinMode(_squareWaveRate);
    } else {
        writeSqwPinMode(DS1307_OFF);
    }
    
    _isInitialized = true;
    registerTimeValues();
    return true;
}

bool RTCDS1307SensV2::update() {
    if (!_isInitialized) {
        _lastUpdateStatus = false;
        return false;
    }
    
    unsigned long currentTime = millis();
    if ((currentTime - _updateTimer) >= _updateInterval) {
        _currentDateTime = now();
        updateTimeValues();
        _updateTimer = currentTime;
        _lastUpdateStatus = true;
        return true;
    }
    _lastUpdateStatus = false;
    return false;
}

bool RTCDS1307SensV2::setDateTime(const DateTime &dt) {
    if (!_isInitialized) {
        return false;
    }
    
    adjust(dt);
    _currentDateTime = dt;
    updateTimeValues();
    return true;
}

void RTCDS1307SensV2::enableSquareWave(bool enable) {
    _useSquareWave = enable;
    
    if (_isInitialized) {
        if (enable) {
            writeSqwPinMode(_squareWaveRate);
        } else {
            writeSqwPinMode(DS1307_OFF);
        }
    }
}

void RTCDS1307SensV2::setSquareWaveRate(Ds1307SqwPinMode rate) {
    _squareWaveRate = rate;
    
    if (_isInitialized && _useSquareWave) {
        writeSqwPinMode(rate);
    }
}

bool RTCDS1307SensV2::readNVRAM(uint8_t address, uint8_t *buf, uint8_t size) {
    if (!_isInitialized || address + size > 56) {
        return false;
    }
    
    readnvram(buf, size, address);
    return true;
}

bool RTCDS1307SensV2::writeNVRAM(uint8_t address, uint8_t *buf, uint8_t size) {
    if (!_isInitialized || address + size > 56) {
        return false;
    }
    
    writenvram(address, buf, size);
    return true;
}
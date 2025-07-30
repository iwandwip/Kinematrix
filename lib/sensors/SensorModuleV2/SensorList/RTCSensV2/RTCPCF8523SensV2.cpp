#include "RTCPCF8523SensV2.h"

RTCPCF8523SensV2::RTCPCF8523SensV2(TwoWire *twoWirePtr)
    : BaseRTCSensV2(twoWirePtr),
      _enableBatteryCheck(false),
      _batteryLow(false),
      _squareWaveMode(PCF8523_SquareWave1HZ),
      _useSquareWave(false) {
}

RTCPCF8523SensV2::~RTCPCF8523SensV2() = default;

void RTCPCF8523SensV2::registerTimeValues() {
    BaseRTCSensV2::registerTimeValues();
    
    if (_enableBatteryCheck) {
        addValueInfo("batteryLow", "Battery Low", "", 0, false);
    }
}

void RTCPCF8523SensV2::updateTimeValues() {
    BaseRTCSensV2::updateTimeValues();
    
    if (_enableBatteryCheck) {
        updateValue("batteryLow", _batteryLow ? 1 : 0);
    }
}

bool RTCPCF8523SensV2::init() {
    if (!begin(_twoWirePtr)) {
        return false;
    }
    
    if (lostPower()) {
        adjust(DateTime(F(__DATE__), F(__TIME__)));
    }
    
    start();
    
    if (_useSquareWave) {
        writeSqwPinMode(_squareWaveMode);
    } else {
        writeSqwPinMode(PCF8523_OFF);
    }
    
    _isInitialized = true;
    registerTimeValues();
    return true;
}

bool RTCPCF8523SensV2::update() {
    if (!_isInitialized) {
        _lastUpdateStatus = false;
        return false;
    }
    
    unsigned long currentTime = millis();
    if ((currentTime - _updateTimer) >= _updateInterval) {
        _currentDateTime = now();
        
        if (_enableBatteryCheck) {
            uint8_t statusReg = 0x03;
            _twoWirePtr->beginTransmission(0x68);
            _twoWirePtr->write(statusReg);
            _twoWirePtr->endTransmission();
            _twoWirePtr->requestFrom(0x68, 1);
            uint8_t status = _twoWirePtr->read();
            _batteryLow = (status & 0x04) != 0;
        }
        
        updateTimeValues();
        _updateTimer = currentTime;
        _lastUpdateStatus = true;
        return true;
    }
    _lastUpdateStatus = false;
    return false;
}

bool RTCPCF8523SensV2::setDateTime(const DateTime &dt) {
    if (!_isInitialized) {
        return false;
    }
    
    adjust(dt);
    _currentDateTime = dt;
    updateTimeValues();
    return true;
}

void RTCPCF8523SensV2::enableBatteryCheck(bool enable) {
    _enableBatteryCheck = enable;
    
    if (enable && !hasValue("batteryLow")) {
        addValueInfo("batteryLow", "Battery Low", "", 0, false);
    }
}

bool RTCPCF8523SensV2::isBatteryLow() {
    if (!_isInitialized) {
        return false;
    }
    
    uint8_t statusReg = 0x03;
    _twoWirePtr->beginTransmission(0x68);
    _twoWirePtr->write(statusReg);
    _twoWirePtr->endTransmission();
    _twoWirePtr->requestFrom(0x68, 1);
    uint8_t status = _twoWirePtr->read();
    return (status & 0x04) != 0;
}

void RTCPCF8523SensV2::enableSquareWave(bool enable, Pcf8523SqwPinMode mode) {
    _useSquareWave = enable;
    _squareWaveMode = mode;
    
    if (_isInitialized) {
        if (enable) {
            writeSqwPinMode(mode);
        } else {
            writeSqwPinMode(PCF8523_OFF);
        }
    }
}

void RTCPCF8523SensV2::calibrate(Pcf8523OffsetMode mode, int8_t offset) {
    if (_isInitialized) {
        RTC_PCF8523::calibrate(mode, offset);
    }
}

void RTCPCF8523SensV2::enableSecondTimer() {
    if (_isInitialized) {
        RTC_PCF8523::enableSecondTimer();
    }
}

void RTCPCF8523SensV2::disableSecondTimer() {
    if (_isInitialized) {
        RTC_PCF8523::disableSecondTimer();
    }
}

void RTCPCF8523SensV2::enableCountdownTimer(PCF8523TimerClockFreq freq, uint8_t numPeriods) {
    if (_isInitialized) {
        RTC_PCF8523::enableCountdownTimer(freq, numPeriods);
    }
}

void RTCPCF8523SensV2::disableCountdownTimer() {
    if (_isInitialized) {
        RTC_PCF8523::disableCountdownTimer();
    }
}
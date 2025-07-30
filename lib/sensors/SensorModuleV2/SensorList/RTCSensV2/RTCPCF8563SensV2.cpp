#include "RTCPCF8563SensV2.h"

RTCPCF8563SensV2::RTCPCF8563SensV2(TwoWire *twoWirePtr)
    : BaseRTCSensV2(twoWirePtr),
      _enableVoltageCheck(false),
      _voltageLow(false),
      _squareWaveMode(PCF8563_SquareWave1Hz),
      _useSquareWave(false) {
}

RTCPCF8563SensV2::~RTCPCF8563SensV2() = default;

void RTCPCF8563SensV2::registerTimeValues() {
    BaseRTCSensV2::registerTimeValues();
    
    if (_enableVoltageCheck) {
        addValueInfo("voltageLow", "Voltage Low", "", 0, false);
    }
}

void RTCPCF8563SensV2::updateTimeValues() {
    BaseRTCSensV2::updateTimeValues();
    
    if (_enableVoltageCheck) {
        updateValue("voltageLow", _voltageLow ? 1 : 0);
    }
}

bool RTCPCF8563SensV2::init() {
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
        writeSqwPinMode(PCF8563_SquareWaveOFF);
    }
    
    disableAlarm();
    
    _isInitialized = true;
    registerTimeValues();
    return true;
}

bool RTCPCF8563SensV2::update() {
    if (!_isInitialized) {
        _lastUpdateStatus = false;
        return false;
    }
    
    unsigned long currentTime = millis();
    if ((currentTime - _updateTimer) >= _updateInterval) {
        _currentDateTime = now();
        
        if (_enableVoltageCheck) {
            _twoWirePtr->beginTransmission(0x51);
            _twoWirePtr->write(0x02);
            _twoWirePtr->endTransmission();
            _twoWirePtr->requestFrom(0x51, 1);
            uint8_t status = _twoWirePtr->read();
            _voltageLow = (status & 0x10) != 0;
        }
        
        updateTimeValues();
        _updateTimer = currentTime;
        _lastUpdateStatus = true;
        return true;
    }
    _lastUpdateStatus = false;
    return false;
}

bool RTCPCF8563SensV2::setDateTime(const DateTime &dt) {
    if (!_isInitialized) {
        return false;
    }
    
    adjust(dt);
    _currentDateTime = dt;
    updateTimeValues();
    return true;
}

void RTCPCF8563SensV2::enableVoltageCheck(bool enable) {
    _enableVoltageCheck = enable;
    
    if (enable && !hasValue("voltageLow")) {
        addValueInfo("voltageLow", "Voltage Low", "", 0, false);
    }
}

bool RTCPCF8563SensV2::isVoltageLow() {
    if (!_isInitialized) {
        return false;
    }
    
    _twoWirePtr->beginTransmission(0x51);
    _twoWirePtr->write(0x02);
    _twoWirePtr->endTransmission();
    _twoWirePtr->requestFrom(0x51, 1);
    uint8_t status = _twoWirePtr->read();
    return (status & 0x10) != 0;
}

void RTCPCF8563SensV2::enableSquareWave(bool enable, Pcf8563SqwPinMode mode) {
    _useSquareWave = enable;
    _squareWaveMode = mode;
    
    if (_isInitialized) {
        if (enable) {
            writeSqwPinMode(mode);
        } else {
            writeSqwPinMode(PCF8563_SquareWaveOFF);
        }
    }
}

void RTCPCF8563SensV2::enableAlarm() {
    if (_isInitialized) {
        _twoWirePtr->beginTransmission(0x51);
        _twoWirePtr->write(0x01);
        _twoWirePtr->endTransmission();
        _twoWirePtr->requestFrom(0x51, 1);
        uint8_t ctrl2 = _twoWirePtr->read();
        ctrl2 |= 0x02;
        _twoWirePtr->beginTransmission(0x51);
        _twoWirePtr->write(0x01);
        _twoWirePtr->write(ctrl2);
        _twoWirePtr->endTransmission();
    }
}

void RTCPCF8563SensV2::disableAlarm() {
    if (_isInitialized) {
        _twoWirePtr->beginTransmission(0x51);
        _twoWirePtr->write(0x01);
        _twoWirePtr->endTransmission();
        _twoWirePtr->requestFrom(0x51, 1);
        uint8_t ctrl2 = _twoWirePtr->read();
        ctrl2 &= ~0x02;
        _twoWirePtr->beginTransmission(0x51);
        _twoWirePtr->write(0x01);
        _twoWirePtr->write(ctrl2);
        _twoWirePtr->endTransmission();
    }
}

bool RTCPCF8563SensV2::alarmFired() {
    if (!_isInitialized) {
        return false;
    }
    
    _twoWirePtr->beginTransmission(0x51);
    _twoWirePtr->write(0x01);
    _twoWirePtr->endTransmission();
    _twoWirePtr->requestFrom(0x51, 1);
    uint8_t ctrl2 = _twoWirePtr->read();
    return (ctrl2 & 0x08) != 0;
}

void RTCPCF8563SensV2::clearAlarm() {
    if (_isInitialized) {
        _twoWirePtr->beginTransmission(0x51);
        _twoWirePtr->write(0x01);
        _twoWirePtr->endTransmission();
        _twoWirePtr->requestFrom(0x51, 1);
        uint8_t ctrl2 = _twoWirePtr->read();
        ctrl2 &= ~0x08;
        _twoWirePtr->beginTransmission(0x51);
        _twoWirePtr->write(0x01);
        _twoWirePtr->write(ctrl2);
        _twoWirePtr->endTransmission();
    }
}

void RTCPCF8563SensV2::setAlarm(uint8_t minute, uint8_t hour, uint8_t day, uint8_t weekday) {
    if (!_isInitialized) {
        return;
    }
    
    uint8_t alarm_min = minute < 60 ? ((minute / 10) << 4) | (minute % 10) : 0x80;
    uint8_t alarm_hour = hour < 24 ? ((hour / 10) << 4) | (hour % 10) : 0x80;
    uint8_t alarm_day = day > 0 && day <= 31 ? ((day / 10) << 4) | (day % 10) : 0x80;
    uint8_t alarm_weekday = weekday <= 6 ? ((weekday / 10) << 4) | (weekday % 10) : 0x80;
    
    _twoWirePtr->beginTransmission(0x51);
    _twoWirePtr->write(0x09);
    _twoWirePtr->write(alarm_min);
    _twoWirePtr->endTransmission();
    
    _twoWirePtr->beginTransmission(0x51);
    _twoWirePtr->write(0x0A);
    _twoWirePtr->write(alarm_hour);
    _twoWirePtr->endTransmission();
    
    _twoWirePtr->beginTransmission(0x51);
    _twoWirePtr->write(0x0B);
    _twoWirePtr->write(alarm_day);
    _twoWirePtr->endTransmission();
    
    _twoWirePtr->beginTransmission(0x51);
    _twoWirePtr->write(0x0C);
    _twoWirePtr->write(alarm_weekday);
    _twoWirePtr->endTransmission();
}
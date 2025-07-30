#include "RTCDS3231SensV2.h"

RTCDS3231SensV2::RTCDS3231SensV2(TwoWire *twoWirePtr)
    : BaseRTCSensV2(twoWirePtr),
      _enableTemperature(false),
      _temperature(0.0),
      _use32kHz(false),
      _useSquareWave(false),
      _squareWaveMode(DS3231_SquareWave1Hz) {
}

RTCDS3231SensV2::~RTCDS3231SensV2() = default;

void RTCDS3231SensV2::registerTimeValues() {
    BaseRTCSensV2::registerTimeValues();
    
    if (_enableTemperature) {
        addValueInfo("temperature", "Temperature", "°C", 2, false);
    }
}

void RTCDS3231SensV2::updateTimeValues() {
    BaseRTCSensV2::updateTimeValues();
    
    if (_enableTemperature) {
        updateValue("temperature", _temperature);
    }
}

bool RTCDS3231SensV2::init() {
    if (!begin(_twoWirePtr)) {
        return false;
    }
    
    if (lostPower()) {
        adjust(DateTime(F(__DATE__), F(__TIME__)));
    }
    
    disableAlarms();
    clearAlarms();
    
    disable32K();
    writeSqwPinMode(DS3231_OFF);
    
    if (_use32kHz) {
        enable32K();
    }
    
    if (_useSquareWave) {
        writeSqwPinMode(_squareWaveMode);
    }
    
    _isInitialized = true;
    registerTimeValues();
    return true;
}

bool RTCDS3231SensV2::update() {
    if (!_isInitialized) {
        _lastUpdateStatus = false;
        return false;
    }
    
    unsigned long currentTime = millis();
    if ((currentTime - _updateTimer) >= _updateInterval) {
        _currentDateTime = now();
        
        if (_enableTemperature) {
            _temperature = getTemperature();
        }
        
        updateTimeValues();
        _updateTimer = currentTime;
        _lastUpdateStatus = true;
        return true;
    }
    _lastUpdateStatus = false;
    return false;
}

bool RTCDS3231SensV2::setDateTime(const DateTime &dt) {
    if (!_isInitialized) {
        return false;
    }
    
    adjust(dt);
    _currentDateTime = dt;
    updateTimeValues();
    return true;
}

void RTCDS3231SensV2::enableTemperature(bool enable) {
    _enableTemperature = enable;
    
    if (enable && !hasValue("temperature")) {
        addValueInfo("temperature", "Temperature", "°C", 2, false);
    }
}

float RTCDS3231SensV2::getTemperature() {
    if (!_isInitialized) {
        return 0.0;
    }
    
    return RTC_DS3231::getTemperature();
}

void RTCDS3231SensV2::enable32kHz(bool enable) {
    _use32kHz = enable;
    
    if (_isInitialized) {
        if (enable) {
            enable32K();
        } else {
            disable32K();
        }
    }
}

void RTCDS3231SensV2::enableSquareWave(bool enable, Ds3231SqwPinMode mode) {
    _useSquareWave = enable;
    _squareWaveMode = mode;
    
    if (_isInitialized) {
        if (enable) {
            writeSqwPinMode(mode);
        } else {
            writeSqwPinMode(DS3231_OFF);
        }
    }
}

bool RTCDS3231SensV2::lostPower() {
    return RTC_DS3231::lostPower();
}

void RTCDS3231SensV2::disableAlarms() {
    disableAlarm(1);
    disableAlarm(2);
}

void RTCDS3231SensV2::clearAlarms() {
    clearAlarm(1);
    clearAlarm(2);
}

void RTCDS3231SensV2::setAgingOffset(int8_t offset) {
    if (_isInitialized) {
        writeSqwPinMode(DS3231_OFF);
        uint8_t val = abs(offset);
        if (offset < 0) val |= 0x80;
        uint8_t agingReg = 0x10;
        _twoWirePtr->beginTransmission(0x68);
        _twoWirePtr->write(agingReg);
        _twoWirePtr->write(val);
        _twoWirePtr->endTransmission();
    }
}

int8_t RTCDS3231SensV2::getAgingOffset() {
    if (!_isInitialized) {
        return 0;
    }
    
    uint8_t agingReg = 0x10;
    _twoWirePtr->beginTransmission(0x68);
    _twoWirePtr->write(agingReg);
    _twoWirePtr->endTransmission();
    _twoWirePtr->requestFrom(0x68, 1);
    uint8_t val = _twoWirePtr->read();
    int8_t offset = val & 0x7F;
    if (val & 0x80) offset = -offset;
    return offset;
}
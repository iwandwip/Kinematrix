#include "BaseRTCSensV2.h"

BaseRTCSensV2::BaseRTCSensV2(TwoWire *twoWirePtr)
    : _twoWirePtr(twoWirePtr),
      _updateTimer(0),
      _updateInterval(1000),
      _enabledComponents(RTC_ALL),
      _isInitialized(false),
      _lastUpdateStatus(false) {
}

BaseRTCSensV2::~BaseRTCSensV2() = default;

void BaseRTCSensV2::registerTimeValues() {
    if (_enabledComponents & RTC_YEAR) {
        addValueInfo("year", "Year", "", 0, false);
    }
    if (_enabledComponents & RTC_MONTH) {
        addValueInfo("month", "Month", "", 0, false);
    }
    if (_enabledComponents & RTC_DAY) {
        addValueInfo("day", "Day", "", 0, false);
    }
    if (_enabledComponents & RTC_HOUR) {
        addValueInfo("hour", "Hour", "", 0, false);
    }
    if (_enabledComponents & RTC_MINUTE) {
        addValueInfo("minute", "Minute", "", 0, false);
    }
    if (_enabledComponents & RTC_SECOND) {
        addValueInfo("second", "Second", "", 0, false);
    }
    if (_enabledComponents & RTC_DAYOFWEEK) {
        addValueInfo("dayOfWeek", "Day of Week", "", 0, false);
    }
    if (_enabledComponents & RTC_UNIXTIME) {
        addValueInfo("unixTime", "Unix Time", "", 0, false);
    }
}

void BaseRTCSensV2::updateTimeValues() {
    if (_enabledComponents & RTC_YEAR) {
        updateValue("year", _currentDateTime.year());
    }
    if (_enabledComponents & RTC_MONTH) {
        updateValue("month", _currentDateTime.month());
    }
    if (_enabledComponents & RTC_DAY) {
        updateValue("day", _currentDateTime.day());
    }
    if (_enabledComponents & RTC_HOUR) {
        updateValue("hour", _currentDateTime.hour());
    }
    if (_enabledComponents & RTC_MINUTE) {
        updateValue("minute", _currentDateTime.minute());
    }
    if (_enabledComponents & RTC_SECOND) {
        updateValue("second", _currentDateTime.second());
    }
    if (_enabledComponents & RTC_DAYOFWEEK) {
        updateValue("dayOfWeek", _currentDateTime.dayOfTheWeek());
    }
    if (_enabledComponents & RTC_UNIXTIME) {
        updateValue("unixTime", _currentDateTime.unixtime());
    }
}

void BaseRTCSensV2::setUpdateInterval(uint32_t interval) {
    _updateInterval = interval;
}

void BaseRTCSensV2::enableTimeComponents(uint8_t components) {
    _enabledComponents |= components;
}

void BaseRTCSensV2::disableTimeComponents(uint8_t components) {
    _enabledComponents &= ~components;
}

DateTime BaseRTCSensV2::getDateTime() {
    return _currentDateTime;
}

uint32_t BaseRTCSensV2::getUnixTime() {
    return _currentDateTime.unixtime();
}

String BaseRTCSensV2::getFormattedTime(const String &format) {
    String result = format;
    
    char buffer[8];
    snprintf(buffer, sizeof(buffer), "%04d", _currentDateTime.year());
    result.replace("YYYY", buffer);
    
    snprintf(buffer, sizeof(buffer), "%02d", _currentDateTime.month());
    result.replace("MM", buffer);
    
    snprintf(buffer, sizeof(buffer), "%02d", _currentDateTime.day());
    result.replace("DD", buffer);
    
    snprintf(buffer, sizeof(buffer), "%02d", _currentDateTime.hour());
    result.replace("hh", buffer);
    
    snprintf(buffer, sizeof(buffer), "%02d", _currentDateTime.minute());
    result.replace("mm", buffer);
    
    snprintf(buffer, sizeof(buffer), "%02d", _currentDateTime.second());
    result.replace("ss", buffer);
    
    return result;
}

uint16_t BaseRTCSensV2::getYear() {
    return _currentDateTime.year();
}

uint8_t BaseRTCSensV2::getMonth() {
    return _currentDateTime.month();
}

uint8_t BaseRTCSensV2::getDay() {
    return _currentDateTime.day();
}

uint8_t BaseRTCSensV2::getHour() {
    return _currentDateTime.hour();
}

uint8_t BaseRTCSensV2::getMinute() {
    return _currentDateTime.minute();
}

uint8_t BaseRTCSensV2::getSecond() {
    return _currentDateTime.second();
}

uint8_t BaseRTCSensV2::getDayOfWeek() {
    return _currentDateTime.dayOfTheWeek();
}

bool BaseRTCSensV2::isUpdated() const {
    return _lastUpdateStatus;
}
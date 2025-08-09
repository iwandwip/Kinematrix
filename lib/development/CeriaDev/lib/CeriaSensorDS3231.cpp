#include "CeriaSensorDS3231.h"

namespace CeriaDevOP {

    CeriaSensorDS3231::CeriaSensorDS3231() : RTC_DS3231() {
        _hasNewData = false;
        _lastUpdateTime = 0;
        _updateInterval = 1000;

        memset(&_currentTime, 0, sizeof(_currentTime));
        memset(&_status, 0, sizeof(_status));
    }

    bool CeriaSensorDS3231::begin(TwoWire *wire) {
        bool success = RTC_DS3231::begin(wire);
        if (success) {
            _lastUpdateTime = millis();
            update();
        }
        return success;
    }

    void CeriaSensorDS3231::update() {
        if (_updateInterval > 0 && (millis() - _lastUpdateTime) < _updateInterval) {
            return;
        }

        DateTime now = RTC_DS3231::now();

        DS3231TimeData newTime;
        newTime.year = now.year();
        newTime.month = now.month();
        newTime.day = now.day();
        newTime.hour = now.hour();
        newTime.minute = now.minute();
        newTime.second = now.second();
        newTime.dayOfWeek = now.dayOfTheWeek();
        newTime.unixTime = now.unixtime();

        newTime.dateString = String(newTime.year) + "-" + formatTwoDigits(newTime.month) + "-" + formatTwoDigits(newTime.day);
        newTime.timeString = formatTwoDigits(newTime.hour) + ":" + formatTwoDigits(newTime.minute) + ":" + formatTwoDigits(newTime.second);
        newTime.dateTimeString = newTime.dateString + " " + newTime.timeString;

        if (newTime.second != _currentTime.second) {
            _currentTime = newTime;
            _hasNewData = true;
            _lastUpdateTime = millis();
            updateStatus();
        }
    }

    void CeriaSensorDS3231::updateTimeData() {
        DateTime now = RTC_DS3231::now();
        _currentTime.year = now.year();
        _currentTime.month = now.month();
        _currentTime.day = now.day();
        _currentTime.hour = now.hour();
        _currentTime.minute = now.minute();
        _currentTime.second = now.second();
        _currentTime.dayOfWeek = now.dayOfTheWeek();
        _currentTime.unixTime = now.unixtime();

        _currentTime.dateString = String(_currentTime.year) + "-" + formatTwoDigits(_currentTime.month) + "-" + formatTwoDigits(_currentTime.day);
        _currentTime.timeString = formatTwoDigits(_currentTime.hour) + ":" + formatTwoDigits(_currentTime.minute) + ":" + formatTwoDigits(_currentTime.second);
        _currentTime.dateTimeString = _currentTime.dateString + " " + _currentTime.timeString;
    }

    void CeriaSensorDS3231::updateStatus() {
        _status.powerLost = RTC_DS3231::lostPower();
        _status.temperature = RTC_DS3231::getTemperature();
        _status.oscillatorRunning = !_status.powerLost;

        Ds3231SqwPinMode mode = RTC_DS3231::readSqwPinMode();
        switch (mode) {
            case DS3231_OFF:
                _status.sqwMode = DS3231_SQW_OFF;
                break;
            case DS3231_SquareWave1Hz:
                _status.sqwMode = DS3231_SQW_1HZ;
                break;
            case DS3231_SquareWave1kHz:
                _status.sqwMode = DS3231_SQW_1KHZ;
                break;
            case DS3231_SquareWave4kHz:
                _status.sqwMode = DS3231_SQW_4KHZ;
                break;
            case DS3231_SquareWave8kHz:
                _status.sqwMode = DS3231_SQW_8KHZ;
                break;
            default:
                _status.sqwMode = DS3231_SQW_OFF;
                break;
        }
    }

    String CeriaSensorDS3231::formatTwoDigits(uint8_t value) {
        if (value < 10) {
            return "0" + String(value);
        }
        return String(value);
    }

    bool CeriaSensorDS3231::hasNewData() {
        bool result = _hasNewData;
        _hasNewData = false;
        return result;
    }

    bool CeriaSensorDS3231::isConnected(uint32_t timeoutMs) {
        return (millis() - _lastUpdateTime) < timeoutMs;
    }

    bool CeriaSensorDS3231::isPowerLost() {
        return RTC_DS3231::lostPower();
    }

    DS3231TimeData CeriaSensorDS3231::getTime() {
        return _currentTime;
    }

    DS3231Status CeriaSensorDS3231::getStatus() {
        return _status;
    }

    uint16_t CeriaSensorDS3231::getYear() {
        return _currentTime.year;
    }

    uint8_t CeriaSensorDS3231::getMonth() {
        return _currentTime.month;
    }

    uint8_t CeriaSensorDS3231::getDay() {
        return _currentTime.day;
    }

    uint8_t CeriaSensorDS3231::getHour() {
        return _currentTime.hour;
    }

    uint8_t CeriaSensorDS3231::getMinute() {
        return _currentTime.minute;
    }

    uint8_t CeriaSensorDS3231::getSecond() {
        return _currentTime.second;
    }

    uint8_t CeriaSensorDS3231::getDayOfWeek() {
        return _currentTime.dayOfWeek;
    }

    uint32_t CeriaSensorDS3231::getUnixTime() {
        return _currentTime.unixTime;
    }

    float CeriaSensorDS3231::getTemperature() {
        return RTC_DS3231::getTemperature();
    }

    String CeriaSensorDS3231::getDateString() {
        return _currentTime.dateString;
    }

    String CeriaSensorDS3231::getTimeString() {
        return _currentTime.timeString;
    }

    String CeriaSensorDS3231::getDateTimeString() {
        return _currentTime.dateTimeString;
    }

    String CeriaSensorDS3231::getTimeString12Hour() {
        uint8_t hour12 = _currentTime.hour;
        String ampm = "AM";

        if (hour12 == 0) {
            hour12 = 12;
        } else if (hour12 > 12) {
            hour12 -= 12;
            ampm = "PM";
        } else if (hour12 == 12) {
            ampm = "PM";
        }

        return formatTwoDigits(hour12) + ":" + formatTwoDigits(_currentTime.minute) + ":" + formatTwoDigits(_currentTime.second) + " " + ampm;
    }

    String CeriaSensorDS3231::getDayName() {
        if (_currentTime.dayOfWeek < 7) {
            return String(_daysOfWeek[_currentTime.dayOfWeek]);
        }
        return "Unknown";
    }

    String CeriaSensorDS3231::getMonthName() {
        if (_currentTime.month >= 1 && _currentTime.month <= 12) {
            return String(_monthNames[_currentTime.month - 1]);
        }
        return "Unknown";
    }

    bool CeriaSensorDS3231::setDateTime(uint16_t year, uint8_t month, uint8_t day, uint8_t hour, uint8_t minute, uint8_t second) {
        if (!validateDateTime(year, month, day, hour, minute, second)) {
            return false;
        }

        DateTime dt(year, month, day, hour, minute, second);
        RTC_DS3231::adjust(dt);
        update();
        return true;
    }

    bool CeriaSensorDS3231::setDateTime(uint32_t unixTime) {
        DateTime dt(unixTime);
        RTC_DS3231::adjust(dt);
        update();
        return true;
    }

    bool CeriaSensorDS3231::setDateTime(const String &dateTime) {
        if (dateTime.length() < 19) return false;

        int year = dateTime.substring(0, 4).toInt();
        int month = dateTime.substring(5, 7).toInt();
        int day = dateTime.substring(8, 10).toInt();
        int hour = dateTime.substring(11, 13).toInt();
        int minute = dateTime.substring(14, 16).toInt();
        int second = dateTime.substring(17, 19).toInt();

        return setDateTime(year, month, day, hour, minute, second);
    }

    void CeriaSensorDS3231::setToCompileTime() {
        DateTime dt = DateTime(F(__DATE__), F(__TIME__));
        RTC_DS3231::adjust(dt);
        update();
    }

    void CeriaSensorDS3231::setSqwMode(DS3231SqwMode mode) {
        Ds3231SqwPinMode rtcMode;
        switch (mode) {
            case DS3231_SQW_OFF:
                rtcMode = DS3231_OFF;
                break;
            case DS3231_SQW_1HZ:
                rtcMode = DS3231_SquareWave1Hz;
                break;
            case DS3231_SQW_1KHZ:
                rtcMode = DS3231_SquareWave1kHz;
                break;
            case DS3231_SQW_4KHZ:
                rtcMode = DS3231_SquareWave4kHz;
                break;
            case DS3231_SQW_8KHZ:
                rtcMode = DS3231_SquareWave8kHz;
                break;
            default:
                rtcMode = DS3231_OFF;
                break;
        }
        RTC_DS3231::writeSqwPinMode(rtcMode);
        updateStatus();
    }

    DS3231SqwMode CeriaSensorDS3231::getSqwMode() {
        return _status.sqwMode;
    }

    bool CeriaSensorDS3231::setAlarm1(uint8_t hour, uint8_t minute, uint8_t second, Ds3231Alarm1Mode mode) {
        DateTime dt(2000, 1, 1, hour, minute, second);
        return RTC_DS3231::setAlarm1(dt, mode);
    }

    bool CeriaSensorDS3231::setAlarm2(uint8_t hour, uint8_t minute, Ds3231Alarm2Mode mode) {
        DateTime dt(2000, 1, 1, hour, minute, 0);
        return RTC_DS3231::setAlarm2(dt, mode);
    }

    bool CeriaSensorDS3231::isAlarmTriggered(DS3231AlarmType alarm) {
        switch (alarm) {
            case DS3231_ALARM_1:
                return RTC_DS3231::alarmFired(1);
            case DS3231_ALARM_2:
                return RTC_DS3231::alarmFired(2);
            case DS3231_ALARM_BOTH:
                return RTC_DS3231::alarmFired(1) || RTC_DS3231::alarmFired(2);
            default:
                return false;
        }
    }

    void CeriaSensorDS3231::clearAlarm(DS3231AlarmType alarm) {
        switch (alarm) {
            case DS3231_ALARM_1:
                RTC_DS3231::clearAlarm(1);
                break;
            case DS3231_ALARM_2:
                RTC_DS3231::clearAlarm(2);
                break;
            case DS3231_ALARM_BOTH:
                RTC_DS3231::clearAlarm(1);
                RTC_DS3231::clearAlarm(2);
                break;
        }
    }

    void CeriaSensorDS3231::setUpdateInterval(uint32_t intervalMs) {
        _updateInterval = intervalMs;
    }

    void CeriaSensorDS3231::enableAutoSync(bool enable) {
        if (enable) {
            _updateInterval = 1000;
        } else {
            _updateInterval = 0;
        }
    }

    String CeriaSensorDS3231::getRTCString() {
        return getDateTimeString() + " (" + getDayName() + ")";
    }

    String CeriaSensorDS3231::getStatusString() {
        String result = "DS3231: ";
        result += getDateTimeString();
        result += ", " + String(getTemperature(), 1) + "°C";
        if (_status.powerLost) {
            result += ", POWER LOST";
        }
        return result;
    }

    String CeriaSensorDS3231::getFullStatusString() {
        String result = "DS3231 Status:\n";
        result += "Time: " + getRTCString() + "\n";
        result += "Temperature: " + String(getTemperature(), 2) + "°C\n";
        result += "Power Lost: " + String(_status.powerLost ? "Yes" : "No") + "\n";
        result += "Oscillator: " + String(_status.oscillatorRunning ? "Running" : "Stopped") + "\n";
        result += "SQW Mode: ";
        switch (_status.sqwMode) {
            case DS3231_SQW_OFF:
                result += "Off";
                break;
            case DS3231_SQW_1HZ:
                result += "1Hz";
                break;
            case DS3231_SQW_1KHZ:
                result += "1kHz";
                break;
            case DS3231_SQW_4KHZ:
                result += "4kHz";
                break;
            case DS3231_SQW_8KHZ:
                result += "8kHz";
                break;
        }
        result += "\nUnix Time: " + String(getUnixTime());
        return result;
    }

    bool CeriaSensorDS3231::waitForSecondChange(uint32_t timeoutMs) {
        uint32_t startTime = millis();
        uint8_t initialSecond = _currentTime.second;

        while (millis() - startTime < timeoutMs) {
            update();
            if (_currentTime.second != initialSecond) {
                return true;
            }
            delay(10);
        }
        return false;
    }

    bool CeriaSensorDS3231::waitForMinuteChange(uint32_t timeoutMs) {
        uint32_t startTime = millis();
        uint8_t initialMinute = _currentTime.minute;

        while (millis() - startTime < timeoutMs) {
            update();
            if (_currentTime.minute != initialMinute) {
                return true;
            }
            delay(100);
        }
        return false;
    }

    bool CeriaSensorDS3231::isLeapYear(uint16_t year) {
        return (year % 4 == 0 && year % 100 != 0) || (year % 400 == 0);
    }

    uint8_t CeriaSensorDS3231::getDaysInMonth(uint8_t month, uint16_t year) {
        uint8_t daysInMonth[] = {31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};
        if (month == 2 && isLeapYear(year)) {
            return 29;
        }
        return daysInMonth[month - 1];
    }

    uint32_t CeriaSensorDS3231::calculateUnixTime(uint16_t year, uint8_t month, uint8_t day, uint8_t hour, uint8_t minute, uint8_t second) {
        DateTime dt(year, month, day, hour, minute, second);
        return dt.unixtime();
    }

    void CeriaSensorDS3231::resetStatistics() {
        _lastUpdateTime = millis();
        _hasNewData = false;
    }

    uint32_t CeriaSensorDS3231::getLastSyncTime() {
        return _lastUpdateTime;
    }

    bool CeriaSensorDS3231::validateDateTime(uint16_t year, uint8_t month, uint8_t day, uint8_t hour, uint8_t minute, uint8_t second) {
        if (year < 2000 || year > 2100) return false;
        if (month < 1 || month > 12) return false;
        if (day < 1 || day > getDaysInMonth(month, year)) return false;
        if (hour > 23) return false;
        if (minute > 59) return false;
        if (second > 59) return false;
        return true;
    }

}
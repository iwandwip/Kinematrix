#include "MHRTCSensV2.h"

MHRTCSensV2::MHRTCSensV2(uint8_t pinEna, uint8_t pinClk, uint8_t pinDat)
    : Ds1302(pinEna, pinClk, pinDat), _pinEna(pinEna), _pinClk(pinClk), _pinDat(pinDat),
      _updateTimer(0), _updateInterval(1000), _lastUpdateStatus(false), 
      _isInitialized(false), _clockRunning(false) {
    
    addValueInfo("year", "Year", "", 0, false);
    addValueInfo("month", "Month", "", 0, false);
    addValueInfo("day", "Day", "", 0, false);
    addValueInfo("hour", "Hour", "", 0, false);
    addValueInfo("minute", "Minute", "", 0, false);
    addValueInfo("second", "Second", "", 0, false);
    addValueInfo("dayOfWeek", "Day of Week", "", 0, false);
    addValueInfo("timestamp", "Unix Timestamp", "s", 0, false);
    addValueInfo("dateString", "Date String", "", 0, false);
    addValueInfo("timeString", "Time String", "", 0, false);
    addValueInfo("dateTimeString", "DateTime String", "", 0, false);
    addValueInfo("isRunning", "Clock Running", "", 0, false);
}

MHRTCSensV2::~MHRTCSensV2() {
}

bool MHRTCSensV2::init() {
    Ds1302::init();
    
    _clockRunning = !isHalted();
    if (_clockRunning) {
        _isInitialized = true;
        Ds1302::DateTime dt;
        Ds1302::getDateTime(&dt);
        updateTimeValues(dt);
        return true;
    }
    
    return false;
}

bool MHRTCSensV2::update() {
    if (!_isInitialized) return false;
    
    uint32_t currentTime = millis();
    if (currentTime - _updateTimer >= _updateInterval) {
        _updateTimer = currentTime;
        
        Ds1302::DateTime dt;
        Ds1302::getDateTime(&dt);
        updateTimeValues(dt);
        
        _lastUpdateStatus = true;
        return true;
    }
    
    _lastUpdateStatus = false;
    return false;
}

bool MHRTCSensV2::isUpdated() const {
    return _lastUpdateStatus;
}

void MHRTCSensV2::setUpdateInterval(uint32_t interval) {
    _updateInterval = interval;
}

void MHRTCSensV2::setPins(uint8_t pinEna, uint8_t pinClk, uint8_t pinDat) {
    _pinEna = pinEna;
    _pinClk = pinClk;
    _pinDat = pinDat;
}

void MHRTCSensV2::updateTimeValues(const Ds1302::DateTime& dt) {
    updateValue("year", (int)(dt.year + 2000));
    updateValue("month", (int)dt.month);
    updateValue("day", (int)dt.day);
    updateValue("hour", (int)dt.hour);
    updateValue("minute", (int)dt.minute);
    updateValue("second", (int)dt.second);
    updateValue("dayOfWeek", (int)dt.dow);
    updateValue("timestamp", (int)getUnixTimestamp());
    updateValue("dateString", getFormattedDate().c_str());
    updateValue("timeString", getFormattedTime().c_str());
    updateValue("dateTimeString", getFormattedDateTime().c_str());
    updateValue("isRunning", _clockRunning ? 1 : 0);
}

bool MHRTCSensV2::setDateTime(uint8_t year, uint8_t month, uint8_t day, 
                              uint8_t hour, uint8_t minute, uint8_t second, 
                              uint8_t dayOfWeek) {
    if (year > 99 || month < 1 || month > 12 || day < 1 || day > 31 ||
        hour > 23 || minute > 59 || second > 59 || dayOfWeek < 1 || dayOfWeek > 7) {
        return false;
    }
    
    Ds1302::DateTime dt;
    dt.year = year;
    dt.month = month;
    dt.day = day;
    dt.hour = hour;
    dt.minute = minute;
    dt.second = second;
    dt.dow = dayOfWeek;
    
    Ds1302::setDateTime(&dt);
    return true;
}

bool MHRTCSensV2::setDateTime(const Ds1302::DateTime& dt) {
    Ds1302::setDateTime(const_cast<Ds1302::DateTime*>(&dt));
    return true;
}

Ds1302::DateTime MHRTCSensV2::getDateTime() {
    Ds1302::DateTime dt;
    Ds1302::getDateTime(&dt);
    return dt;
}

bool MHRTCSensV2::isClockRunning() const {
    return !const_cast<MHRTCSensV2*>(this)->isHalted();
}

void MHRTCSensV2::startClock() {
    start();
    _clockRunning = true;
}

void MHRTCSensV2::stopClock() {
    halt();
    _clockRunning = false;
}

void MHRTCSensV2::setYear(uint8_t year) {
    Ds1302::DateTime dt = getDateTime();
    dt.year = year;
    setDateTime(dt);
}

void MHRTCSensV2::setMonth(uint8_t month) {
    Ds1302::DateTime dt = getDateTime();
    dt.month = month;
    setDateTime(dt);
}

void MHRTCSensV2::setDay(uint8_t day) {
    Ds1302::DateTime dt = getDateTime();
    dt.day = day;
    setDateTime(dt);
}

void MHRTCSensV2::setHour(uint8_t hour) {
    Ds1302::DateTime dt = getDateTime();
    dt.hour = hour;
    setDateTime(dt);
}

void MHRTCSensV2::setMinute(uint8_t minute) {
    Ds1302::DateTime dt = getDateTime();
    dt.minute = minute;
    setDateTime(dt);
}

void MHRTCSensV2::setSecond(uint8_t second) {
    Ds1302::DateTime dt = getDateTime();
    dt.second = second;
    setDateTime(dt);
}

void MHRTCSensV2::setDayOfWeek(uint8_t dow) {
    Ds1302::DateTime dt = getDateTime();
    dt.dow = dow;
    setDateTime(dt);
}

String MHRTCSensV2::getFormattedDateTime(const char* format) {
    Ds1302::DateTime dt = getDateTime();
    String result = String(format);
    
    result.replace("YYYY", String(dt.year + 2000));
    result.replace("YY", String(dt.year));
    result.replace("MM", dt.month < 10 ? "0" + String(dt.month) : String(dt.month));
    result.replace("M", String(dt.month));
    result.replace("DD", dt.day < 10 ? "0" + String(dt.day) : String(dt.day));
    result.replace("D", String(dt.day));
    result.replace("HH", dt.hour < 10 ? "0" + String(dt.hour) : String(dt.hour));
    result.replace("H", String(dt.hour));
    result.replace("mm", dt.minute < 10 ? "0" + String(dt.minute) : String(dt.minute));
    result.replace("m", String(dt.minute));
    result.replace("ss", dt.second < 10 ? "0" + String(dt.second) : String(dt.second));
    result.replace("s", String(dt.second));
    
    return result;
}

String MHRTCSensV2::getFormattedDate(const char* format) {
    return getFormattedDateTime(format);
}

String MHRTCSensV2::getFormattedTime(const char* format) {
    return getFormattedDateTime(format);
}

const char* MHRTCSensV2::getDayOfWeekName(uint8_t dow) const {
    switch (dow) {
        case DOW_MON: return "Monday";
        case DOW_TUE: return "Tuesday";
        case DOW_WED: return "Wednesday";
        case DOW_THU: return "Thursday";
        case DOW_FRI: return "Friday";
        case DOW_SAT: return "Saturday";
        case DOW_SUN: return "Sunday";
        default: return "Unknown";
    }
}

const char* MHRTCSensV2::getMonthName(uint8_t month) const {
    switch (month) {
        case MONTH_JAN: return "January";
        case MONTH_FEB: return "February";
        case MONTH_MAR: return "March";
        case MONTH_APR: return "April";
        case MONTH_MAY: return "May";
        case MONTH_JUN: return "June";
        case MONTH_JUL: return "July";
        case MONTH_AUG: return "August";
        case MONTH_SEP: return "September";
        case MONTH_OCT: return "October";
        case MONTH_NOV: return "November";
        case MONTH_DEC: return "December";
        default: return "Unknown";
    }
}

uint32_t MHRTCSensV2::getUnixTimestamp() {
    Ds1302::DateTime dt = getDateTime();
    
    uint32_t year = dt.year + 2000;
    uint32_t days = 0;
    
    for (uint32_t y = 1970; y < year; y++) {
        days += isLeapYear(y - 2000) ? 366 : 365;
    }
    
    for (uint8_t m = 1; m < dt.month; m++) {
        days += getDaysInMonth(m, dt.year);
    }
    
    days += dt.day - 1;
    
    return days * 86400UL + dt.hour * 3600UL + dt.minute * 60UL + dt.second;
}

bool MHRTCSensV2::setUnixTimestamp(uint32_t timestamp) {
    uint32_t days = timestamp / 86400UL;
    uint32_t seconds = timestamp % 86400UL;
    
    uint32_t year = 1970;
    while (days >= (isLeapYear(year - 2000) ? 366 : 365)) {
        days -= isLeapYear(year - 2000) ? 366 : 365;
        year++;
    }
    
    uint8_t month = 1;
    while (days >= getDaysInMonth(month, year - 2000)) {
        days -= getDaysInMonth(month, year - 2000);
        month++;
    }
    
    uint8_t day = days + 1;
    uint8_t hour = seconds / 3600;
    uint8_t minute = (seconds % 3600) / 60;
    uint8_t second = seconds % 60;
    
    return setDateTime(year - 2000, month, day, hour, minute, second);
}

bool MHRTCSensV2::isLeapYear(uint8_t year) const {
    uint32_t fullYear = year + 2000;
    return (fullYear % 4 == 0 && fullYear % 100 != 0) || (fullYear % 400 == 0);
}

uint8_t MHRTCSensV2::getDaysInMonth(uint8_t month, uint8_t year) const {
    switch (month) {
        case 1: case 3: case 5: case 7: case 8: case 10: case 12:
            return 31;
        case 4: case 6: case 9: case 11:
            return 30;
        case 2:
            return isLeapYear(year) ? 29 : 28;
        default:
            return 0;
    }
}
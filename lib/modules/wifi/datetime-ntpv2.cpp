#include "datetime-ntpv2.h"

uint8_t daysInMonth(uint16_t year, uint8_t month) {
    if (month == 2) {
        return 28 + (year % 4 == 0 && (year % 100 != 0 || year % 400 == 0));
    }
    const uint8_t days[] = {31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};
    return days[month - 1];
}

uint16_t date2days(uint16_t y, uint8_t m, uint8_t d) {
    if (y >= 2000) y -= 2000;
    uint16_t days = d;
    for (uint8_t i = 1; i < m; ++i) {
        days += daysInMonth(y, i);
    }
    return days + 365 * y + (y + 3) / 4 - 1;
}

uint32_t time2long(uint16_t days, uint8_t h, uint8_t m, uint8_t s) {
    return ((days * 24L + h) * 60 + m) * 60 + s;
}

TimeSpanNTPV2::TimeSpanNTPV2(int32_t seconds)
        : _seconds(seconds) {}

TimeSpanNTPV2::TimeSpanNTPV2(int16_t days, int8_t hours, int8_t minutes, int8_t seconds)
        : _seconds((int32_t) days * 86400L + (int32_t) hours * 3600 + (int32_t) minutes * 60 + seconds) {}

TimeSpanNTPV2::TimeSpanNTPV2(const TimeSpanNTPV2 &copy)
        : _seconds(copy._seconds) {}

int16_t TimeSpanNTPV2::days() const {
    return _seconds / 86400L;
}

int8_t TimeSpanNTPV2::hours() const {
    return (_seconds / 3600) % 24;
}

int8_t TimeSpanNTPV2::minutes() const {
    return (_seconds / 60) % 60;
}

int8_t TimeSpanNTPV2::seconds() const {
    return _seconds % 60;
}

int32_t TimeSpanNTPV2::totalseconds() const {
    return _seconds;
}

String DateTimeNTPV2::leadingZeros(int value, int width) {
    String result = String(value);
    while (result.length() < width) {
        result = "0" + result;
    }
    return result;
}

DateTimeNTPV2::DateTimeNTPV2(const char *ntpServer, long gmtOffset_sec, int daylightOffset_sec, unsigned long syncInterval) {
    _ntpServer = ntpServer;
    _gmtOffset_sec = gmtOffset_sec;
    _daylightOffset_sec = daylightOffset_sec;
    _timeInitialized = false;
    _syncInterval = syncInterval;
    _yOff = 0;
    _m = 0;
    _d = 0;
    _hh = 0;
    _mm = 0;
    _ss = 0;
}

DateTimeNTPV2::DateTimeNTPV2(long gmtOffset_sec, int daylightOffset_sec) {
    _ntpServer = "pool.ntp.org";
    _gmtOffset_sec = gmtOffset_sec;
    _daylightOffset_sec = daylightOffset_sec;
    _timeInitialized = false;
    _syncInterval = 3600000;
    _yOff = 0;
    _m = 0;
    _d = 0;
    _hh = 0;
    _mm = 0;
    _ss = 0;
}

DateTimeNTPV2::DateTimeNTPV2(uint32_t t) {
    _ntpServer = "pool.ntp.org";
    _gmtOffset_sec = 0;
    _daylightOffset_sec = 0;
    _timeInitialized = true;
    _syncInterval = 3600000;

    if (t == 0) {
        _yOff = _m = _d = _hh = _mm = _ss = 0;
    } else {
        t -= 946684800UL;
        _ss = t % 60;
        t /= 60;
        _mm = t % 60;
        t /= 60;
        _hh = t % 24;
        uint16_t days = t / 24;
        uint8_t leap;
        for (_yOff = 0;; ++_yOff) {
            leap = (_yOff % 4 == 0);
            if (days < 365U + leap)
                break;
            days -= 365 + leap;
        }
        for (_m = 1;; ++_m) {
            uint8_t daysPerMonth = daysInMonth(_yOff + 2000, _m);
            if (days < daysPerMonth)
                break;
            days -= daysPerMonth;
        }
        _d = days + 1;
    }
}

DateTimeNTPV2::DateTimeNTPV2(uint16_t year, uint8_t month, uint8_t day, uint8_t hour, uint8_t min, uint8_t sec) {
    _ntpServer = "pool.ntp.org";
    _gmtOffset_sec = 0;
    _daylightOffset_sec = 0;
    _timeInitialized = true;
    _syncInterval = 3600000;

    if (year >= 2000) year -= 2000;
    _yOff = year;
    _m = month;
    _d = day;
    _hh = hour;
    _mm = min;
    _ss = sec;
}

DateTimeNTPV2::DateTimeNTPV2(const DateTimeNTPV2 &copy) {
    _ntpServer = copy._ntpServer;
    _gmtOffset_sec = copy._gmtOffset_sec;
    _daylightOffset_sec = copy._daylightOffset_sec;
    _timeInitialized = copy._timeInitialized;
    _syncInterval = copy._syncInterval;
    _lastSync = copy._lastSync;

    _yOff = copy._yOff;
    _m = copy._m;
    _d = copy._d;
    _hh = copy._hh;
    _mm = copy._mm;
    _ss = copy._ss;
}

bool DateTimeNTPV2::begin() {
    if (WiFi.status() != WL_CONNECTED) {
        return false;
    }

    configTime(_gmtOffset_sec, _daylightOffset_sec, _ntpServer);

    if (!updateLocalTime()) {
        return false;
    }

    _lastSync = time(NULL);
    _timeInitialized = true;
    syncWithUnixTime();
    return true;
}

bool DateTimeNTPV2::update() {
    if (!_timeInitialized) {
        return begin();
    }

    if (WiFi.status() != WL_CONNECTED) {
        return false;
    }

    time_t now = time(NULL);
    if ((now - _lastSync) * 1000 >= _syncInterval) {
        bool result = updateLocalTime();
        if (result) {
            _lastSync = now;
            syncWithUnixTime();
        }
        return result;
    }

    return updateLocalTime();
}

bool DateTimeNTPV2::forceUpdate() {
    if (WiFi.status() != WL_CONNECTED) {
        return false;
    }

    bool result = updateLocalTime();
    if (result) {
        _lastSync = time(NULL);
        syncWithUnixTime();
    }
    return result;
}

bool DateTimeNTPV2::updateLocalTime() {
    if (!getLocalTime(&_timeinfo)) {
        return false;
    }
    return true;
}

void DateTimeNTPV2::setTimezone(long gmtOffset_sec, int daylightOffset_sec) {
    _gmtOffset_sec = gmtOffset_sec;
    _daylightOffset_sec = daylightOffset_sec;
    configTime(_gmtOffset_sec, _daylightOffset_sec, _ntpServer);
    updateLocalTime();
    syncWithUnixTime();
}

void DateTimeNTPV2::setSyncInterval(unsigned long interval) {
    _syncInterval = interval;
}

unsigned long DateTimeNTPV2::getTime() {
    time_t now;
    if (!getLocalTime(&_timeinfo)) {
        return 0;
    }
    time(&now);
    return now;
}

time_t DateTimeNTPV2::now() {
    update();
    time_t current_time;
    time(&current_time);
    return current_time;
}

String DateTimeNTPV2::getISO8601Time() {
    update();

    String iso8601 = String(getYear()) + "-" +
                     leadingZeros(getMonth(), 2) + "-" +
                     leadingZeros(getDay(), 2) + "T" +
                     leadingZeros(getHour(), 2) + ":" +
                     leadingZeros(getMinute(), 2) + ":" +
                     leadingZeros(getSecond(), 2);

    if (_gmtOffset_sec == 0 && _daylightOffset_sec == 0) {
        iso8601 += "Z";
    } else {
        int totalOffset = _gmtOffset_sec + _daylightOffset_sec;
        char sign = totalOffset >= 0 ? '+' : '-';
        totalOffset = abs(totalOffset);
        int hours = totalOffset / 3600;
        int minutes = (totalOffset % 3600) / 60;

        iso8601 += String(sign) + leadingZeros(hours, 2) + ":" + leadingZeros(minutes, 2);
    }

    return iso8601;
}

time_t DateTimeNTPV2::getUnixTime() {
    update();
    return mktime(&_timeinfo);
}

String DateTimeNTPV2::getFormattedTime(const char *format) {
    update();

    char buffer[64];
    strftime(buffer, sizeof(buffer), format, &_timeinfo);
    return String(buffer);
}

String DateTimeNTPV2::getTimeString() {
    return getFormattedTime("%H:%M:%S");
}

String DateTimeNTPV2::getDateString() {
    return getFormattedTime("%Y-%m-%d");
}

String DateTimeNTPV2::getDateTimeString() {
    return getFormattedTime("%Y-%m-%d %H:%M:%S");
}

String DateTimeNTPV2::getRFC2822Time() {
    return getFormattedTime("%a, %d %b %Y %H:%M:%S %z");
}

String DateTimeNTPV2::getRFC3339Time() {
    update();
    String base = getFormattedTime("%Y-%m-%dT%H:%M:%S");

    if (_gmtOffset_sec == 0 && _daylightOffset_sec == 0) {
        base += "Z";
    } else {
        int totalOffset = _gmtOffset_sec + _daylightOffset_sec;
        char sign = totalOffset >= 0 ? '+' : '-';
        totalOffset = abs(totalOffset);
        int hours = totalOffset / 3600;
        int minutes = (totalOffset % 3600) / 60;

        base += String(sign) + leadingZeros(hours, 2) + ":" + leadingZeros(minutes, 2);
    }

    return base;
}

String DateTimeNTPV2::getDayOfWeek() {
    return getFormattedTime("%A");
}

String DateTimeNTPV2::getShortDayOfWeek() {
    return getFormattedTime("%a");
}

String DateTimeNTPV2::getMonthName() {
    return getFormattedTime("%B");
}

String DateTimeNTPV2::getShortMonthName() {
    return getFormattedTime("%b");
}

bool DateTimeNTPV2::isTimeInitialized() {
    return _timeInitialized;
}

bool DateTimeNTPV2::isValid() const {
    if (_yOff == 0 && _m == 0 && _d == 0 && _hh == 0 && _mm == 0 && _ss == 0) return false;
    return true;
}

char *DateTimeNTPV2::toString(char *buffer) {
    update();
    sprintf(buffer, "%04u-%02u-%02u %02u:%02u:%02u", getYear(), getMonth(), getDay(), getHour(), getMinute(), getSecond());
    return buffer;
}

uint16_t DateTimeNTPV2::getYear() {
    update();
    return _timeinfo.tm_year + 1900;
}

uint8_t DateTimeNTPV2::getMonth() {
    update();
    return _timeinfo.tm_mon + 1;
}

uint8_t DateTimeNTPV2::getDay() {
    update();
    return _timeinfo.tm_mday;
}

uint8_t DateTimeNTPV2::getHour() {
    update();
    return _timeinfo.tm_hour;
}

uint8_t DateTimeNTPV2::getMinute() {
    update();
    return _timeinfo.tm_min;
}

uint8_t DateTimeNTPV2::getSecond() {
    update();
    return _timeinfo.tm_sec;
}

uint8_t DateTimeNTPV2::getDayOfWeekInt() {
    update();
    return _timeinfo.tm_wday;
}

uint8_t DateTimeNTPV2::getDayOfYear() {
    update();
    return _timeinfo.tm_yday;
}

uint16_t DateTimeNTPV2::year() const {
    return 2000U + _yOff;
}

uint8_t DateTimeNTPV2::month() const {
    return _m;
}

uint8_t DateTimeNTPV2::day() const {
    return _d;
}

uint8_t DateTimeNTPV2::hour() const {
    return _hh;
}

uint8_t DateTimeNTPV2::twelveHour() const {
    if (_hh == 0) return 12;
    else if (_hh > 12) return _hh - 12;
    else return _hh;
}

uint8_t DateTimeNTPV2::isPM() const {
    return _hh >= 12;
}

uint8_t DateTimeNTPV2::minute() const {
    return _mm;
}

uint8_t DateTimeNTPV2::second() const {
    return _ss;
}

uint8_t DateTimeNTPV2::dayOfTheWeek() const {
    uint16_t day = date2days(_yOff + 2000, _m, _d);
    return (day + 6) % 7;
}

uint32_t DateTimeNTPV2::secondstime() const {
    uint32_t t;
    t = time2long(date2days(_yOff + 2000, _m, _d), _hh, _mm, _ss);
    return t;
}

uint32_t DateTimeNTPV2::unixtime() const {
    uint32_t t;
    t = secondstime();
    t += 946684800;
    return t;
}

String DateTimeNTPV2::timestamp(timestampOpt opt) {
    update();
    char buffer[30];
    switch (opt) {
        case TIMESTAMP_TIME:
            sprintf(buffer, "%02u:%02u:%02u", getHour(), getMinute(), getSecond());
            break;
        case TIMESTAMP_DATE:
            sprintf(buffer, "%04u-%02u-%02u", getYear(), getMonth(), getDay());
            break;
        default:
            sprintf(buffer, "%04u-%02u-%02uT%02u:%02u:%02u", getYear(), getMonth(), getDay(), getHour(), getMinute(), getSecond());
            break;
    }
    return {buffer};
}

DateTimeNTPV2 DateTimeNTPV2::operator+(const TimeSpanNTPV2 &span) const {
    return DateTimeNTPV2(unixtime() + span.totalseconds());
}

DateTimeNTPV2 DateTimeNTPV2::operator-(const TimeSpanNTPV2 &span) const {
    return DateTimeNTPV2(unixtime() - span.totalseconds());
}

TimeSpanNTPV2 DateTimeNTPV2::operator-(const DateTimeNTPV2 &right) const {
    return TimeSpanNTPV2(unixtime() - right.unixtime());
}

bool DateTimeNTPV2::operator<(const DateTimeNTPV2 &right) const {
    return (unixtime() < right.unixtime());
}

bool DateTimeNTPV2::operator>(const DateTimeNTPV2 &right) const {
    return right < *this;
}

bool DateTimeNTPV2::operator<=(const DateTimeNTPV2 &right) const {
    return !(*this > right);
}

bool DateTimeNTPV2::operator>=(const DateTimeNTPV2 &right) const {
    return !(*this < right);
}

bool DateTimeNTPV2::operator==(const DateTimeNTPV2 &right) const {
    return (unixtime() == right.unixtime());
}

bool DateTimeNTPV2::operator!=(const DateTimeNTPV2 &right) const {
    return !(*this == right);
}

void DateTimeNTPV2::syncWithUnixTime() {
    time_t now = time(NULL);
    if (now == 0) {
        return;
    }

    struct tm *timeinfo = localtime(&now);
    _yOff = timeinfo->tm_year - 100;
    _m = timeinfo->tm_mon + 1;
    _d = timeinfo->tm_mday;
    _hh = timeinfo->tm_hour;
    _mm = timeinfo->tm_min;
    _ss = timeinfo->tm_sec;
}
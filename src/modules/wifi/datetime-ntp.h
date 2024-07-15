#ifdef USE_DATETIME_NTP
#pragma once
// https://randomnerdtutorials.com/esp32-datalogging-google-sheets/
#ifndef KINEMATRIX_DATETIMENTP_H
#define KINEMATRIX_DATETIMENTP_H

#if defined(ESP32) || defined(ESP8266)

#include "Arduino.h"
#include "WiFi.h"
#include "time.h"
#include "RTClib.h"

uint8_t daysInMonth(uint16_t year, uint8_t month);
uint16_t date2days(uint16_t y, uint8_t m, uint8_t d);
uint32_t time2long(uint16_t days, uint8_t h, uint8_t m, uint8_t s);

class DateTimeNTP {
public:
    DateTimeNTP(const char *ssid, const char *password, long gmtOffset_sec, int daylightOffset_sec);
    DateTimeNTP(long gmtOffset_sec, int daylightOffset_sec);
    DateTimeNTP(uint32_t t = 0);
    DateTimeNTP(uint16_t year, uint8_t month, uint8_t day, uint8_t hour = 0, uint8_t min = 0, uint8_t sec = 0);
    DateTimeNTP(const DateTimeNTP &copy);
    void begin();
    unsigned long getTime();
    bool isValid() const;
    char *toString(char *buffer) const;

    uint16_t year() const { return 2000U + yOff; }
    uint8_t month() const { return m; }
    uint8_t day() const { return d; }
    uint8_t hour() const { return hh; }
    uint8_t twelveHour() const;
    uint8_t isPM() const { return hh >= 12; }
    uint8_t minute() const { return mm; }
    uint8_t second() const { return ss; }
    uint8_t dayOfTheWeek() const;
    uint32_t secondstime() const;
    uint32_t unixtime() const;

    enum timestampOpt {
        TIMESTAMP_FULL, //!< `YYYY-MM-DDThh:mm:ss`
        TIMESTAMP_TIME, //!< `hh:mm:ss`
        TIMESTAMP_DATE  //!< `YYYY-MM-DD`
    };
    String timestamp(timestampOpt opt = TIMESTAMP_FULL) const;

    DateTimeNTP operator+(const TimeSpan &span) const;
    DateTimeNTP operator-(const TimeSpan &span) const;
    TimeSpan operator-(const DateTimeNTP &right) const;
    bool operator<(const DateTimeNTP &right) const;
    bool operator>(const DateTimeNTP &right) const { return right < *this; }
    bool operator<=(const DateTimeNTP &right) const { return !(*this > right); }
    bool operator>=(const DateTimeNTP &right) const { return !(*this < right); }
    bool operator==(const DateTimeNTP &right) const;
    bool operator!=(const DateTimeNTP &right) const { return !(*this == right); }

protected:
    const char *ssid;
    const char *password;
    long gmtOffset_sec;
    int daylightOffset_sec;

    uint8_t yOff; ///< Year offset from 2000
    uint8_t m;    ///< Month 1-12
    uint8_t d;    ///< Day 1-31
    uint8_t hh;   ///< Hours 0-23
    uint8_t mm;   ///< Minutes 0-59
    uint8_t ss;   ///< Seconds 0-59
};


DateTimeNTP::DateTimeNTP(const char *ssid, const char *password, long gmtOffset_sec, int daylightOffset_sec)
        : ssid(ssid), password(password), gmtOffset_sec(gmtOffset_sec), daylightOffset_sec(daylightOffset_sec) {
}

DateTimeNTP::DateTimeNTP(long gmtOffset_sec, int daylightOffset_sec)
        : ssid(nullptr), password(nullptr), gmtOffset_sec(gmtOffset_sec), daylightOffset_sec(daylightOffset_sec) {
}

DateTimeNTP::DateTimeNTP(uint32_t t) {
    if (t == 0) {
        yOff = m = d = hh = mm = ss = 0;
    } else {
        t -= 946684800UL; // UNIX time starts on Jan 1 1970. In seconds, that's 946684800.
        ss = t % 60;
        t /= 60;
        mm = t % 60;
        t /= 60;
        hh = t % 24;
        uint16_t days = t / 24;
        uint8_t leap;
        for (yOff = 0;; ++yOff) {
            leap = (yOff % 4 == 0);
            if (days < 365U + leap)
                break;
            days -= 365 + leap;
        }
        for (m = 1;; ++m) {
            uint8_t daysPerMonth = 31;
            if (m == 4 || m == 6 || m == 9 || m == 11)
                daysPerMonth = 30;
            else if (m == 2)
                daysPerMonth = 28 + leap;
            if (days < daysPerMonth)
                break;
            days -= daysPerMonth;
        }
        d = days + 1;
    }
}

DateTimeNTP::DateTimeNTP(uint16_t year, uint8_t month, uint8_t day, uint8_t hour, uint8_t min, uint8_t sec) {
    if (year >= 2000) year -= 2000;
    yOff = year;
    m = month;
    d = day;
    hh = hour;
    mm = min;
    ss = sec;
}

DateTimeNTP::DateTimeNTP(const DateTimeNTP &copy) {
    yOff = copy.yOff;
    m = copy.m;
    d = copy.d;
    hh = copy.hh;
    mm = copy.mm;
    ss = copy.ss;
}

void DateTimeNTP::begin() {
    if (ssid != nullptr && password != nullptr) {
        Serial.print("Connecting to WiFi ");
        Serial.println(ssid);
        WiFi.begin(ssid, password);
        while (WiFi.status() != WL_CONNECTED) {
            delay(500);
            Serial.print(".");
        }
        Serial.println(" Connected!");
    }

    configTime(gmtOffset_sec, daylightOffset_sec, "pool.ntp.org");
    Serial.println("Time synchronized with NTP server.");
}

unsigned long DateTimeNTP::getTime() {
    time_t now;
    struct tm timeinfo;
    if (!getLocalTime(&timeinfo)) {
        return 0;
    }
    time(&now);
    return now;
}

bool DateTimeNTP::isValid() const {
    if (yOff == 0 && m == 0 && d == 0 && hh == 0 && mm == 0 && ss == 0) return false;
    return true;
}

char *DateTimeNTP::toString(char *buffer) const {
    sprintf(buffer, "%04u-%02u-%02u %02u:%02u:%02u", year(), month(), day(), hour(), minute(), second());
    return buffer;
}

uint8_t DateTimeNTP::twelveHour() const {
    if (hh == 0) return 12;
    else if (hh > 12) return hh - 12;
    else return hh;
}

uint8_t DateTimeNTP::dayOfTheWeek() const {
    uint16_t day = date2days(yOff + 2000, m, d);
    return (day + 6) % 7;
}

uint32_t DateTimeNTP::secondstime() const {
    uint32_t t;
    t = time2long(date2days(yOff + 2000, m, d), hh, mm, ss);
    return t;
}

uint32_t DateTimeNTP::unixtime() const {
    uint32_t t;
    t = secondstime();
    t += 946684800; // Seconds from 1970-01-01T00:00:00 to 2000-01-01T00:00:00
    return t;
}

String DateTimeNTP::timestamp(timestampOpt opt) const {
    char buffer[30];
    switch (opt) {
        case TIMESTAMP_TIME:
            sprintf(buffer, "%02u:%02u:%02u", hour(), minute(), second());
            break;
        case TIMESTAMP_DATE:
            sprintf(buffer, "%04u-%02u-%02u", year(), month(), day());
            break;
        default:
            sprintf(buffer, "%04u-%02u-%02uT%02u:%02u:%02u", year(), month(), day(), hour(), minute(), second());
            break;
    }
    return String(buffer);
}

DateTimeNTP DateTimeNTP::operator+(const TimeSpan &span) const {
    return DateTimeNTP(unixtime() + span.totalseconds());
}

DateTimeNTP DateTimeNTP::operator-(const TimeSpan &span) const {
    return DateTimeNTP(unixtime() - span.totalseconds());
}

TimeSpan DateTimeNTP::operator-(const DateTimeNTP &right) const {
    return TimeSpan(unixtime() - right.unixtime());
}

bool DateTimeNTP::operator<(const DateTimeNTP &right) const {
    return (unixtime() < right.unixtime());
}

bool DateTimeNTP::operator==(const DateTimeNTP &right) const {
    return (unixtime() == right.unixtime());
}

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

#endif
#endif
#endif

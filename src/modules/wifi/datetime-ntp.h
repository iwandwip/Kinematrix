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

#endif
#endif
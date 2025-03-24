#ifndef KINEMATRIX_DATE_TIME_NTP_V2
#define KINEMATRIX_DATE_TIME_NTP_V2

#include <Arduino.h>

#if defined(ESP32) || defined(ARDUINO_RASPBERRY_PI_PICO_W)
#include <WiFi.h>
#elif defined(ESP8266)
#include <ESP8266WiFi.h>
#elif __has_include(<WiFiNINA.h>)
#include <WiFiNINA.h>
#elif __has_include(<WiFi101.h>)
#include <WiFi101.h>
#elif __has_include(<WiFiS3.h>)
#include <WiFiS3.h>
#endif

#include <time.h>
#include <sys/time.h>

uint8_t daysInMonth(uint16_t year, uint8_t month);
uint16_t date2days(uint16_t y, uint8_t m, uint8_t d);
uint32_t time2long(uint16_t days, uint8_t h, uint8_t m, uint8_t s);

class TimeSpanNTPV2 {
private:
    int32_t _seconds;

public:
    explicit TimeSpanNTPV2(int32_t seconds = 0);
    TimeSpanNTPV2(int16_t days, int8_t hours, int8_t minutes, int8_t seconds);
    TimeSpanNTPV2(const TimeSpanNTPV2 &copy);

    int16_t days() const;
    int8_t hours() const;
    int8_t minutes() const;
    int8_t seconds() const;
    int32_t totalseconds() const;
};

class DateTimeNTPV2 {
private:
    const char *_ntpServer;
    long _gmtOffset_sec;
    int _daylightOffset_sec;
    struct tm _timeinfo;
    bool _timeInitialized;
    time_t _lastSync;
    unsigned long _syncInterval;

    uint8_t _yOff;
    uint8_t _m;
    uint8_t _d;
    uint8_t _hh;
    uint8_t _mm;
    uint8_t _ss;

    String leadingZeros(int value, int width);

public:
    explicit DateTimeNTPV2(const char *ntpServer = "pool.ntp.org",
                           long gmtOffset_sec = 0,
                           int daylightOffset_sec = 0,
                           unsigned long syncInterval = 3600000);

    DateTimeNTPV2(long gmtOffset_sec, int daylightOffset_sec);

    explicit DateTimeNTPV2(uint32_t t);

    DateTimeNTPV2(uint16_t year, uint8_t month, uint8_t day,
                  uint8_t hour = 0, uint8_t min = 0, uint8_t sec = 0);

    DateTimeNTPV2(const DateTimeNTPV2 &copy);

    bool begin();
    bool update();
    bool forceUpdate();
    bool updateLocalTime();
    void setTimezone(long gmtOffset_sec, int daylightOffset_sec);
    void setSyncInterval(unsigned long interval);
    unsigned long getTime();
    time_t now();

    String getISO8601Time();
    time_t getUnixTime();
    String getFormattedTime(const char *format);
    String getTimeString();
    String getDateString();
    String getDateTimeString();
    String getRFC2822Time();
    String getRFC3339Time();
    String getDayOfWeek();
    String getShortDayOfWeek();
    String getMonthName();
    String getShortMonthName();

    bool isTimeInitialized();
    bool isValid() const;
    char *toString(char *buffer);

    uint16_t getYear();
    uint8_t getMonth();
    uint8_t getDay();
    uint8_t getHour();
    uint8_t getMinute();
    uint8_t getSecond();
    uint8_t getDayOfWeekInt();
    uint8_t getDayOfYear();

    uint16_t year() const;
    uint8_t month() const;
    uint8_t day() const;
    uint8_t hour() const;
    uint8_t twelveHour() const;
    uint8_t isPM() const;
    uint8_t minute() const;
    uint8_t second() const;

    uint8_t dayOfTheWeek() const;
    uint32_t secondstime() const;
    uint32_t unixtime() const;

    enum timestampOpt {
        TIMESTAMP_FULL,
        TIMESTAMP_TIME,
        TIMESTAMP_DATE
    };
    String timestamp(timestampOpt opt = TIMESTAMP_FULL);

    DateTimeNTPV2 operator+(const TimeSpanNTPV2 &span) const;
    DateTimeNTPV2 operator-(const TimeSpanNTPV2 &span) const;
    TimeSpanNTPV2 operator-(const DateTimeNTPV2 &right) const;
    bool operator<(const DateTimeNTPV2 &right) const;
    bool operator>(const DateTimeNTPV2 &right) const;
    bool operator<=(const DateTimeNTPV2 &right) const;
    bool operator>=(const DateTimeNTPV2 &right) const;
    bool operator==(const DateTimeNTPV2 &right) const;
    bool operator!=(const DateTimeNTPV2 &right) const;

    void syncWithUnixTime();
};

#endif
#ifndef CERIA_SENSOR_DS3231_H
#define CERIA_SENSOR_DS3231_H

#include "Arduino.h"
#include "RTClib.h"
#include <Wire.h>

namespace CeriaDevOP {

    enum DS3231SqwMode {
        DS3231_SQW_OFF = 0,
        DS3231_SQW_1HZ = 1,
        DS3231_SQW_1KHZ = 2,
        DS3231_SQW_4KHZ = 3,
        DS3231_SQW_8KHZ = 4
    };

    enum DS3231AlarmType {
        DS3231_ALARM_1 = 1,
        DS3231_ALARM_2 = 2,
        DS3231_ALARM_BOTH = 3
    };

    struct DS3231TimeData {
        uint16_t year;
        uint8_t month;
        uint8_t day;
        uint8_t hour;
        uint8_t minute;
        uint8_t second;
        uint8_t dayOfWeek;
        uint32_t unixTime;
        String dateString;
        String timeString;
        String dateTimeString;
    };

    struct DS3231Status {
        bool powerLost;
        bool alarmTriggered;
        float temperature;
        bool oscillatorRunning;
        DS3231SqwMode sqwMode;
    };

    class CeriaSensorDS3231 : public RTC_DS3231 {
    private:
        bool _hasNewData;
        uint32_t _lastUpdateTime;
        DS3231TimeData _currentTime;
        DS3231Status _status;
        uint32_t _updateInterval;

        const char *_daysOfWeek[7] = {"Sunday", "Monday", "Tuesday", "Wednesday", "Thursday", "Friday", "Saturday"};
        const char *_monthNames[12] = {"Jan", "Feb", "Mar", "Apr", "May", "Jun", "Jul", "Aug", "Sep", "Oct", "Nov", "Dec"};

        void updateTimeData();
        void updateStatus();
        String formatTwoDigits(uint8_t value);

    public:
        CeriaSensorDS3231();

        bool begin(TwoWire *wire = &Wire);
        void update();

        bool hasNewData();
        bool isConnected(uint32_t timeoutMs = 5000);
        bool isPowerLost();

        DS3231TimeData getTime();
        DS3231Status getStatus();

        uint16_t getYear();
        uint8_t getMonth();
        uint8_t getDay();
        uint8_t getHour();
        uint8_t getMinute();
        uint8_t getSecond();
        uint8_t getDayOfWeek();
        uint32_t getUnixTime();
        float getTemperature();

        String getDateString();
        String getTimeString();
        String getDateTimeString();
        String getTimeString12Hour();
        String getDayName();
        String getMonthName();

        bool setDateTime(uint16_t year, uint8_t month, uint8_t day, uint8_t hour, uint8_t minute, uint8_t second);
        bool setDateTime(uint32_t unixTime);
        bool setDateTime(const String &dateTime); // Format: "YYYY-MM-DD HH:MM:SS"
        void setToCompileTime();

        void setSqwMode(DS3231SqwMode mode);
        DS3231SqwMode getSqwMode();

        bool setAlarm1(uint8_t hour, uint8_t minute, uint8_t second, Ds3231Alarm1Mode mode);
        bool setAlarm2(uint8_t hour, uint8_t minute, Ds3231Alarm2Mode mode);
        bool isAlarmTriggered(DS3231AlarmType alarm = DS3231_ALARM_BOTH);
        void clearAlarm(DS3231AlarmType alarm = DS3231_ALARM_BOTH);

        void setUpdateInterval(uint32_t intervalMs);
        void enableAutoSync(bool enable = true);

        String getRTCString();
        String getStatusString();
        String getFullStatusString();

        bool waitForSecondChange(uint32_t timeoutMs = 2000);
        bool waitForMinuteChange(uint32_t timeoutMs = 61000);

        bool isLeapYear(uint16_t year);
        uint8_t getDaysInMonth(uint8_t month, uint16_t year);
        uint32_t calculateUnixTime(uint16_t year, uint8_t month, uint8_t day, uint8_t hour, uint8_t minute, uint8_t second);

        void resetStatistics();
        uint32_t getLastSyncTime();
        bool validateDateTime(uint16_t year, uint8_t month, uint8_t day, uint8_t hour, uint8_t minute, uint8_t second);
    };

}

#endif
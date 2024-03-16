/*
 *  rtc-sens.h
 *
 *  rtc sensor lib
 *  Created on: 2023. 4. 3
 */

#pragma once

#ifndef RTC_SENS_H
#define RTC_SENS_H

#include "Arduino.h"
#include "base/sensor-module.h"
#include "RTClib.h"

const char daysOfTheWeek[7][12] =
        {"Sunday", "Monday",
         "Tuesday", "Wednesday",
         "Thursday", "Friday",
         "Saturday"};

typedef enum {
    NOW_YEAR,
    NOW_MONTH,
    NOW_DAY,
    NOW_DAY_OF_THE_WEEK,
    NOW_HOUR,
    NOW_MINUTES,
    NOW_SECOND,
    RTC_TOTAL_DATA
} date_time_t;

struct rtc_data_t {
    uint8_t year;
    uint8_t month;
    uint8_t day;
    uint8_t daysOfTheWeek;
    uint8_t hour;
    uint8_t minute;
    uint8_t second;
};

class RTC_DS3231Sens : public BaseSens, public RTC_DS3231 {
private:
    /*variables*/
    void *sensorClass;
    TwoWire *wirePtr;
    DateTime dateTime;
    int sensorValue[7];
    uint8_t sensorPin;
    uint32_t sensorTimer;

    /*optional object*/
    void (*sensorCallbackFunc)();
    using RTC_DS3231::RTC_DS3231;

public:
    explicit RTC_DS3231Sens(const DateTime &dt = DateTime(), TwoWire *wireInstance = &Wire);
    ~RTC_DS3231Sens();
    void init() override;
    void update() override;
    void getValue(int *output) override;
    float getValueRTC_DS3231Sens() const;
    void getValueRTC(rtc_data_t *data);
    String getDaysOfTheWeek(uint8_t index);
    void setPins(uint8_t _pin);
};

#endif  // RTC_SENS_H
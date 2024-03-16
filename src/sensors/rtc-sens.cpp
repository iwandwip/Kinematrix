/*
 *  rtc-sens.cpp
 *
 *  rtc sensor c
 *  Created on: 2023. 4. 3
 */

#include "rtc-sens.h"
#include "Arduino.h"

RTC_DS3231Sens::RTC_DS3231Sens(const DateTime &dt, TwoWire *wireInstance)
        : sensorClass(nullptr),
          wirePtr(wireInstance),
          dateTime(dt),
          sensorTimer(0),
          sensorCallbackFunc(nullptr) {
}

RTC_DS3231Sens::~RTC_DS3231Sens() = default;

void RTC_DS3231Sens::init() {
    if (!RTC_DS3231::begin(wirePtr)) {
        Serial.println("Couldn't find RTC");
        while (1) delay(10);
    }
    if (RTC_DS3231::lostPower()) {
        Serial.println("RTC lost power, let's set the time!");
        RTC_DS3231::adjust(DateTime(F(__DATE__), F(__TIME__)));
    }
    if (dateTime == DateTime()) {
//        RTC_DS3231::adjust(DateTime(F(__DATE__), F(__TIME__)));
    } else {
        RTC_DS3231::adjust(dateTime);
    }
}

void RTC_DS3231Sens::update() {
    DateTime now = RTC_DS3231::now();
    sensorValue[NOW_YEAR] = now.year();
    sensorValue[NOW_MONTH] = now.month();
    sensorValue[NOW_DAY] = now.day();

    sensorValue[NOW_DAY_OF_THE_WEEK] = now.dayOfTheWeek();
    sensorValue[NOW_HOUR] = now.hour();
    sensorValue[NOW_MINUTES] = now.minute();
    sensorValue[NOW_SECOND] = now.second();
}

void RTC_DS3231Sens::getValue(int *output) {
    for (int i = 0; i < 7; ++i) {
        output[i] = sensorValue[i];
    }
}

float RTC_DS3231Sens::getValueRTC_DS3231Sens() const {
    return 0.0f;
}

void RTC_DS3231Sens::getValueRTC(rtc_data_t *data) {
    data->year = sensorValue[NOW_YEAR];
    data->month = sensorValue[NOW_MONTH];
    data->day = sensorValue[NOW_DAY];
    data->daysOfTheWeek = sensorValue[NOW_DAY_OF_THE_WEEK];
    data->hour = sensorValue[NOW_HOUR];
    data->minute = sensorValue[NOW_MINUTES];
    data->second = sensorValue[NOW_SECOND];
}

String RTC_DS3231Sens::getDaysOfTheWeek(uint8_t index) {
    return {daysOfTheWeek[index]};
}

void RTC_DS3231Sens::setPins(uint8_t _pin) {
    sensorPin = _pin;
}
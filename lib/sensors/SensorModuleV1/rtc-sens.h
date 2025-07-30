/*
 *  rtc-sens.h
 *
 *  rtc sensor lib
 *  Created on: 2023. 4. 3
 */

#pragma once

#ifndef RTC_SENS_H
#define RTC_SENS_H

#pragma message("[COMPILED]: rtc-sens.h")

#include "Arduino.h"
#include "base/sensor-module.h"
#include "RTClib.h"

const char daysOfTheWeek[7][12] =
        {"Sunday", "Monday",
         "Tuesday", "Wednesday",
         "Thursday", "Friday",
         "Saturday"};

class RTC_DS3231Sens : public BaseSens, public RTC_DS3231 {
private:
    /*variables*/
    JsonDocument *doc;
    const char *name;

    TwoWire *wirePtr;
    uint8_t timeCfg;
    DateTime dateTime;
    uint8_t sensorPin;

    /*optional object*/
    using RTC_DS3231::RTC_DS3231;

public:
    explicit RTC_DS3231Sens(uint8_t timeConfig = RTC_DS3231Sens::ALL,
                            const DateTime &dt = DateTime(),
                            TwoWire *wireInstance = &Wire);
    ~RTC_DS3231Sens();
    bool init() override;
    bool update() override;

    void setDocument(const char *objName) override;
    void setDocumentValue(JsonDocument *docBase) override;
    JsonDocument getDocument() override;
    JsonVariant getVariant(const char *searchName) override;

    DateTime getDateTime();
    static uint32_t toUnixTime(const String &timestamp);

    static const uint8_t ALL = 0b11111111;
    static const uint8_t Y = 0b00000001;
    static const uint8_t M = 0b00000010;
    static const uint8_t D = 0b00000100;
    static const uint8_t d = 0b00001000;
    static const uint8_t h = 0b00010000;
    static const uint8_t m = 0b00100000;
    static const uint8_t s = 0b01000000;
    static const uint8_t T = 0b10000000;
};

#endif  // RTC_SENS_H
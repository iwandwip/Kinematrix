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

class RTC_DS3231Sens : public BaseSens, public RTC_DS3231 {
private:
    /*variables*/
    JsonDocument *doc;
    const char *name;

    TwoWire *wirePtr;
    DateTime dateTime;
    uint8_t sensorPin;

    /*optional object*/
    using RTC_DS3231::RTC_DS3231;

public:
    explicit RTC_DS3231Sens(const DateTime &dt = DateTime(), TwoWire *wireInstance = &Wire);
    ~RTC_DS3231Sens();
    void init() override;
    void update() override;

    void setDocument(const char *objName) override;
    void setDocumentValue(JsonDocument *docBase) override;
    JsonDocument getDocument() override;
    JsonVariant getVariant(const char *searchName) override;
};

#endif  // RTC_SENS_H
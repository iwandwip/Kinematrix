/*
 *  rtc-sens.h
 *
 *  rtc sensor lib
 *  Created on: 2023. 4. 3
 */

#ifdef USE_RTC_SENS
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


RTC_DS3231Sens::RTC_DS3231Sens(uint8_t timeConfig,
                               const DateTime &dt,
                               TwoWire *wireInstance)
        : timeCfg(timeConfig),
          wirePtr(wireInstance),
          dateTime(dt) {
}

RTC_DS3231Sens::~RTC_DS3231Sens() = default;

bool RTC_DS3231Sens::init() {
    if (strcmp(name, "") == 0 && doc == nullptr) {
        name = "RTC_DS3231Sens";
        doc = new JsonDocument;
    }
    if (!RTC_DS3231::begin(wirePtr)) {
        return false;
    }
//    if (RTC_DS3231::lostPower()) {
//        Serial.println("RTC lost power, let's set the time!");
//        RTC_DS3231::adjust(DateTime(F(__DATE__), F(__TIME__)));
//    }
    if (dateTime == DateTime()) {
        // RTC_DS3231::adjust(DateTime(F(__DATE__), F(__TIME__)));
    } else {
        Serial.println("| [INFO] RTC set time");
        String timestamp = RTC_DS3231::now().timestamp();
        timestamp.replace('T', ' ');
        Serial.println(timestamp);
        RTC_DS3231::adjust(dateTime);
    }

    if (timeCfg & RTC_DS3231Sens::Y) (*doc)[name]["Y"] = 0;
    if (timeCfg & RTC_DS3231Sens::M) (*doc)[name]["M"] = 0;
    if (timeCfg & RTC_DS3231Sens::D) (*doc)[name]["D"] = 0;
    if (timeCfg & RTC_DS3231Sens::d) (*doc)[name]["d"] = 0;

    if (timeCfg & RTC_DS3231Sens::h) (*doc)[name]["h"] = 0;
    if (timeCfg & RTC_DS3231Sens::m) (*doc)[name]["m"] = 0;
    if (timeCfg & RTC_DS3231Sens::s) (*doc)[name]["s"] = 0;
    if (timeCfg & RTC_DS3231Sens::T) (*doc)[name]["T"] = 0;
    return true;
}

bool RTC_DS3231Sens::update() {
    DateTime now = RTC_DS3231::now();

    if (timeCfg & RTC_DS3231Sens::Y) (*doc)[name]["Y"] = now.year();
    if (timeCfg & RTC_DS3231Sens::M) (*doc)[name]["M"] = now.month();
    if (timeCfg & RTC_DS3231Sens::D) (*doc)[name]["D"] = now.day();
    if (timeCfg & RTC_DS3231Sens::d) (*doc)[name]["d"] = daysOfTheWeek[now.dayOfTheWeek()];

    if (timeCfg & RTC_DS3231Sens::h) (*doc)[name]["h"] = now.hour();
    if (timeCfg & RTC_DS3231Sens::m) (*doc)[name]["m"] = now.minute();
    if (timeCfg & RTC_DS3231Sens::s) (*doc)[name]["s"] = now.second();
    if (timeCfg & RTC_DS3231Sens::T) (*doc)[name]["T"] = now.timestamp();
    return true;
}

void RTC_DS3231Sens::setDocument(const char *objName) {
    name = objName;
}

void RTC_DS3231Sens::setDocumentValue(JsonDocument *docBase) {
    doc = docBase;
}

JsonDocument RTC_DS3231Sens::getDocument() {
    return (*doc);
}

JsonVariant RTC_DS3231Sens::getVariant(const char *searchName) {
    return (*doc)[searchName];
}

DateTime RTC_DS3231Sens::getDateTime() {
    return RTC_DS3231::now();
}

uint32_t RTC_DS3231Sens::toUnixTime(const String &timestamp) {
    return DateTime(timestamp.c_str()).unixtime();
}

#endif  // RTC_SENS_H
#endif
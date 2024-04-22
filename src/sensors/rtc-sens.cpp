/*
 *  rtc-sens.cpp
 *
 *  rtc sensor c
 *  Created on: 2023. 4. 3
 */

#include "rtc-sens.h"
#include "Arduino.h"

RTC_DS3231Sens::RTC_DS3231Sens(uint8_t timeConfig,
                               const DateTime &dt,
                               TwoWire *wireInstance)
        : timeCfg(timeConfig),
          wirePtr(wireInstance),
          dateTime(dt) {
}

RTC_DS3231Sens::~RTC_DS3231Sens() = default;

void RTC_DS3231Sens::init() {
    if (strcmp(name, "") == 0 && doc == nullptr) {
        name = "RTC_DS3231Sens";
        doc = new JsonDocument;
    }
    if (!RTC_DS3231::begin(wirePtr)) {
        Serial.println("Couldn't find RTC");
        Serial.flush();
        while (true) delay(10);
    }
//    if (RTC_DS3231::lostPower()) {
//        Serial.println("RTC lost power, let's set the time!");
//        RTC_DS3231::adjust(DateTime(F(__DATE__), F(__TIME__)));
//    }
    if (dateTime == DateTime()) {
        // RTC_DS3231::adjust(DateTime(F(__DATE__), F(__TIME__)));
        Serial.println("| [INFO] RTC init success");
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
}

void RTC_DS3231Sens::update() {
    DateTime now = RTC_DS3231::now();

    if (timeCfg & RTC_DS3231Sens::Y) (*doc)[name]["Y"] = now.year();
    if (timeCfg & RTC_DS3231Sens::M) (*doc)[name]["M"] = now.month();
    if (timeCfg & RTC_DS3231Sens::D) (*doc)[name]["D"] = now.day();
    if (timeCfg & RTC_DS3231Sens::d) (*doc)[name]["d"] = daysOfTheWeek[now.dayOfTheWeek()];

    if (timeCfg & RTC_DS3231Sens::h) (*doc)[name]["h"] = now.hour();
    if (timeCfg & RTC_DS3231Sens::m) (*doc)[name]["m"] = now.minute();
    if (timeCfg & RTC_DS3231Sens::s) (*doc)[name]["s"] = now.second();
    if (timeCfg & RTC_DS3231Sens::T) (*doc)[name]["T"] = now.timestamp();
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

uint32_t RTC_DS3231Sens::toUnixTime(const String &timestamp) {
    return DateTime(timestamp.c_str()).unixtime();
}
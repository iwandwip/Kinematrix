/*
 *  rtc-sens.cpp
 *
 *  rtc sensor c
 *  Created on: 2023. 4. 3
 */

#include "rtc-sens.h"
#include "Arduino.h"

RTC_DS3231Sens::RTC_DS3231Sens(const DateTime &dt, TwoWire *wireInstance)
        : wirePtr(wireInstance),
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
        while (1) delay(10);
    }
    if (RTC_DS3231::lostPower()) {
        Serial.println("RTC lost power, let's set the time!");
        RTC_DS3231::adjust(DateTime(F(__DATE__), F(__TIME__)));
    }
    if (dateTime == DateTime()) {
        // RTC_DS3231::adjust(DateTime(F(__DATE__), F(__TIME__)));
    } else {
        RTC_DS3231::adjust(dateTime);
    }

    (*doc)[name]["y"] = 0;
    (*doc)[name]["m"] = 0;
    (*doc)[name]["d"] = 0;
    (*doc)[name]["day"] = "";

    (*doc)[name]["hh"] = 0;
    (*doc)[name]["mm"] = 0;
    (*doc)[name]["ss"] = 0;
    (*doc)[name]["ux"] = 0;
}

void RTC_DS3231Sens::update() {
    DateTime now = RTC_DS3231::now();

    (*doc)[name]["y"] = now.year();
    (*doc)[name]["m"] = now.month();
    (*doc)[name]["d"] = now.day();
    (*doc)[name]["day"] = daysOfTheWeek[now.dayOfTheWeek()];

    (*doc)[name]["hh"] = now.hour();
    (*doc)[name]["mm"] = now.minute();
    (*doc)[name]["ss"] = now.second();
    (*doc)[name]["ux"] = now.unixtime();
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
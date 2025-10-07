#pragma once

#ifndef BASE_RTC_SENS_V2_H
#define BASE_RTC_SENS_V2_H

#pragma message("[COMPILED]: BaseRTCSensV2.h")

#include "Arduino.h"
#include "../../SensorModule/SensorModuleV2.h"
#include "RTClib.h"

#define RTC_YEAR      0x01
#define RTC_MONTH     0x02
#define RTC_DAY       0x04
#define RTC_HOUR      0x08
#define RTC_MINUTE    0x10
#define RTC_SECOND    0x20
#define RTC_DAYOFWEEK 0x40
#define RTC_UNIXTIME  0x80
#define RTC_ALL       0xFF

class BaseRTCSensV2 : public BaseSensV2 {
protected:
    TwoWire *_twoWirePtr;
    uint32_t _updateTimer;
    uint32_t _updateInterval;
    uint8_t _enabledComponents;
    DateTime _currentDateTime;
    bool _isInitialized;
    bool _lastUpdateStatus;

    virtual void registerTimeValues();
    virtual void updateTimeValues();
    
public:
    BaseRTCSensV2(TwoWire *twoWirePtr = &Wire);
    virtual ~BaseRTCSensV2();

    void setUpdateInterval(uint32_t interval);
    void enableTimeComponents(uint8_t components);
    void disableTimeComponents(uint8_t components);
    
    DateTime getDateTime();
    uint32_t getUnixTime();
    virtual bool setDateTime(const DateTime &dt) = 0;
    
    String getFormattedTime(const String &format = "YYYY-MM-DD hh:mm:ss");
    
    uint16_t getYear();
    uint8_t getMonth();
    uint8_t getDay();
    uint8_t getHour();
    uint8_t getMinute();
    uint8_t getSecond();
    uint8_t getDayOfWeek();
    
    bool isUpdated() const override;
};

#endif
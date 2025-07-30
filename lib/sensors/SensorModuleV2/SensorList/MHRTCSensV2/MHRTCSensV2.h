#pragma once

#ifndef MHRTC_SENS_V2_H
#define MHRTC_SENS_V2_H

#pragma message("[COMPILED]: MHRTCSensV2.h")

#include "Arduino.h"
#include "../../SensorModule/SensorModuleV2.h"
#include "Ds1302.h"

class MHRTCSensV2 : public BaseSensV2, public Ds1302 {
private:
    uint8_t _pinEna;
    uint8_t _pinClk;
    uint8_t _pinDat;
    uint32_t _updateTimer;
    uint32_t _updateInterval;
    bool _lastUpdateStatus;
    bool _isInitialized;
    bool _clockRunning;

    void updateTimeValues(const Ds1302::DateTime& dt);

public:
    MHRTCSensV2(uint8_t pinEna, uint8_t pinClk, uint8_t pinDat);
    virtual ~MHRTCSensV2();

    bool init() override;
    bool update() override;
    bool isUpdated() const override;

    void setUpdateInterval(uint32_t interval);
    void setPins(uint8_t pinEna, uint8_t pinClk, uint8_t pinDat);

    bool setDateTime(uint8_t year, uint8_t month, uint8_t day, 
                     uint8_t hour, uint8_t minute, uint8_t second, 
                     uint8_t dayOfWeek = 1);
    bool setDateTime(const Ds1302::DateTime& dt);
    
    Ds1302::DateTime getDateTime();
    bool isClockRunning() const;
    void startClock();
    void stopClock();
    
    void setYear(uint8_t year);
    void setMonth(uint8_t month);
    void setDay(uint8_t day);
    void setHour(uint8_t hour);
    void setMinute(uint8_t minute);
    void setSecond(uint8_t second);
    void setDayOfWeek(uint8_t dow);

    String getFormattedDateTime(const char* format = "YYYY-MM-DD HH:mm:ss");
    String getFormattedDate(const char* format = "YYYY-MM-DD");
    String getFormattedTime(const char* format = "HH:mm:ss");
    
    const char* getDayOfWeekName(uint8_t dow) const;
    const char* getMonthName(uint8_t month) const;
    
    uint32_t getUnixTimestamp();
    bool setUnixTimestamp(uint32_t timestamp);
    
    bool isLeapYear(uint8_t year) const;
    uint8_t getDaysInMonth(uint8_t month, uint8_t year) const;
};

#endif
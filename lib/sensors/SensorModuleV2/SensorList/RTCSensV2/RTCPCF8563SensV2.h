#pragma once

#ifndef RTC_PCF8563_SENS_V2_H
#define RTC_PCF8563_SENS_V2_H

#pragma message("[COMPILED]: RTCPCF8563SensV2.h")

#include "Arduino.h"
#include "BaseRTCSensV2.h"
#include "RTClib.h"

class RTCPCF8563SensV2 : public BaseRTCSensV2, public RTC_PCF8563 {
private:
    bool _enableVoltageCheck;
    bool _voltageLow;
    Pcf8563SqwPinMode _squareWaveMode;
    bool _useSquareWave;
    
protected:
    void registerTimeValues() override;
    void updateTimeValues() override;
    
public:
    RTCPCF8563SensV2(TwoWire *twoWirePtr = &Wire);
    virtual ~RTCPCF8563SensV2();

    bool init() override;
    bool update() override;
    bool setDateTime(const DateTime &dt) override;
    
    void enableVoltageCheck(bool enable);
    bool isVoltageLow();
    
    void enableSquareWave(bool enable, Pcf8563SqwPinMode mode = PCF8563_SquareWave1Hz);
    
    void enableAlarm();
    void disableAlarm();
    bool alarmFired();
    void clearAlarm();
    void setAlarm(uint8_t minute, uint8_t hour = 0, uint8_t day = 0, uint8_t weekday = 0);
};

#endif
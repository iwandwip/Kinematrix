#pragma once

#ifndef RTC_DS3231_SENS_V2_H
#define RTC_DS3231_SENS_V2_H

#pragma message("[COMPILED]: RTCDS3231SensV2.h")

#include "Arduino.h"
#include "BaseRTCSensV2.h"
#include "RTClib.h"

#define RTC_TEMPERATURE 0x100

class RTCDS3231SensV2 : public BaseRTCSensV2, public RTC_DS3231 {
private:
    bool _enableTemperature;
    float _temperature;
    bool _use32kHz;
    bool _useSquareWave;
    Ds3231SqwPinMode _squareWaveMode;
    
protected:
    void registerTimeValues() override;
    void updateTimeValues() override;
    
public:
    RTCDS3231SensV2(TwoWire *twoWirePtr = &Wire);
    virtual ~RTCDS3231SensV2();

    bool init() override;
    bool update() override;
    bool setDateTime(const DateTime &dt) override;
    
    void enableTemperature(bool enable);
    float getTemperature();
    
    void enable32kHz(bool enable);
    void enableSquareWave(bool enable, Ds3231SqwPinMode mode = DS3231_SquareWave1Hz);
    
    bool lostPower();
    void disableAlarms();
    void clearAlarms();
    
    void setAgingOffset(int8_t offset);
    int8_t getAgingOffset();
};

#endif
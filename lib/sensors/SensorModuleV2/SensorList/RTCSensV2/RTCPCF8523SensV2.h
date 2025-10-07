#pragma once

#ifndef RTC_PCF8523_SENS_V2_H
#define RTC_PCF8523_SENS_V2_H

#pragma message("[COMPILED]: RTCPCF8523SensV2.h")

#include "Arduino.h"
#include "BaseRTCSensV2.h"
#include "RTClib.h"

class RTCPCF8523SensV2 : public BaseRTCSensV2, public RTC_PCF8523 {
private:
    bool _enableBatteryCheck;
    bool _batteryLow;
    Pcf8523SqwPinMode _squareWaveMode;
    bool _useSquareWave;
    
protected:
    void registerTimeValues() override;
    void updateTimeValues() override;
    
public:
    RTCPCF8523SensV2(TwoWire *twoWirePtr = &Wire);
    virtual ~RTCPCF8523SensV2();

    bool init() override;
    bool update() override;
    bool setDateTime(const DateTime &dt) override;
    
    void enableBatteryCheck(bool enable);
    bool isBatteryLow();
    
    void enableSquareWave(bool enable, Pcf8523SqwPinMode mode = PCF8523_SquareWave1HZ);
    void calibrate(Pcf8523OffsetMode mode, int8_t offset);
    
    void enableSecondTimer();
    void disableSecondTimer();
    void enableCountdownTimer(PCF8523TimerClockFreq freq, uint8_t numPeriods);
    void disableCountdownTimer();
};

#endif
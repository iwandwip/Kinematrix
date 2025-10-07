#pragma once

#ifndef RTC_DS1307_SENS_V2_H
#define RTC_DS1307_SENS_V2_H

#pragma message("[COMPILED]: RTCDS1307SensV2.h")

#include "Arduino.h"
#include "BaseRTCSensV2.h"
#include "RTClib.h"

class RTCDS1307SensV2 : public BaseRTCSensV2, public RTC_DS1307 {
private:
    bool _useSquareWave;
    Ds1307SqwPinMode _squareWaveRate;
    
public:
    RTCDS1307SensV2(TwoWire *twoWirePtr = &Wire);
    virtual ~RTCDS1307SensV2();

    bool init() override;
    bool update() override;
    bool setDateTime(const DateTime &dt) override;
    
    void enableSquareWave(bool enable);
    void setSquareWaveRate(Ds1307SqwPinMode rate);
    bool readNVRAM(uint8_t address, uint8_t *buf, uint8_t size);
    bool writeNVRAM(uint8_t address, uint8_t *buf, uint8_t size);
};

#endif
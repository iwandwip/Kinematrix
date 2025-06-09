#pragma once

#ifndef RTC_SENS_V2
#define RTC_SENS_V2

#pragma message("[COMPILED]: RTCDS1307Sens.h")

#include "Arduino.h"
#include "../SensorModuleV2.h"
#include "RTClib.h"

class RTCDS1307Sens : public BaseSensV2, public RTC_DS1307 {
private:
    TwoWire *twoWirePtr;
    uint32_t _updateTimer;
    uint32_t _updateInterval;

    using RTC_DS1307::RTC_DS1307;

public:
    RTCDS1307Sens(const DateTime &dt = DateTime(), TwoWire *_twoWirePtr = &Wire);
    virtual ~RTCDS1307Sens();

    bool init() override;
    bool update() override;

    void setUpdateInterval(uint32_t interval);
};

#endif  // RTC_SENS_V2
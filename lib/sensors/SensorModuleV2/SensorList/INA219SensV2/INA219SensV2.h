#pragma once

#ifndef INA219_SENS_V2_H
#define INA219_SENS_V2_H

#pragma message("[COMPILED]: INA219SensV2.h")

#include "Arduino.h"
#include "../../SensorModule/SensorModuleV2.h"
#include "Adafruit_INA219.h"

class INA219SensV2 : public BaseSensV2, public Adafruit_INA219 {
private:
    uint32_t _updateTimer;
    uint32_t _updateInterval;
    bool _lastUpdateStatus;

    using Adafruit_INA219::Adafruit_INA219;

public:
    INA219SensV2(uint8_t addr = INA219_ADDRESS);
    virtual ~INA219SensV2();

    bool init() override;
    bool update() override;
    bool isUpdated() const override;

    void setUpdateInterval(uint32_t interval);
};

#endif  // INA219_SENS_V2_H
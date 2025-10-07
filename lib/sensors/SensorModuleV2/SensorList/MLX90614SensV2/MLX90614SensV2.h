#pragma once

#ifndef MLX90614_SENS_V2_H
#define MLX90614_SENS_V2_H

#pragma message("[COMPILED]: MLX90614SensV2.h")

#include "Arduino.h"
#include "../../SensorModule/SensorModuleV2.h"
#include "Adafruit_MLX90614.h"

class MLX90614SensV2 : public BaseSensV2, public Adafruit_MLX90614 {
private:
    uint32_t _updateTimer;
    uint32_t _updateInterval;
    bool _lastUpdateStatus;

    using Adafruit_MLX90614::Adafruit_MLX90614;

public:
    MLX90614SensV2();
    virtual ~MLX90614SensV2();

    bool init() override;
    bool update() override;
    bool isUpdated() const override;

    void setUpdateInterval(uint32_t interval);
};

#endif  // MLX90614_SENS_V2_H
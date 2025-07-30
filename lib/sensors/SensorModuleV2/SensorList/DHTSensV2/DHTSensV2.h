#pragma once

#ifndef DHT_SENS_V2_H
#define DHT_SENS_V2_H

#pragma message("[COMPILED]: dht-sens-v2.h")

#include "Arduino.h"
#include "../../SensorModule/SensorModuleV2.h"
#include "DHT.h"

class DHTSensV2 : public BaseSensV2, public DHT {
private:
    uint8_t _sensorPin;
    uint32_t _updateTimer;
    uint32_t _updateInterval;
    bool _lastUpdateStatus;

    using DHT::DHT;

public:
    DHTSensV2(uint8_t pin, uint8_t type);
    virtual ~DHTSensV2();

    bool init() override;
    bool update() override;
    bool isUpdated() const override;

    void setUpdateInterval(uint32_t interval);
    void setPins(uint8_t pin);
    void reinitializeSensor();
};

#endif  // DHT_SENS_V2_H
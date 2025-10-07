#pragma once

#ifndef ABSTRACT_SENS_V2_H
#define ABSTRACT_SENS_V2_H

#pragma message("[COMPILED]: abstract-sens-v2.h")

#include "Arduino.h"
#include "../../SensorModule/SensorModuleV2.h"

class AbstractSensV2 : public BaseSensV2 {
private:
    int _enumRandomMode;
    float _dummyValue1;
    float _dummyValue2;
    uint32_t _updateTimer;
    uint32_t _updateInterval;
    bool _lastUpdateStatus;

public:
    AbstractSensV2(int enumRandomMode = 0);
    AbstractSensV2(int enumRandomMode, float dummyValue1, float dummyValue2 = 0);
    virtual ~AbstractSensV2();

    bool init() override;
    bool update() override;
    bool isUpdated() const override;

    void setUpdateInterval(uint32_t interval);
    void setRandomMode(int enumRandomMode);
    void setDummyValues(float dummyValue1, float dummyValue2 = 0);

    void addCustomValue(const char *key, const char *label, const char *unit, uint8_t precision, bool calibrable);
};

#endif  // ABSTRACT_SENS_V2_H
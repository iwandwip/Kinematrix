#pragma once

#ifndef ANALOG_SENS_V2_H
#define ANALOG_SENS_V2_H

#pragma message("[COMPILED]: analog-sens-v2.h")

#include "Arduino.h"
#include "../SensorModuleV2.h"

class AnalogSensV2 : public BaseSensV2 {
private:
    float _voltageReference;
    int _adcRange;
    uint8_t _sensorPin;
    uint32_t _updateTimer;
    uint32_t _updateInterval;

    typedef void (*CustomDataCallback)(BaseSensV2 *sensor, int analogValue, float voltageValue);
    CustomDataCallback _onCustomData;

public:
    AnalogSensV2(uint8_t pin, float vref, int adcRange, CustomDataCallback onCustomData = nullptr);
    virtual ~AnalogSensV2();

    bool init() override;
    bool update() override;

    void setUpdateInterval(uint32_t interval);
    void setPins(uint8_t pin);

    void setCustomDataCallback(CustomDataCallback callback);

    void addCustomValue(const char *key, const char *label, const char *unit, uint8_t precision, bool calibrable);
};

#endif  // ANALOG_SENS_V2_H
#pragma once

#ifndef CUSTOM_SENSOR_TEMPLATE_V2_H
#define CUSTOM_SENSOR_TEMPLATE_V2_H

#pragma message("[COMPILED]: custom-sensor-template-v2.h")

#include "Arduino.h"
#include "../SensorModuleV2.h"

class CustomSensorTemplateV2 : public BaseSensV2 {
private:
    uint8_t _sensorPin;
    uint32_t _updateTimer;
    uint32_t _updateInterval;
    float _calibrationFactor;
    int _sensorMode;

public:
    CustomSensorTemplateV2(uint8_t pin);
    virtual ~CustomSensorTemplateV2();
    bool init() override;
    bool update() override;
    void setUpdateInterval(uint32_t interval);
    void setCalibrationFactor(float factor);
    void setSensorMode(int mode);
    void addCustomSensorValue(const char *key, const char *label, const char *unit, uint8_t precision, bool calibrable);
};

#endif
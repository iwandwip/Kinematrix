/*
 *  flowmeterv2-sens.h
 *
 *  flowmeterv2 sensor lib
 *  Created on: 2023. 4. 3
 */

#pragma once

#ifndef FLOWMETERV2_SENS_H
#define FLOWMETERV2_SENS_H

#pragma message("[COMPILED]: abstract-sens.h")

#include "Arduino.h"
#include "base/sensor-module.h"

#ifndef FLOW_SENSOR_V2
#define FLOW_SENSOR_V2

#include "Arduino.h"

class FlowSensor {
private:
    uint8_t _pin;
    volatile unsigned long _totalpulse;
    volatile unsigned long _pulse;
    float _pulse1liter;
    float _flowratehour;
    float _flowrateminute;
    float _flowratesecound;
    float _volume;
    unsigned long _timebefore = 0;
    unsigned long _interval;
    bool _isTimeToRead();

public:
    FlowSensor(uint16_t type, uint8_t pin);
    void begin(void (*userFunc)(void), bool pullup = false);
    void count();
    bool read(long calibration = 0);
    void setType(uint16_t type);
    void setPin(uint8_t pin);
    void setInterval(unsigned long interval);
    unsigned long getPulse();
    float getFlowRate_h();
    float getFlowRate_m();
    float getFlowRate_s();
    float getVolume();
    void resetPulse();
    void resetVolume();
};

#endif

/////////////////////////////////////////////

class FlowMeterV2Sens : public BaseSens, public FlowSensor {
private:
    JsonDocument *doc;
    const char *name;

    uint8_t sensorPin;
    uint32_t sensorTimer;

    using FlowSensor::FlowSensor;

public:
    FlowMeterV2Sens(uint16_t type, uint8_t pin, uint32_t interval, void (*callback)(void));
    virtual ~FlowMeterV2Sens();
    bool init() override;
    bool update() override;

    void setDocument(const char *objName) override;
    void setDocumentValue(JsonDocument *docBase) override;
    JsonDocument getDocument() override;
    JsonVariant getVariant(const char *searchName) override;

    float getValueFlowMeterV2Sens() const;
    void setPins(uint8_t _pin);
    void readInterrupt();
};

#endif  // FLOWMETERV2_SENS_H
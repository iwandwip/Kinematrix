#pragma once

#ifndef FLOWMETERV3_SENS_H
#define FLOWMETERV3_SENS_H

#include "Arduino.h"
#include "base/sensor-module.h"

#if defined(ESP32)
#define PLATFORM_ESP32
#elif defined(ESP8266)
#define PLATFORM_ESP8266
#else
#define PLATFORM_ARDUINO
#endif

class FlowMeterV3Sens;

struct FlowSensorNode {
    uint8_t pin;
    FlowMeterV3Sens *instance;
    FlowSensorNode *next;

    FlowSensorNode(uint8_t _pin, FlowMeterV3Sens *_instance) :
            pin(_pin), instance(_instance), next(nullptr) {}
};

#ifndef FLOW_SENSOR_V3
#define FLOW_SENSOR_V3

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

    virtual ~FlowSensor() {}

    void setType(uint16_t type);
    void setPin(uint8_t pin);
    void setInterval(unsigned long interval);
    void count();
    bool read(long calibration = 0);
    unsigned long getPulse();
    float getFlowRate_h();
    float getFlowRate_m();
    float getFlowRate_s();
    float getVolume();
    void resetPulse();
    void resetVolume();
    uint8_t getPin();
};

#endif

typedef void (*InterruptCallback)();

class FlowMeterV3Sens : public BaseSens, public FlowSensor {
private:
    JsonDocument *doc;
    const char *name;
    uint8_t sensorPin;
    uint32_t sensorTimer;
    bool usePullup;
    InterruptCallback userCallback;

    static FlowSensorNode *sensorList;

    bool registerInstance();

#ifdef PLATFORM_ESP32
    static void IRAM_ATTR isr(void *arg);
#endif

    using FlowSensor::FlowSensor;

public:
    FlowMeterV3Sens(uint16_t type, uint8_t pin, uint32_t interval, bool pullup = false, InterruptCallback callback = nullptr);
    virtual ~FlowMeterV3Sens();
    bool init() override;
    bool update() override;

    void setDocument(const char *objName) override;
    void setDocumentValue(JsonDocument *docBase) override;
    JsonDocument getDocument() override;
    JsonVariant getVariant(const char *searchName) override;

    float getValueFlowMeterV3Sens() const;
    void setPins(uint8_t _pin);
    void readInterrupt();

    static void handleInterrupt(uint8_t pin);

    void setPullup(bool pullup);
    void setCallback(InterruptCallback callback);
};

#endif
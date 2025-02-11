/*
 *  mq-sens.h
 *
 *  mq sensor lib
 *  Created on: 2023. 4. 3
 */

#pragma once

#ifndef MQ_UNIFIED_SENS_H
#define MQ_UNIFIED_SENS_H

#pragma message("[COMPILED]: abstract-sens.h")

#include "Arduino.h"
#include "base/sensor-module.h"
#include "MQUnifiedsensor.h"

class MQUnifiedSens : public BaseSens, public MQUnifiedsensor{
private:
    JsonDocument *doc;
    const char *name;

    uint8_t sensorPin;
    uint32_t sensorTimer;

    using MQUnifiedsensor::MQUnifiedsensor;

public:
    virtual ~MQUnifiedSens();
    bool init() override;
    bool update() override;

    void setDocument(const char *objName) override;
    void setDocumentValue(JsonDocument *docBase) override;
    JsonDocument getDocument() override;
    JsonVariant getVariant(const char *searchName) override;

    float getValueMQUnifiedSens() const;
    void setPins(uint8_t _pin);
};

#endif  // MQ_UNIFIED_SENS_H
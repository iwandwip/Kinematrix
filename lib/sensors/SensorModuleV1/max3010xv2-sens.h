/*
 *  max3010x-sens.h
 *
 *  max3010x sensor lib
 *  Created on: 2023. 4. 3
 */

#pragma once

#ifndef MAX_3010X_V2_SENS_H
#define MAX_3010X_V2_SENS_H

#pragma message("[COMPILED]: max3010x-sens.h")

#include "Arduino.h"
#include "base/sensor-module.h"
#include "MAX30105.h"
#include "heartRate.h"
#include "spo2_algorithm.h"

#define MAX_BRIGHTNESS 255

class MAX3010xV2Sens : public BaseSens, public MAX30105 {
private:
    JsonDocument *doc;
    const char *name;

    uint32_t irBuffer[100];
    uint32_t redBuffer[100];

    int32_t bufferLength;
    int32_t spo2;
    int8_t validSPO2;
    int32_t heartRate;
    int8_t validHeartRate;

    unsigned long previousMillis = 0;
    const long interval = 10;
    int step = 0;
    int index = 25;

    uint8_t sensorPin;
    uint32_t sensorTimer;

public:
    virtual ~MAX3010xV2Sens();
    bool init() override;
    bool update() override;

    void setDocument(const char *objName) override;
    void setDocumentValue(JsonDocument *docBase) override;
    JsonDocument getDocument() override;
    JsonVariant getVariant(const char *searchName) override;

    float getValueMAX3010xV2Sens() const;
    void setPins(uint8_t _pin);
};

#endif  // MAX_3010X_V2_SENS_H
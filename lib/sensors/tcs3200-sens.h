/*
 *  tcs3200-sens.h
 *
 *  tcs3200 sensor lib
 *  Created on: 2023. 4. 3
 */

#pragma once

#ifndef TCS_3200_SENS_H
#define TCS_3200_SENS_H

#pragma message("[COMPILED]: tcs3200-sens.h")

#include "Arduino.h"
#include "base/sensor-module.h"
#include "tcs3200.h"

class TCS3200Sens : public BaseSens, public tcs3200 {
private:
    JsonDocument *doc;
    const char *name;

    uint8_t sensorPin;
    uint32_t sensorTimer;

    using tcs3200::tcs3200;

public:
    virtual ~TCS3200Sens();
    bool init() override;
    bool update() override;

    void setDocument(const char *objName) override;
    void setDocumentValue(JsonDocument *docBase) override;
    JsonDocument getDocument() override;
    JsonVariant getVariant(const char *searchName) override;

    static String closestColorCustomRGB(int r, int g, int b, int distinctRGB[][3], String distinctColors[], int num_of_colors);

    float getValueTCS3200Sens() const;
    void setPins(uint8_t _pin);
};

#endif  // TCS_3200_SENS_H
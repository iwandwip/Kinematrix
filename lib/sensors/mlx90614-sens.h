/*
 *  mlx90614-sens.h
 *
 *  mlx90614 sensor lib
 *  Created on: 2023. 4. 3
 */

#pragma once

#ifndef MLX90614_SENS_H
#define MLX90614_SENS_H

#pragma message("[COMPILED]: abstract-sens.h")

#include "Arduino.h"
#include "base/sensor-module.h"
#include "Adafruit_MLX90614.h"

class MLX90614Sens : public BaseSens, public Adafruit_MLX90614 {
private:
    JsonDocument *doc;
    const char *name;

    uint8_t sensorPin;
    uint32_t sensorTimer;

    using Adafruit_MLX90614::Adafruit_MLX90614;

public:
    MLX90614Sens();
    virtual ~MLX90614Sens();
    bool init() override;
    bool update() override;

    void setDocument(const char *objName) override;
    void setDocumentValue(JsonDocument *docBase) override;
    JsonDocument getDocument() override;
    JsonVariant getVariant(const char *searchName) override;

    float getValueMLX90614Sens() const;
    void setPins(uint8_t _pin);
};

#endif  // MLX90614_SENS_H
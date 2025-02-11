/*
 *  pzem004t-sens.h
 *
 *  pzem004t sensor lib
 *  Created on: 2023. 4. 3
 */

#pragma once

#ifndef PZEM004T_SENS_H
#define PZEM004T_SENS_H

#pragma message("[COMPILED]: pzem004t-sens.h")

#include "Arduino.h"
#include "base/sensor-module.h"
#include "lib/PZEM004t/PZEM004Tc30_custom.h"
#if defined(ESP32)
#include "HardwareSerial.h"
#else
#include "SoftwareSerial.h"
#endif

class PZEM004TSens : public BaseSens, public PZEM004Tv30 {
private:
    JsonDocument *doc;
    const char *name;

    uint8_t sensorPin;
    uint32_t sensorTimer;
    uint8_t configuration = 0b00111111;

    using PZEM004Tv30::PZEM004Tv30;
public:
    static const uint8_t VOLTAGE = 0b00000001;
    static const uint8_t CURRENT = 0b00000010;
    static const uint8_t POWER = 0b00000100;
    static const uint8_t ENERGY = 0b00001000;
    static const uint8_t FREQUENCY = 0b00010000;
    static const uint8_t PF = 0b00100000;
    static const uint8_t ALL = 0b00111111;

    virtual ~PZEM004TSens();
    bool init() override;
    bool update() override;
    void setConfiguration(uint8_t config);

    void setDocument(const char *objName) override;
    void setDocumentValue(JsonDocument *docBase) override;
    JsonDocument getDocument() override;
    JsonVariant getVariant(const char *searchName) override;
};

#endif  // PZEM004T_SENS_H
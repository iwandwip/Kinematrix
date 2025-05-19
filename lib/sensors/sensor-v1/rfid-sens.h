/*
 *  rfid-sens.h
 *
 *  rfid sensor lib
 *  Created on: 2023. 4. 3
 */

#pragma once

#ifndef RFID_SENS_H
#define RFID_SENS_H

#pragma message("[COMPILED]: rfid-sens.h")

#include "Arduino.h"
#include "base/sensor-module.h"
#include "MFRC522.h"
#include "SPI.h"

class RFID_Mfrc522 : public BaseSens, public MFRC522 {
private:
    JsonDocument *doc;
    const char *name;

    uint8_t sensorPin;
    uint32_t sensorTimer;

    using MFRC522::MFRC522;
public:
    ~RFID_Mfrc522();
    bool init() override;
    bool update() override;

    void setDocument(const char *objName) override;
    void setDocumentValue(JsonDocument *docBase) override;
    JsonDocument getDocument() override;
    JsonVariant getVariant(const char *searchName) override;

    String getValueRFID_Mfrc522() const;
    void setPins(uint8_t _pin);
};

#endif  // RFID_SENS_H
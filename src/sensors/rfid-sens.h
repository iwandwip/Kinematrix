/*
 *  rfid-sens.h
 *
 *  rfid sensor lib
 *  Created on: 2023. 4. 3
 */
#ifdef USE_RFID_SENS
#pragma once

#ifndef RFID_SENS_H
#define RFID_SENS_H

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


RFID_Mfrc522::~RFID_Mfrc522() = default;

bool RFID_Mfrc522::init() {
    if (strcmp(name, "") == 0 && doc == nullptr) {
        name = "RFID_Mfrc522";
        doc = new JsonDocument;
    }
    SPI.begin();
    MFRC522::PCD_Init();
    (*doc)[name] = "";
    return true;
}

bool RFID_Mfrc522::update() {
    String uuid = "";
    if (MFRC522::PICC_IsNewCardPresent() && MFRC522::PICC_ReadCardSerial()) {
        for (byte i = 0; i < MFRC522::uid.size; i++) {
            uuid += String(MFRC522::uid.uidByte[i], HEX);
        }
        (*doc)[name] = uuid;
        MFRC522::PICC_HaltA();
        MFRC522::PCD_StopCrypto1();
        return true;
    }
    (*doc)[name] = "";
    return false;
}

void RFID_Mfrc522::setDocument(const char *objName) {
    name = objName;
}

void RFID_Mfrc522::setDocumentValue(JsonDocument *docBase) {
    doc = docBase;
}

JsonDocument RFID_Mfrc522::getDocument() {
    return (*doc);
}

JsonVariant RFID_Mfrc522::getVariant(const char *searchName) {
    return (*doc)[searchName];
}

String RFID_Mfrc522::getValueRFID_Mfrc522() const {
    return (*doc)[name].as<String>();
}

void RFID_Mfrc522::setPins(uint8_t _pin) {
    sensorPin = _pin;
}

#endif  // RFID_SENS_H
#endif
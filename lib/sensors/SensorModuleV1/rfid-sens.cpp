/*
 *  rfid-sens.cpp
 *
 *  rfid sensor c
 *  Created on: 2023. 4. 3
 */

#include "Arduino.h"
#include "rfid-sens.h"
#include "SPI.h"

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
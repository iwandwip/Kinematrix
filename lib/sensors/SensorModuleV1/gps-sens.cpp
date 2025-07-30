/*
 *  gps-sens.cpp
 *
 *  gps sensor c
 *  Created on: 2023. 4. 3
 */

#include "gps-sens.h"
#include "Arduino.h"

#if defined(ESP32)

GPSSens::GPSSens(HardwareSerial *_serialPtr, long baud, SerialConfig cfg,
                 int8_t rxPin, int8_t txPin) {
    serialPtr = _serialPtr;
    serialPtr->begin(baud, cfg, rxPin, txPin);
    serialPtr->setTimeout(100);
}

#endif

GPSSens::~GPSSens() = default;

bool GPSSens::init() {
    if (strcmp(name, "") == 0 && doc == nullptr) {
        name = "GPSSens";
        doc = new JsonDocument;
    }
    (*doc)[name]["lat"] = 0.0;
    (*doc)[name]["lng"] = 0.0;
    return true;
}

bool GPSSens::update() {
#if defined(ESP32)
    while (serialPtr->available() > 0) {
        if (gps.encode((char) serialPtr->read()) && gps.location.isValid()) {
            (*doc)[name]["lat"] = gps.location.lat();
            (*doc)[name]["lng"] = gps.location.lng();
            return true;
        }
    }
#endif
    return false;
}

void GPSSens::setDocument(const char *objName) {
    name = objName;
}

void GPSSens::setDocumentValue(JsonDocument *docBase) {
    doc = docBase;
}

JsonDocument GPSSens::getDocument() {
    return (*doc);
}

JsonVariant GPSSens::getVariant(const char *searchName) {
    return (*doc)[searchName];
}

float GPSSens::getValueGPSSens() const {
    return (*doc)[name].as<float>();
}

void GPSSens::setPins(uint8_t _pin) {
    sensorPin = _pin;
}
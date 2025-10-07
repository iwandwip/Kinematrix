/*
 *  dust-sens.cpp
 *
 *  dust sensor c
 *  Created on: 2023. 4. 3
 */

#include "dust-sens.h"
#include "Arduino.h"

DustSens::~DustSens() = default;

bool DustSens::init() {
    if (strcmp(name, "") == 0 && doc == nullptr) {
        name = "DustSens";
        doc = new JsonDocument;
    }
    GP2YDustSensor::begin();
    (*doc)[name] = 0;
    return true;
}

bool DustSens::update() {
    if (millis() - sensorTimer >= 1000) {
        (*doc)[name] = GP2YDustSensor::getDustDensity();
        sensorTimer = millis();
        return true;
    }
    return false;
}

void DustSens::setDocument(const char *objName) {
    name = objName;
}

void DustSens::setDocumentValue(JsonDocument *docBase) {
    doc = docBase;
}

JsonDocument DustSens::getDocument() {
    return (*doc);
}

JsonVariant DustSens::getVariant(const char *searchName) {
    return (*doc)[searchName];
}

float DustSens::getValueDustSens() const {
    return (*doc)[name].as<float>();
}

void DustSens::setPins(uint8_t _pin) {

}
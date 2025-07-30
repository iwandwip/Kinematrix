/*
 *  mq-sens.cpp
 *
 *  mq sensor c
 *  Created on: 2023. 4. 3
 */

#include "Arduino.h"
#include "mq-sens.h"

MQUnifiedSens::~MQUnifiedSens() = default;

bool MQUnifiedSens::init() {
    if (strcmp(name, "") == 0 && doc == nullptr) {
        name = "MQUnifiedSens";
        doc = new JsonDocument;
    }
    (*doc)[name] = 0;
    MQUnifiedsensor::init();
    return true;
}

bool MQUnifiedSens::update() {
    MQUnifiedsensor::update();
    (*doc)[name] = MQUnifiedsensor::readSensor();
    return true;
}

void MQUnifiedSens::setDocument(const char *objName) {
    name = objName;
}

void MQUnifiedSens::setDocumentValue(JsonDocument *docBase) {
    doc = docBase;
}

JsonDocument MQUnifiedSens::getDocument() {
    return (*doc);
}

JsonVariant MQUnifiedSens::getVariant(const char *searchName) {
    return (*doc)[searchName];
}

float MQUnifiedSens::getValueMQUnifiedSens() const {
    return (*doc)[name].as<float>();
}

void MQUnifiedSens::setPins(uint8_t _pin) {
    sensorPin = _pin;
}
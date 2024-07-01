/*
 *  ultrasonic-sens.cpp
 *
 *  ultrasonic sensor c
 *  Created on: 2023. 4. 3
 */

#include "ultrasonic-sens.h"
#include "Arduino.h"
#include "SPI.h"

UltrasonicSens::~UltrasonicSens() = default;

bool UltrasonicSens::init() {

    if (strcmp(name, "") == 0 && doc == nullptr) {
        name = "UltrasonicSens";
        doc = new JsonDocument;
    }
    (*doc)[name] = 0;
    return true;
}

bool UltrasonicSens::update() {
    if (millis() - sensorTimer >= 50) {
        (*doc)[name] = (int) this->ping_cm();
        sensorTimer = millis();
        return true;
    }
    return false;
}

void UltrasonicSens::setDocument(const char *objName) {
    name = objName;
}

JsonDocument UltrasonicSens::getDocument() {
    return (*doc);
}

void UltrasonicSens::setDocumentValue(JsonDocument *docBase) {
    doc = docBase;
}

JsonVariant UltrasonicSens::getVariant(const char *searchName) {
    return (*doc)[searchName];
}

float UltrasonicSens::getValueCm() const {
    return (*doc)[name].as<float>();
}

float UltrasonicSens::getValueIn() {
    return (float) this->ping_in();
}

void UltrasonicSens::setPins(uint8_t _pin) {
    sensorPin = _pin;
}
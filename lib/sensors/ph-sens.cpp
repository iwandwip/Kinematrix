/*
 *  ph-sens.cpp
 *
 *  ph sensor c
 *  Created on: 2023. 4. 3
 */

#include "ph-sens.h"
#include "Arduino.h"

PhSens::PhSens(uint8_t sensorPin, float voltage, float resolution, float *calibrationValue)
        : doc(nullptr),
          name(""),
          voltage(voltage),
          resolution(resolution),
          calibrationValue(calibrationValue),
          sensorPin(sensorPin) {

}

PhSens::~PhSens() = default;

bool PhSens::init() {
    if (strcmp(name, "") == 0 && doc == nullptr) {
        name = "PhSens";
        doc = new JsonDocument;
    }
    (*doc)[name]["volt"] = 0;
    (*doc)[name]["ph"] = 0;
    return true;
}

bool PhSens::update() {
    for (int i = 0; i < PH_SENS_BUFFER_SIZE; i++) {
        bufferAnalog[i] = analogRead(sensorPin);
        delay(30);
    }
    for (int i = 0; i < 9; i++) {
        for (int j = i + 1; j < PH_SENS_BUFFER_SIZE; j++) {
            if (bufferAnalog[i] > bufferAnalog[j]) {
                uint32_t temporaryAnalogValue = bufferAnalog[i];
                bufferAnalog[i] = bufferAnalog[j];
                bufferAnalog[j] = temporaryAnalogValue;
            }
        }
    }
    uint32_t averageAnalogValue = 0;
    for (int i = 2; i < 8; i++) {
        averageAnalogValue += bufferAnalog[i];
    }

    float phVoltageValue = (float) averageAnalogValue * voltage / resolution / 6.0;
    float phValue = -5.70 * phVoltageValue + *calibrationValue;

    (*doc)[name]["volt"] = phVoltageValue;
    (*doc)[name]["ph"] = phValue;

    return true;
}

void PhSens::setDocument(const char *objName) {
    name = objName;
}

void PhSens::setDocumentValue(JsonDocument *docBase) {
    doc = docBase;
}

JsonDocument PhSens::getDocument() {
    return (*doc);
}

JsonVariant PhSens::getVariant(const char *searchName) {
    return (*doc)[searchName];
}

float PhSens::getValuePhSens() const {
    return (*doc)[name].as<float>();
}

void PhSens::setPins(uint8_t _pin) {
    sensorPin = _pin;
}
/*
 *  ph-sens.cpp
 *
 *  ph sensor c
 *  Created on: 2023. 4. 3
 */

#include "ph-sens.h"
#include "Arduino.h"

PhSens::PhSens(uint8_t sensorPin, float voltage, float resolution, float *calibrationValue, uint8_t bufferSize)
        : doc(nullptr),
          name(""),
          voltage(voltage),
          resolution(resolution),
          calibrationValue(calibrationValue),
          sensorPin(sensorPin),
          bufferSize(bufferSize) {
    bufferAnalog = new uint32_t[bufferSize];
}

PhSens::~PhSens() {
    delete[] bufferAnalog;
}

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
    for (int i = 0; i < bufferSize; i++) {
        bufferAnalog[i] = analogRead(sensorPin);
        delay(30);
    }

    for (int i = 0; i < bufferSize - 1; i++) {
        for (int j = i + 1; j < bufferSize; j++) {
            if (bufferAnalog[i] > bufferAnalog[j]) {
                uint32_t temporaryAnalogValue = bufferAnalog[i];
                bufferAnalog[i] = bufferAnalog[j];
                bufferAnalog[j] = temporaryAnalogValue;
            }
        }
    }

    uint32_t averageAnalogValue = 0;
    int startIdx = bufferSize / 4;
    int endIdx = bufferSize - startIdx;

    for (int i = startIdx; i < endIdx; i++) {
        averageAnalogValue += bufferAnalog[i];
    }

    double phVoltageValue = (float) averageAnalogValue * voltage / resolution / (float)(endIdx - startIdx);
    double phValue = -5.70f * phVoltageValue + *calibrationValue;

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
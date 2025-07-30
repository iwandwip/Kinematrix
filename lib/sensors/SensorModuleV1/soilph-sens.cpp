/*
 *  soilph-sens.cpp
 *
 *  soilph sensor c
 *  Created on: 2023. 4. 3
 */

#include "soilph-sens.h"
#include "Arduino.h"

SoilPHSens::SoilPHSens(uint8_t _pin, uint8_t _relayPin)
        : pHState(0),
          sensorPin(_pin),
          sensorTimer(0),
          relayPin(_relayPin) {
}

SoilPHSens::~SoilPHSens() = default;

bool SoilPHSens::init() {
    pinMode(sensorPin, INPUT);
    pinMode(relayPin, OUTPUT);
    if (strcmp(name, "") == 0 && doc == nullptr) {
        name = "SoilPHSens";
        doc = new JsonDocument;
    }
    (*doc)[name]["raw"] = 0;
    (*doc)[name]["val"] = 0;
    (*doc)[name]["volt"] = 0.0;
    return true;
}

bool SoilPHSens::update() {
    if (millis() - sensorTimer >= 1000) {
        float value = analogRead(sensorPin);
        float soilPh = (-0.0139 * value) + 7.7851;
        if (value > 0 && value <= 500) {
            (*doc)[name]["val"] = soilPh;
        }
        (*doc)[name]["raw"] = value;
        (*doc)[name]["volt"] = (value * 5.0) / 1023.0;
        sensorTimer = millis();
        return true;
    }

//    if (millis() - pHTimer >= 10100) {
//        float value = analogRead(sensorPin);
//        digitalWrite(relayPin, HIGH);
//        double soilPh = (-0.0693 * value) + 7.3855; // y = -0.0693x + 7.3855
//        soilPh = constrain(soilPh, 0.0f, 14.0f);
//        (*doc)[name]["raw"] = value;
//        (*doc)[name]["val"] = soilPh;
//        (*doc)[name]["volt"] = (value * 5.0) / 1023.0;
//        pHTimer = millis();
//    } else if (millis() - pHTimer >= 10000) {
//        digitalWrite(relayPin, LOW);
//    }

//    if (pHState == 0) {
//        pHTimer = millis();
//        digitalWrite(relayPin, LOW);
//        pHState = 1;
//    }
//    if (millis() - pHTimer >= 10000 && pHState == 1) {
//        pHTimer = millis();
//        pHState = 2;
//
//        float value = analogRead(sensorPin);
//        if (value <= 120) {
//            double soilPh = (-0.0693 * value) + 7.3855; // y = -0.0693x + 7.3855
//            soilPh = constrain(soilPh, 0.0f, 14.0f);
//            (*doc)[name]["raw"] = value;
//            (*doc)[name]["val"] = soilPh;
//            (*doc)[name]["volt"] = (value * 5.0) / 1023.0;
//        }
//        digitalWrite(relayPin, HIGH);
//        return true;
//    }
//    if (millis() - pHTimer >= 5000 && pHState == 2) {
//        pHState = 0;
//    }
    return false;
}

void SoilPHSens::setDocument(const char *objName) {
    name = objName;
}

void SoilPHSens::setDocumentValue(JsonDocument *docBase) {
    doc = docBase;
}

JsonDocument SoilPHSens::getDocument() {
    return (*doc);
}

JsonVariant SoilPHSens::getVariant(const char *searchName) {
    return (*doc)[searchName];
}

float SoilPHSens::getValueSoilPHSens() const {
    return (*doc)[name].as<float>();
}

void SoilPHSens::setPins(uint8_t _pin) {
    sensorPin = _pin;
}
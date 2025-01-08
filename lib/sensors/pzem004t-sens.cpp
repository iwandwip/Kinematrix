/*
 *  pzem004t-sens.cpp
 *
 *  pzem004t sensor c
 *  Created on: 2023. 4. 3
 */

#include "pzem004t-sens.h"
#include "Arduino.h"

PZEM004TSens::~PZEM004TSens() = default;

bool PZEM004TSens::init() {
    if (strcmp(name, "") == 0 && doc == nullptr) {
        name = "PZEM004TSens";
        doc = new JsonDocument;
    }
    if (configuration & VOLTAGE) (*doc)[name]["voltage"] = 0;
    if (configuration & CURRENT) (*doc)[name]["current"] = 0;
    if (configuration & POWER) (*doc)[name]["power"] = 0;
    if (configuration & ENERGY) (*doc)[name]["energy"] = 0;
    if (configuration & FREQUENCY) (*doc)[name]["frequency"] = 0;
    if (configuration & PF) (*doc)[name]["pf"] = 0;
    return true;
}

bool PZEM004TSens::update() {
    if (millis() - sensorTimer >= 500) {
        if (configuration & VOLTAGE) (*doc)[name]["voltage"] = PZEM004Tv30::voltage();
        if (configuration & CURRENT) (*doc)[name]["current"] = PZEM004Tv30::current();
        if (configuration & POWER) (*doc)[name]["power"] = PZEM004Tv30::power();
        if (configuration & ENERGY) (*doc)[name]["energy"] = PZEM004Tv30::energy();
        if (configuration & FREQUENCY) (*doc)[name]["frequency"] = PZEM004Tv30::frequency();
        if (configuration & PF) (*doc)[name]["pf"] = PZEM004Tv30::pf();
        sensorTimer = millis();
    }
    return true;
}

void PZEM004TSens::setConfiguration(uint8_t config) {
    configuration = config;
}

void PZEM004TSens::setDocument(const char *objName) {
    name = objName;
}

void PZEM004TSens::setDocumentValue(JsonDocument *docBase) {
    doc = docBase;
}

JsonDocument PZEM004TSens::getDocument() {
    return (*doc);
}

JsonVariant PZEM004TSens::getVariant(const char *searchName) {
    return (*doc)[searchName];
}
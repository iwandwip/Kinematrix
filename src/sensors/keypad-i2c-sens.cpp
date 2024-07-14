/*
 *  keypad-i2c-sens.cpp
 *
 *  keypad-i2c sensor c
 *  Created on: 2023. 4. 3
 */
#ifdef USE_KEYPAD_I2C_SENS
#include "keypad-i2c-sens.h"
#include "Arduino.h"

KeypadSens::~KeypadSens() = default;

bool KeypadSens::init() {
    if (strcmp(name, "") == 0 && doc == nullptr) {
        name = "KeypadSens";
        doc = new JsonDocument;
    }
    (*doc)[name]["key"] = "";
    (*doc)[name]["before"] = "";
    return true;
}

bool KeypadSens::update() {
    char getKey = Keypad::getKey();
    (*doc)[name]["key"] = String(getKey);
    if (getKey != NO_KEY) {
        (*doc)[name]["before"] = String(getKey);
        return true;
    }
    return true;
}

void KeypadSens::setDocument(const char *objName) {
    name = objName;
}

void KeypadSens::setDocumentValue(JsonDocument *docBase) {
    doc = docBase;
}

JsonDocument KeypadSens::getDocument() {
    return (*doc);
}

JsonVariant KeypadSens::getVariant(const char *searchName) {
    return (*doc)[searchName];
}

float KeypadSens::getValueKeypadSens() const {
    return (*doc)[name].as<float>();
}

void KeypadSens::setPins(uint8_t _pin) {
    sensorPin = _pin;
}

////////////////////////////////////////////////////////////////

KeypadI2CSens::~KeypadI2CSens() = default;

bool KeypadI2CSens::init() {
    if (strcmp(name, "") == 0 && doc == nullptr) {
        name = "KeypadI2CSens";
        doc = new JsonDocument;
    }
    Keypad_I2C::begin();
    (*doc)[name]["key"] = "";
    (*doc)[name]["before"] = "";
    return true;
}

bool KeypadI2CSens::update() {
    char getKey = Keypad_I2C::getKey();
    (*doc)[name]["key"] = String(getKey);
    if (getKey != NO_KEY) {
        (*doc)[name]["before"] = String(getKey);
        return true;
    }
    return true;
}

void KeypadI2CSens::setDocument(const char *objName) {
    name = objName;
}

void KeypadI2CSens::setDocumentValue(JsonDocument *docBase) {
    doc = docBase;
}

JsonDocument KeypadI2CSens::getDocument() {
    return (*doc);
}

JsonVariant KeypadI2CSens::getVariant(const char *searchName) {
    return (*doc)[searchName];
}

float KeypadI2CSens::getValueKeypadI2CSens() const {
    return (*doc)[name].as<float>();
}

void KeypadI2CSens::setPins(uint8_t _pin) {
    sensorPin = _pin;
}
#endif
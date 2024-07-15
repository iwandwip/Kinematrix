/*  https://medium.com/@asyasyarif/keypad-dengan-i2c-interface-745e7d58a2a7
 *  https://github.com/joeyoung/arduino_keypads/tree/master
 *  keypad-i2c-sens.h
 *
 *  keypad-i2c sensor lib
 *  Created on: 2023. 4. 3
 */
#ifdef USE_KEYPAD_I2C_SENS
#pragma once

#ifndef KEYPAD_I2C_SENS
#define KEYPAD_I2C_SENS

#include "Arduino.h"
#include "base/sensor-module.h"
#include "lib/Keypad_I2c/Keypad_I2C.h"
#include "lib/Keypad/Keypad.h"

class KeypadSens : public BaseSens, public Keypad {
private:
    JsonDocument *doc;
    const char *name;

    String key;

    uint8_t sensorPin;
    uint32_t sensorTimer;

    using Keypad::Keypad;
public:
    virtual ~KeypadSens();
    bool init() override;
    bool update() override;

    void setDocument(const char *objName) override;
    void setDocumentValue(JsonDocument *docBase) override;
    JsonDocument getDocument() override;
    JsonVariant getVariant(const char *searchName) override;

    float getValueKeypadSens() const;
    void setPins(uint8_t _pin);
};

////////////////////////////////////////////////////////////////

class KeypadI2CSens : public BaseSens, public Keypad_I2C {
private:
    JsonDocument *doc;
    const char *name;

    String key;

    uint8_t sensorPin;
    uint32_t sensorTimer;

    using Keypad_I2C::Keypad_I2C;
public:
    virtual ~KeypadI2CSens();
    bool init() override;
    bool update() override;

    void setDocument(const char *objName) override;
    void setDocumentValue(JsonDocument *docBase) override;
    JsonDocument getDocument() override;
    JsonVariant getVariant(const char *searchName) override;

    float getValueKeypadI2CSens() const;
    void setPins(uint8_t _pin);
};

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
#endif  // KEYPAD_I2C_SENS
#endif
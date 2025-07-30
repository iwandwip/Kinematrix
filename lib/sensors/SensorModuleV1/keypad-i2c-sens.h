/*  https://medium.com/@asyasyarif/keypad-dengan-i2c-interface-745e7d58a2a7
 *  https://github.com/joeyoung/arduino_keypads/tree/master
 *  keypad-i2c-sens.h
 *
 *  keypad-i2c sensor lib
 *  Created on: 2023. 4. 3
 */

#pragma once

#ifndef KEYPAD_I2C_SENS
#define KEYPAD_I2C_SENS

#pragma message("[COMPILED]: keypad-i2c-sens.h")

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

#endif  // KEYPAD_I2C_SENS
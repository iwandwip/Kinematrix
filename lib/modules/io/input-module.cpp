/*  
 *  input-module.cpp
 *
 *  input module c
 *  Created on: 2023. 4. 3
 */

#include "input-module.h"

DigitalIn::DigitalIn(int pin)
        : DigitalIn(pin, INPUT_PULLUP) {
}

DigitalIn::DigitalIn(int pin, int mode) {
    btnPin = pin;
    debounceTime = 0;
    count = 0;
    countMode = COUNT_FALLING;
    lastDebounceTime = 0;

    pinMode(btnPin, mode);

    previousSteadyState = getStateRaw();
    lastSteadyState = previousSteadyState;
    lastFlickerableState = previousSteadyState;
}

void DigitalIn::setDebounceTime(unsigned long time) {
    debounceTime = time;
}

int DigitalIn::getState() const {
    return lastSteadyState;
}

int DigitalIn::getStateRaw() const {
    return digitalRead(btnPin);
}

bool DigitalIn::isPressed(unsigned long time) {
    if (millis() - debounceTime >= time) {
        debounceTime = millis();
        return true;
    }
    return false;
}

bool DigitalIn::isPressed() const {
    if (previousSteadyState == HIGH && lastSteadyState == LOW)
        return true;
    else
        return false;
}

bool DigitalIn::isLongPressed(unsigned long time) const {
    if (lastSteadyState == LOW && (millis() - lastDebounceTime) >= time)
        return true;
    else
        return false;
}

bool DigitalIn::isReleased() const {
    if (previousSteadyState == LOW && lastSteadyState == HIGH)
        return true;
    else
        return false;
}

void DigitalIn::setCountMode(int mode) {
    countMode = mode;
}

unsigned long DigitalIn::getCount() const {
    return count;
}

void DigitalIn::resetCount() {
    count = 0;
}

void DigitalIn::resetState() {
    previousSteadyState = HIGH;
    lastSteadyState = HIGH;
    lastDebounceTime = millis();
}

void DigitalIn::update() {
    int currentState = getStateRaw();
    unsigned long currentTime = millis();

    if (currentState != lastFlickerableState) {
        lastDebounceTime = currentTime;
        lastFlickerableState = currentState;
    }

    if ((currentTime - lastDebounceTime) >= debounceTime) {
        previousSteadyState = lastSteadyState;
        lastSteadyState = currentState;
    }

    if (previousSteadyState != lastSteadyState) {
        if (countMode == COUNT_BOTH)
            count++;
        else if (countMode == COUNT_FALLING) {
            if (previousSteadyState == HIGH && lastSteadyState == LOW)
                count++;
        } else if (countMode == COUNT_RISING) {
            if (previousSteadyState == LOW && lastSteadyState == HIGH)
                count++;
        }
    }
}

void DigitalIn::updateAll(DigitalIn *ptr, ...) {
    va_list args;
    va_start(args, ptr);
    DigitalIn *currentIndex = ptr;
    while (currentIndex != nullptr) {
        currentIndex->update();
        currentIndex = va_arg(args, DigitalIn *);
    }
    va_end(args);
}

DigitalIn *DigitalIn::stop() {
    return nullptr;
}

////////////////////////////////////////////////////////////////////////

DigitalInBase::DigitalInBase(int pin)
        : DigitalInBase(pin, INPUT_PULLUP) {
}

DigitalInBase::DigitalInBase(int pin, int mode) {
    btnPin = pin;
    debounceTime = 0;
    count = 0;
    countMode = COUNT_FALLING;
    lastDebounceTime = 0;

    previousSteadyState = getStateRaw();
    lastSteadyState = previousSteadyState;
    lastFlickerableState = previousSteadyState;
}

void DigitalInBase::setDebounceTime(unsigned long time) {
    debounceTime = time;
}

int DigitalInBase::getState() const {
    return lastSteadyState;
}

int DigitalInBase::getStateRaw() const {
    return 0;
}

bool DigitalInBase::isPressed(unsigned long time) {
    if (millis() - debounceTime >= time) {
        debounceTime = millis();
        return true;
    }
    return false;
}

bool DigitalInBase::isPressed() const {
    if (previousSteadyState == HIGH && lastSteadyState == LOW)
        return true;
    else
        return false;
}

bool DigitalInBase::isLongPressed(unsigned long time) const {
    if (lastSteadyState == LOW && (millis() - lastDebounceTime) >= time)
        return true;
    else
        return false;
}

bool DigitalInBase::isReleased() const {
    if (previousSteadyState == LOW && lastSteadyState == HIGH)
        return true;
    else
        return false;
}

void DigitalInBase::setCountMode(int mode) {
    countMode = mode;
}

unsigned long DigitalInBase::getCount() const {
    return count;
}

void DigitalInBase::resetCount() {
    count = 0;
}

void DigitalInBase::resetState() {
    previousSteadyState = HIGH;
    lastSteadyState = HIGH;
}

void DigitalInBase::update() {
    int currentState = getStateRaw();
    unsigned long currentTime = millis();

    if (currentState != lastFlickerableState) {
        lastDebounceTime = currentTime;
        lastFlickerableState = currentState;
    }

    if ((currentTime - lastDebounceTime) >= debounceTime) {
        previousSteadyState = lastSteadyState;
        lastSteadyState = currentState;
    }

    if (previousSteadyState != lastSteadyState) {
        if (countMode == COUNT_BOTH)
            count++;
        else if (countMode == COUNT_FALLING) {
            if (previousSteadyState == HIGH && lastSteadyState == LOW)
                count++;
        } else if (countMode == COUNT_RISING) {
            if (previousSteadyState == LOW && lastSteadyState == HIGH)
                count++;
        }
    }
}

void DigitalInBase::updateAll(DigitalInBase *ptr, ...) {
    va_list args;
    va_start(args, ptr);
    DigitalInBase *currentIndex = ptr;
    while (currentIndex != nullptr) {
        currentIndex->update();
        currentIndex = va_arg(args, DigitalInBase *);
    }
    va_end(args);
}

DigitalInBase *DigitalInBase::stop() {
    return nullptr;
}

////////////////////////////////////////////////////////////////////////

DigitalInI2C::DigitalInI2C(int pin)
        : DigitalInBase(pin) {

}

DigitalInI2C::DigitalInI2C(int pin, int mode)
        : DigitalInBase(pin, mode) {

}

void DigitalInI2C::init(PCF8574 *_pcf8574) {
    pcf8574 = _pcf8574;
    previousSteadyState = getStateRaw();
    lastSteadyState = previousSteadyState;
    lastFlickerableState = previousSteadyState;
}

int DigitalInI2C::getStateRaw() const {
    return pcf8574->digitalRead(btnPin);
}
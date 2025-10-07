#include "pcf8574-input-module.h"

PCF8574DigitalIn::PCF8574DigitalIn(uint8_t pin) {
    btnPin = pin;
    pcfModule = nullptr;
    debounceTime = 50;
    count = 0;
    countMode = COUNT_FALLING;
    lastDebounceTime = 0;

    previousSteadyState = HIGH;
    lastSteadyState = HIGH;
    lastFlickerableState = HIGH;
}

PCF8574DigitalIn::PCF8574DigitalIn(uint8_t pin, PCF8574Module* module) {
    btnPin = pin;
    pcfModule = module;
    debounceTime = 50;
    count = 0;
    countMode = COUNT_FALLING;
    lastDebounceTime = 0;

    pcfModule->setupButton(btnPin);

    previousSteadyState = getStateRaw();
    lastSteadyState = previousSteadyState;
    lastFlickerableState = previousSteadyState;
}

void PCF8574DigitalIn::init(PCF8574Module* module) {
    pcfModule = module;
    pcfModule->setupButton(btnPin);

    previousSteadyState = getStateRaw();
    lastSteadyState = previousSteadyState;
    lastFlickerableState = previousSteadyState;
}

void PCF8574DigitalIn::setDebounceTime(unsigned long time) {
    debounceTime = time;
}

int PCF8574DigitalIn::getState() const {
    return lastSteadyState;
}

int PCF8574DigitalIn::getStateRaw() const {
    if (pcfModule == nullptr) return HIGH;
    return pcfModule->digitalRead(btnPin);
}

bool PCF8574DigitalIn::isPressed(unsigned long time) {
    if (millis() - lastDebounceTime >= time) {
        lastDebounceTime = millis();
        return true;
    }
    return false;
}

bool PCF8574DigitalIn::isPressed() const {
    if (previousSteadyState == HIGH && lastSteadyState == LOW)
        return true;
    else
        return false;
}

bool PCF8574DigitalIn::isLongPressed(unsigned long time) const {
    if (lastSteadyState == LOW && (millis() - lastDebounceTime) >= time)
        return true;
    else
        return false;
}

bool PCF8574DigitalIn::isReleased() const {
    if (previousSteadyState == LOW && lastSteadyState == HIGH)
        return true;
    else
        return false;
}

void PCF8574DigitalIn::setCountMode(int mode) {
    countMode = mode;
}

unsigned long PCF8574DigitalIn::getCount() const {
    return count;
}

void PCF8574DigitalIn::resetCount() {
    count = 0;
}

void PCF8574DigitalIn::resetState() {
    previousSteadyState = HIGH;
    lastSteadyState = HIGH;
}

void PCF8574DigitalIn::update() {
    if (pcfModule == nullptr) return;

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

void PCF8574DigitalIn::updateAll(PCF8574DigitalIn* ptr, ...) {
    va_list args;
    va_start(args, ptr);
    PCF8574DigitalIn* currentIndex = ptr;
    while (currentIndex != nullptr) {
        currentIndex->update();
        currentIndex = va_arg(args, PCF8574DigitalIn*);
    }
    va_end(args);
}

PCF8574DigitalIn* PCF8574DigitalIn::stop() {
    return nullptr;
}
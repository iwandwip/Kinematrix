#include "pcf8574-output-module.h"

PCF8574DigitalOut::PCF8574DigitalOut() {
    _pin = 0;
    pcfModule = nullptr;
    _state = false;
    _onDelayTime = 0;
    _onDelayStart = 0;
    _onDelayState = false;
    _offDelayTime = 0;
    _offDelayStart = 0;
    _offDelayState = false;
    _reverse = false;
    _toggleTime = 0;
}

PCF8574DigitalOut::PCF8574DigitalOut(uint8_t pin, PCF8574Module *module, bool isReverse) {
    _pin = pin;
    pcfModule = module;
    _reverse = isReverse;
    _state = false;
    _onDelayTime = 0;
    _onDelayStart = 0;
    _onDelayState = false;
    _offDelayTime = 0;
    _offDelayStart = 0;
    _offDelayState = false;
    _toggleTime = 0;

    init(module);
}

void PCF8574DigitalOut::init(PCF8574Module *module) {
    pcfModule = module;
    pcfModule->setupPin(_pin, PCF_OUTPUT);
    off();
}

void PCF8574DigitalOut::on() {
    if (pcfModule == nullptr) return;

    if (_reverse)
        pcfModule->digitalWrite(_pin, LOW);
    else
        pcfModule->digitalWrite(_pin, HIGH);
    _state = true;
}

void PCF8574DigitalOut::off() {
    if (pcfModule == nullptr) return;

    if (_reverse)
        pcfModule->digitalWrite(_pin, HIGH);
    else
        pcfModule->digitalWrite(_pin, LOW);
    _state = false;
}

void PCF8574DigitalOut::onToOffDelay(unsigned long delay_time) {
    on();
    delay(delay_time);
    off();
}

void PCF8574DigitalOut::offToOnDelay(unsigned long delay_time) {
    off();
    delay(delay_time);
    on();
}

void PCF8574DigitalOut::set(bool state) {
    if (state) {
        on();
    } else {
        off();
    }
}

void PCF8574DigitalOut::toggle(void (*toggleCallback)()) {
    if (toggleCallback != nullptr) toggleCallback();
    set(!_state);
}

void PCF8574DigitalOut::toggleDelay(unsigned long delay_time, void (*toggleCallback)()) {
    if (toggleCallback != nullptr) toggleCallback();
    set(!_state);
    delay(delay_time);
}

void PCF8574DigitalOut::toggleInit(unsigned long delay_time, int num, void (*toggleCallback)()) {
    for (int i = 0; i < num; i++) {
        if (toggleCallback != nullptr) toggleCallback();
        on();
        delay(delay_time);
        off();
        delay(delay_time);
    }
}

void PCF8574DigitalOut::toggleAsync(unsigned long delay_time, void (*toggleCallback)(bool state)) {
    if (millis() - _toggleTime >= delay_time) {
        set(!_state);
        if (toggleCallback != nullptr) toggleCallback(_state);
        _toggleTime = millis();
    }
}

void PCF8574DigitalOut::onDelay(unsigned long delay_time) {
    _onDelayTime = delay_time;
    _onDelayStart = millis();
    _onDelayState = true;
}

void PCF8574DigitalOut::offDelay(unsigned long delay_time) {
    _offDelayTime = delay_time;
    _offDelayStart = millis();
    _offDelayState = true;
}

void PCF8574DigitalOut::update() {
    if (_onDelayState) {
        unsigned long currentMillisOn = millis();
        if (currentMillisOn - _onDelayStart >= _onDelayTime) {
            on();
            _onDelayState = false;
        }
    }
    if (_offDelayState) {
        unsigned long currentMillisOff = millis();
        if (currentMillisOff - _offDelayStart >= _offDelayTime) {
            off();
            _offDelayState = false;
        }
    }
}

void PCF8574DigitalOut::reverse() {
    _reverse = true;
}

void PCF8574DigitalOut::setPin(uint8_t pin, bool isReverse) {
    _pin = pin;
    _reverse = isReverse;
    if (pcfModule != nullptr) {
        pcfModule->setupPin(_pin, PCF_OUTPUT);
        off();
    }
}

bool PCF8574DigitalOut::getState() const {
    return _state;
}

void PCF8574DigitalOut::updateAll(PCF8574DigitalOut *ptr, ...) {
    va_list args;
    va_start(args, ptr);
    PCF8574DigitalOut *currentIndex = ptr;
    while (currentIndex != nullptr) {
        currentIndex->update();
        currentIndex = va_arg(args, PCF8574DigitalOut*);
    }
    va_end(args);
}

PCF8574DigitalOut *PCF8574DigitalOut::stop() {
    return nullptr;
}
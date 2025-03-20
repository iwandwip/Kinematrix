#include "pcf8574-module.h"

PCF8574Module::PCF8574Module(uint8_t address) : pcf(address) {
    useInterrupt = false;
    ledStatus = 0;
}

PCF8574Module::PCF8574Module(uint8_t address, uint8_t interruptPin, void (*interruptFunction)())
        : pcf(address, interruptPin, interruptFunction) {
    useInterrupt = true;
    ledStatus = 0;
}

PCF8574Module::PCF8574Module(uint8_t address, int sda, int scl)
#if !defined(__AVR) && !defined(ARDUINO_ARCH_SAMD) && !defined(TEENSYDUINO) && !defined(ARDUINO_ARCH_RENESAS)
        : pcf(address, sda, scl)
#else
: pcf(address)
#endif
{
    useInterrupt = false;
    ledStatus = 0;
}

PCF8574Module::PCF8574Module(uint8_t address, int sda, int scl, uint8_t interruptPin, void (*interruptFunction)())
#if !defined(__AVR) && !defined(ARDUINO_ARCH_SAMD) && !defined(TEENSYDUINO) && !defined(ARDUINO_ARCH_RENESAS)
        : pcf(address, sda, scl, interruptPin, interruptFunction)
#else
: pcf(address, interruptPin, interruptFunction)
#endif
{
    useInterrupt = true;
    ledStatus = 0;
}

#if defined(ESP32) || defined(ARDUINO_ARCH_SAMD) || defined(ARDUINO_ARCH_RP2040) || defined(ARDUINO_ARCH_STM32) || defined(ARDUINO_ARCH_RENESAS)

PCF8574Module::PCF8574Module(TwoWire *wire, uint8_t address)
        : pcf(wire, address) {
    useInterrupt = false;
    ledStatus = 0;
}

PCF8574Module::PCF8574Module(TwoWire *wire, uint8_t address, uint8_t interruptPin, void (*interruptFunction)())
        : pcf(wire, address, interruptPin, interruptFunction) {
    useInterrupt = true;
    ledStatus = 0;
}

#endif

#if defined(ESP32)

PCF8574Module::PCF8574Module(TwoWire *wire, uint8_t address, int sda, int scl)
        : pcf(wire, address, sda, scl) {
    useInterrupt = false;
    ledStatus = 0;
}

PCF8574Module::PCF8574Module(TwoWire *wire, uint8_t address, int sda, int scl, uint8_t interruptPin, void (*interruptFunction)())
        : pcf(wire, address, sda, scl, interruptPin, interruptFunction) {
    useInterrupt = true;
    ledStatus = 0;
}

#endif

bool PCF8574Module::begin() {
    return pcf.begin();
}

void PCF8574Module::setupPin(uint8_t pin, uint8_t mode, uint8_t initialState) {
    switch (mode) {
        case PCF_INPUT:
            pcf.pinMode(pin, INPUT);
            break;
        case PCF_INPUT_PULLUP:
            pcf.pinMode(pin, INPUT_PULLUP);
            break;
        case PCF_OUTPUT:
            pcf.pinMode(pin, OUTPUT, initialState);
            if (initialState == HIGH) {
                ledStatus |= (1 << pin);
            } else {
                ledStatus &= ~(1 << pin);
            }
            break;
    }
}

uint8_t PCF8574Module::digitalRead(uint8_t pin) {
    return pcf.digitalRead(pin);
}

bool PCF8574Module::digitalWrite(uint8_t pin, uint8_t value) {
    if (value == HIGH) {
        ledStatus |= (1 << pin);
    } else {
        ledStatus &= ~(1 << pin);
    }
    return pcf.digitalWrite(pin, value);
}

byte PCF8574Module::digitalReadAll() {
#ifdef PCF8574_LOW_MEMORY
    return pcf.digitalReadAll();
#else
    PCF8574::DigitalInput input = pcf.digitalReadAll();
    byte result = 0;
    if (input.p0) result |= (1 << 0);
    if (input.p1) result |= (1 << 1);
    if (input.p2) result |= (1 << 2);
    if (input.p3) result |= (1 << 3);
    if (input.p4) result |= (1 << 4);
    if (input.p5) result |= (1 << 5);
    if (input.p6) result |= (1 << 6);
    if (input.p7) result |= (1 << 7);
    return result;
#endif
}

bool PCF8574Module::digitalWriteAll(byte values) {
    ledStatus = values;

#ifdef PCF8574_LOW_MEMORY
    return pcf.digitalWriteAll(values);
#else
    PCF8574::DigitalInput input;
    input.p0 = (values & (1 << 0)) ? HIGH : LOW;
    input.p1 = (values & (1 << 1)) ? HIGH : LOW;
    input.p2 = (values & (1 << 2)) ? HIGH : LOW;
    input.p3 = (values & (1 << 3)) ? HIGH : LOW;
    input.p4 = (values & (1 << 4)) ? HIGH : LOW;
    input.p5 = (values & (1 << 5)) ? HIGH : LOW;
    input.p6 = (values & (1 << 6)) ? HIGH : LOW;
    input.p7 = (values & (1 << 7)) ? HIGH : LOW;
    return pcf.digitalWriteAll(input);
#endif
}

void PCF8574Module::setupButton(uint8_t pin) {
    pcf.pinMode(pin, INPUT_PULLUP);
}

bool PCF8574Module::readButton(uint8_t pin) {
    return pcf.digitalRead(pin) == LOW;
}

void PCF8574Module::setupLED(uint8_t pin, uint8_t initialState) {
    pcf.pinMode(pin, OUTPUT, initialState);
    if (initialState == HIGH) {
        ledStatus |= (1 << pin);
    } else {
        ledStatus &= ~(1 << pin);
    }
}

bool PCF8574Module::turnOnLED(uint8_t pin) {
    ledStatus |= (1 << pin);
    return pcf.digitalWrite(pin, HIGH);
}

bool PCF8574Module::turnOffLED(uint8_t pin) {
    ledStatus &= ~(1 << pin);
    return pcf.digitalWrite(pin, LOW);
}

bool PCF8574Module::toggleLED(uint8_t pin) {
    if ((ledStatus & (1 << pin)) != 0) {
        return turnOffLED(pin);
    } else {
        return turnOnLED(pin);
    }
}

void PCF8574Module::setupEncoder(uint8_t pinA, uint8_t pinB) {
    pcf.pinMode(pinA, INPUT_PULLUP);
    pcf.pinMode(pinB, INPUT_PULLUP);
    pcf.encoder(pinA, pinB);
}

bool PCF8574Module::readEncoder(uint8_t pinA, uint8_t pinB, volatile long *encoderValue, bool reverseRotation) {
#ifdef BASIC_ENCODER_ALGORITHM
    return pcf.readEncoderValue(pinA, pinB, encoderValue, reverseRotation);
#elif defined(MISCHIANTI_ENCODER_ALGORITHM)
    return pcf.readEncoderValueMischianti(pinA, pinB, encoderValue, reverseRotation);
#elif defined(SEQUENCE_ENCODER_ALGORITHM)
    return pcf.readEncoderValueSequence(pinA, pinB, encoderValue, reverseRotation);
#elif defined(SEQUENCE_ENCODER_ALGORITHM_REDUCED)
    return pcf.readEncoderValueSequenceReduced(pinA, pinB, encoderValue, reverseRotation);
#elif defined(POKI_ENCODER_ALGORITHM)
    return pcf.readEncoderValuePoki(pinA, pinB, encoderValue, reverseRotation);
#else
    return pcf.readEncoderValue(pinA, pinB, encoderValue, reverseRotation);
#endif
}
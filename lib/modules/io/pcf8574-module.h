#ifndef KINEMATRIX_PCF8574_MODULE_H
#define KINEMATRIX_PCF8574_MODULE_H

#include "Arduino.h"
#include "PCF8574.h"

#define PCF_INPUT 0
#define PCF_INPUT_PULLUP 1
#define PCF_OUTPUT 2

#define PCF_PIN0 P0
#define PCF_PIN1 P1
#define PCF_PIN2 P2
#define PCF_PIN3 P3
#define PCF_PIN4 P4
#define PCF_PIN5 P5
#define PCF_PIN6 P6
#define PCF_PIN7 P7

class PCF8574Module {
public:
    PCF8574Module(uint8_t address = 0x20);
    PCF8574Module(uint8_t address, uint8_t interruptPin, void (*interruptFunction)());
    PCF8574Module(uint8_t address, int sda, int scl);
    PCF8574Module(uint8_t address, int sda, int scl, uint8_t interruptPin, void (*interruptFunction)());

#if defined(ESP32) || defined(ARDUINO_ARCH_SAMD) || defined(ARDUINO_ARCH_RP2040) || defined(ARDUINO_ARCH_STM32) || defined(ARDUINO_ARCH_RENESAS)
    PCF8574Module(TwoWire *wire, uint8_t address = 0x20);
    PCF8574Module(TwoWire *wire, uint8_t address, uint8_t interruptPin, void (*interruptFunction)());
#endif

#if defined(ESP32)
    PCF8574Module(TwoWire *wire, uint8_t address, int sda, int scl);
    PCF8574Module(TwoWire *wire, uint8_t address, int sda, int scl, uint8_t interruptPin, void (*interruptFunction)());
#endif

    bool begin();
    void setupPin(uint8_t pin, uint8_t mode, uint8_t initialState = LOW);
    uint8_t digitalRead(uint8_t pin);
    bool digitalWrite(uint8_t pin, uint8_t value);
    byte digitalReadAll();
    bool digitalWriteAll(byte values);
    void setupButton(uint8_t pin);
    bool readButton(uint8_t pin);
    void setupLED(uint8_t pin, uint8_t initialState = LOW);
    bool turnOnLED(uint8_t pin);
    bool turnOffLED(uint8_t pin);
    bool toggleLED(uint8_t pin);
    void setupEncoder(uint8_t pinA, uint8_t pinB);
    bool readEncoder(uint8_t pinA, uint8_t pinB, volatile long *encoderValue, bool reverseRotation = false);

    PCF8574 *getPCF() { return &pcf; }

private:
    PCF8574 pcf;
    bool useInterrupt;
    byte ledStatus;
};

#endif
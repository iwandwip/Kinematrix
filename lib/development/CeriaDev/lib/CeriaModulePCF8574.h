#ifndef CERIA_MODULE_PCF8574_H
#define CERIA_MODULE_PCF8574_H

#include "Arduino.h"
#include "PCF8574.h"

namespace CeriaDevOP {

    enum PCF8574InterruptMode {
        PCF8574_NO_INTERRUPT = 0,
        PCF8574_WITH_INTERRUPT = 1
    };

    enum PCF8574PinMode {
        PCF8574_PIN_INPUT = INPUT,
        PCF8574_PIN_INPUT_PULLUP = INPUT_PULLUP,
        PCF8574_PIN_OUTPUT = OUTPUT
    };

    enum PCF8574EncoderAlgorithm {
        PCF8574_ENCODER_BASIC = 0,
        PCF8574_ENCODER_MISCHIANTI = 1,
        PCF8574_ENCODER_SEQUENCE = 2,
        PCF8574_ENCODER_SEQUENCE_REDUCED = 3,
        PCF8574_ENCODER_POKI = 4
    };

    class CeriaModulePCF8574 : public PCF8574 {
    public:
        CeriaModulePCF8574(uint8_t address = 0x38);
        CeriaModulePCF8574(uint8_t address, uint8_t interruptPin, void (*interruptFunction)());

#if !defined(__AVR) && !defined(ARDUINO_ARCH_SAMD) && !defined(TEENSYDUINO)
        CeriaModulePCF8574(uint8_t address, int sda, int scl);
        CeriaModulePCF8574(uint8_t address, int sda, int scl, uint8_t interruptPin, void (*interruptFunction)());
#endif

#if defined(ESP32) || defined(ARDUINO_ARCH_SAMD) || defined(ARDUINO_ARCH_RP2040) || defined(ARDUINO_ARCH_STM32)
        CeriaModulePCF8574(TwoWire *pWire, uint8_t address);
        CeriaModulePCF8574(TwoWire *pWire, uint8_t address, uint8_t interruptPin, void (*interruptFunction)());
#endif
#if defined(ESP32)
        CeriaModulePCF8574(TwoWire *pWire, uint8_t address, int sda, int scl);
        CeriaModulePCF8574(TwoWire *pWire, uint8_t address, int sda, int scl, uint8_t interruptPin, void (*interruptFunction)());
#endif

        bool begin();
        bool begin(uint8_t address);
        void update();

        bool hasNewData();
        bool isConnected(uint32_t timeoutMs = 5000);

        void setPinMode(uint8_t pin, PCF8574PinMode mode, uint8_t output_start = HIGH);
        uint8_t readPin(uint8_t pin);
        bool writePin(uint8_t pin, uint8_t value);

        void setupEncoder(uint8_t pinA, uint8_t pinB);
        bool readEncoder(uint8_t pinA, uint8_t pinB, volatile long *encoderValue, bool reverseRotation = false);
        int8_t readEncoderStep(uint8_t pinA, uint8_t pinB);

        void setInterruptMode(PCF8574InterruptMode mode);
        void enableInterrupt();
        void disableInterrupt();

        uint8_t readAllPins();
        bool writeAllPins(uint8_t pinValues);

        void setLatency(int latencyMs);
        int getLatency();

        String getPinStatusString();
        String getModuleStatusString();

        bool waitForChange(uint32_t timeoutMs = 5000);

    private:
        bool _hasNewData;
        uint32_t _lastUpdateTime;
        uint8_t _i2cAddress;
        PCF8574InterruptMode _interruptMode;
        uint8_t _lastPinStates;
        bool _encoderSetup;
        uint8_t _encoderPinA;
        uint8_t _encoderPinB;
    };

}

#endif
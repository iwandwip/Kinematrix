#include "CeriaModulePCF8574.h"

namespace CeriaDevOP {

    CeriaModulePCF8574::CeriaModulePCF8574(uint8_t address) : PCF8574(address) {
        _hasNewData = false;
        _lastUpdateTime = 0;
        _i2cAddress = address;
        _interruptMode = PCF8574_NO_INTERRUPT;
        _lastPinStates = 0;
        _encoderSetup = false;
        _encoderPinA = 0;
        _encoderPinB = 0;
    }

    CeriaModulePCF8574::CeriaModulePCF8574(uint8_t address, uint8_t interruptPin, void (*interruptFunction)())
            : PCF8574(address, interruptPin, interruptFunction) {
        _hasNewData = false;
        _lastUpdateTime = 0;
        _i2cAddress = address;
        _interruptMode = PCF8574_WITH_INTERRUPT;
        _lastPinStates = 0;
        _encoderSetup = false;
        _encoderPinA = 0;
        _encoderPinB = 0;
    }

#if !defined(__AVR) && !defined(ARDUINO_ARCH_SAMD) && !defined(TEENSYDUINO)

    CeriaModulePCF8574::CeriaModulePCF8574(uint8_t address, int sda, int scl) : PCF8574(address, sda, scl) {
        _hasNewData = false;
        _lastUpdateTime = 0;
        _i2cAddress = address;
        _interruptMode = PCF8574_NO_INTERRUPT;
        _lastPinStates = 0;
        _encoderSetup = false;
        _encoderPinA = 0;
        _encoderPinB = 0;
    }

    CeriaModulePCF8574::CeriaModulePCF8574(uint8_t address, int sda, int scl, uint8_t interruptPin, void (*interruptFunction)())
            : PCF8574(address, sda, scl, interruptPin, interruptFunction) {
        _hasNewData = false;
        _lastUpdateTime = 0;
        _i2cAddress = address;
        _interruptMode = PCF8574_WITH_INTERRUPT;
        _lastPinStates = 0;
        _encoderSetup = false;
        _encoderPinA = 0;
        _encoderPinB = 0;
    }

#endif

#if defined(ESP32) || defined(ARDUINO_ARCH_SAMD) || defined(ARDUINO_ARCH_RP2040) || defined(ARDUINO_ARCH_STM32)

    CeriaModulePCF8574::CeriaModulePCF8574(TwoWire *pWire, uint8_t address) : PCF8574(pWire, address) {
        _hasNewData = false;
        _lastUpdateTime = 0;
        _i2cAddress = address;
        _interruptMode = PCF8574_NO_INTERRUPT;
        _lastPinStates = 0;
        _encoderSetup = false;
        _encoderPinA = 0;
        _encoderPinB = 0;
    }

    CeriaModulePCF8574::CeriaModulePCF8574(TwoWire *pWire, uint8_t address, uint8_t interruptPin, void (*interruptFunction)())
            : PCF8574(pWire, address, interruptPin, interruptFunction) {
        _hasNewData = false;
        _lastUpdateTime = 0;
        _i2cAddress = address;
        _interruptMode = PCF8574_WITH_INTERRUPT;
        _lastPinStates = 0;
        _encoderSetup = false;
        _encoderPinA = 0;
        _encoderPinB = 0;
    }

#endif

#if defined(ESP32)

    CeriaModulePCF8574::CeriaModulePCF8574(TwoWire *pWire, uint8_t address, int sda, int scl)
            : PCF8574(pWire, address, sda, scl) {
        _hasNewData = false;
        _lastUpdateTime = 0;
        _i2cAddress = address;
        _interruptMode = PCF8574_NO_INTERRUPT;
        _lastPinStates = 0;
        _encoderSetup = false;
        _encoderPinA = 0;
        _encoderPinB = 0;
    }

    CeriaModulePCF8574::CeriaModulePCF8574(TwoWire *pWire, uint8_t address, int sda, int scl, uint8_t interruptPin, void (*interruptFunction)())
            : PCF8574(pWire, address, sda, scl, interruptPin, interruptFunction) {
        _hasNewData = false;
        _lastUpdateTime = 0;
        _i2cAddress = address;
        _interruptMode = PCF8574_WITH_INTERRUPT;
        _lastPinStates = 0;
        _encoderSetup = false;
        _encoderPinA = 0;
        _encoderPinB = 0;
    }

#endif

    bool CeriaModulePCF8574::begin() {
        bool success = PCF8574::begin();
        if (success) {
            _lastUpdateTime = millis();
            _lastPinStates = readAllPins();
        }
        return success;
    }

    bool CeriaModulePCF8574::begin(uint8_t address) {
        _i2cAddress = address;
        bool success = PCF8574::begin(address);
        if (success) {
            _lastUpdateTime = millis();
            _lastPinStates = readAllPins();
        }
        return success;
    }

    void CeriaModulePCF8574::update() {
        uint8_t currentStates = readAllPins();
        if (currentStates != _lastPinStates) {
            _hasNewData = true;
            _lastUpdateTime = millis();
            _lastPinStates = currentStates;
        }
    }

    bool CeriaModulePCF8574::hasNewData() {
        bool result = _hasNewData;
        _hasNewData = false;
        return result;
    }

    bool CeriaModulePCF8574::isConnected(uint32_t timeoutMs) {
        return isLastTransmissionSuccess() && (millis() - _lastUpdateTime) < timeoutMs;
    }

    void CeriaModulePCF8574::setPinMode(uint8_t pin, PCF8574PinMode mode, uint8_t output_start) {
        PCF8574::pinMode(pin, mode, output_start);
    }

    uint8_t CeriaModulePCF8574::readPin(uint8_t pin) {
        return PCF8574::digitalRead(pin);
    }

    bool CeriaModulePCF8574::writePin(uint8_t pin, uint8_t value) {
        return PCF8574::digitalWrite(pin, value);
    }

    void CeriaModulePCF8574::setupEncoder(uint8_t pinA, uint8_t pinB) {
        PCF8574::encoder(pinA, pinB);
        _encoderSetup = true;
        _encoderPinA = pinA;
        _encoderPinB = pinB;
    }

    bool CeriaModulePCF8574::readEncoder(uint8_t pinA, uint8_t pinB, volatile long *encoderValue, bool reverseRotation) {
#ifdef BASIC_ENCODER_ALGORITHM
        return PCF8574::readEncoderValue(pinA, pinB, encoderValue, reverseRotation);
#elif defined(MISCHIANTI_ENCODER_ALGORITHM)
        return PCF8574::readEncoderValueMischianti(pinA, pinB, encoderValue, reverseRotation);
#elif defined(SEQUENCE_ENCODER_ALGORITHM)
        return PCF8574::readEncoderValueSequence(pinA, pinB, encoderValue, reverseRotation);
#elif defined(SEQUENCE_ENCODER_ALGORITHM_REDUCED)
        return PCF8574::readEncoderValueSequenceReduced(pinA, pinB, encoderValue, reverseRotation);
#elif defined(POKI_ENCODER_ALGORITHM)
        return PCF8574::readEncoderValuePoki(pinA, pinB, encoderValue, reverseRotation);
#else
        return PCF8574::readEncoderValue(pinA, pinB, encoderValue, reverseRotation);
#endif
    }

    int8_t CeriaModulePCF8574::readEncoderStep(uint8_t pinA, uint8_t pinB) {
#ifdef BASIC_ENCODER_ALGORITHM
        return PCF8574::readEncoderValue(pinA, pinB);
#elif defined(MISCHIANTI_ENCODER_ALGORITHM)
        return PCF8574::readEncoderValueMischianti(pinA, pinB);
#elif defined(SEQUENCE_ENCODER_ALGORITHM)
        return PCF8574::readEncoderValueSequence(pinA, pinB);
#elif defined(SEQUENCE_ENCODER_ALGORITHM_REDUCED)
        return PCF8574::readEncoderValueSequenceReduced(pinA, pinB);
#elif defined(POKI_ENCODER_ALGORITHM)
        return PCF8574::readEncoderValuePoki(pinA, pinB);
#else
        return PCF8574::readEncoderValue(pinA, pinB);
#endif
    }

    void CeriaModulePCF8574::setInterruptMode(PCF8574InterruptMode mode) {
        _interruptMode = mode;
    }

    void CeriaModulePCF8574::enableInterrupt() {
        if (_interruptMode == PCF8574_WITH_INTERRUPT) {
            PCF8574::attachInterrupt();
        }
    }

    void CeriaModulePCF8574::disableInterrupt() {
        if (_interruptMode == PCF8574_WITH_INTERRUPT) {
            PCF8574::detachInterrupt();
        }
    }

    uint8_t CeriaModulePCF8574::readAllPins() {
#ifndef PCF8574_LOW_MEMORY
        PCF8574::DigitalInput inputs = PCF8574::digitalReadAll();
        return (inputs.p7 << 7) | (inputs.p6 << 6) | (inputs.p5 << 5) | (inputs.p4 << 4) |
               (inputs.p3 << 3) | (inputs.p2 << 2) | (inputs.p1 << 1) | inputs.p0;
#else
        return PCF8574::digitalReadAll();
#endif
    }

    bool CeriaModulePCF8574::writeAllPins(uint8_t pinValues) {
#ifndef PCF8574_LOW_MEMORY
        PCF8574::DigitalInput outputs;
        outputs.p0 = (pinValues & 0x01) ? HIGH : LOW;
        outputs.p1 = (pinValues & 0x02) ? HIGH : LOW;
        outputs.p2 = (pinValues & 0x04) ? HIGH : LOW;
        outputs.p3 = (pinValues & 0x08) ? HIGH : LOW;
        outputs.p4 = (pinValues & 0x10) ? HIGH : LOW;
        outputs.p5 = (pinValues & 0x20) ? HIGH : LOW;
        outputs.p6 = (pinValues & 0x40) ? HIGH : LOW;
        outputs.p7 = (pinValues & 0x80) ? HIGH : LOW;
        return PCF8574::digitalWriteAll(outputs);
#else
        return PCF8574::digitalWriteAll(pinValues);
#endif
    }

    void CeriaModulePCF8574::setLatency(int latencyMs) {
        PCF8574::setLatency(latencyMs);
    }

    int CeriaModulePCF8574::getLatency() {
        return PCF8574::getLatency();
    }

    String CeriaModulePCF8574::getPinStatusString() {
        if (!isLastTransmissionSuccess()) return "PCF8574 Error";

        uint8_t pinStates = readAllPins();
        String result = "Pins: ";
        for (int i = 7; i >= 0; i--) {
            result += ((pinStates & (1 << i)) ? "1" : "0");
        }
        result += " (0x" + String(pinStates, HEX) + ")";
        return result;
    }

    String CeriaModulePCF8574::getModuleStatusString() {
        if (!isLastTransmissionSuccess()) return "PCF8574 Error";

        String result = "PCF8574@0x" + String(_i2cAddress, HEX);
        result += ", Pins: 0x" + String(readAllPins(), HEX);
        result += ", Latency: " + String(getLatency()) + "ms";
        if (_interruptMode == PCF8574_WITH_INTERRUPT) {
            result += ", INT: ON";
        } else {
            result += ", INT: OFF";
        }
        if (_encoderSetup) {
            result += ", ENC: P" + String(_encoderPinA) + "/P" + String(_encoderPinB);
        }
        return result;
    }

    bool CeriaModulePCF8574::waitForChange(uint32_t timeoutMs) {
        uint32_t startTime = millis();
        uint8_t initialStates = readAllPins();

        while (millis() - startTime < timeoutMs) {
            update();
            if (hasNewData() && readAllPins() != initialStates) {
                return true;
            }
            delay(1);
        }
        return false;
    }

}
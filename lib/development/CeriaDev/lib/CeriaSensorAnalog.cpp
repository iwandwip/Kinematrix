#include "CeriaSensorAnalog.h"

namespace CeriaDevOP {

    CeriaSensorAnalog::CeriaSensorAnalog(uint8_t pin) {
        _pin = pin;
        _rawValue = 0;
        _voltage = 0.0;
        _percentage = 0.0;
        _lastUpdateTime = 0;
        _hasNewData = false;

        _minValue = 0;
        _maxValue = 1023;
        _referenceVoltage = 3.3;
        _resolution = 10;
        _samples = 1;
        _updateInterval = 0;

        _calibrationOffset = 0.0;
        _calibrationMultiplier = 1.0;
        _calibrationEnabled = false;

        _totalReadings = 0;
        _averageValue = 0.0;
        _minReading = 65535;
        _maxReading = 0;
    }

    void CeriaSensorAnalog::begin(float referenceVoltage) {
        _referenceVoltage = referenceVoltage;

#if defined(ESP32)
        // ESP32 - 12-bit ADC, 3.3V reference
        _resolution = 12;
        _maxValue = 4095;
        analogReadResolution(_resolution);
        if (referenceVoltage == 3.3) _referenceVoltage = 3.3;
#elif defined(ESP8266)
        // ESP8266 - 10-bit ADC, 3.3V reference, single analog pin
        _resolution = 10;
        _maxValue = 1023;
        if (referenceVoltage == 3.3) _referenceVoltage = 3.3;
#elif defined(ARDUINO_AVR_UNO) || defined(ARDUINO_AVR_NANO) || defined(__AVR_ATmega328P__) || defined(__AVR_ATmega168__)
        // Arduino Uno/Nano - 10-bit ADC, 5V default reference
        _resolution = 10;
        _maxValue = 1023;
        if (referenceVoltage == 3.3) _referenceVoltage = 5.0;  // Default to 5V for AVR
#elif defined(ARDUINO_AVR_MEGA2560) || defined(__AVR_ATmega1280__) || defined(__AVR_ATmega2560__)
        // Arduino Mega - 10-bit ADC, 5V default reference
        _resolution = 10;
        _maxValue = 1023;
        if (referenceVoltage == 3.3) _referenceVoltage = 5.0;  // Default to 5V for AVR
#elif defined(ARDUINO_SAM_DUE)
        // Arduino Due - 12-bit ADC, 3.3V reference
        _resolution = 12;
        _maxValue = 4095;
        analogReadResolution(_resolution);
        if (referenceVoltage == 3.3) _referenceVoltage = 3.3;
#else
        // Default fallback - 10-bit ADC
        _resolution = 10;
        _maxValue = 1023;
#endif

        pinMode(_pin, INPUT);
        _lastUpdateTime = millis();
        update();
    }

    void CeriaSensorAnalog::update() {
        if (_updateInterval > 0 && (millis() - _lastUpdateTime) < _updateInterval) {
            return;
        }

        uint32_t sum = 0;
        for (uint8_t i = 0; i < _samples; i++) {
            sum += analogRead(_pin);
            if (_samples > 1) delay(1);
        }

        uint16_t newValue = sum / _samples;

        if (newValue != _rawValue) {
            _rawValue = newValue;
            _hasNewData = true;
            _lastUpdateTime = millis();

            _voltage = (_rawValue * _referenceVoltage) / getResolutionMax();

            if (_calibrationEnabled) {
                _voltage = (_voltage + _calibrationOffset) * _calibrationMultiplier;
            }

            _percentage = mapFloat(_rawValue, _minValue, _maxValue, 0.0, 100.0);
            _percentage = constrain(_percentage, 0.0, 100.0);

            _totalReadings++;
            _averageValue = ((_averageValue * (_totalReadings - 1)) + _rawValue) / _totalReadings;

            if (_rawValue < _minReading) _minReading = _rawValue;
            if (_rawValue > _maxReading) _maxReading = _rawValue;
        }
    }

    bool CeriaSensorAnalog::hasNewData() {
        bool result = _hasNewData;
        _hasNewData = false;
        return result;
    }

    bool CeriaSensorAnalog::isConnected(uint32_t timeoutMs) {
        return (millis() - _lastUpdateTime) < timeoutMs;
    }

    uint16_t CeriaSensorAnalog::getRaw() {
        return _rawValue;
    }

    float CeriaSensorAnalog::getVoltage() {
        return _voltage;
    }

    float CeriaSensorAnalog::getPercentage() {
        return _percentage;
    }

    float CeriaSensorAnalog::getScaled(float minScale, float maxScale) {
        return mapFloat(_rawValue, _minValue, _maxValue, minScale, maxScale);
    }

    void CeriaSensorAnalog::setRange(uint16_t minValue, uint16_t maxValue) {
        _minValue = minValue;
        _maxValue = maxValue;
    }

    void CeriaSensorAnalog::setReferenceVoltage(float voltage) {
        _referenceVoltage = voltage;
    }

    void CeriaSensorAnalog::setAnalogReference(AnalogReference reference) {
#if defined(ARDUINO_AVR_UNO) || defined(ARDUINO_AVR_NANO) || defined(__AVR_ATmega328P__) || defined(__AVR_ATmega168__)
        // Arduino Uno/Nano (ATmega328P/168)
        switch (reference) {
            case ANALOG_REF_DEFAULT:
                analogReference(DEFAULT);
                break;
            case ANALOG_REF_INTERNAL_1V1:
                analogReference(INTERNAL);    // 1.1V internal reference
                break;
            case ANALOG_REF_EXTERNAL:
                analogReference(EXTERNAL);    // External AREF reference
                break;
            default:
                analogReference(DEFAULT);
                break;
        }
#elif defined(ARDUINO_AVR_MEGA2560) || defined(__AVR_ATmega1280__) || defined(__AVR_ATmega2560__)
        // Arduino Mega (ATmega1280/2560)
        switch (reference) {
            case ANALOG_REF_DEFAULT:
                analogReference(DEFAULT);
                break;
            case ANALOG_REF_INTERNAL_1V1:
                analogReference(INTERNAL1V1);  // 1.1V internal reference
                break;
            case ANALOG_REF_INTERNAL_2V56:
                analogReference(INTERNAL2V56); // 2.56V internal reference
                break;
            case ANALOG_REF_EXTERNAL:
                analogReference(EXTERNAL);     // External AREF reference
                break;
            default:
                analogReference(DEFAULT);
                break;
        }
#elif defined(ARDUINO_SAM_DUE)
        // Arduino Due
        switch (reference) {
            case ANALOG_REF_DEFAULT:
            case ANALOG_REF_3V3:
                analogReference(AR_DEFAULT);   // 3.3V reference (Due only option)
                break;
            default:
                analogReference(AR_DEFAULT);
                break;
        }
#elif defined(ESP32)
        // ESP32 - No analogReference() function, handled by setReferenceVoltage()
#elif defined(ESP8266)
        // ESP8266 - No analogReference() function, handled by setReferenceVoltage()  
#endif
    }

    void CeriaSensorAnalog::setResolution(AnalogResolution resolution) {
        _resolution = resolution;
        _maxValue = (1 << _resolution) - 1;

#if defined(ESP32)
        analogReadResolution(_resolution);
#endif
    }

    void CeriaSensorAnalog::setSampling(uint8_t samples) {
        _samples = constrain(samples, 1, 100);
    }

    void CeriaSensorAnalog::setUpdateInterval(uint32_t intervalMs) {
        _updateInterval = intervalMs;
    }

    void CeriaSensorAnalog::enableCalibration(float offset, float multiplier) {
        _calibrationOffset = offset;
        _calibrationMultiplier = multiplier;
        _calibrationEnabled = true;
    }

    void CeriaSensorAnalog::disableCalibration() {
        _calibrationEnabled = false;
        _calibrationOffset = 0.0;
        _calibrationMultiplier = 1.0;
    }

    void CeriaSensorAnalog::calibrateZero() {
        update();
        _calibrationOffset = -_voltage;
        _calibrationEnabled = true;
    }

    void CeriaSensorAnalog::calibrateSpan(float knownValue) {
        update();
        if (_voltage != 0.0) {
            _calibrationMultiplier = knownValue / _voltage;
            _calibrationEnabled = true;
        }
    }

    void CeriaSensorAnalog::resetStatistics() {
        _totalReadings = 0;
        _averageValue = 0.0;
        _minReading = 65535;
        _maxReading = 0;
    }

    uint32_t CeriaSensorAnalog::getTotalReadings() {
        return _totalReadings;
    }

    float CeriaSensorAnalog::getAverageValue() {
        return _averageValue;
    }

    uint16_t CeriaSensorAnalog::getMinReading() {
        return _minReading == 65535 ? 0 : _minReading;
    }

    uint16_t CeriaSensorAnalog::getMaxReading() {
        return _maxReading;
    }

    String CeriaSensorAnalog::getAnalogString() {
        return String(_rawValue) + " (" + String(_voltage, 3) + "V, " + String(_percentage, 1) + "%)";
    }

    String CeriaSensorAnalog::getStatusString() {
        String result = "Pin A" + String(_pin) + ": ";
        result += String(_rawValue) + "/" + String(getResolutionMax());
        result += " = " + String(_voltage, 3) + "V";
        result += " (" + String(_percentage, 1) + "%)";
        if (_calibrationEnabled) {
            result += " [CAL]";
        }
        return result;
    }

    String CeriaSensorAnalog::getStatisticsString() {
        String result = "Stats: Readings=" + String(_totalReadings);
        result += ", Avg=" + String(_averageValue, 1);
        result += ", Min=" + String(getMinReading());
        result += ", Max=" + String(_maxReading);
        result += ", Range=" + String(_minValue) + "-" + String(_maxValue);
        return result;
    }

    bool CeriaSensorAnalog::waitForReading(uint32_t timeoutMs) {
        uint32_t startTime = millis();
        while (millis() - startTime < timeoutMs) {
            update();
            if (hasNewData()) {
                return true;
            }
            delay(1);
        }
        return false;
    }

    bool CeriaSensorAnalog::waitForValue(float targetValue, float tolerance, uint32_t timeoutMs) {
        uint32_t startTime = millis();
        while (millis() - startTime < timeoutMs) {
            update();
            if (abs(_voltage - targetValue) <= tolerance) {
                return true;
            }
            delay(10);
        }
        return false;
    }

    float CeriaSensorAnalog::mapFloat(float value, float fromMin, float fromMax, float toMin, float toMax) {
        if (fromMax == fromMin) return toMin;
        return (value - fromMin) * (toMax - toMin) / (fromMax - fromMin) + toMin;
    }

    uint16_t CeriaSensorAnalog::getResolutionMax() {
        return (1 << _resolution) - 1;
    }

}
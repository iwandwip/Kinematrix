#ifndef CERIA_SENSOR_ANALOG_H
#define CERIA_SENSOR_ANALOG_H

#include "Arduino.h"

namespace CeriaDevOP {

    enum AnalogResolution {
        ANALOG_8BIT = 8,
        ANALOG_10BIT = 10,
        ANALOG_12BIT = 12,
        ANALOG_16BIT = 16
    };

    enum AnalogReference {
        ANALOG_REF_DEFAULT = 0,
#if defined(ARDUINO_AVR_UNO) || defined(ARDUINO_AVR_NANO) || defined(__AVR_ATmega328P__) || defined(__AVR_ATmega168__)
        ANALOG_REF_INTERNAL_1V1 = 1,    // 1.1V internal reference for Uno/Nano
        ANALOG_REF_EXTERNAL = 2,        // External AREF reference
#elif defined(ARDUINO_AVR_MEGA2560) || defined(__AVR_ATmega1280__) || defined(__AVR_ATmega2560__)
        ANALOG_REF_INTERNAL_1V1 = 1,    // 1.1V internal reference for Mega
        ANALOG_REF_INTERNAL_2V56 = 2,   // 2.56V internal reference for Mega
        ANALOG_REF_EXTERNAL = 3,        // External AREF reference for Mega
#elif defined(ARDUINO_SAM_DUE)
        ANALOG_REF_3V3 = 1,             // 3.3V reference for Due
#endif
    };

    class CeriaSensorAnalog {
    private:
        uint8_t _pin;
        uint16_t _rawValue;
        float _voltage;
        float _percentage;
        uint32_t _lastUpdateTime;
        bool _hasNewData;

        uint16_t _minValue;
        uint16_t _maxValue;
        float _referenceVoltage;
        uint16_t _resolution;
        uint8_t _samples;
        uint32_t _updateInterval;

        float _calibrationOffset;
        float _calibrationMultiplier;
        bool _calibrationEnabled;

        uint32_t _totalReadings;
        float _averageValue;
        uint16_t _minReading;
        uint16_t _maxReading;

    public:
        CeriaSensorAnalog(uint8_t pin);

        void begin(float referenceVoltage = 3.3);
        void update();

        bool hasNewData();
        bool isConnected(uint32_t timeoutMs = 5000);

        uint16_t getRaw();
        float getVoltage();
        float getPercentage();
        float getScaled(float minScale = 0.0, float maxScale = 100.0);

        void setRange(uint16_t minValue, uint16_t maxValue);
        void setReferenceVoltage(float voltage);
        void setAnalogReference(AnalogReference reference);
        void setResolution(AnalogResolution resolution);
        void setSampling(uint8_t samples);
        void setUpdateInterval(uint32_t intervalMs);

        void enableCalibration(float offset = 0.0, float multiplier = 1.0);
        void disableCalibration();
        void calibrateZero();
        void calibrateSpan(float knownValue);

        void resetStatistics();
        uint32_t getTotalReadings();
        float getAverageValue();
        uint16_t getMinReading();
        uint16_t getMaxReading();

        String getAnalogString();
        String getStatusString();
        String getStatisticsString();

        bool waitForReading(uint32_t timeoutMs = 5000);
        bool waitForValue(float targetValue, float tolerance = 0.1, uint32_t timeoutMs = 10000);

        float mapFloat(float value, float fromMin, float fromMax, float toMin, float toMax);
        uint16_t getResolutionMax();
    };

}

#endif
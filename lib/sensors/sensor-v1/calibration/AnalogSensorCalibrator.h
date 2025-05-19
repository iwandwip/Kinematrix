#ifndef ANALOG_SENSOR_CALIBRATOR_H
#define ANALOG_SENSOR_CALIBRATOR_H

#include <Arduino.h>
#include <EEPROM.h>

#define CALIBRATION_NONE 0
#define CALIBRATION_ONE_POINT 1
#define CALIBRATION_TWO_POINT 2
#define CALIBRATION_MULTI_POINT 3

class AnalogSensorCalibrator {
public:
    AnalogSensorCalibrator(int pin, float referenceVoltage = 5.0, float adcRange = 0);
    ~AnalogSensorCalibrator();

    float readRaw();
    float readAverage();
    float readVoltage();
    float readValue();

    void begin();
    void setBufferSize(uint8_t size);
    void setMaxCalibrationPoints(uint8_t maxPoints);

    bool calibrateOnePoint(float knownValue, float rawValue = -1);
    bool calibrateTwoPoint(float knownValue1, float rawValue1,
                           float knownValue2, float rawValue2);

    void startCalibration(uint8_t calibrationType);
    bool addCalibrationPoint(float knownValue, float rawValue = -1);
    bool calculateCalibration();
    void cancelCalibration();

    bool clearCalibrationPoints();
    bool removeCalibrationPoint(uint8_t index);
    uint8_t getCalibrationPointCount();

    void setInterpolationMethod(uint8_t method);
    void setPolynomialDegree(uint8_t degree);

    bool saveCalibration(int eepromAddress = 0);
    bool loadCalibration(int eepromAddress = 0);

    bool isCalibrated();
    uint8_t getCalibrationMethod();

    void printCalibrationDetails();

protected:
    struct CalibrationPoint {
        float rawValue;
        float knownValue;
    };

    int _pin;
    float _refVoltage;
    float _adcRange;
    uint8_t _calibrationType;
    bool _isCalibrated;
    bool _inCalibrationMode;

    float *_buffer;
    uint8_t _bufferSize;
    uint8_t _bufferIndex;
    bool _bufferFilled;

    CalibrationPoint *_calibPoints;
    uint8_t _maxCalibPoints;
    uint8_t _numCalibPoints;

    float _scale;
    float _slope, _offset;

    uint8_t _interpolationMethod;
    uint8_t _polynomialDegree;
    float *_polynomialCoeffs;

    void initializeBuffers();
    void cleanupMemory();
    float linearInterpolate(float rawValue);
    float polynomialInterpolate(float rawValue);
    bool calculatePolynomialRegression();
    bool sortCalibrationPoints();
};

#endif
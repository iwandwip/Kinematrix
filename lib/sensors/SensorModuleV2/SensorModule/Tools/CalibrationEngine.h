#ifndef CALIBRATION_ENGINE_H
#define CALIBRATION_ENGINE_H

#include <Arduino.h>

#define CALIBRATION_NONE 0
#define CALIBRATION_ONE_POINT 1
#define CALIBRATION_TWO_POINT 2
#define CALIBRATION_MULTI_POINT 3

class CalibrationEngine {
public:
    struct CalibrationPoint {
        float rawValue;
        float knownValue;
    };

    CalibrationEngine();
    ~CalibrationEngine();

    void setMaxCalibrationPoints(uint8_t maxPoints);
    void setInterpolationMethod(uint8_t method);
    void setPolynomialDegree(uint8_t degree);

    bool calibrateOnePoint(float knownValue, float rawValue);
    bool calibrateTwoPoint(float knownValue1, float rawValue1, float knownValue2, float rawValue2);
    
    void startCalibration(uint8_t calibrationType);
    bool addCalibrationPoint(float knownValue, float rawValue);
    bool calculateCalibration();
    void cancelCalibration();
    bool clearCalibrationPoints();
    
    float readCalibratedValue(float rawValue);
    
    bool isCalibrated() const;
    uint8_t getCalibrationMethod() const;
    uint8_t getCalibrationPointCount() const;
    
    const CalibrationPoint* getCalibrationPoints() const;
    float getSlope() const;
    float getOffset() const;
    float getScale() const;
    const float* getPolynomialCoeffs() const;
    uint8_t getPolynomialDegree() const;

private:
    CalibrationPoint *_calibPoints;
    uint8_t _maxCalibPoints;
    uint8_t _numCalibPoints;
    uint8_t _calibrationType;
    bool _isCalibrated;
    
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
#include "CalibrationEngine.h"

CalibrationEngine::CalibrationEngine()
    : _calibPoints(nullptr), _maxCalibPoints(10), _numCalibPoints(0),
      _calibrationType(CALIBRATION_NONE), _isCalibrated(false),
      _scale(1.0f), _slope(1.0f), _offset(0.0f),
      _interpolationMethod(0), _polynomialDegree(2), _polynomialCoeffs(nullptr) {
    initializeBuffers();
}

CalibrationEngine::~CalibrationEngine() {
    cleanupMemory();
}

void CalibrationEngine::setMaxCalibrationPoints(uint8_t maxPoints) {
    if (maxPoints > 0 && maxPoints != _maxCalibPoints) {
        _maxCalibPoints = maxPoints;
        initializeBuffers();
    }
}

void CalibrationEngine::setInterpolationMethod(uint8_t method) {
    _interpolationMethod = method;
}

void CalibrationEngine::setPolynomialDegree(uint8_t degree) {
    if (degree > 0 && degree <= 6) {
        _polynomialDegree = degree;
        if (_polynomialCoeffs) {
            delete[] _polynomialCoeffs;
        }
        _polynomialCoeffs = new float[_polynomialDegree + 1];
        for (uint8_t i = 0; i <= _polynomialDegree; i++) {
            _polynomialCoeffs[i] = 0.0f;
        }
    }
}

bool CalibrationEngine::calibrateOnePoint(float knownValue, float rawValue) {
    _calibrationType = CALIBRATION_ONE_POINT;
    _scale = knownValue / rawValue;
    _isCalibrated = true;
    return true;
}

bool CalibrationEngine::calibrateTwoPoint(float knownValue1, float rawValue1, float knownValue2, float rawValue2) {
    _calibrationType = CALIBRATION_TWO_POINT;
    
    if (rawValue2 != rawValue1) {
        _slope = (knownValue2 - knownValue1) / (rawValue2 - rawValue1);
        _offset = knownValue1 - _slope * rawValue1;
        _isCalibrated = true;
        return true;
    }
    return false;
}

void CalibrationEngine::startCalibration(uint8_t calibrationType) {
    _calibrationType = calibrationType;
    _numCalibPoints = 0;
    _isCalibrated = false;
}

bool CalibrationEngine::addCalibrationPoint(float knownValue, float rawValue) {
    if (_numCalibPoints >= _maxCalibPoints) {
        return false;
    }
    
    _calibPoints[_numCalibPoints].rawValue = rawValue;
    _calibPoints[_numCalibPoints].knownValue = knownValue;
    _numCalibPoints++;
    return true;
}

bool CalibrationEngine::calculateCalibration() {
    if (_numCalibPoints < 2) {
        return false;
    }
    
    sortCalibrationPoints();
    
    if (_interpolationMethod == 0) {
        if (_numCalibPoints == 2) {
            return calibrateTwoPoint(
                _calibPoints[0].knownValue, _calibPoints[0].rawValue,
                _calibPoints[1].knownValue, _calibPoints[1].rawValue
            );
        }
    } else {
        return calculatePolynomialRegression();
    }
    
    _isCalibrated = true;
    return true;
}

void CalibrationEngine::cancelCalibration() {
    _numCalibPoints = 0;
    _isCalibrated = false;
    _calibrationType = CALIBRATION_NONE;
}

bool CalibrationEngine::clearCalibrationPoints() {
    _numCalibPoints = 0;
    _isCalibrated = false;
    _calibrationType = CALIBRATION_NONE;
    _scale = 1.0f;
    _slope = 1.0f;
    _offset = 0.0f;
    return true;
}

float CalibrationEngine::readCalibratedValue(float rawValue) {
    if (!_isCalibrated) {
        return rawValue;
    }
    
    switch (_calibrationType) {
        case CALIBRATION_ONE_POINT:
            return rawValue * _scale;
            
        case CALIBRATION_TWO_POINT:
            return _slope * rawValue + _offset;
            
        case CALIBRATION_MULTI_POINT:
            if (_interpolationMethod == 0) {
                return linearInterpolate(rawValue);
            } else {
                return polynomialInterpolate(rawValue);
            }
            
        default:
            return rawValue;
    }
}

bool CalibrationEngine::isCalibrated() const {
    return _isCalibrated;
}

uint8_t CalibrationEngine::getCalibrationMethod() const {
    return _calibrationType;
}

uint8_t CalibrationEngine::getCalibrationPointCount() const {
    return _numCalibPoints;
}

const CalibrationEngine::CalibrationPoint* CalibrationEngine::getCalibrationPoints() const {
    return _calibPoints;
}

float CalibrationEngine::getSlope() const {
    return _slope;
}

float CalibrationEngine::getOffset() const {
    return _offset;
}

float CalibrationEngine::getScale() const {
    return _scale;
}

const float* CalibrationEngine::getPolynomialCoeffs() const {
    return _polynomialCoeffs;
}

uint8_t CalibrationEngine::getPolynomialDegree() const {
    return _polynomialDegree;
}

void CalibrationEngine::initializeBuffers() {
    cleanupMemory();
    
    _calibPoints = new CalibrationPoint[_maxCalibPoints];
    _polynomialCoeffs = new float[_polynomialDegree + 1];
    
    for (uint8_t i = 0; i <= _polynomialDegree; i++) {
        _polynomialCoeffs[i] = 0.0f;
    }
}

void CalibrationEngine::cleanupMemory() {
    if (_calibPoints) {
        delete[] _calibPoints;
        _calibPoints = nullptr;
    }
    if (_polynomialCoeffs) {
        delete[] _polynomialCoeffs;
        _polynomialCoeffs = nullptr;
    }
}

float CalibrationEngine::linearInterpolate(float rawValue) {
    if (_numCalibPoints < 2) {
        return rawValue;
    }
    
    for (uint8_t i = 0; i < _numCalibPoints - 1; i++) {
        if (rawValue >= _calibPoints[i].rawValue && rawValue <= _calibPoints[i + 1].rawValue) {
            float ratio = (rawValue - _calibPoints[i].rawValue) / 
                         (_calibPoints[i + 1].rawValue - _calibPoints[i].rawValue);
            return _calibPoints[i].knownValue + 
                   ratio * (_calibPoints[i + 1].knownValue - _calibPoints[i].knownValue);
        }
    }
    
    if (rawValue < _calibPoints[0].rawValue) {
        return _calibPoints[0].knownValue;
    }
    return _calibPoints[_numCalibPoints - 1].knownValue;
}

float CalibrationEngine::polynomialInterpolate(float rawValue) {
    if (!_polynomialCoeffs) {
        return rawValue;
    }
    
    float result = 0.0f;
    float xPower = 1.0f;
    
    for (uint8_t i = 0; i <= _polynomialDegree; i++) {
        result += _polynomialCoeffs[i] * xPower;
        xPower *= rawValue;
    }
    
    return result;
}

bool CalibrationEngine::calculatePolynomialRegression() {
    return true;
}

bool CalibrationEngine::sortCalibrationPoints() {
    for (uint8_t i = 0; i < _numCalibPoints - 1; i++) {
        for (uint8_t j = 0; j < _numCalibPoints - i - 1; j++) {
            if (_calibPoints[j].rawValue > _calibPoints[j + 1].rawValue) {
                CalibrationPoint temp = _calibPoints[j];
                _calibPoints[j] = _calibPoints[j + 1];
                _calibPoints[j + 1] = temp;
            }
        }
    }
    return true;
}
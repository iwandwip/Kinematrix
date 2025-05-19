#include "AnalogSensorCalibrator.h"

AnalogSensorCalibrator::AnalogSensorCalibrator(int pin, float referenceVoltage, float adcRange) {
    _pin = pin;
    _refVoltage = referenceVoltage;

    if (adcRange <= 0) {
#if defined(ARDUINO_ARCH_ESP32)
        _adcRange = 4095.0;
#elif defined(ARDUINO_ARCH_ESP8266)
        _adcRange = 1023.0;
#else
    _adcRange = 1023.0;
#endif
    } else {
        _adcRange = adcRange;
    }

    _calibrationType = CALIBRATION_NONE;
    _isCalibrated = false;
    _inCalibrationMode = false;

    _bufferSize = 10;
    _maxCalibPoints = 5;
    _numCalibPoints = 0;

    _scale = 1.0;
    _slope = 1.0;
    _offset = 0.0;

    _interpolationMethod = 0;
    _polynomialDegree = 2;

    _buffer = NULL;
    _calibPoints = NULL;
    _polynomialCoeffs = NULL;

    pinMode(_pin, INPUT);
}

AnalogSensorCalibrator::~AnalogSensorCalibrator() {
    cleanupMemory();
}

float AnalogSensorCalibrator::readRaw() {
    return analogRead(_pin);
}

float AnalogSensorCalibrator::readAverage() {
    float raw = readRaw();

    if (_buffer != NULL) {
        _buffer[_bufferIndex] = raw;
        _bufferIndex = (_bufferIndex + 1) % _bufferSize;

        if (_bufferIndex == 0) {
            _bufferFilled = true;
        }

        float sum = 0;
        int count = _bufferFilled ? _bufferSize : _bufferIndex;

        for (int i = 0; i < count; i++) {
            sum += _buffer[i];
        }

        return (count > 0) ? sum / count : raw;
    }

    return raw;
}

float AnalogSensorCalibrator::readVoltage() {
    float avgADC = readAverage();
    return (avgADC / _adcRange) * _refVoltage;
}

float AnalogSensorCalibrator::readValue() {
    float voltage = readVoltage();

    if (!_isCalibrated) {
        return voltage;
    }

    switch (_calibrationType) {
        case CALIBRATION_ONE_POINT:
            return voltage * _scale;

        case CALIBRATION_TWO_POINT:
            return _slope * voltage + _offset;

        case CALIBRATION_MULTI_POINT:
            if (_interpolationMethod == 0) {
                return linearInterpolate(voltage);
            } else {
                return polynomialInterpolate(voltage);
            }

        default:
            return voltage;
    }
}

void AnalogSensorCalibrator::begin() {
#if defined(ARDUINO_ARCH_ESP8266) || defined(ARDUINO_ARCH_ESP32)
    EEPROM.begin(512);
#endif

    initializeBuffers();
}

void AnalogSensorCalibrator::setBufferSize(uint8_t size) {
    if (size > 0 && size != _bufferSize) {
        _bufferSize = size;
        initializeBuffers();
    }
}

void AnalogSensorCalibrator::setMaxCalibrationPoints(uint8_t maxPoints) {
    if (maxPoints > 0 && maxPoints != _maxCalibPoints) {
        _maxCalibPoints = maxPoints;
        initializeBuffers();
    }
}

bool AnalogSensorCalibrator::calibrateOnePoint(float knownValue, float rawValue) {
    if (rawValue < 0) {
        rawValue = readVoltage();
    }

    if (rawValue <= 0 || knownValue == 0) {
        return false;
    }

    _scale = knownValue / rawValue;
    _calibrationType = CALIBRATION_ONE_POINT;
    _isCalibrated = true;

    clearCalibrationPoints();
    addCalibrationPoint(knownValue, rawValue);

    return true;
}

bool AnalogSensorCalibrator::calibrateTwoPoint(float knownValue1, float rawValue1,
                                               float knownValue2, float rawValue2) {
    if (rawValue1 == rawValue2) {
        return false;
    }

    _slope = (knownValue2 - knownValue1) / (rawValue2 - rawValue1);
    _offset = knownValue1 - _slope * rawValue1;
    _calibrationType = CALIBRATION_TWO_POINT;
    _isCalibrated = true;

    clearCalibrationPoints();
    addCalibrationPoint(knownValue1, rawValue1);
    addCalibrationPoint(knownValue2, rawValue2);

    return true;
}

void AnalogSensorCalibrator::startCalibration(uint8_t calibrationType) {
    _inCalibrationMode = true;
    _calibrationType = calibrationType;
    clearCalibrationPoints();
}

bool AnalogSensorCalibrator::addCalibrationPoint(float knownValue, float rawValue) {
    if (_numCalibPoints >= _maxCalibPoints) {
        return false;
    }

    if (rawValue < 0) {
        rawValue = readVoltage();
    }

    _calibPoints[_numCalibPoints].rawValue = rawValue;
    _calibPoints[_numCalibPoints].knownValue = knownValue;
    _numCalibPoints++;

    return true;
}

bool AnalogSensorCalibrator::calculateCalibration() {
    if (_numCalibPoints < 1) {
        return false;
    }

    sortCalibrationPoints();

    if (_numCalibPoints == 1) {
        _scale = _calibPoints[0].knownValue / _calibPoints[0].rawValue;
        _calibrationType = CALIBRATION_ONE_POINT;
        _isCalibrated = true;
        _inCalibrationMode = false;
        return true;
    } else if (_numCalibPoints == 2) {
        float rawValue1 = _calibPoints[0].rawValue;
        float knownValue1 = _calibPoints[0].knownValue;
        float rawValue2 = _calibPoints[1].rawValue;
        float knownValue2 = _calibPoints[1].knownValue;

        _slope = (knownValue2 - knownValue1) / (rawValue2 - rawValue1);
        _offset = knownValue1 - _slope * rawValue1;
        _calibrationType = CALIBRATION_TWO_POINT;
        _isCalibrated = true;
        _inCalibrationMode = false;
        return true;
    } else {
        _calibrationType = CALIBRATION_MULTI_POINT;

        if (_interpolationMethod == 0) {
            _isCalibrated = true;
            _inCalibrationMode = false;
            return true;
        } else {
            bool success = calculatePolynomialRegression();
            if (success) {
                _isCalibrated = true;
                _inCalibrationMode = false;
            }
            return success;
        }
    }
}

void AnalogSensorCalibrator::cancelCalibration() {
    _inCalibrationMode = false;
}

bool AnalogSensorCalibrator::clearCalibrationPoints() {
    if (_calibPoints != NULL) {
        for (uint8_t i = 0; i < _maxCalibPoints; i++) {
            _calibPoints[i].rawValue = 0.0;
            _calibPoints[i].knownValue = 0.0;
        }
        _numCalibPoints = 0;
        return true;
    }
    return false;
}

bool AnalogSensorCalibrator::removeCalibrationPoint(uint8_t index) {
    if (index >= _numCalibPoints || _calibPoints == NULL) {
        return false;
    }

    for (uint8_t i = index; i < _numCalibPoints - 1; i++) {
        _calibPoints[i] = _calibPoints[i + 1];
    }

    _numCalibPoints--;
    return true;
}

uint8_t AnalogSensorCalibrator::getCalibrationPointCount() {
    return _numCalibPoints;
}

void AnalogSensorCalibrator::setInterpolationMethod(uint8_t method) {
    _interpolationMethod = method;
}

void AnalogSensorCalibrator::setPolynomialDegree(uint8_t degree) {
    if (degree < _maxCalibPoints - 1) {
        _polynomialDegree = degree;
    }
}

bool AnalogSensorCalibrator::saveCalibration(int eepromAddress) {
    if (!_isCalibrated) {
        return false;
    }

    int addr = eepromAddress;

    EEPROM.write(addr++, _calibrationType);

    switch (_calibrationType) {
        case CALIBRATION_ONE_POINT:
            EEPROM.put(addr, _scale);
            addr += sizeof(float);
            break;

        case CALIBRATION_TWO_POINT:
            EEPROM.put(addr, _slope);
            addr += sizeof(float);
            EEPROM.put(addr, _offset);
            addr += sizeof(float);
            break;

        case CALIBRATION_MULTI_POINT:
            EEPROM.write(addr++, _numCalibPoints);
            EEPROM.write(addr++, _interpolationMethod);

            if (_interpolationMethod == 1) {
                EEPROM.write(addr++, _polynomialDegree);
                for (uint8_t i = 0; i <= _polynomialDegree; i++) {
                    EEPROM.put(addr, _polynomialCoeffs[i]);
                    addr += sizeof(float);
                }
            } else {
                for (uint8_t i = 0; i < _numCalibPoints; i++) {
                    EEPROM.put(addr, _calibPoints[i].rawValue);
                    addr += sizeof(float);
                    EEPROM.put(addr, _calibPoints[i].knownValue);
                    addr += sizeof(float);
                }
            }
            break;
    }

#if defined(ARDUINO_ARCH_ESP8266) || defined(ARDUINO_ARCH_ESP32)
    return EEPROM.commit();
#else
    return true;
#endif
}

bool AnalogSensorCalibrator::loadCalibration(int eepromAddress) {
    int addr = eepromAddress;
    uint8_t numPoints;
    float rawValue, knownValue;

    uint8_t calType = EEPROM.read(addr++);

    if (calType > CALIBRATION_MULTI_POINT) {
        return false;
    }

    _calibrationType = calType;

    switch (_calibrationType) {
        case CALIBRATION_ONE_POINT:
            EEPROM.get(addr, _scale);
            addr += sizeof(float);

            clearCalibrationPoints();
            addCalibrationPoint(1.0, 1.0 / _scale);

            _isCalibrated = true;
            break;

        case CALIBRATION_TWO_POINT:
            EEPROM.get(addr, _slope);
            addr += sizeof(float);
            EEPROM.get(addr, _offset);
            addr += sizeof(float);

            clearCalibrationPoints();
            addCalibrationPoint(_offset, 0);
            addCalibrationPoint(_slope + _offset, 1);

            _isCalibrated = true;
            break;

        case CALIBRATION_MULTI_POINT: {
            numPoints = EEPROM.read(addr++);
            _interpolationMethod = EEPROM.read(addr++);

            if (numPoints > _maxCalibPoints) {
                return false;
            }

            if (_interpolationMethod == 1) {
                _polynomialDegree = EEPROM.read(addr++);

                if (_polynomialDegree >= _maxCalibPoints) {
                    return false;
                }

                for (uint8_t i = 0; i <= _polynomialDegree; i++) {
                    EEPROM.get(addr, _polynomialCoeffs[i]);
                    addr += sizeof(float);
                }
            } else {
                clearCalibrationPoints();

                for (uint8_t i = 0; i < numPoints; i++) {
                    EEPROM.get(addr, rawValue);
                    addr += sizeof(float);
                    EEPROM.get(addr, knownValue);
                    addr += sizeof(float);

                    addCalibrationPoint(knownValue, rawValue);
                }
            }

            _isCalibrated = true;
        }
            break;

        default:
            _isCalibrated = false;
            break;
    }

    return _isCalibrated;
}

bool AnalogSensorCalibrator::isCalibrated() {
    return _isCalibrated;
}

uint8_t AnalogSensorCalibrator::getCalibrationMethod() {
    return _calibrationType;
}

void AnalogSensorCalibrator::printCalibrationDetails() {
    if (!_isCalibrated) {
        Serial.println("Sensor not calibrated");
        return;
    }

    Serial.println("\n===== CALIBRATION DETAILS =====");

    switch (_calibrationType) {
        case CALIBRATION_ONE_POINT:
            Serial.println("Method: One-point Calibration");
            Serial.print("Scale: ");
            Serial.println(_scale, 4);
            Serial.println("Formula: value = voltage * scale");
            break;

        case CALIBRATION_TWO_POINT:
            Serial.println("Method: Two-point Calibration (Linear)");
            Serial.print("Slope: ");
            Serial.println(_slope, 4);
            Serial.print("Offset: ");
            Serial.println(_offset, 4);
            Serial.println("Formula: value = slope * voltage + offset");
            break;

        case CALIBRATION_MULTI_POINT:
            Serial.println("Method: Multi-point Calibration");

            if (_interpolationMethod == 0) {
                Serial.println("Interpolation: Linear Segmented");
            } else {
                Serial.print("Interpolation: Polynomial Degree ");
                Serial.println(_polynomialDegree);

                Serial.println("Coefficients:");
                for (uint8_t i = 0; i <= _polynomialDegree; i++) {
                    Serial.print("  a");
                    Serial.print(i);
                    Serial.print(" = ");
                    Serial.println(_polynomialCoeffs[i], 6);
                }
            }
            break;
    }

    Serial.println("\nCalibration Points:");
    for (uint8_t i = 0; i < _numCalibPoints; i++) {
        Serial.print("  Point ");
        Serial.print(i + 1);
        Serial.print(": ");
        Serial.print(_calibPoints[i].rawValue, 3);
        Serial.print("V = ");
        Serial.print(_calibPoints[i].knownValue, 3);
        Serial.println(" units");
    }

    Serial.println("\nCurrent Readings:");
    Serial.print("  ADC Value: ");
    Serial.println(readRaw());
    Serial.print("  Voltage: ");
    Serial.print(readVoltage(), 3);
    Serial.println("V");
    Serial.print("  Calibrated Value: ");
    Serial.println(readValue(), 3);
}

void AnalogSensorCalibrator::initializeBuffers() {
    cleanupMemory();

    _buffer = new float[_bufferSize];
    for (uint8_t i = 0; i < _bufferSize; i++) {
        _buffer[i] = 0.0;
    }
    _bufferIndex = 0;
    _bufferFilled = false;

    _calibPoints = new CalibrationPoint[_maxCalibPoints];
    for (uint8_t i = 0; i < _maxCalibPoints; i++) {
        _calibPoints[i].rawValue = 0.0;
        _calibPoints[i].knownValue = 0.0;
    }
    _numCalibPoints = 0;

    _polynomialCoeffs = new float[_maxCalibPoints];
    for (uint8_t i = 0; i < _maxCalibPoints; i++) {
        _polynomialCoeffs[i] = 0.0;
    }
}

void AnalogSensorCalibrator::cleanupMemory() {
    if (_buffer != NULL) {
        delete[] _buffer;
        _buffer = NULL;
    }

    if (_calibPoints != NULL) {
        delete[] _calibPoints;
        _calibPoints = NULL;
    }

    if (_polynomialCoeffs != NULL) {
        delete[] _polynomialCoeffs;
        _polynomialCoeffs = NULL;
    }
}

float AnalogSensorCalibrator::linearInterpolate(float rawValue) {
    if (rawValue <= _calibPoints[0].rawValue) {
        return _calibPoints[0].knownValue;
    }

    if (rawValue >= _calibPoints[_numCalibPoints - 1].rawValue) {
        return _calibPoints[_numCalibPoints - 1].knownValue;
    }

    for (uint8_t i = 0; i < _numCalibPoints - 1; i++) {
        if (rawValue >= _calibPoints[i].rawValue && rawValue <= _calibPoints[i + 1].rawValue) {
            float x0 = _calibPoints[i].rawValue;
            float y0 = _calibPoints[i].knownValue;
            float x1 = _calibPoints[i + 1].rawValue;
            float y1 = _calibPoints[i + 1].knownValue;

            return y0 + (y1 - y0) * (rawValue - x0) / (x1 - x0);
        }
    }

    return rawValue;
}

float AnalogSensorCalibrator::polynomialInterpolate(float rawValue) {
    if (_polynomialCoeffs != NULL) {
        float result = 0;
        float x_power = 1;

        for (int i = 0; i <= _polynomialDegree; i++) {
            result += _polynomialCoeffs[i] * x_power;
            x_power *= rawValue;
        }

        return result;
    }

    return linearInterpolate(rawValue);
}

bool AnalogSensorCalibrator::calculatePolynomialRegression() {
    if (_numCalibPoints <= 1 || _polynomialCoeffs == NULL) {
        return false;
    }

    if (_polynomialDegree == 2 && _numCalibPoints >= 3) {
        float x1 = _calibPoints[0].rawValue;
        float y1 = _calibPoints[0].knownValue;
        float x2 = _calibPoints[1].rawValue;
        float y2 = _calibPoints[1].knownValue;
        float x3 = _calibPoints[2].rawValue;
        float y3 = _calibPoints[2].knownValue;

        float x1_2 = x1 * x1;
        float x2_2 = x2 * x2;
        float x3_2 = x3 * x3;

        float det = x1_2 * (x2 - x3) - x1 * (x2_2 - x3_2) + (x2_2 * x3 - x2 * x3_2);

        if (abs(det) < 0.000001) {
            return false;
        }

        _polynomialCoeffs[2] = (y1 * (x2 - x3) - x1 * (y2 - y3) + (y2 * x3 - x2 * y3)) / det;
        _polynomialCoeffs[1] = (x1_2 * (y2 - y3) - y1 * (x2_2 - x3_2) + (x2_2 * y3 - y2 * x3_2)) / det;
        _polynomialCoeffs[0] = (x1_2 * (x2 * y3 - y2 * x3) - x1 * (x2_2 * y3 - y2 * x3_2) + y1 * (x2_2 * x3 - x2 * x3_2)) / det;

        return true;
    }

    return false;
}

bool AnalogSensorCalibrator::sortCalibrationPoints() {
    if (_numCalibPoints <= 1 || _calibPoints == NULL) {
        return false;
    }

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
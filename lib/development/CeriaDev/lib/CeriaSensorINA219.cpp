#include "CeriaSensorINA219.h"

namespace CeriaDevOP {

    CeriaSensorINA219::CeriaSensorINA219(uint8_t addr) : Adafruit_INA219(addr) {
        _hasNewData = false;
        _lastUpdateTime = 0;
        _calibrationMode = INA219_CAL_32V_2A;
        _i2cAddress = addr;
        _allowNegativeReadings = true;
        _defaultVoltageUnit = INA219_UNIT_BASE;
        _defaultCurrentUnit = INA219_UNIT_MILLI;
        _defaultPowerUnit = INA219_UNIT_MILLI;
        _errorCount = 0;
        _minUpdateInterval = 50;
    }

    bool CeriaSensorINA219::begin(TwoWire *wire, INA219CalibrationMode mode) {
        if (!Adafruit_INA219::begin(wire)) {
            return false;
        }
        setCalibrationMode(mode);
        _lastUpdateTime = millis();
        return true;
    }

    void CeriaSensorINA219::update() {
        uint32_t currentTime = millis();
        if (currentTime - _lastUpdateTime < _minUpdateInterval) {
            return;
        }
        
        if (!checkI2CConnection()) {
            _errorCount++;
            if (_errorCount > 5) {
                resetSensor();
                _errorCount = 0;
            }
            return;
        }
        
        float voltage = getBusVoltage_V();
        float current = getCurrent_mA();
        float power = getPower_mW();
        
        if (voltage >= 0 && voltage <= 32.0 && success()) {
            _hasNewData = true;
            _lastUpdateTime = currentTime;
            _errorCount = 0;
        } else {
            _errorCount++;
        }
    }

    bool CeriaSensorINA219::hasNewData() {
        bool result = _hasNewData;
        _hasNewData = false;
        return result;
    }

    bool CeriaSensorINA219::isConnected(uint32_t timeoutMs) {
        return (millis() - _lastUpdateTime) < timeoutMs && success();
    }

    float CeriaSensorINA219::getVoltage(INA219Unit unit) {
        float voltage = getBusVoltage_V();
        return _convertValue(voltage, unit);
    }

    float CeriaSensorINA219::getCurrent(INA219Unit unit) {
        float current = getCurrent_mA() / 1000.0;  // Convert to base unit (A)
        if (!_allowNegativeReadings && current < 0) {
            current = 0;
        }
        return _convertValue(current, unit);
    }

    float CeriaSensorINA219::getCurrentRaw(INA219Unit unit) {
        float current = getCurrent_mA() / 1000.0;  // Convert to base unit (A)
        return _convertValue(current, unit);
    }

    float CeriaSensorINA219::getPower(INA219Unit unit) {
        float power = getPower_mW() / 1000.0;  // Convert to base unit (W)
        return _convertValue(power, unit);
    }

    void CeriaSensorINA219::setCalibrationMode(INA219CalibrationMode mode) {
        _calibrationMode = mode;

        switch (mode) {
            case INA219_CAL_32V_2A:
                setCalibration_32V_2A();
                break;
            case INA219_CAL_32V_1A:
                setCalibration_32V_1A();
                break;
            case INA219_CAL_16V_400MA:
                setCalibration_16V_400mA();
                break;
        }
    }

    void CeriaSensorINA219::setPowerSave(bool enabled) {
        powerSave(enabled);
    }

    void CeriaSensorINA219::setAllowNegativeReadings(bool allow) {
        _allowNegativeReadings = allow;
    }

    void CeriaSensorINA219::setDefaultUnits(INA219Unit voltageUnit, INA219Unit currentUnit, INA219Unit powerUnit) {
        _defaultVoltageUnit = voltageUnit;
        _defaultCurrentUnit = currentUnit;
        _defaultPowerUnit = powerUnit;
    }

    float CeriaSensorINA219::_convertValue(float value, INA219Unit unit) {
        switch (unit) {
            case INA219_UNIT_BASE:
                return value;
            case INA219_UNIT_MILLI:
                return value * 1000.0;
            case INA219_UNIT_MICRO:
                return value * 1000000.0;
            default:
                return value;
        }
    }

    String CeriaSensorINA219::getUnitSymbol(INA219Unit unit, bool isVoltage, bool isPower) {
        String baseSymbol;
        if (isPower) {
            baseSymbol = "W";
        } else if (isVoltage) {
            baseSymbol = "V";
        } else {
            baseSymbol = "A";
        }
        
        switch (unit) {
            case INA219_UNIT_BASE:
                return baseSymbol;
            case INA219_UNIT_MILLI:
                return "m" + baseSymbol;
            case INA219_UNIT_MICRO:
                return "µ" + baseSymbol;
            default:
                return baseSymbol;
        }
    }

    String CeriaSensorINA219::getPowerString(INA219Unit voltageUnit, INA219Unit currentUnit, INA219Unit powerUnit) {
        if (!success()) return "INA219 Error";

        float voltage = getVoltage(voltageUnit);
        float current = getCurrent(currentUnit);
        float power = getPower(powerUnit);
        
        String voltageStr = String(voltage, (voltageUnit == INA219_UNIT_MICRO) ? 0 : 2) + getUnitSymbol(voltageUnit, true, false);
        
        String currentStr = String(abs(current), (currentUnit == INA219_UNIT_MICRO) ? 0 : 1) + getUnitSymbol(currentUnit, false, false);
        if (_allowNegativeReadings && current < 0) {
            currentStr = "-" + currentStr;
        }
        
        String powerStr = String(power, (powerUnit == INA219_UNIT_MICRO) ? 0 : 1) + getUnitSymbol(powerUnit, false, true);
        
        return voltageStr + ", " + currentStr + ", " + powerStr;
    }

    String CeriaSensorINA219::getCurrentDirection() {
        if (!success()) return "Error";
        
        float current = getCurrentRaw(INA219_UNIT_MILLI);
        if (current > 0.5) {
            return "Discharging";
        } else if (current < -0.5) {
            return "Charging";
        } else {
            return "Standby";
        }
    }

    bool CeriaSensorINA219::waitForReading(uint32_t timeoutMs) {
        uint32_t startTime = millis();
        while (millis() - startTime < timeoutMs) {
            update();
            if (hasNewData() && success()) {
                return true;
            }
            delay(10);
        }
        return false;
    }

    bool CeriaSensorINA219::resetSensor() {
        Wire.beginTransmission(_i2cAddress);
        Wire.write(0x00);
        Wire.write(0x80);
        Wire.write(0x00);
        uint8_t result = Wire.endTransmission();
        
        if (result == 0) {
            delay(10);
            setCalibrationMode(_calibrationMode);
            return true;
        }
        return false;
    }

    bool CeriaSensorINA219::checkI2CConnection() {
        Wire.beginTransmission(_i2cAddress);
        uint8_t result = Wire.endTransmission();
        return (result == 0);
    }

    uint32_t CeriaSensorINA219::getErrorCount() {
        return _errorCount;
    }

}
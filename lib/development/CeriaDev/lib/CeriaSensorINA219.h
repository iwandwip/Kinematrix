#ifndef CERIA_SENSOR_INA219_H
#define CERIA_SENSOR_INA219_H

#include "Arduino.h"
#include "Adafruit_INA219.h"
#include <Wire.h>

namespace CeriaDevOP {

    enum INA219CalibrationMode {
        INA219_CAL_32V_2A = 0,
        INA219_CAL_32V_1A = 1,
        INA219_CAL_16V_400MA = 2
    };

    enum INA219Unit {
        INA219_UNIT_BASE = 0,     // V, A, W
        INA219_UNIT_MILLI = 1,    // mV, mA, mW
        INA219_UNIT_MICRO = 2     // µV, µA, µW
    };

    class CeriaSensorINA219 : public Adafruit_INA219 {
    public:
        CeriaSensorINA219(uint8_t addr = 0x40);

        bool begin(TwoWire *wire = &Wire, INA219CalibrationMode mode = INA219_CAL_32V_2A);
        void update();

        bool hasNewData();
        bool isConnected(uint32_t timeoutMs = 5000);

        float getVoltage(INA219Unit unit = INA219_UNIT_BASE);
        float getCurrent(INA219Unit unit = INA219_UNIT_MILLI);
        float getCurrentRaw(INA219Unit unit = INA219_UNIT_MILLI);
        float getPower(INA219Unit unit = INA219_UNIT_MILLI);

        void setCalibrationMode(INA219CalibrationMode mode);
        void setPowerSave(bool enabled);
        void setAllowNegativeReadings(bool allow);
        void setDefaultUnits(INA219Unit voltageUnit, INA219Unit currentUnit, INA219Unit powerUnit);

        String getPowerString(INA219Unit voltageUnit = INA219_UNIT_BASE, 
                            INA219Unit currentUnit = INA219_UNIT_MILLI, 
                            INA219Unit powerUnit = INA219_UNIT_MILLI);
        String getCurrentDirection();
        String getUnitSymbol(INA219Unit unit, bool isVoltage = false, bool isPower = false);
        
        bool resetSensor();
        bool checkI2CConnection();
        uint32_t getErrorCount();

        bool waitForReading(uint32_t timeoutMs = 5000);

    private:
        bool _hasNewData;
        uint32_t _lastUpdateTime;
        INA219CalibrationMode _calibrationMode;
        uint8_t _i2cAddress;
        bool _allowNegativeReadings;
        INA219Unit _defaultVoltageUnit;
        INA219Unit _defaultCurrentUnit;
        INA219Unit _defaultPowerUnit;
        uint32_t _errorCount;
        uint32_t _minUpdateInterval;
        
        float _convertValue(float value, INA219Unit unit);
    };

}

#endif
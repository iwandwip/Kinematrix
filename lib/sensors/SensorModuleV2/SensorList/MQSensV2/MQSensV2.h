#pragma once

#ifndef MQ_SENS_V2_H
#define MQ_SENS_V2_H

#pragma message("[COMPILED]: MQSensV2.h")

#include "Arduino.h"
#include "../../SensorModule/SensorModuleV2.h"
#include "MQUnifiedsensor.h"

class MQSensV2 : public BaseSensV2, public MQUnifiedsensor {
private:
    uint32_t _updateTimer;
    uint32_t _updateInterval;
    bool _lastUpdateStatus;
    bool _enableCalibration;
    bool _enableMultipleGases;
    bool _enableRatioMode;
    float _correctionFactor;
    
    struct GasProfile {
        char* name;
        float a;
        float b;
        int regressionMethod;
        char* unit;
        uint8_t precision;
    };
    
    GasProfile* _gasProfiles;
    uint8_t _gasCount;
    uint8_t _gasCapacity;
    int8_t _activeGasIndex;
    
    void initGasProfiles();
    void addGasProfile(const char* name, float a, float b, int regressionMethod, const char* unit, uint8_t precision);
    void registerGasValues();
    
public:
    MQSensV2(String board, float voltageResolution, int adcBitResolution, int pin, String mqType);
    MQSensV2(String board, String mqType);
    virtual ~MQSensV2();

    bool init() override;
    bool update() override;
    bool isUpdated() const override;
    
    void setUpdateInterval(uint32_t interval);
    void setCorrectionFactor(float factor);
    void enableCalibration(bool enable);
    void enableMultipleGases(bool enable);
    void enableRatioMode(bool enable);
    
    void addCustomGas(const char* name, float a, float b, int regressionMethod, const char* unit = "ppm", uint8_t precision = 2);
    void setActiveGas(const char* gasName);
    void setActiveGas(int gasIndex);
    
    float calibrateInCleanAir(float ratioInCleanAir = 9.83);
    float readGasConcentration(const char* gasName = nullptr);
    float readRatio();
    float getResistance();
    
    void loadPresetGas(const char* mqType);
    void printSensorInfo();
    
    int getGasCount() const;
    const char* getGasName(int index) const;
    const char* getActiveGasName() const;
    bool isCalibrated() const;
    
protected:
    float getR0Internal() const { return ((MQUnifiedsensor*)this)->getR0(); }
};

#endif
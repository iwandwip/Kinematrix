#pragma once

#ifndef GP2Y_DUST_SENS_V2_H
#define GP2Y_DUST_SENS_V2_H

#pragma message("[COMPILED]: GP2YDustSensV2.h")

#include "Arduino.h"
#include "../../SensorModule/SensorModuleV2.h"
#include "GP2YDustSensor.h"

class GP2YDustSensV2 : public BaseSensV2, public GP2YDustSensor {
private:
    bool _enableFiltering;
    float _filterAlpha;
    float _filteredDensity;
    bool _enableCalibration;
    float _calibrationMin;
    float _calibrationMax;
    float _outputMin;
    float _outputMax;
    bool _enableAlert;
    float _alertThreshold;
    bool _autoBaseline;
    uint32_t _baselineReadings;
    uint32_t _baselineInterval;
    uint32_t _lastBaselineUpdate;
    uint32_t _updateTimer;
    uint32_t _updateInterval;
    bool _lastUpdateStatus;
    
    void updateFilteredValue(float newValue);
    void updateBaseline();
    
public:
    GP2YDustSensV2(GP2YDustSensorType type, uint8_t ledPin, uint8_t analogPin, uint16_t avgCount = 60);
    virtual ~GP2YDustSensV2();

    bool init() override;
    bool update() override;
    bool isUpdated() const override;
    
    void setUpdateInterval(uint32_t interval);
    void enableFiltering(bool enable, float alpha = 0.1);
    void enableCalibration(bool enable, float minDensity = 0, float maxDensity = 500, float minOutput = 0, float maxOutput = 100);
    void enableAlert(bool enable, float threshold = 200);
    void enableAutoBaseline(bool enable, uint32_t interval = 300000);
    
    float getDustDensityMgM3();
    float getFilteredDensity();
    float getCalibratedValue();
    float getAirQualityIndex();
    String getAirQualityLevel();
    
    void calibrateBaseline();
    void resetCalibration();
    void resetFilter();
    
    bool isAlertTriggered();
    bool isAirQualityGood();
    float getEfficiency();
};

#endif
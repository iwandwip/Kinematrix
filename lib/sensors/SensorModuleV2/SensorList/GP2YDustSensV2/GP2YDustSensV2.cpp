#include "GP2YDustSensV2.h"

GP2YDustSensV2::GP2YDustSensV2(GP2YDustSensorType type, uint8_t ledPin, uint8_t analogPin, uint16_t avgCount)
    : GP2YDustSensor(type, ledPin, analogPin, avgCount),
      _enableFiltering(false),
      _filterAlpha(0.1),
      _filteredDensity(0.0),
      _enableCalibration(false),
      _calibrationMin(0),
      _calibrationMax(500),
      _outputMin(0),
      _outputMax(100),
      _enableAlert(false),
      _alertThreshold(200),
      _autoBaseline(false),
      _baselineReadings(0),
      _baselineInterval(300000),
      _lastBaselineUpdate(0),
      _updateTimer(0),
      _updateInterval(2000) {
    
    addValueInfo("dustDensity", "Dust Density", "ug/m3", 2, true);
    addValueInfo("dustDensityMgM3", "Dust Density", "mg/m3", 3, false);
    addValueInfo("airQualityIndex", "Air Quality Index", "", 1, false);
    addValueInfo("baseline", "Baseline Voltage", "V", 3, false);
    addValueInfo("runningAverage", "Running Average", "ug/m3", 2, false);
    
    if (_enableFiltering) {
        addValueInfo("filteredDensity", "Filtered Density", "ug/m3", 2, false);
    }
    
    if (_enableCalibration) {
        addValueInfo("calibratedValue", "Calibrated Value", "%", 1, false);
    }
}

GP2YDustSensV2::~GP2YDustSensV2() = default;

bool GP2YDustSensV2::init() {
    GP2YDustSensor::begin();
    
    if (_autoBaseline) {
        _lastBaselineUpdate = millis();
    }
    
    return true;
}

bool GP2YDustSensV2::update() {
    if (millis() - _updateTimer >= _updateInterval) {
        uint16_t rawDensity = getDustDensity();
        float density = (float)rawDensity;
        
        updateValue("dustDensity", density);
        updateValue("dustDensityMgM3", density / 1000.0);
        updateValue("airQualityIndex", getAirQualityIndex());
        updateValue("baseline", getBaseline());
        updateValue("runningAverage", (float)getRunningAverage());
        
        if (_enableFiltering) {
            updateFilteredValue(density);
            updateValue("filteredDensity", _filteredDensity);
        }
        
        if (_enableCalibration) {
            updateValue("calibratedValue", getCalibratedValue());
        }
        
        if (_autoBaseline) {
            updateBaseline();
        }
        
        _updateTimer = millis();
        return true;
    }
    return false;
}

void GP2YDustSensV2::updateFilteredValue(float newValue) {
    if (_baselineReadings == 0) {
        _filteredDensity = newValue;
    } else {
        _filteredDensity = (_filterAlpha * newValue) + ((1.0 - _filterAlpha) * _filteredDensity);
    }
    _baselineReadings++;
}

void GP2YDustSensV2::updateBaseline() {
    unsigned long currentTime = millis();
    if ((currentTime - _lastBaselineUpdate) >= _baselineInterval) {
        float baseline = getBaseline();
        if (baseline > 0) {
            updateValue("baseline", baseline);
        }
        _lastBaselineUpdate = currentTime;
    }
}

void GP2YDustSensV2::enableFiltering(bool enable, float alpha) {
    _enableFiltering = enable;
    _filterAlpha = constrain(alpha, 0.01, 1.0);
    
    if (enable && !hasValue("filteredDensity")) {
        addValueInfo("filteredDensity", "Filtered Density", "ug/m3", 2, false);
    }
}

void GP2YDustSensV2::enableCalibration(bool enable, float minDensity, float maxDensity, float minOutput, float maxOutput) {
    _enableCalibration = enable;
    _calibrationMin = minDensity;
    _calibrationMax = maxDensity;
    _outputMin = minOutput;
    _outputMax = maxOutput;
    
    if (enable && !hasValue("calibratedValue")) {
        addValueInfo("calibratedValue", "Calibrated Value", "%", 1, false);
    }
}

void GP2YDustSensV2::enableAlert(bool enable, float threshold) {
    _enableAlert = enable;
    _alertThreshold = threshold;
}

void GP2YDustSensV2::enableAutoBaseline(bool enable, uint32_t interval) {
    _autoBaseline = enable;
    _baselineInterval = interval;
    if (enable) {
        _lastBaselineUpdate = millis();
    }
}

float GP2YDustSensV2::getDustDensityMgM3() {
    return getDustDensity() / 1000.0;
}

float GP2YDustSensV2::getFilteredDensity() {
    return _filteredDensity;
}

float GP2YDustSensV2::getCalibratedValue() {
    if (!_enableCalibration) return 0.0;
    
    float density = _enableFiltering ? _filteredDensity : (float)getDustDensity();
    float calibrated = ((density - _calibrationMin) / (_calibrationMax - _calibrationMin)) * (_outputMax - _outputMin) + _outputMin;
    return constrain(calibrated, _outputMin, _outputMax);
}

float GP2YDustSensV2::getAirQualityIndex() {
    float density = getDustDensity();
    
    if (density <= 35) return 1;
    else if (density <= 75) return 2;
    else if (density <= 115) return 3;
    else if (density <= 150) return 4;
    else if (density <= 250) return 5;
    else return 6;
}

String GP2YDustSensV2::getAirQualityLevel() {
    float aqi = getAirQualityIndex();
    
    switch ((int)aqi) {
        case 1: return "Good";
        case 2: return "Moderate";
        case 3: return "Unhealthy for Sensitive";
        case 4: return "Unhealthy";
        case 5: return "Very Unhealthy";
        case 6: return "Hazardous";
        default: return "Unknown";
    }
}

void GP2YDustSensV2::calibrateBaseline() {
    float baseline = getBaseline();
    if (baseline > 0) {
        updateValue("baseline", baseline);
    }
}

void GP2YDustSensV2::resetCalibration() {
    _calibrationMin = 0;
    _calibrationMax = 500;
    _outputMin = 0;
    _outputMax = 100;
}

void GP2YDustSensV2::resetFilter() {
    _filteredDensity = 0.0;
    _baselineReadings = 0;
}

bool GP2YDustSensV2::isAlertTriggered() {
    if (!_enableAlert) return false;
    
    float density = _enableFiltering ? _filteredDensity : (float)getDustDensity();
    return density > _alertThreshold;
}

bool GP2YDustSensV2::isAirQualityGood() {
    return getAirQualityIndex() <= 2;
}

bool GP2YDustSensV2::isUpdated() const {
    return _lastUpdateStatus;
}

void GP2YDustSensV2::setUpdateInterval(uint32_t interval) {
    _updateInterval = interval;
}

float GP2YDustSensV2::getEfficiency() {
    float density = getDustDensity();
    float baseline = getBaseline() * 1000;
    
    if (baseline <= 0) return 0.0;
    
    float efficiency = ((baseline - density) / baseline) * 100.0;
    return constrain(efficiency, 0.0, 100.0);
}
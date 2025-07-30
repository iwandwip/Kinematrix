#include "CustomSensorTemplateV2.h"
#include "Arduino.h"

CustomSensorTemplateV2::CustomSensorTemplateV2(uint8_t pin)
        : _sensorPin(pin),
          _updateTimer(0),
          _updateInterval(1000),
          _lastUpdateStatus(false),
          _calibrationFactor(1.0),
          _sensorMode(0) {
    addValueInfo("rawValue", "Raw Value", "", 0, true);
    addValueInfo("value", "Processed Value", "units", 2, true);
}

CustomSensorTemplateV2::~CustomSensorTemplateV2() = default;

bool CustomSensorTemplateV2::init() {
    if (_sensorPin >= NUM_DIGITAL_PINS) {
        return false;
    }
    pinMode(_sensorPin, INPUT);

    updateValue("rawValue", 0);
    updateValue("value", 0.0f);

    return true;
}

bool CustomSensorTemplateV2::update() {
    unsigned long currentTime = millis();
    if ((currentTime - _updateTimer) >= _updateInterval) {
        int rawValue = analogRead(_sensorPin);

        float processedValue = rawValue * _calibrationFactor;

        if (_sensorMode == 1) {
            if (processedValue >= 0) {
                processedValue = sqrt(processedValue);
            } else {
                processedValue = 0;
            }
        }

        updateValue("rawValue", rawValue);
        updateValue("value", processedValue);

        _updateTimer = currentTime;
        _lastUpdateStatus = true;
        return true;
    }
    _lastUpdateStatus = false;
    return false;
}

void CustomSensorTemplateV2::setUpdateInterval(uint32_t interval) {
    _updateInterval = interval;
}

void CustomSensorTemplateV2::setCalibrationFactor(float factor) {
    _calibrationFactor = factor;
}

void CustomSensorTemplateV2::setSensorMode(int mode) {
    _sensorMode = mode;
}

bool CustomSensorTemplateV2::isUpdated() const {
    return _lastUpdateStatus;
}

void CustomSensorTemplateV2::addCustomSensorValue(const char *key, const char *label, const char *unit, uint8_t precision, bool calibrable) {
    addValueInfo(key, label, unit, precision, calibrable);
}
#include "CustomSensorTemplateV2.h"
#include "Arduino.h"

CustomSensorTemplateV2::CustomSensorTemplateV2(uint8_t pin)
        : _sensorPin(pin),
          _updateTimer(0),
          _updateInterval(1000),
          _calibrationFactor(1.0),
          _sensorMode(0) {
    addValueInfo("rawValue", "Raw Value", "", 0, TYPE_INT);
    addValueInfo("value", "Processed Value", "units", 2, TYPE_FLOAT);
}

CustomSensorTemplateV2::~CustomSensorTemplateV2() = default;

bool CustomSensorTemplateV2::init() {
    pinMode(_sensorPin, INPUT);

    updateValue("rawValue", 0);
    updateValue("value", 0.0f);

    return true;
}

bool CustomSensorTemplateV2::update() {
    if (millis() - _updateTimer >= _updateInterval) {
        int rawValue = analogRead(_sensorPin);

        float processedValue = rawValue * _calibrationFactor;

        if (_sensorMode == 1) {
            processedValue = sqrt(processedValue);
        }

        updateValue("rawValue", rawValue);
        updateValue("value", processedValue);

        _updateTimer = millis();
        return true;
    }
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

void CustomSensorTemplateV2::addCustomSensorValue(const char *key, const char *label, const char *unit, uint8_t precision, SensorValueType type) {
    addValueInfo(key, label, unit, precision, type);
}
#include "AnalogSensV2.h"
#include "Arduino.h"

AnalogSensV2::AnalogSensV2(uint8_t pin, float vref, int adcRange, CustomDataCallback onCustomData)
        : _voltageReference(vref),
          _adcRange(adcRange),
          _sensorPin(pin),
          _updateTimer(0),
          _updateInterval(100),
          _onCustomData(onCustomData) {

    addValueInfo("raw", "Raw Value", "", 0, TYPE_INT);
    addValueInfo("volt", "Voltage", "V", 3, TYPE_FLOAT);
}

AnalogSensV2::~AnalogSensV2() = default;

bool AnalogSensV2::init() {
    pinMode(_sensorPin, INPUT);
    updateValue("raw", 0);
    updateValue("volt", 0.0f);
    return true;
}

bool AnalogSensV2::update() {
    if (millis() - _updateTimer >= _updateInterval) {
        int analogValue = analogRead(_sensorPin);
        float voltageValue = analogValue * (_voltageReference / _adcRange);

        updateValue("raw", analogValue);
        updateValue("volt", voltageValue);

        if (_onCustomData != nullptr) {
            _onCustomData(this, analogValue, voltageValue);
        }

        _updateTimer = millis();
        return true;
    }
    return false;
}

void AnalogSensV2::setUpdateInterval(uint32_t interval) {
    _updateInterval = interval;
}

void AnalogSensV2::setPins(uint8_t pin) {
    _sensorPin = pin;
}

void AnalogSensV2::setCustomDataCallback(CustomDataCallback callback) {
    _onCustomData = callback;
}

void AnalogSensV2::addCustomValue(const char *key, const char *label, const char *unit, uint8_t precision, SensorValueType type) {
    addValueInfo(key, label, unit, precision, type);
}
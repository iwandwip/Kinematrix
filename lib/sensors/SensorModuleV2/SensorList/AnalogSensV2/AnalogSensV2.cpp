#include "AnalogSensV2.h"
#include "Arduino.h"

AnalogSensV2::AnalogSensV2(uint8_t pin, float vref, int adcRange, CustomDataCallback onCustomData)
        : _voltageReference(vref),
          _adcRange(adcRange),
          _sensorPin(pin),
          _updateTimer(0),
          _updateInterval(100),
          _lastUpdateStatus(false),
          _onCustomData(onCustomData) {
    addValueInfo("raw", "Raw Value", "", 0, false);
    addValueInfo("volt", "Voltage", "V", 3, true);
}

AnalogSensV2::~AnalogSensV2() = default;

bool AnalogSensV2::init() {
    if (_sensorPin >= NUM_DIGITAL_PINS) {
        return false;
    }
    pinMode(_sensorPin, INPUT);
    updateValue("raw", 0);
    updateValue("volt", 0.0f);
    return true;
}

bool AnalogSensV2::update() {
    unsigned long currentTime = millis();
    if ((currentTime - _updateTimer) >= _updateInterval) {
        int analogValue = analogRead(_sensorPin);
        float voltageValue = analogValue * (_voltageReference / _adcRange);

        updateValue("raw", analogValue);
        updateValue("volt", voltageValue);

        if (_onCustomData != nullptr) {
            _onCustomData(this, analogValue, voltageValue);
        }

        _updateTimer = currentTime;
        _lastUpdateStatus = true;
        return true;
    }
    _lastUpdateStatus = false;
    return false;
}

bool AnalogSensV2::isUpdated() const {
    return _lastUpdateStatus;
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

void AnalogSensV2::addCustomValue(const char *key, const char *label, const char *unit, uint8_t precision, bool calibrable) {
    addValueInfo(key, label, unit, precision, calibrable);
}
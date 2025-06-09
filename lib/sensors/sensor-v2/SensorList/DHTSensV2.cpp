#include "DHTSensV2.h"
#include "Arduino.h"

DHTSensV2::DHTSensV2(uint8_t pin, uint8_t type)
        : DHT(pin, type),
          _sensorPin(pin),
          _updateTimer(0),
          _updateInterval(2000) {
    addValueInfo("temp", "Temperature", "°C", 1, true);
    addValueInfo("hum", "Humidity", "%", 0, true);
}

DHTSensV2::~DHTSensV2() = default;

bool DHTSensV2::init() {
    DHT::begin();
    updateValue("temp", 0.0f);
    updateValue("hum", 0.0f);
    return true;
}

bool DHTSensV2::update() {
    if (millis() - _updateTimer >= _updateInterval) {
        float temp = DHT::readTemperature();
        float hum = DHT::readHumidity();

        if (isnan(temp) || isnan(hum)) {
            _updateTimer = millis();
            return false;
        }

        updateValue("temp", temp);
        updateValue("hum", hum);
        _updateTimer = millis();
        return true;
    }
    return false;
}

void DHTSensV2::setUpdateInterval(uint32_t interval) {
    _updateInterval = interval;
}

void DHTSensV2::setPins(uint8_t pin) {
    _sensorPin = pin;
}
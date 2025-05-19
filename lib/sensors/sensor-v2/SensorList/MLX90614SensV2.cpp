#include "MLX90614SensV2.h"
#include "Arduino.h"

MLX90614SensV2::MLX90614SensV2()
        : _updateTimer(0),
          _updateInterval(2000) {
    addValueInfo("temp", "Temperature", "°C", 1, TYPE_FLOAT);
}

MLX90614SensV2::~MLX90614SensV2() = default;

bool MLX90614SensV2::init() {
    if (!Adafruit_MLX90614::begin()) return false;
    Adafruit_MLX90614::readEmissivity();
    updateValue("temp", 0.0f);
    return true;
}

bool MLX90614SensV2::update() {
    if (millis() - _updateTimer >= _updateInterval) {
        float temp = Adafruit_MLX90614::readObjectTempC();
        updateValue("temp", temp);
        _updateTimer = millis();
        return true;
    }
    return false;
}

void MLX90614SensV2::setUpdateInterval(uint32_t interval) {
    _updateInterval = interval;
}
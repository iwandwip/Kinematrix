#include "MLX90614SensV2.h"
#include "Arduino.h"

MLX90614SensV2::MLX90614SensV2()
        : _updateTimer(0),
          _updateInterval(2000),
          _lastUpdateStatus(false) {
    addValueInfo("temp", "Temperature", "°C", 1, true);
}

MLX90614SensV2::~MLX90614SensV2() = default;

bool MLX90614SensV2::init() {
    if (!Adafruit_MLX90614::begin()) return false;
    Adafruit_MLX90614::readEmissivity();
    updateValue("temp", 0.0f);
    return true;
}

bool MLX90614SensV2::update() {
    unsigned long currentTime = millis();
    if ((currentTime - _updateTimer) >= _updateInterval) {
        float temp = Adafruit_MLX90614::readObjectTempC();
        updateValue("temp", temp);
        _updateTimer = currentTime;
        _lastUpdateStatus = true;
        return true;
    }
    _lastUpdateStatus = false;
    return false;
}

bool MLX90614SensV2::isUpdated() const {
    return _lastUpdateStatus;
}

void MLX90614SensV2::setUpdateInterval(uint32_t interval) {
    _updateInterval = interval;
}
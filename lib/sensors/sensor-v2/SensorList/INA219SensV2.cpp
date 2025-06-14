#include "INA219SensV2.h"
#include "Arduino.h"

INA219SensV2::INA219SensV2(uint8_t addr)
        : Adafruit_INA219(addr),
          _updateTimer(0),
          _updateInterval(2000) {
    addValueInfo("voltage", "Voltage", "V", 2, true);
    addValueInfo("current", "Current", "A", 2, true);
    addValueInfo("power", "Power", "W", 2, true);
}

INA219SensV2::~INA219SensV2() = default;

bool INA219SensV2::init() {
    if (!Adafruit_INA219::begin()) return false;
    updateValue("voltage", 0.0f);
    updateValue("current", 0.0f);
    updateValue("power", 0.0f);
    return true;
}

bool INA219SensV2::update() {
    if (millis() - _updateTimer >= _updateInterval) {
        updateValue("voltage", Adafruit_INA219::getBusVoltage_V());
        updateValue("current", Adafruit_INA219::getCurrent_mA() / 1000);
        updateValue("power", Adafruit_INA219::getPower_mW() / 1000);
        _updateTimer = millis();
        return true;
    }
    return false;
}

void INA219SensV2::setUpdateInterval(uint32_t interval) {
    _updateInterval = interval;
}
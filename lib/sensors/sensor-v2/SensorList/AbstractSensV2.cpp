#include "AbstractSensV2.h"
#include "Arduino.h"

AbstractSensV2::AbstractSensV2(int enumRandomMode)
        : _enumRandomMode(enumRandomMode),
          _dummyValue1(0),
          _dummyValue2(0),
          _updateTimer(0),
          _updateInterval(500) {

    addValueInfo("value", "Value", "", 1, true);
}

AbstractSensV2::AbstractSensV2(int enumRandomMode, float dummyValue1, float dummyValue2)
        : _enumRandomMode(enumRandomMode),
          _dummyValue1(dummyValue1),
          _dummyValue2(dummyValue2),
          _updateTimer(0),
          _updateInterval(500) {

    addValueInfo("value", "Value", "", 1, true);
}

AbstractSensV2::~AbstractSensV2() = default;

bool AbstractSensV2::init() {
    updateValue("value", 0.0f);
    return true;
}

bool AbstractSensV2::update() {
    if (millis() - _updateTimer >= _updateInterval) {
        float newValue = 0.0f;

        switch (_enumRandomMode) {
            case 0:
                newValue = float(random(1 * 10, 1000 * 10)) * 0.1;
                break;
            case 1:
                newValue = _dummyValue1;
                break;
            case 2:
                newValue = float(random(0 * 10, (long) _dummyValue1 * 10)) * 0.1;
                break;
            case 3:
                newValue = float(random((long) _dummyValue1 * 10, (long) _dummyValue2 * 10)) * 0.1;
                break;
        }

        updateValue("value", newValue);
        _updateTimer = millis();
        return true;
    }
    return false;
}

void AbstractSensV2::setUpdateInterval(uint32_t interval) {
    _updateInterval = interval;
}

void AbstractSensV2::setRandomMode(int enumRandomMode) {
    _enumRandomMode = enumRandomMode;
}

void AbstractSensV2::setDummyValues(float dummyValue1, float dummyValue2) {
    _dummyValue1 = dummyValue1;
    _dummyValue2 = dummyValue2;
}

void AbstractSensV2::addCustomValue(const char *key, const char *label, const char *unit, uint8_t precision, bool calibrable) {
    addValueInfo(key, label, unit, precision, calibrable);
}
#include "AbstractSensV2.h"
#include "Arduino.h"

AbstractSensV2::AbstractSensV2(int enumRandomMode)
        : _enumRandomMode(enumRandomMode),
          _dummyValue1(0),
          _dummyValue2(0),
          _updateTimer(0),
          _updateInterval(500),
          _lastUpdateStatus(false) {

    addValueInfo("value", "Value", "", 1, true);
    addValueInfo("random", "Random", "", 2, false);
    addValueInfo("sine", "Sine Wave", "", 3, false);
    addValueInfo("cosine", "Cosine Wave", "", 3, false);
    addValueInfo("square", "Square Wave", "", 0, false);
    addValueInfo("triangle", "Triangle Wave", "", 2, false);
    addValueInfo("counter", "Counter", "", 0, false);
}

AbstractSensV2::AbstractSensV2(int enumRandomMode, float dummyValue1, float dummyValue2)
        : _enumRandomMode(enumRandomMode),
          _dummyValue1(dummyValue1),
          _dummyValue2(dummyValue2),
          _updateTimer(0),
          _updateInterval(500),
          _lastUpdateStatus(false) {

    addValueInfo("value", "Value", "", 1, true);
    addValueInfo("random", "Random", "", 2, false);
    addValueInfo("sine", "Sine Wave", "", 3, false);
    addValueInfo("cosine", "Cosine Wave", "", 3, false);
    addValueInfo("square", "Square Wave", "", 0, false);
    addValueInfo("triangle", "Triangle Wave", "", 2, false);
    addValueInfo("counter", "Counter", "", 0, false);
}

AbstractSensV2::~AbstractSensV2() = default;

bool AbstractSensV2::init() {
    updateValue("value", 0.0f);
    return true;
}

bool AbstractSensV2::update() {
    unsigned long currentTime = millis();
    if ((currentTime - _updateTimer) >= _updateInterval) {
        static uint32_t counter = 0;
        float newValue = 0.0f;
        float timeSeconds = currentTime / 1000.0f;

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
        updateValue("random", float(random(0, 1000)) / 10.0f);
        updateValue("sine", sin(timeSeconds) * 50.0f + 50.0f);
        updateValue("cosine", cos(timeSeconds) * 50.0f + 50.0f);
        updateValue("square", (sin(timeSeconds) > 0) ? 100 : 0);
        updateValue("triangle", abs(fmod(timeSeconds, 2.0f) - 1.0f) * 100.0f);
        updateValue("counter", counter++);
        
        _updateTimer = currentTime;
        _lastUpdateStatus = true;
        return true;
    }
    _lastUpdateStatus = false;
    return false;
}

bool AbstractSensV2::isUpdated() const {
    return _lastUpdateStatus;
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
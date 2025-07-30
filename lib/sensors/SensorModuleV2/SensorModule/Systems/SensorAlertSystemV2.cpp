#include "SensorAlertSystemV2.h"
#include "../SensorModuleV2.h"

SensorAlertSystemV2::SensorAlertSystemV2()
        : _thresholds(nullptr), _thresholdCount(0), _thresholdCapacity(0),
          _globalCallback(nullptr), _sensorCallbacks(nullptr), _sensorCallbackNames(nullptr),
          _callbackCount(0), _callbackCapacity(0),
          _defaultHysteresis(5000), _defaultDebounceTime(0) {
}

SensorAlertSystemV2::~SensorAlertSystemV2() {
    removeAllThresholds();
    clearCallbacks();
}

bool SensorAlertSystemV2::setThreshold(const char *sensorName, const char *valueKey,
                                       float lowThreshold, float highThreshold, AlertType type) {
    if (!sensorName || !valueKey) {
        return false;
    }

    int existingIndex = findThresholdIndex(sensorName, valueKey);
    if (existingIndex >= 0) {
        AlertThreshold *threshold = _thresholds[existingIndex];
        threshold->lowThreshold = lowThreshold;
        threshold->highThreshold = highThreshold;
        threshold->type = type;
        threshold->state = ALERT_INACTIVE;
        return true;
    }

    AlertThreshold *newThreshold = new AlertThreshold();

    newThreshold->sensorName = strdup(sensorName);
    newThreshold->valueKey = strdup(valueKey);
    newThreshold->lowThreshold = lowThreshold;
    newThreshold->highThreshold = highThreshold;
    newThreshold->type = type;
    newThreshold->state = ALERT_INACTIVE;
    newThreshold->lastTriggeredTime = 0;
    newThreshold->repeatCount = 0;
    newThreshold->hysteresis = _defaultHysteresis;
    newThreshold->debounceTime = _defaultDebounceTime;
    newThreshold->conditionMetTime = 0;
    newThreshold->debouncing = false;

    if (_thresholdCount >= _thresholdCapacity) {
        uint16_t newCapacity = _thresholdCapacity + 4;
        AlertThreshold **newArray = (AlertThreshold **) realloc(_thresholds, newCapacity * sizeof(AlertThreshold *));

        if (!newArray) {
            delete newThreshold;
            return false;
        }

        _thresholds = newArray;
        _thresholdCapacity = newCapacity;
    }

    _thresholds[_thresholdCount++] = newThreshold;
    return true;
}

bool SensorAlertSystemV2::setThresholdParams(const char *sensorName, const char *valueKey,
                                             uint32_t hysteresis, uint32_t debounceTime) {
    int index = findThresholdIndex(sensorName, valueKey);
    if (index < 0) {
        return false;
    }

    _thresholds[index]->hysteresis = hysteresis;
    _thresholds[index]->debounceTime = debounceTime;
    return true;
}

bool SensorAlertSystemV2::removeThreshold(const char *sensorName, const char *valueKey) {
    int index = findThresholdIndex(sensorName, valueKey);
    if (index < 0) {
        return false;
    }

    cleanupThreshold(_thresholds[index]);

    for (uint16_t i = index; i < _thresholdCount - 1; i++) {
        _thresholds[i] = _thresholds[i + 1];
    }

    _thresholdCount--;
    return true;
}

void SensorAlertSystemV2::removeAllThresholds() {
    if (_thresholds) {
        for (uint16_t i = 0; i < _thresholdCount; i++) {
            cleanupThreshold(_thresholds[i]);
        }

        free(_thresholds);
        _thresholds = nullptr;
        _thresholdCount = 0;
        _thresholdCapacity = 0;
    }
}

AlertState SensorAlertSystemV2::getAlertState(const char *sensorName, const char *valueKey) {
    int index = findThresholdIndex(sensorName, valueKey);
    if (index < 0) {
        return ALERT_INACTIVE;
    }

    return _thresholds[index]->state;
}

bool SensorAlertSystemV2::isAlertActive(const char *sensorName, const char *valueKey) {
    return getAlertState(sensorName, valueKey) == ALERT_ACTIVE;
}

void SensorAlertSystemV2::acknowledgeAlert(const char *sensorName, const char *valueKey) {
    int index = findThresholdIndex(sensorName, valueKey);
    if (index >= 0 && _thresholds[index]->state == ALERT_ACTIVE) {
        _thresholds[index]->state = ALERT_ACKNOWLEDGED;
    }
}

void SensorAlertSystemV2::acknowledgeAllAlerts() {
    for (uint16_t i = 0; i < _thresholdCount; i++) {
        if (_thresholds[i]->state == ALERT_ACTIVE) {
            _thresholds[i]->state = ALERT_ACKNOWLEDGED;
        }
    }
}

void SensorAlertSystemV2::resetAlert(const char *sensorName, const char *valueKey) {
    int index = findThresholdIndex(sensorName, valueKey);
    if (index >= 0) {
        _thresholds[index]->state = ALERT_INACTIVE;
        _thresholds[index]->lastTriggeredTime = 0;
        _thresholds[index]->repeatCount = 0;
        _thresholds[index]->conditionMetTime = 0;
        _thresholds[index]->debouncing = false;
    }
}

void SensorAlertSystemV2::resetAllAlerts() {
    for (uint16_t i = 0; i < _thresholdCount; i++) {
        _thresholds[i]->state = ALERT_INACTIVE;
        _thresholds[i]->lastTriggeredTime = 0;
        _thresholds[i]->repeatCount = 0;
        _thresholds[i]->conditionMetTime = 0;
        _thresholds[i]->debouncing = false;
    }
}

void SensorAlertSystemV2::setGlobalAlertCallback(AlertCallback callback) {
    _globalCallback = callback;
}

bool SensorAlertSystemV2::setSensorAlertCallback(const char *sensorName, AlertCallback callback) {
    if (!sensorName) {
        return false;
    }

    int existingIndex = findCallbackIndex(sensorName);
    if (existingIndex >= 0) {
        _sensorCallbacks[existingIndex] = callback;
        return true;
    }

    if (_callbackCount >= _callbackCapacity) {
        uint16_t newCapacity = _callbackCapacity + 4;
        AlertCallback *newCallbacks = (AlertCallback *) realloc(_sensorCallbacks, newCapacity * sizeof(AlertCallback));
        char **newNames = (char **) realloc(_sensorCallbackNames, newCapacity * sizeof(char *));

        if (!newCallbacks || !newNames) {
            return false;
        }

        _sensorCallbacks = newCallbacks;
        _sensorCallbackNames = newNames;
        _callbackCapacity = newCapacity;
    }

    _sensorCallbacks[_callbackCount] = callback;
    _sensorCallbackNames[_callbackCount] = strdup(sensorName);
    _callbackCount++;

    return true;
}

void SensorAlertSystemV2::clearCallbacks() {
    _globalCallback = nullptr;

    if (_sensorCallbackNames) {
        for (uint16_t i = 0; i < _callbackCount; i++) {
            if (_sensorCallbackNames[i]) {
                free(_sensorCallbackNames[i]);
            }
        }

        free(_sensorCallbackNames);
        _sensorCallbackNames = nullptr;
    }

    if (_sensorCallbacks) {
        free(_sensorCallbacks);
        _sensorCallbacks = nullptr;
    }

    _callbackCount = 0;
    _callbackCapacity = 0;
}

void SensorAlertSystemV2::setDefaultHysteresis(uint32_t hysteresis) {
    _defaultHysteresis = hysteresis;
}

void SensorAlertSystemV2::setDefaultDebounceTime(uint32_t debounceTime) {
    _defaultDebounceTime = debounceTime;
}

void SensorAlertSystemV2::checkAlerts(SensorModuleV2 *module) {
    if (!module || !_thresholds || _thresholdCount == 0) {
        return;
    }

    for (uint16_t i = 0; i < _thresholdCount; i++) {
        AlertThreshold *threshold = _thresholds[i];
        BaseSensV2 *sensor = module->getSensorByName(threshold->sensorName);

        if (sensor) {
            float value = sensor->getFloatValue(threshold->valueKey);
            checkThresholdCondition(threshold, sensor, value);
        }
    }
}

void SensorAlertSystemV2::cleanupThreshold(AlertThreshold *threshold) {
    if (threshold) {
        if (threshold->sensorName) free(threshold->sensorName);
        if (threshold->valueKey) free(threshold->valueKey);
        delete threshold;
    }
}

int SensorAlertSystemV2::findThresholdIndex(const char *sensorName, const char *valueKey) {
    if (!_thresholds) {
        return -1;
    }

    for (uint16_t i = 0; i < _thresholdCount; i++) {
        if (strcmp(_thresholds[i]->sensorName, sensorName) == 0 &&
            strcmp(_thresholds[i]->valueKey, valueKey) == 0) {
            return i;
        }
    }

    return -1;
}

void SensorAlertSystemV2::checkThresholdCondition(AlertThreshold *threshold, BaseSensV2* /*sensor*/, float value) {
    bool conditionMet = false;

    switch (threshold->type) {
        case ALERT_ABOVE:
            conditionMet = value > threshold->highThreshold;
            break;

        case ALERT_BELOW:
            conditionMet = value < threshold->lowThreshold;
            break;

        case ALERT_BETWEEN:
            conditionMet = value >= threshold->lowThreshold && value <= threshold->highThreshold;
            break;

        case ALERT_OUTSIDE:
            conditionMet = value < threshold->lowThreshold || value > threshold->highThreshold;
            break;
    }

    uint32_t currentTime = millis();
    
    bool timeValid = true;
    if (threshold->lastTriggeredTime > 0 && currentTime < threshold->lastTriggeredTime) {
        timeValid = false;
    }
    if (threshold->conditionMetTime > 0 && currentTime < threshold->conditionMetTime) {
        timeValid = false;
    }

    if (threshold->debounceTime > 0) {
        if (conditionMet && !threshold->debouncing) {
            threshold->debouncing = true;
            threshold->conditionMetTime = currentTime;
        } else if (conditionMet && threshold->debouncing && timeValid) {
            uint32_t elapsed = currentTime >= threshold->conditionMetTime ? 
                currentTime - threshold->conditionMetTime : 
                (0xFFFFFFFF - threshold->conditionMetTime) + currentTime + 1;
            
            if (elapsed >= threshold->debounceTime) {
                uint32_t hysteresisElapsed = threshold->lastTriggeredTime == 0 ? 
                    threshold->hysteresis : 
                    (currentTime >= threshold->lastTriggeredTime ? 
                        currentTime - threshold->lastTriggeredTime : 
                        (0xFFFFFFFF - threshold->lastTriggeredTime) + currentTime + 1);
                
                if (threshold->state != ALERT_ACTIVE &&
                    (threshold->lastTriggeredTime == 0 || hysteresisElapsed >= threshold->hysteresis)) {
                    triggerAlert(threshold, value);
                }
            }
        } else if (!conditionMet) {
            threshold->debouncing = false;
        }
    } else {
        uint32_t hysteresisElapsed = threshold->lastTriggeredTime == 0 ? 
            threshold->hysteresis : 
            (timeValid && currentTime >= threshold->lastTriggeredTime ? 
                currentTime - threshold->lastTriggeredTime : 
                (0xFFFFFFFF - threshold->lastTriggeredTime) + currentTime + 1);
        
        if (conditionMet &&
            threshold->state != ALERT_ACTIVE &&
            (threshold->lastTriggeredTime == 0 || hysteresisElapsed >= threshold->hysteresis)) {
            triggerAlert(threshold, value);
        } else if (!conditionMet && threshold->state == ALERT_ACTIVE) {
            threshold->state = ALERT_INACTIVE;
        }
    }
}

void SensorAlertSystemV2::triggerAlert(AlertThreshold *threshold, float value) {
    threshold->state = ALERT_ACTIVE;
    threshold->lastTriggeredTime = millis();
    threshold->repeatCount++;

    AlertInfo info;
    info.sensorName = threshold->sensorName;
    info.valueKey = threshold->valueKey;
    info.currentValue = value;
    info.lowThreshold = threshold->lowThreshold;
    info.highThreshold = threshold->highThreshold;
    info.type = threshold->type;
    info.state = ALERT_ACTIVE;
    info.timeTriggered = threshold->lastTriggeredTime;
    info.repeatCount = threshold->repeatCount;

    callCallbacks(info);
}

void SensorAlertSystemV2::callCallbacks(AlertInfo info) {
    if (_globalCallback) {
        _globalCallback(info);
    }

    int index = findCallbackIndex(info.sensorName);
    if (index >= 0 && _sensorCallbacks[index]) {
        _sensorCallbacks[index](info);
    }
}

int SensorAlertSystemV2::findCallbackIndex(const char *sensorName) {
    if (!_sensorCallbackNames) {
        return -1;
    }

    for (uint16_t i = 0; i < _callbackCount; i++) {
        if (strcmp(_sensorCallbackNames[i], sensorName) == 0) {
            return i;
        }
    }

    return -1;
}
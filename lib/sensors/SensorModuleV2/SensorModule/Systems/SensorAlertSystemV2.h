#ifndef SENSOR_ALERT_SYSTEM_V2_H
#define SENSOR_ALERT_SYSTEM_V2_H

#include "Arduino.h"

enum AlertType {
    ALERT_ABOVE,
    ALERT_BELOW,
    ALERT_BETWEEN,
    ALERT_OUTSIDE
};

enum AlertState {
    ALERT_INACTIVE,
    ALERT_ACTIVE,
    ALERT_ACKNOWLEDGED
};

struct AlertInfo {
    char *sensorName;
    char *valueKey;
    float currentValue;
    float lowThreshold;
    float highThreshold;
    AlertType type;
    AlertState state;
    uint32_t timeTriggered;
    uint8_t repeatCount;
};

typedef void (*AlertCallback)(AlertInfo alertInfo);

struct AlertThreshold {
    char *sensorName;
    char *valueKey;
    float lowThreshold;
    float highThreshold;
    AlertType type;
    AlertState state;
    uint32_t lastTriggeredTime;
    uint32_t repeatCount;
    uint32_t hysteresis;
    uint32_t debounceTime;
    uint32_t conditionMetTime;
    bool debouncing;
};

class SensorModuleV2;

class BaseSensV2;

class SensorAlertSystemV2 {
private:
    AlertThreshold **_thresholds;
    uint16_t _thresholdCount;
    uint16_t _thresholdCapacity;

    AlertCallback _globalCallback;
    AlertCallback *_sensorCallbacks;
    char **_sensorCallbackNames;
    uint16_t _callbackCount;
    uint16_t _callbackCapacity;

    uint32_t _defaultHysteresis;
    uint32_t _defaultDebounceTime;

    void cleanupThreshold(AlertThreshold *threshold);
    int findThresholdIndex(const char *sensorName, const char *valueKey);
    void checkThresholdCondition(AlertThreshold *threshold, BaseSensV2 *sensor, float value);
    void triggerAlert(AlertThreshold *threshold, float value);
    void callCallbacks(AlertInfo info);
    int findCallbackIndex(const char *sensorName);

public:
    SensorAlertSystemV2();
    ~SensorAlertSystemV2();

    bool setThreshold(const char *sensorName, const char *valueKey,
                      float lowThreshold, float highThreshold, AlertType type = ALERT_OUTSIDE);

    bool setThresholdParams(const char *sensorName, const char *valueKey,
                            uint32_t hysteresis, uint32_t debounceTime);

    bool removeThreshold(const char *sensorName, const char *valueKey);
    void removeAllThresholds();

    AlertState getAlertState(const char *sensorName, const char *valueKey);
    bool isAlertActive(const char *sensorName, const char *valueKey);

    void acknowledgeAlert(const char *sensorName, const char *valueKey);
    void acknowledgeAllAlerts();

    void resetAlert(const char *sensorName, const char *valueKey);
    void resetAllAlerts();

    void setGlobalAlertCallback(AlertCallback callback);
    bool setSensorAlertCallback(const char *sensorName, AlertCallback callback);
    void clearCallbacks();

    void setDefaultHysteresis(uint32_t hysteresis);
    void setDefaultDebounceTime(uint32_t debounceTime);

    void checkAlerts(SensorModuleV2 *module);
};

#endif
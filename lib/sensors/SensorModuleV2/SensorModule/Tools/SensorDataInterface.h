#ifndef SENSOR_DATA_INTERFACE_H
#define SENSOR_DATA_INTERFACE_H

#include <Arduino.h>
#include "../SensorModuleV2.h"

class SensorDataInterface {
public:
    struct SensorContext {
        BaseSensV2 *sensor;
        const char *valueKey;
    };

    typedef float (*SensorReadCallback)();
    typedef float (*SensorReadCallbackWithContext)(void *context);

    SensorDataInterface();
    ~SensorDataInterface();

    void setSensorReadCallback(SensorReadCallback callback);
    void setSensorReadCallbackWithContext(SensorReadCallbackWithContext callback, void *context);
    void setSensorValueContext(BaseSensV2 *sensor, const char *valueKey);
    void setSensorValuePointer(float *valuePtr);

    float readRawValue();
    void setSensorInfo(const char *sensorName, const char *valueLabel);
    
    const char* getSensorName() const;
    const char* getValueLabel() const;

private:
    SensorReadCallback _readCallback;
    SensorReadCallbackWithContext _readCallbackWithContext;
    void *_callbackContext;
    float *_sensorValuePtr;
    SensorContext _sensorContext;
    
    char *_activeSensorName;
    char *_activeSensorValueLabel;
    
    static float sensorContextCallback(void *context);
};

#endif
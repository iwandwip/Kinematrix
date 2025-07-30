#include "SensorDataInterface.h"

SensorDataInterface::SensorDataInterface()
    : _readCallback(nullptr), _readCallbackWithContext(nullptr),
      _callbackContext(nullptr), _sensorValuePtr(nullptr),
      _activeSensorName(nullptr), _activeSensorValueLabel(nullptr) {
    _sensorContext.sensor = nullptr;
    _sensorContext.valueKey = nullptr;
}

SensorDataInterface::~SensorDataInterface() {
    if (_activeSensorName) free(_activeSensorName);
    if (_activeSensorValueLabel) free(_activeSensorValueLabel);
}

void SensorDataInterface::setSensorReadCallback(SensorReadCallback callback) {
    _readCallback = callback;
    _readCallbackWithContext = nullptr;
    _callbackContext = nullptr;
    _sensorValuePtr = nullptr;
}

void SensorDataInterface::setSensorReadCallbackWithContext(SensorReadCallbackWithContext callback, void *context) {
    _readCallbackWithContext = callback;
    _callbackContext = context;
    _readCallback = nullptr;
    _sensorValuePtr = nullptr;
}

void SensorDataInterface::setSensorValueContext(BaseSensV2 *sensor, const char *valueKey) {
    _sensorContext.sensor = sensor;
    _sensorContext.valueKey = valueKey;
    _readCallbackWithContext = sensorContextCallback;
    _callbackContext = &_sensorContext;
    _readCallback = nullptr;
    _sensorValuePtr = nullptr;
}

void SensorDataInterface::setSensorValuePointer(float *valuePtr) {
    _sensorValuePtr = valuePtr;
    _readCallback = nullptr;
    _readCallbackWithContext = nullptr;
    _callbackContext = nullptr;
}

float SensorDataInterface::readRawValue() {
    if (_readCallback) {
        return _readCallback();
    } else if (_readCallbackWithContext && _callbackContext) {
        return _readCallbackWithContext(_callbackContext);
    } else if (_sensorValuePtr) {
        return *_sensorValuePtr;
    }
    return 0.0f;
}

void SensorDataInterface::setSensorInfo(const char *sensorName, const char *valueLabel) {
    if (_activeSensorName) free(_activeSensorName);
    if (_activeSensorValueLabel) free(_activeSensorValueLabel);
    
    _activeSensorName = sensorName ? strdup(sensorName) : nullptr;
    _activeSensorValueLabel = valueLabel ? strdup(valueLabel) : nullptr;
}

const char* SensorDataInterface::getSensorName() const {
    return _activeSensorName;
}

const char* SensorDataInterface::getValueLabel() const {
    return _activeSensorValueLabel;
}

float SensorDataInterface::sensorContextCallback(void *context) {
    SensorContext *ctx = static_cast<SensorContext *>(context);
    if (ctx && ctx->sensor && ctx->valueKey) {
        return ctx->sensor->getFloatValue(ctx->valueKey);
    }
    return 0.0f;
}
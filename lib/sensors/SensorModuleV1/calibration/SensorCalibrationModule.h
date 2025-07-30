#ifndef SENSOR_CALIBRATION_MODULE_H
#define SENSOR_CALIBRATION_MODULE_H

#include "../base/sensor-module.h"
#include "InteractiveSerialGeneralSensorCalibrator.h"

#define MAX_CALIBRATORS 32
#define SENSOR_CAL_EEPROM_START_ADDR 100

class SensorCalibrationModule : public SensorModule {
private:
    InteractiveSerialGeneralSensorCalibrator **_calibrators;
    bool *_hasCalibrator;
    Stream *_serial;
    bool _calibrationMode;
    uint8_t _activeIndex;
    uint32_t _calibrationModeTimeout;
    uint32_t _lastCalibrationActivity;
    bool _autoSaveCalibration;
    int _eepromStartAddress;
    bool _inSensorMenu;

    struct SensorContext {
        SensorCalibrationModule *module;
        uint8_t sensorIndex;
    };

    SensorContext **_sensorContexts;

    void clearSerialBuffer() const;
    int findSensorIndex(const char *name) const;
    bool isCalibrationModeTimedOut() const;
    void processCalibrationCommand(char cmd);
    void printCalibrationMenu();
    void processSensorCalibrationCommand(char cmd);
    float getRawValueForSensor(uint8_t index) const;
    static float sensorReadCallback(void *context);

public:
    SensorCalibrationModule();
    virtual ~SensorCalibrationModule();

    virtual void init(void (*initializeCallback)(void) = nullptr) override;
    virtual void update(void (*updateCallback)(void) = nullptr) override;

    bool addCalibrator(uint8_t index);
    bool addCalibrator(const char *name);
    bool removeCalibrator(uint8_t index);
    bool removeCalibrator(const char *name);
    bool hasCalibrator(uint8_t index) const;
    bool hasCalibrator(const char *name) const;

    InteractiveSerialGeneralSensorCalibrator *getCalibrator(uint8_t index) const;
    InteractiveSerialGeneralSensorCalibrator *getCalibrator(const char *name) const;

    void startCalibrationMode(Stream *serialPtr, uint32_t timeout = 300000);
    void stopCalibrationMode();
    bool isInCalibrationMode() const;

    float getRawValue(uint8_t index) const;
    float getRawValue(const char *name) const;
    float getCalibratedValue(uint8_t index) const;
    float getCalibratedValue(const char *name) const;

    bool saveAllCalibrations(int baseEepromAddress = SENSOR_CAL_EEPROM_START_ADDR);
    bool loadAllCalibrations(int baseEepromAddress = SENSOR_CAL_EEPROM_START_ADDR);

    void setAutoSaveCalibration(bool autoSave);
    bool getAutoSaveCalibration() const;
    void setEEPROMStartAddress(int address);
    int getEEPROMStartAddress() const;

    void setCalibrationParameter(uint8_t index, const char *units, uint8_t precision = 2);
    void setCalibrationParameter(const char *name, const char *units, uint8_t precision = 2);

    void calibrateOnePoint(uint8_t index, float knownValue);
    void calibrateOnePoint(const char *name, float knownValue);
    void calibrateTwoPoint(uint8_t index, float knownValue1, float rawValue1, float knownValue2, float rawValue2);
    void calibrateTwoPoint(const char *name, float knownValue1, float rawValue1, float knownValue2, float rawValue2);

    virtual void debug(const char *searchName, bool showHeapMemory = false, bool endl = true) override;
    virtual void debug(bool showHeapMemory = false) override;
};

#endif
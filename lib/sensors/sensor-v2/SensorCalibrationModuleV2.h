#ifndef SENSOR_CALIBRATION_MODULE_V2_H
#define SENSOR_CALIBRATION_MODULE_V2_H

#include "SensorModuleV2.h"
#include "InteractiveSerialGeneralSensorCalibratorV2.h"

struct CalibrationEntry {
    BaseSensV2 *sensor;
    char *sensorName;
    char *valueKey;
    InteractiveSerialGeneralSensorCalibratorV2 *calibrator;
    bool isActive;
};

struct CalibrationLoadResult {
    uint16_t totalEntries;
    uint16_t successCount;
    uint16_t notCalibratedCount;
    uint16_t errorCount;
};

class SensorCalibrationModuleV2 : public SensorModuleV2 {
private:
    CalibrationEntry *_entries;
    uint16_t _entryCount;
    uint16_t _entryCapacity;

    Stream *_serial;
    bool _calibrationMode;
    uint16_t _activeEntryIndex;
    uint32_t _calibrationModeTimeout;
    uint32_t _lastCalibrationActivity;
    bool _autoSaveCalibration;
    int _eepromStartAddress;
    unsigned long _timeout;

    void clearSerialBuffer() const;
    bool isCalibrationModeTimedOut() const;
    void processCalibrationCommand(char cmd);
    void printCalibrationMenu();

    int findEntryIndex(const char *sensorName, const char *valueKey) const;
    static void calibrationCompletedCallback(void *context);
    void startCalibratorForEntry(uint16_t entryIndex);

public:
    SensorCalibrationModuleV2();
    virtual ~SensorCalibrationModuleV2();

    void init();
    void update();

    void setSerialOutput(Stream *serialPtr);

    void discoverSensorValues();

    bool addCalibrationEntry(BaseSensV2 *sensor, const char *sensorName, const char *valueKey);
    bool addCalibrationEntry(const char *sensorName, const char *valueKey);

    void calibrateOnePoint(const char *sensorName, const char *valueKey, float knownValue);
    void calibrateTwoPoint(const char *sensorName, const char *valueKey,
                           float knownValue1, float rawValue1,
                           float knownValue2, float rawValue2);

    void startCalibrationMode(Stream *serialPtr, uint32_t timeout = 300000);
    void startCalibrationMode(uint32_t timeout = 300000);
    void stopCalibrationMode();
    bool isInCalibrationMode() const;

    float getRawValue(const char *sensorName, const char *valueKey) const;
    float getCalibratedValue(const char *sensorName, const char *valueKey) const;

    bool saveAllCalibrations(int baseEepromAddress = 0);
    bool loadAllCalibrations(int baseEepromAddress = 0);
    CalibrationLoadResult loadAllCalibrationsWithStatus(int baseEepromAddress = 0);

    void setAutoSaveCalibration(bool autoSave);
    bool getAutoSaveCalibration() const;
    void setEEPROMStartAddress(int address);
    int getEEPROMStartAddress() const;

    void setCalibrationTimeout(unsigned long timeout);
    unsigned long getCalibrationTimeout() const;

    void listAllSensorValues();
    void printCalibrationStatus();
};

#endif
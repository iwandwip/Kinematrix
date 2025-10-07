#ifndef MULTI_SENSOR_CALIBRATION_MANAGER_H
#define MULTI_SENSOR_CALIBRATION_MANAGER_H

#include <Arduino.h>
#include "InteractiveSerialGeneralSensorCalibrator.h"

#define MAX_CALIBRATORS 8
#define MAX_NAME_LENGTH 16

class MultiSensorCalibrationManager {
public:
    MultiSensorCalibrationManager();
    ~MultiSensorCalibrationManager();

    bool addCalibrator(const char *name, InteractiveSerialGeneralSensorCalibrator *calibrator);
    void setCalibrationInfo(uint8_t index, const char *name, const char *units, uint8_t precision = 2);
    void setCalibrationInfo(const char *name, const char *units, uint8_t precision = 2);

    bool selectCalibrator(uint8_t index);
    bool selectCalibrator(const char *name);
    InteractiveSerialGeneralSensorCalibrator *getActiveCalibrator();
    uint8_t getActiveCalibratorIndex();
    const char *getActiveCalibratorName();

    void begin(Stream *serialPtr, unsigned long timeout = 10000);
    void update();
    void processCommand(char cmd);
    void printMainMenu();

    void readAllSensors();
    void showAllStats();
    bool saveAllCalibrations(int baseEepromAddress = 0);
    bool loadAllCalibrations(int baseEepromAddress = 0);

    void enableContinuousReading(bool enable);
    void setReadInterval(unsigned long interval);

private:
    InteractiveSerialGeneralSensorCalibrator **_calibrators;
    char **_sensorNames;
    uint8_t _activeIndex;
    uint8_t _numCalibrators;

    Stream *_serial;
    unsigned long _timeout;
    unsigned long _readInterval;
    unsigned long _lastReadTime;
    bool _continuousReading;

    float *_allReadings;
    float *_allCalibrated;

    void clearSerialBuffer();
    int findCalibratorByName(const char *name);
    bool isSerialTimedOut(unsigned long startTime) const;

    int readInt(const char *prompt, int defaultValue = 0, unsigned long timeout = 0);
    char readChar(const char *prompt, char defaultValue = '\0', unsigned long timeout = 0);
    bool readYesNo(const char *prompt, bool defaultValue = true, unsigned long timeout = 0);
};

#endif
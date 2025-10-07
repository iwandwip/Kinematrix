#ifndef INTERACTIVE_CALIBRATOR_H
#define INTERACTIVE_CALIBRATOR_H

#include <Arduino.h>
#include <EEPROM.h>
#include "SensorDataInterface.h"
#include "CalibrationEngine.h"
#include "CalibrationStorage.h"
#include "SerialInputHandler.h"
#include "CalibrationUI.h"

typedef void (*CalibrationCompletedCallback)(void *context);

class InteractiveCalibrator {
public:
    InteractiveCalibrator();
    ~InteractiveCalibrator();

    void begin(Stream *serialPtr, unsigned long timeout = 10000, int eepromAddress = 0);
    bool isActive() const;
    bool isFinished() const;

    void setSensorReadCallback(SensorDataInterface::SensorReadCallback callback);
    void setSensorReadCallbackWithContext(SensorDataInterface::SensorReadCallbackWithContext callback, void *context);
    void setSensorValueContext(BaseSensV2 *sensor, const char *valueKey);
    void setSensorValuePointer(float *valuePtr);

    void setSensorInfo(const char *sensorName, const char *valueLabel);
    void setCompletedCallback(CalibrationCompletedCallback callback, void *callbackContext);
    void setEEPROMAddress(int address);
    int getEEPROMAddress() const;

    float readRawValue();
    float readCalibratedValue();

    void setReadInterval(unsigned long interval);
    void enableContinuousReading(bool enable);
    void update();

    bool calibrateOnePoint(float knownValue, float rawValue = -1);
    bool calibrateTwoPoint(float knownValue1, float rawValue1, float knownValue2, float rawValue2);

    void setMaxCalibrationPoints(uint8_t maxPoints);
    void startCalibration(uint8_t calibrationType);
    bool addCalibrationPoint(float knownValue, float rawValue = -1);
    bool calculateCalibration();
    void cancelCalibration();

    bool clearCalibrationPoints();
    bool isCalibrated();
    uint8_t getCalibrationMethod();
    uint8_t getCalibrationPointCount();

    void setInterpolationMethod(uint8_t method);
    void setPolynomialDegree(uint8_t degree);

    bool processCommand(char cmd);
    void printMenu();

    void setMenuPrompt(const char *prompt);
    void setMenuOptions(const char *options[], uint8_t numOptions);

    void startReading();
    void stopReading();
    void readSensor();

    void runOnePointCalibration();
    void runTwoPointCalibration();
    void runMultiPointCalibration();
    void showCalibrationDetails();
    void runClearCalibration();

    float readFloat(const char *prompt, float defaultValue = 0.0, unsigned long timeout = 0);
    int readInt(const char *prompt, int defaultValue = 0, unsigned long timeout = 0);
    bool readYesNo(const char *prompt, bool defaultValue = true, unsigned long timeout = 0);
    char readChar(const char *prompt, char defaultValue = '\0', unsigned long timeout = 0);
    String readString(const char *prompt, const char *defaultValue = "", unsigned long timeout = 0);

    bool saveCalibration(int eepromAddress = -1);
    bool loadCalibration(int eepromAddress = -1);

    bool saveCalibrationProfile(uint8_t profileNumber);
    bool loadCalibrationProfile(uint8_t profileNumber);
    void listCalibrationProfiles();

    void showStatistics(uint16_t numSamples = 100);
    void drawCalibrationCurve(uint8_t width = 40, uint8_t height = 10);

    void setDisplayUnits(const char *units);
    void setDisplayPrecision(uint8_t precision);
    const char *getDisplayUnits() const;
    uint8_t getDisplayPrecision() const;

    bool loadPreferences(int eepromAddress = 400);
    bool savePreferences(int eepromAddress = 400);
    void resetToDefaults();

    void clearSerialBuffer();
    void waitForSerialInput();

    bool isWaitingForInput() const;
    bool handleSerialInput();
    void end();

private:
    SensorDataInterface *_dataInterface;
    CalibrationEngine *_engine;
    CalibrationStorage *_storage;
    SerialInputHandler *_inputHandler;
    CalibrationUI *_ui;
    
    CalibrationCompletedCallback _completedCallback;
    void *_completedCallbackContext;
    
    void notifyCalibrationCompleted();
    void initializeEEPROM();
};

#endif
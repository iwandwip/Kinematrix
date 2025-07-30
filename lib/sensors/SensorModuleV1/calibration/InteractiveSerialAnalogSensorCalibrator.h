#ifndef INTERACTIVE_SERIAL_ANALOG_SENSOR_CALIBRATOR_H
#define INTERACTIVE_SERIAL_ANALOG_SENSOR_CALIBRATOR_H

#include "AnalogSensorCalibrator.h"

enum CalibrationState {
    STATE_IDLE,
    STATE_MENU,
    STATE_READING,
    STATE_ONE_POINT_CAL,
    STATE_TWO_POINT_CAL,
    STATE_MULTI_POINT_CAL,
    STATE_CALIBRATION_DETAILS,
    STATE_SETTINGS
};

class InteractiveSerialAnalogSensorCalibrator : public AnalogSensorCalibrator {
public:
    InteractiveSerialAnalogSensorCalibrator(int pin, float referenceVoltage = 5.0, float adcRange = 0);
    ~InteractiveSerialAnalogSensorCalibrator();

    void begin(Stream *serialPtr, unsigned long timeout = 10000);

    void setReadInterval(unsigned long interval);
    void enableContinuousReading(bool enable);
    void update();

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

    bool saveCalibrationProfile(uint8_t profileNumber);
    bool loadCalibrationProfile(uint8_t profileNumber);
    void listCalibrationProfiles();

    void showStatistics(uint16_t numSamples = 100);
    void drawCalibrationCurve(uint8_t width = 40, uint8_t height = 10);

    void setDisplayUnits(const char *units);
    void setDisplayPrecision(uint8_t precision);

    bool loadPreferences(int eepromAddress = 400);
    bool savePreferences(int eepromAddress = 400);
    void resetToDefaults();

private:
    Stream *_serial;
    unsigned long _timeout;
    unsigned long _readInterval;
    unsigned long _lastReadTime;
    bool _continuousReading;
    CalibrationState _state;

    const char *_menuPrompt;
    const char **_menuOptions;
    uint8_t _numMenuOptions;

    char _displayUnits[8];
    uint8_t _displayPrecision;

    float collectStableMeasurement(uint8_t numSamples = 10, unsigned long delayBetweenSamples = 100);
    bool isSerialTimedOut(unsigned long startTime) const;
    void clearSerialBuffer();
    void waitForSerialInput();
    void printReadingHeader();
    void printStatsHeader();

    uint16_t getEepromProfileAddress(uint8_t profileNumber) const;
};

#endif
#ifndef INTERACTIVE_SERIAL_GENERAL_SENSOR_CALIBRATOR_H
#define INTERACTIVE_SERIAL_GENERAL_SENSOR_CALIBRATOR_H

#include <Arduino.h>
#include <EEPROM.h>

#define CALIBRATION_NONE 0
#define CALIBRATION_ONE_POINT 1
#define CALIBRATION_TWO_POINT 2
#define CALIBRATION_MULTI_POINT 3

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

class InteractiveSerialGeneralSensorCalibrator {
public:
    typedef float (*SensorReadCallback)();
    typedef float (*SensorReadCallbackWithContext)(void *context);

    InteractiveSerialGeneralSensorCalibrator();
    ~InteractiveSerialGeneralSensorCalibrator();

    void begin(Stream *serialPtr, unsigned long timeout = 10000);

    void setSensorReadCallback(SensorReadCallback callback);
    void setSensorReadCallbackWithContext(SensorReadCallbackWithContext callback, void *context);
    void setSensorValuePointer(float *valuePtr);

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

    bool saveCalibration(int eepromAddress = 0);
    bool loadCalibration(int eepromAddress = 0);

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

private:
    struct CalibrationPoint {
        float rawValue;
        float knownValue;
    };

    Stream *_serial;
    unsigned long _timeout;
    unsigned long _readInterval;
    unsigned long _lastReadTime;
    bool _continuousReading;
    CalibrationState _state;

    SensorReadCallback _readCallback;
    SensorReadCallbackWithContext _readCallbackWithContext;
    void *_callbackContext;
    float *_sensorValuePtr;

    uint8_t _calibrationType;
    bool _isCalibrated;
    bool _inCalibrationMode;

    CalibrationPoint *_calibPoints;
    uint8_t _maxCalibPoints;
    uint8_t _numCalibPoints;

    float _scale;
    float _slope, _offset;

    uint8_t _interpolationMethod;
    uint8_t _polynomialDegree;
    float *_polynomialCoeffs;

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
    void printCalibrationDetails();

    void initializeBuffers();
    void cleanupMemory();
    float linearInterpolate(float rawValue);
    float polynomialInterpolate(float rawValue);
    bool calculatePolynomialRegression();
    bool sortCalibrationPoints();

    uint16_t getEepromProfileAddress(uint8_t profileNumber) const;
};

#endif
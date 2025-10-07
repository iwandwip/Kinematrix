#ifndef CALIBRATION_UI_H
#define CALIBRATION_UI_H

#include <Arduino.h>
#include "SensorDataInterface.h"
#include "CalibrationEngine.h"
#include "CalibrationStorage.h"
#include "SerialInputHandler.h"

enum CalibrationStateV2 {
    STATE_IDLE,
    STATE_MENU,
    STATE_READING,
    STATE_ONE_POINT_CAL,
    STATE_TWO_POINT_CAL,
    STATE_MULTI_POINT_CAL,
    STATE_CALIBRATION_DETAILS,
    STATE_SETTINGS,
    STATE_WAITING_FOR_INPUT
};

class CalibrationUI {
public:
    CalibrationUI();
    ~CalibrationUI();

    void begin(Stream *serialPtr);
    void setReadInterval(unsigned long interval);
    void enableContinuousReading(bool enable);
    
    void setMenuPrompt(const char *prompt);
    void setMenuOptions(const char *options[], uint8_t numOptions);
    
    bool processCommand(char cmd, SensorDataInterface* dataInterface, 
                       CalibrationEngine* engine, CalibrationStorage* storage,
                       SerialInputHandler* inputHandler);
    void printMenu();
    
    void startReading(SensorDataInterface* dataInterface);
    void stopReading();
    void readSensor(SensorDataInterface* dataInterface);
    
    void runOnePointCalibration(SensorDataInterface* dataInterface, 
                               CalibrationEngine* engine, SerialInputHandler* inputHandler);
    void runTwoPointCalibration(SensorDataInterface* dataInterface, 
                               CalibrationEngine* engine, SerialInputHandler* inputHandler);
    void runMultiPointCalibration(SensorDataInterface* dataInterface, 
                                 CalibrationEngine* engine, SerialInputHandler* inputHandler);
    void showCalibrationDetails(const CalibrationEngine* engine, const CalibrationStorage* storage);
    void runClearCalibration(CalibrationEngine* engine);
    
    void showStatistics(SensorDataInterface* dataInterface, const CalibrationStorage* storage, uint16_t numSamples = 100);
    void drawCalibrationCurve(const CalibrationEngine* engine, uint8_t width = 40, uint8_t height = 10);
    
    CalibrationStateV2 getState() const;
    void setState(CalibrationStateV2 state);
    bool isFinished() const;
    void setFinished(bool finished);
    
    void update(SensorDataInterface* dataInterface);

private:
    Stream *_serial;
    unsigned long _readInterval;
    unsigned long _lastReadTime;
    bool _continuousReading;
    CalibrationStateV2 _state;
    bool _isFinished;
    
    const char *_menuPrompt;
    
    float collectStableMeasurement(SensorDataInterface* dataInterface, uint8_t numSamples = 10, unsigned long delayBetweenSamples = 100);
    void printReadingHeader();
    void printStatsHeader();
    void printCalibrationDetails(const CalibrationEngine* engine);
};

#endif
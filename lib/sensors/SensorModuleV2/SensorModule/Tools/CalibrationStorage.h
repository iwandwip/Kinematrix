#ifndef CALIBRATION_STORAGE_H
#define CALIBRATION_STORAGE_H

#include <Arduino.h>
#include <EEPROM.h>
#include "CalibrationEngine.h"

class CalibrationStorage {
public:
    CalibrationStorage();
    ~CalibrationStorage();

    void setEEPROMAddress(int address);
    int getEEPROMAddress() const;

    bool saveCalibration(const CalibrationEngine* engine, int eepromAddress = -1);
    bool loadCalibration(CalibrationEngine* engine, int eepromAddress = -1);

    bool saveCalibrationProfile(const CalibrationEngine* engine, uint8_t profileNumber);
    bool loadCalibrationProfile(CalibrationEngine* engine, uint8_t profileNumber);
    void listCalibrationProfiles(Stream* serial);

    bool loadPreferences(int eepromAddress = 400);
    bool savePreferences(int eepromAddress = 400);
    void resetToDefaults();

    void setDisplayUnits(const char *units);
    void setDisplayPrecision(uint8_t precision);
    const char *getDisplayUnits() const;
    uint8_t getDisplayPrecision() const;

private:
    int _eepromAddress;
    char _displayUnits[8];
    uint8_t _displayPrecision;
    
    uint16_t getEepromProfileAddress(uint8_t profileNumber) const;
};

#endif
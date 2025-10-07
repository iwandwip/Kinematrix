#include "CalibrationStorage.h"

CalibrationStorage::CalibrationStorage()
    : _eepromAddress(0), _displayPrecision(2) {
    strcpy(_displayUnits, "units");
}

CalibrationStorage::~CalibrationStorage() {
}

void CalibrationStorage::setEEPROMAddress(int address) {
    _eepromAddress = address;
}

int CalibrationStorage::getEEPROMAddress() const {
    return _eepromAddress;
}

bool CalibrationStorage::saveCalibration(const CalibrationEngine* engine, int eepromAddress) {
    if (!engine) return false;
    
    int addr = (eepromAddress == -1) ? _eepromAddress : eepromAddress;
    
    EEPROM.put(addr, engine->getCalibrationMethod());
    addr += sizeof(uint8_t);
    
    EEPROM.put(addr, engine->getScale());
    addr += sizeof(float);
    
    EEPROM.put(addr, engine->getSlope());
    addr += sizeof(float);
    
    EEPROM.put(addr, engine->getOffset());
    addr += sizeof(float);
    
    EEPROM.put(addr, engine->getCalibrationPointCount());
    addr += sizeof(uint8_t);
    
    const CalibrationEngine::CalibrationPoint* points = engine->getCalibrationPoints();
    if (points) {
        for (uint8_t i = 0; i < engine->getCalibrationPointCount(); i++) {
            EEPROM.put(addr, points[i].rawValue);
            addr += sizeof(float);
            EEPROM.put(addr, points[i].knownValue);
            addr += sizeof(float);
        }
    }
    
#if defined(ARDUINO_ARCH_ESP8266) || defined(ARDUINO_ARCH_ESP32)
    EEPROM.commit();
#endif
    
    return true;
}

bool CalibrationStorage::loadCalibration(CalibrationEngine* engine, int eepromAddress) {
    if (!engine) return false;
    
    int addr = (eepromAddress == -1) ? _eepromAddress : eepromAddress;
    
    uint8_t calibMethod;
    EEPROM.get(addr, calibMethod);
    addr += sizeof(uint8_t);
    
    if (calibMethod == CALIBRATION_NONE) {
        return false;
    }
    
    float scale, slope, offset;
    EEPROM.get(addr, scale);
    addr += sizeof(float);
    
    EEPROM.get(addr, slope);
    addr += sizeof(float);
    
    EEPROM.get(addr, offset);
    addr += sizeof(float);
    
    uint8_t pointCount;
    EEPROM.get(addr, pointCount);
    addr += sizeof(uint8_t);
    
    engine->clearCalibrationPoints();
    engine->startCalibration(calibMethod);
    
    for (uint8_t i = 0; i < pointCount; i++) {
        float rawValue, knownValue;
        EEPROM.get(addr, rawValue);
        addr += sizeof(float);
        EEPROM.get(addr, knownValue);
        addr += sizeof(float);
        
        engine->addCalibrationPoint(knownValue, rawValue);
    }
    
    engine->calculateCalibration();
    return true;
}

bool CalibrationStorage::saveCalibrationProfile(const CalibrationEngine* engine, uint8_t profileNumber) {
    if (profileNumber >= 8) return false;
    
    uint16_t addr = getEepromProfileAddress(profileNumber);
    return saveCalibration(engine, addr);
}

bool CalibrationStorage::loadCalibrationProfile(CalibrationEngine* engine, uint8_t profileNumber) {
    if (profileNumber >= 8) return false;
    
    uint16_t addr = getEepromProfileAddress(profileNumber);
    return loadCalibration(engine, addr);
}

void CalibrationStorage::listCalibrationProfiles(Stream* serial) {
    if (!serial) return;
    
    serial->println("\nCalibration Profiles:");
    for (uint8_t i = 0; i < 8; i++) {
        uint16_t addr = getEepromProfileAddress(i);
        uint8_t calibMethod;
        EEPROM.get(addr, calibMethod);
        
        serial->print("Profile ");
        serial->print(i);
        serial->print(": ");
        
        if (calibMethod == CALIBRATION_NONE) {
            serial->println("Empty");
        } else {
            serial->print("Method ");
            serial->println(calibMethod);
        }
    }
}

bool CalibrationStorage::loadPreferences(int eepromAddress) {
    EEPROM.get(eepromAddress, _displayPrecision);
    EEPROM.get(eepromAddress + sizeof(uint8_t), _displayUnits);
    return true;
}

bool CalibrationStorage::savePreferences(int eepromAddress) {
    EEPROM.put(eepromAddress, _displayPrecision);
    EEPROM.put(eepromAddress + sizeof(uint8_t), _displayUnits);
    
#if defined(ARDUINO_ARCH_ESP8266) || defined(ARDUINO_ARCH_ESP32)
    EEPROM.commit();
#endif
    
    return true;
}

void CalibrationStorage::resetToDefaults() {
    _displayPrecision = 2;
    strcpy(_displayUnits, "units");
}

void CalibrationStorage::setDisplayUnits(const char *units) {
    if (units) {
        strncpy(_displayUnits, units, sizeof(_displayUnits) - 1);
        _displayUnits[sizeof(_displayUnits) - 1] = '\0';
    }
}

void CalibrationStorage::setDisplayPrecision(uint8_t precision) {
    _displayPrecision = precision;
}

const char *CalibrationStorage::getDisplayUnits() const {
    return _displayUnits;
}

uint8_t CalibrationStorage::getDisplayPrecision() const {
    return _displayPrecision;
}

uint16_t CalibrationStorage::getEepromProfileAddress(uint8_t profileNumber) const {
    return 100 + (profileNumber * 64);
}
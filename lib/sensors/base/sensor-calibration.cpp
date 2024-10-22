/*
 *  sensor-calibration.cpp
 *
 *  sensor calibration c
 *  Created on: 2023. 4. 3
 */

#include "sensor-calibration.h"

#ifdef ESP32
AnalogCalibration::AnalogCalibration(const char *calibrationName, Preferences *preferences)
        : calibrationDataArray(nullptr),
          calibrationName(calibrationName),
          calibrationDataCount(0),
          preferences(preferences) {}
#elif defined(ESP8266)
#else

AnalogCalibration::AnalogCalibration(const char *calibrationName, EEPROMLib *eepromLib, int initialAddress)
        : calibrationDataArray(nullptr),
          calibrationDataCount(0),
          calibrationName(calibrationName),
          eepromLib(eepromLib),
          eepromAddress(initialAddress) {}

#endif

AnalogCalibration::~AnalogCalibration() {
    delete[] calibrationDataArray;

}

void AnalogCalibration::setAnalogConfig(int _sensorPin, float _voltageReference, int _adcRange) {
    sensorPin = _sensorPin;
    voltageReference = _voltageReference;
    adcRange = _adcRange;
}

void AnalogCalibration::calibrateSensor() {
    Serial.println("| Kalibrasi Sensor " + String(calibrationName));
    Serial.print("| Masukan Angka Kalibrasi: ");
    int numPoints = 0;
    while (numPoints == 0) {
        if (Serial.available()) {
            numPoints = Serial.readStringUntil('\n').toInt();
        }
    }

    Serial.println(numPoints);
    if (calibrationDataArray != nullptr) delete[] calibrationDataArray;
    calibrationDataArray = new CalibrationData[numPoints];
    calibrationDataCount = numPoints;

    for (int i = 0; i < numPoints; i++) {
        Serial.print("| Masukan Data ke " + String(i + 1) + ": ");
        CalibrationData cal = getCalibrationValueFromUser(i);
        calibrationDataArray[i].calibrationValue = cal.calibrationValue;
        calibrationDataArray[i].voltage = cal.voltage;

        Serial.print("| index: ");
        Serial.print(i + 1);
        Serial.print("| " + String(calibrationName) + ": ");
        Serial.print(calibrationDataArray[i].calibrationValue);
        Serial.print("| volt: ");
        Serial.print(calibrationDataArray[i].voltage);
        Serial.println();
    }

#ifdef ESP32
    preferences->begin(calibrationName, false);
    preferences->putUInt("numPoints", numPoints);
    for (int i = 0; i < numPoints; i++) {
        String calKey = "cal" + String(i);
        String voltKey = "volt" + String(i);
        preferences->putFloat(calKey.c_str(), calibrationDataArray[i].calibrationValue);
        preferences->putFloat(voltKey.c_str(), calibrationDataArray[i].voltage);
    }
    preferences->end();
#elif defined(ESP8266)
#else
    int address = eepromAddress;
    eepromLib->writeInt(address, numPoints);
    address += sizeof(int);
    for (int i = 0; i < numPoints; i++) {
        address = eepromLib->writeFloat(address, calibrationDataArray[i].calibrationValue);
        address = eepromLib->writeFloat(address, calibrationDataArray[i].voltage);
    }
#endif

    Serial.println("| Kalibrasi Selesai");
}

void AnalogCalibration::calibrateSensorCustom() {
    Serial.println("| Kalibrasi Sensor " + String(calibrationName));
    Serial.print("| Masukan Angka Kalibrasi: ");
    int numPoints = 0;
    while (numPoints == 0) {
        if (Serial.available()) {
            numPoints = Serial.readStringUntil('\n').toInt();
        }
    }

    Serial.println(numPoints);
    delete[] calibrationDataArray;
    calibrationDataArray = new CalibrationData[numPoints];
    calibrationDataCount = numPoints;

    for (int i = 0; i < numPoints; i++) {
        Serial.print("| Masukan Data ke " + String(i + 1) + ": ");
        CalibrationData cal = getCalibrationValueFromUserCustom(i);
        calibrationDataArray[i].calibrationValue = cal.calibrationValue;
        calibrationDataArray[i].voltage = cal.voltage;

        Serial.print("| index: ");
        Serial.print(i + 1);
        Serial.print("| " + String(calibrationName) + ": ");
        Serial.print(calibrationDataArray[i].calibrationValue);
        Serial.print("| volt: ");
        Serial.print(calibrationDataArray[i].voltage);
        Serial.println();
    }

#ifdef ESP32
    preferences->begin(calibrationName, false);
    preferences->putUInt("numPoints", numPoints);
    for (int i = 0; i < numPoints; i++) {
        String calKey = "cal" + String(i);
        String voltKey = "volt" + String(i);
        preferences->putFloat(calKey.c_str(), calibrationDataArray[i].calibrationValue);
        preferences->putFloat(voltKey.c_str(), calibrationDataArray[i].voltage);
    }
    preferences->end();
#elif defined(ESP8266)
#else
    int address = eepromAddress;
    eepromLib->writeInt(address, numPoints);
    address += sizeof(int);
    for (int i = 0; i < numPoints; i++) {
        address = eepromLib->writeFloat(address, calibrationDataArray[i].calibrationValue);
        address = eepromLib->writeFloat(address, calibrationDataArray[i].voltage);
    }
#endif

    Serial.println("| Kalibrasi Selesai");
}

void AnalogCalibration::loadCalibration() {
#ifdef ESP32
    preferences->begin(calibrationName, true);
    int numPoints = preferences->getUInt("numPoints", 0);
#elif defined(ESP8266)
    int numPoints;
#else
    int address = eepromAddress;
    int numPoints = eepromLib->readInt(address);
    address += sizeof(int);
#endif

    calibrationDataCount = numPoints;
    if (numPoints > 0) {
        delete[] calibrationDataArray;
        calibrationDataArray = new CalibrationData[numPoints];
        Serial.print("| Load: ");
        Serial.println(calibrationDataCount);
        for (int i = 0; i < numPoints; i++) {
#ifdef ESP32
            String calKey = "cal" + String(i);
            String voltKey = "volt" + String(i);
            calibrationDataArray[i].calibrationValue = preferences->getFloat(calKey.c_str(), 0.0);
            calibrationDataArray[i].voltage = preferences->getFloat(voltKey.c_str(), 0.0);
#elif defined(ESP8266)
#else
            calibrationDataArray[i].calibrationValue = eepromLib->readFloat(address);
            address += sizeof(float);
            calibrationDataArray[i].voltage = eepromLib->readFloat(address);
            address += sizeof(float);
#endif

            Serial.print("| index: ");
            Serial.print(i + 1);
            Serial.print("| " + String(calibrationName) + ": ");
            Serial.print(calibrationDataArray[i].calibrationValue);
            Serial.print("| voltage: ");
            Serial.print(calibrationDataArray[i].voltage);
            Serial.println();
        }
#ifdef ESP32
        Serial.print("| ESP.getFreeHeap(): ");
        Serial.print(ESP.getFreeHeap());
#elif defined(ESP8266)
#else
        Serial.print("| freeMemory(): ");
        Serial.print(freeMemory());
#endif
        Serial.println();
    }
#ifdef ESP32
    preferences->end();
#endif
}

float AnalogCalibration::voltageToValue(float voltage, interpolation_cfg cfg) {
    if (calibrationDataCount < 2) return 0.0;
    if (voltage < calibrationDataArray[0].voltage) {
        return calibrationDataArray[0].calibrationValue;
    } else if (voltage > calibrationDataArray[calibrationDataCount - 1].voltage) {
        return calibrationDataArray[calibrationDataCount - 1].calibrationValue;
    }
    if (cfg == LAGRANGE_INTERPOLATION) return lagrangeInterpolation(voltage);
    else if (cfg == LINEAR_INTERPOLATION) return linearInterpolation(voltage);
    else return 0.0;
}

AnalogCalibration::CalibrationData AnalogCalibration::getCalibrationValueFromUser(int i) const {
    Serial.println();
    CalibrationData cal{};
    while (true) {
        float voltage = (float)analogRead(sensorPin) * (voltageReference / (float)adcRange);
        static uint32_t voltageTimer;
        if (millis() - voltageTimer >= 1000) {
            Serial.print("| voltage: ");
            Serial.print(voltage);
            Serial.print("| Masukan Data ke " + String(i + 1) + ": ");
            Serial.println();
            voltageTimer = millis();
        }
        if (Serial.available()) {
            float calibrationValueUser = Serial.readStringUntil('\n').toFloat();
            cal.voltage = voltage;
            cal.calibrationValue = calibrationValueUser;
            return cal;
        }
    }
}

AnalogCalibration::CalibrationData AnalogCalibration::getCalibrationValueFromUserCustom(int i) {
    Serial.println();
    CalibrationData cal{};
    Serial.println("| calibrationValue: ");
    while (true) {
        if (Serial.available()) {
            float calibrationValueUser = Serial.readStringUntil('\n').toFloat();
            cal.calibrationValue = calibrationValueUser;
            break;
        }
    }
    Serial.println("| voltageValue: ");
    while (true) {
        if (Serial.available()) {
            float voltageValueUser = Serial.readStringUntil('\n').toFloat();
            cal.voltage = voltageValueUser;
            return cal;
        }
    }
}

float AnalogCalibration::lagrangeInterpolation(float voltage) {
    float result = 0.0;
    for (int i = 0; i < calibrationDataCount; i++) {
        float term = calibrationDataArray[i].calibrationValue;
        for (int j = 0; j < calibrationDataCount; j++) {
            if (i != j) {
                term *= (voltage - calibrationDataArray[j].voltage) /
                        (calibrationDataArray[i].voltage - calibrationDataArray[j].voltage);
            }
        }
        result += term;
    }
    return result;
}

float AnalogCalibration::linearInterpolation(float voltage) {
    for (int i = 0; i < calibrationDataCount - 1; i++) {
        if (voltage >= calibrationDataArray[i].voltage && voltage <= calibrationDataArray[i + 1].voltage) {
            float x0 = calibrationDataArray[i].voltage;
            float y0 = calibrationDataArray[i].calibrationValue;
            float x1 = calibrationDataArray[i + 1].voltage;
            float y1 = calibrationDataArray[i + 1].calibrationValue;
            return y0 + (voltage - x0) * (y1 - y0) / (x1 - x0);
        }
    }
    return voltage < calibrationDataArray[0].voltage ? calibrationDataArray[0].calibrationValue : calibrationDataArray[
            calibrationDataCount - 1].calibrationValue;
}
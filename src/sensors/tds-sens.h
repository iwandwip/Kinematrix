/***************************************************
 DFRobot Gravity: Analog TDS Sensor/Meter
 <https://www.dfrobot.com/wiki/index.php/Gravity:_Analog_TDS_Sensor_/_Meter_For_Arduino_SKU:_SEN0244>

 ***************************************************
 This sample code shows how to read the tds value and calibrate it with the standard buffer solution.
 707ppm(1413us/cm)@25^c standard buffer solution is recommended.

 Created 2018-1-3
 By Jason <jason.ling@dfrobot.com@dfrobot.com>

 GNU Lesser General Public License.
 See <http://www.gnu.org/licenses/> for details.
 All above must be included in any redistribution.
 ****************************************************/

/*
 *  tds-sens.h
 *
 *  tds sensor lib
 *  Created on: 2023. 4. 3
 */

#ifdef USE_TDS_SENS
#pragma once

#ifndef GRAVITY_TDS_H
#define GRAVITY_TDS_H

#include "Arduino.h"

#define ReceivedBufferLength 15
#define TdsFactor 0.5  // tds = ec / 2

class GravityTDS {
public:
    GravityTDS();
    ~GravityTDS();

    void begin();  //initialization
    void update(); //read and calculate
    void setPin(int _pin);
    void setTemperature(float temp);  //set the temperature and execute temperature compensation
    void setAref(float value);  //reference voltage on ADC, default 5.0V on Arduino UNO
    void setAdcRange(float range);  //1024 for 10bit ADC;4096 for 12bit ADC
    void setKvalueAddress(int address); //set the EEPROM address to store the k value,default address:0x08
    float getKvalue();
    float getTdsValue();
    float getEcValue();

private:
    int pin;
    float aref;  // default 5.0V on Arduino UNO
    float adcRange;
    float temperature;
    int kValueAddress;     //the address of the K value stored in the EEPROM
    char cmdReceivedBuffer[ReceivedBufferLength + 1];   // store the serial cmd from the serial monitor
    byte cmdReceivedBufferIndex;

    float kValue;      // k value of the probe,you can calibrate in buffer solution ,such as 706.5ppm(1413us/cm)@25^C
    float analogValue;
    float voltage;
    float ecValue; //before temperature compensation
    float ecValue25; //after temperature compensation
    float tdsValue;

    void readKValues();
    boolean cmdSerialDataAvailable();
    byte cmdParse();
    void ecCalibration(byte mode);
};

#endif

#ifndef TDS_SENS_H
#define TDS_SENS_H

#include "base/sensor-module.h"
#include "EEPROM.h"

class TDSSens : public BaseSens, public GravityTDS {
private:
    JsonDocument *doc;
    const char *name;

    float *sensorTemp;

    uint8_t sensorPin;
    uint32_t sensorTimer;
    using GravityTDS::GravityTDS;

public:
    explicit TDSSens(uint8_t _pin, float *_temp, float _vref = 5.0, float _adcRange = 1024.0);
    ~TDSSens();
    bool init() override;
    bool update() override;

    void setDocument(const char *objName) override;
    void setDocumentValue(JsonDocument *docBase) override;
    JsonDocument getDocument() override;
    JsonVariant getVariant(const char *searchName) override;

    float getValueTDSSens() const;
    void setPins(uint8_t _pin);
};


#define EEPROM_write(address, p) {int i = 0; byte *pp = (byte*)&(p);for(; i < sizeof(p); i++) EEPROM.write(address+i, pp[i]);}
#define EEPROM_read(address, p)  {int i = 0; byte *pp = (byte*)&(p);for(; i < sizeof(p); i++) pp[i]=EEPROM.read(address+i);}

GravityTDS::GravityTDS() {
#if defined(ESP8266)
    this->pin = 2;
#elif defined(ESP32)
    this->pin = 2;
#else
    this->pin = 2;
#endif
    this->temperature = 25.0;
    this->aref = 5.0;
    this->adcRange = 1024.0;
    this->kValueAddress = 8;
    this->kValue = 1.0;
}

GravityTDS::~GravityTDS() {
}

void GravityTDS::setPin(int _pin) {
    this->pin = _pin;
}

void GravityTDS::setTemperature(float temp) {
    this->temperature = temp;
}

void GravityTDS::setAref(float value) {
    this->aref = value;
}

void GravityTDS::setAdcRange(float range) {
    this->adcRange = range;
}

void GravityTDS::setKvalueAddress(int address) {
    this->kValueAddress = address;
}

void GravityTDS::begin() {
    pinMode(this->pin, INPUT);
    readKValues();
}

float GravityTDS::getKvalue() {
    return this->kValue;
}

void GravityTDS::update() {
    this->analogValue = analogRead(this->pin);
    this->voltage = this->analogValue / this->adcRange * this->aref;
    this->ecValue = (133.42 * this->voltage * this->voltage * this->voltage - 255.86 * this->voltage * this->voltage +
                     857.39 * this->voltage) * this->kValue;
    this->ecValue25 = this->ecValue / (1.0 + 0.02 * (this->temperature - 25.0));  //temperature compensation
    this->tdsValue = ecValue25 * TdsFactor;
    if (cmdSerialDataAvailable() > 0) {
        ecCalibration(cmdParse());  // if received serial cmd from the serial monitor, enter into the calibration mode
    }
}

float GravityTDS::getTdsValue() {
    return tdsValue;
}

float GravityTDS::getEcValue() {
    return ecValue25;
}


void GravityTDS::readKValues() {
    EEPROM_read(this->kValueAddress, this->kValue);
    if (EEPROM.read(this->kValueAddress) == 0xFF && EEPROM.read(this->kValueAddress + 1) == 0xFF &&
        EEPROM.read(this->kValueAddress + 2) == 0xFF && EEPROM.read(this->kValueAddress + 3) == 0xFF) {
        this->kValue = 1.0;   // default value: K = 1.0
        EEPROM_write(this->kValueAddress, this->kValue);
    }
}

boolean GravityTDS::cmdSerialDataAvailable() {
    char cmdReceivedChar;
    static unsigned long cmdReceivedTimeOut = millis();
    while (Serial.available() > 0) {
        if (millis() - cmdReceivedTimeOut > 500U) {
            cmdReceivedBufferIndex = 0;
            memset(cmdReceivedBuffer, 0, (ReceivedBufferLength + 1));
        }
        cmdReceivedTimeOut = millis();
        cmdReceivedChar = Serial.read();
        if (cmdReceivedChar == '\n' || cmdReceivedBufferIndex == ReceivedBufferLength) {
            cmdReceivedBufferIndex = 0;
            strupr(cmdReceivedBuffer);
            return true;
        } else {
            cmdReceivedBuffer[cmdReceivedBufferIndex] = cmdReceivedChar;
            cmdReceivedBufferIndex++;
        }
    }
    return false;
}

byte GravityTDS::cmdParse() {
    byte modeIndex = 0;
    if (strstr(cmdReceivedBuffer, "ENTER") != NULL)
        modeIndex = 1;
    else if (strstr(cmdReceivedBuffer, "EXIT") != NULL)
        modeIndex = 3;
    else if (strstr(cmdReceivedBuffer, "CAL:") != NULL)
        modeIndex = 2;
    return modeIndex;
}

void GravityTDS::ecCalibration(byte mode) {
    char *cmdReceivedBufferPtr;
    static boolean ecCalibrationFinish = 0;
    static boolean enterCalibrationFlag = 0;
    float KValueTemp, rawECsolution;
    switch (mode) {
        case 0:
            if (enterCalibrationFlag)
                Serial.println(F("Command Error"));
            break;

        case 1:
            enterCalibrationFlag = 1;
            ecCalibrationFinish = 0;
            Serial.println();
            Serial.println(F(">>>Enter Calibration Mode<<<"));
            Serial.println(F(">>>Please put the probe into the standard buffer solution<<<"));
            Serial.println();
            break;

        case 2:
            cmdReceivedBufferPtr = strstr(cmdReceivedBuffer, "CAL:");
            cmdReceivedBufferPtr += strlen("CAL:");
            rawECsolution = strtod(cmdReceivedBufferPtr, NULL) / (float) (TdsFactor);
            rawECsolution = rawECsolution * (1.0 + 0.02 * (temperature - 25.0));
            if (enterCalibrationFlag) {
                // Serial.print("rawECsolution:");
                // Serial.print(rawECsolution);
                // Serial.print("  ecvalue:");
                // Serial.println(ecValue);
                KValueTemp = rawECsolution / (133.42 * voltage * voltage * voltage - 255.86 * voltage * voltage +
                                              857.39 *
                                              voltage);  //calibrate in the  buffer solution, such as 707ppm(1413us/cm)@25^c
                if ((rawECsolution > 0) && (rawECsolution < 2000) && (KValueTemp > 0.25) && (KValueTemp < 4.0)) {
                    Serial.println();
                    Serial.print(F(">>>Confirm Successful,K:"));
                    Serial.print(KValueTemp);
                    Serial.println(F(", Send EXIT to Save and Exit<<<"));
                    kValue = KValueTemp;
                    ecCalibrationFinish = 1;
                } else {
                    Serial.println();
                    Serial.println(F(">>>Confirm Failed,Try Again<<<"));
                    Serial.println();
                    ecCalibrationFinish = 0;
                }
            }
            break;

        case 3:
            if (enterCalibrationFlag) {
                Serial.println();
                if (ecCalibrationFinish) {
                    EEPROM_write(kValueAddress, kValue);
                    Serial.print(F(">>>Calibration Successful,K Value Saved"));
                } else Serial.print(F(">>>Calibration Failed"));
                Serial.println(F(",Exit Calibration Mode<<<"));
                Serial.println();
                ecCalibrationFinish = 0;
                enterCalibrationFlag = 0;
            }
            break;
    }
}

TDSSens::TDSSens(uint8_t _pin, float *_temp, float _vref, float _adcRange)
        : sensorTemp(_temp),
          sensorPin(_pin),
          sensorTimer(0) {
    GravityTDS::setPin(sensorPin);
    GravityTDS::setAref(_vref);
    GravityTDS::setAdcRange(_adcRange);
}

TDSSens::~TDSSens() = default;

bool TDSSens::init() {
    if (strcmp(name, "") == 0 && doc == nullptr) {
        name = "TDSSens";
        doc = new JsonDocument;
    }
#if defined(ESP32)
    if (!EEPROM.begin(512)) return false;
#endif
    (*doc)[name] = 0;
    GravityTDS::begin();
    return true;
}

bool TDSSens::update() {
    if (millis() - sensorTimer >= 500) {
        GravityTDS::setTemperature(*sensorTemp);
        GravityTDS::update();
        (*doc)[name] = GravityTDS::getTdsValue();
        sensorTimer = millis();
        return true;
    }
    return false;
}

void TDSSens::setDocument(const char *objName) {
    name = objName;
}

void TDSSens::setDocumentValue(JsonDocument *docBase) {
    doc = docBase;
}

JsonDocument TDSSens::getDocument() {
    return (*doc);
}

JsonVariant TDSSens::getVariant(const char *searchName) {
    return (*doc)[searchName];
}

float TDSSens::getValueTDSSens() const {
    return (*doc)[name].as<float>();
}

void TDSSens::setPins(uint8_t _pin) {
    sensorPin = _pin;
}
#endif  // TDS_SENS_H
#endif
/*
 *  max3010x-sens.cpp
 *
 *  max3010x sensor c
 *  Created on: 2023. 4. 3
 */

#include "max3010xv2-sens.h"
#include "Arduino.h"

MAX3010xV2Sens::~MAX3010xV2Sens() = default;

bool MAX3010xV2Sens::init() {
#if defined(ESP32) || defined(ESP8266)
    disableLoopWDT();
#endif
    if (strcmp(name, "") == 0 && doc == nullptr) {
        name = "MAX3010xV2Sens";
        doc = new JsonDocument;
    }
    if (!MAX30105::begin(Wire, I2C_SPEED_FAST)) Serial.println(F("MAX30105 was not found. Please check wiring/power."));

    byte ledBrightness = 60;  //Options: 0=Off to 255=50mA
    byte sampleAverage = 4;   //Options: 1, 2, 4, 8, 16, 32
    byte ledMode = 2;         //Options: 1 = Red only, 2 = Red + IR, 3 = Red + IR + Green
    byte sampleRate = 100;    //Options: 50, 100, 200, 400, 800, 1000, 1600, 3200
    int pulseWidth = 411;     //Options: 69, 118, 215, 411
    int adcRange = 4096;      //Options: 2048, 4096, 8192, 16384

    MAX30105::setup(ledBrightness, sampleAverage, ledMode, sampleRate, pulseWidth,
                    adcRange);  //Configure sensor with these settings
    bufferLength = 100;
    for (byte i = 0; i < bufferLength; i++) {
        while (MAX30105::available() == false)
            MAX30105::check();
        redBuffer[i] = MAX30105::getRed();
        irBuffer[i] = MAX30105::getIR();
        MAX30105::nextSample();
    }
#if defined(ESP32) || defined(ESP8266)
    maxim_heart_rate_and_oxygen_saturation(irBuffer, bufferLength, redBuffer, &spo2, &validSPO2, &heartRate,
                                           &validHeartRate);
#endif
    (*doc)[name]["hr"] = 0.0;
    (*doc)[name]["spo02"] = 0.0;
#if defined(ESP32) || defined(ESP8266)
    enableLoopWDT();
#endif
    return true;
}

bool MAX3010xV2Sens::update() {
    unsigned long currentMillis = millis();

    if (currentMillis - previousMillis >= interval) {
        previousMillis = currentMillis;

        if (step == 0) {
            // Step 1: Copy buffer
            if (index < 100) {
                redBuffer[index - 25] = redBuffer[index];
                irBuffer[index - 25] = irBuffer[index];
                index++;
            } else {
                index = 75;
                step++;
            }
        } else if (step == 1) {
            // Step 2: Update buffer with new data
            if (index < 100) {
                while (MAX30105::available() == false)
                    MAX30105::check();
                redBuffer[index] = MAX30105::getRed();
                irBuffer[index] = MAX30105::getIR();
                MAX30105::nextSample();

                if (validHeartRate) (*doc)[name]["hr"] = heartRate;
                if (validSPO2) (*doc)[name]["spo02"] = spo2;

                index++;
            } else {
                step++;
            }
        } else if (step == 2) {
#if defined(ESP32) || defined(ESP8266)
            maxim_heart_rate_and_oxygen_saturation(irBuffer, bufferLength, redBuffer, &spo2, &validSPO2, &heartRate,
                                                   &validHeartRate);
#endif
            index = 25;
            step = 0;
            return true;
        }
    }
    return false;
}

void MAX3010xV2Sens::setDocument(const char *objName) {
    name = objName;
}

void MAX3010xV2Sens::setDocumentValue(JsonDocument *docBase) {
    doc = docBase;
}

JsonDocument MAX3010xV2Sens::getDocument() {
    return (*doc);
}

JsonVariant MAX3010xV2Sens::getVariant(const char *searchName) {
    return (*doc)[searchName];
}

float MAX3010xV2Sens::getValueMAX3010xV2Sens() const {
    return (*doc)[name].as<float>();
}

void MAX3010xV2Sens::setPins(uint8_t _pin) {
    sensorPin = _pin;
}
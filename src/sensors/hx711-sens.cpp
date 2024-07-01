/*
 *  hx711-sens.cpp
 *
 *  hx711 sensor c
 *  Created on: 2023. 4. 3
 */

#include "hx711-sens.h"
#include "Arduino.h"

HX711Sens::HX711Sens(uint8_t _sensorDOUTPin, uint8_t _sensorSCKPin, float _format)
        : name(""),
          sensorTimer{0, 0},
          sensorDOUTPin(_sensorDOUTPin),
          sensorSCKPin(_sensorSCKPin),
          format(_format),
          sensorFilterCb(nullptr) {
}

bool HX711Sens::init() {
    this->begin(this->sensorDOUTPin, this->sensorSCKPin);
    if (strcmp(name, "") == 0 && doc == nullptr) {
        name = "HX711Sens";
        doc = new JsonDocument;
    }
    (*doc)[name] = 0;
    return true;
}

bool HX711Sens::update() {
    if (millis() - sensorTimer[0] >= 500) {
        if (this->is_ready()) {
            float units = this->get_units();
            if (units < 0) units = 0.0;
            units = units / format;
            if (sensorFilterCb != nullptr) {
                units = sensorFilterCb(units);
            }
            (*doc)[name] = units;
            return true;
        }
        sensorTimer[0] = millis();
    }
    return false;
}

void HX711Sens::setDocument(const char *objName) {
    name = objName;
}

void HX711Sens::setDocumentValue(JsonDocument *docBase) {
    doc = docBase;
}

JsonDocument HX711Sens::getDocument() {
    return (*doc);
}

JsonVariant HX711Sens::getVariant(const char *searchName) {
    return (*doc)[searchName];
}

void HX711Sens::filter(float (*sensorFilterCallback)(float value)) {
    sensorFilterCb = sensorFilterCallback;
}

float HX711Sens::getCalibrateFactorInit(float weight) {
    float calibrationFactor = 0.0;
    if (this->is_ready()) {
        this->set_scale();
        Serial.println("Tare... remove any weights from the scale.");
        delay(5000);
        this->tare();
        Serial.println("Tare done...");
        Serial.print("Place a known weight on the scale...");
        delay(5000);
        float reading = this->get_units(10);
        Serial.print("| reading: ");
        Serial.print(reading);
        Serial.print("| result: ");
        Serial.print(reading / weight);
        Serial.println();
        delay(5000);
        return reading;
    }
    return calibrationFactor;
}

bool HX711Sens::isReady() {
    return this->is_ready();
}

void HX711Sens::setScaleDelay(long time, float scale) {
    this->set_scale(scale);
    delay(time);
}

void HX711Sens::setScale(float scale) {
    this->set_scale(scale);
}

void HX711Sens::tareDelay(long time) {
    this->tare();
    delay(time);
}

float HX711Sens::getUnits(byte time) {
    return this->get_units(time);
}

uint32_t HX711Sens::getADC(byte times) {
    return this->read_average(times);
}

float HX711Sens::getCalibrateFactor(float units, float weight) {
    return units / weight;
}

float HX711Sens::getValueWeight(bool isCanZero) const {
    float value = (*doc)[name].as<float>();
    if (isCanZero) return value;
    return value < 0 ? 0 : value;
}

void HX711Sens::setPins(uint8_t _sensorDOUTPin, uint8_t _sensorSCKPin) {
    this->sensorDOUTPin = _sensorDOUTPin;
    this->sensorSCKPin = _sensorSCKPin;
}
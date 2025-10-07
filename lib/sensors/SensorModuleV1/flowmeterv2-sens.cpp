/*
 *  flowmeterv2-sens.cpp
 *
 *  flowmeterv2 sensor c
 *  Created on: 2023. 4. 3
 */

#include "flowmeterv2-sens.h"
#include "Arduino.h"

FlowSensor::FlowSensor(uint16_t type, uint8_t pin) {
    this->_pin = pin;
    this->_pulse1liter = type;
    this->_interval = 1000;  // default interval 1 detik
}

void FlowSensor::setType(uint16_t type) {
    this->_pulse1liter = type;
}

void FlowSensor::setPin(uint8_t pin) {
    detachInterrupt(this->_pin);
    this->_pin = pin;
}

void FlowSensor::begin(void (*userFunc)(void), bool pullup) {
    if (pullup)
        pinMode(this->_pin, INPUT);
    else
        pinMode(this->_pin, INPUT_PULLUP);

    attachInterrupt(digitalPinToInterrupt(this->_pin), userFunc, RISING);
}

void FlowSensor::count() {
    this->_pulse++;
}

void FlowSensor::setInterval(unsigned long interval) {
    this->_interval = interval;
}

bool FlowSensor::_isTimeToRead() {
    return (millis() - this->_timebefore >= this->_interval);
}

bool FlowSensor::read(long calibration) {
    if (!_isTimeToRead()) return false;

    float timeInSeconds = this->_interval / 1000.0;
    this->_flowratesecound = (this->_pulse / (this->_pulse1liter + calibration)) / timeInSeconds;
    this->_volume += (this->_pulse / (this->_pulse1liter + calibration));
    this->_totalpulse += this->_pulse;
    this->_pulse = 0;
    this->_timebefore = millis();
    return true;
}

unsigned long FlowSensor::getPulse() {
    return this->_totalpulse;
}

float FlowSensor::getFlowRate_h() {
    this->_flowratehour = this->_flowratesecound * 3600;
    return this->_flowratehour;
}

float FlowSensor::getFlowRate_m() {
    this->_flowrateminute = this->_flowratesecound * 60;
    return this->_flowrateminute;
}

float FlowSensor::getFlowRate_s() {
    return this->_flowratesecound;
}

float FlowSensor::getVolume() {
    return this->_volume;
}

void FlowSensor::resetPulse() {
    this->_pulse = 0;
    this->_totalpulse = 0;
}

void FlowSensor::resetVolume() {
    this->_volume = 0;
}

//////////////////////////////////////////////////////////////////////////////////////////

FlowMeterV2Sens::FlowMeterV2Sens(uint16_t type, uint8_t pin, uint32_t interval, void (*callback)(void))
        : FlowSensor(type, pin) {
    FlowSensor::begin(callback);
    FlowSensor::setInterval(interval);
    FlowSensor::resetVolume();
}

FlowMeterV2Sens::~FlowMeterV2Sens() = default;

bool FlowMeterV2Sens::init() {
    if (strcmp(name, "") == 0 && doc == nullptr) {
        name = "FlowMeterV2Sens";
        doc = new JsonDocument;
    }
    (*doc)[name]["rate"] = 0.0;
    (*doc)[name]["volume"] = 0.0;
    return true;
}

bool FlowMeterV2Sens::update() {
    FlowSensor::read();
    (*doc)[name]["rate"] = FlowSensor::getFlowRate_m();
    (*doc)[name]["volume"] = FlowSensor::getVolume();
    return true;
}

void FlowMeterV2Sens::setDocument(const char *objName) {
    name = objName;
}

void FlowMeterV2Sens::setDocumentValue(JsonDocument *docBase) {
    doc = docBase;
}

JsonDocument FlowMeterV2Sens::getDocument() {
    return (*doc);
}

JsonVariant FlowMeterV2Sens::getVariant(const char *searchName) {
    return (*doc)[searchName];
}

float FlowMeterV2Sens::getValueFlowMeterV2Sens() const {
    return (*doc)[name].as<float>();
}

void FlowMeterV2Sens::setPins(uint8_t _pin) {
    sensorPin = _pin;
}

void FlowMeterV2Sens::readInterrupt() {
    FlowSensor::count();
}
/*
 *  rpm-sens.cpp
 *
 *  rpm sensor c
 *  Created on: 2023. 4. 3
 */
#ifdef USE_RPM_SENS
#include "rpm-sens.h"
#include "Arduino.h"
//
//RPMSens::RPMSens(int _encoderPin, void (*_interruptCallback)(), int _mode)
//        : doc(nullptr),
//          name(""),
//          encoderCount(0),
//          interval(100),
//          encoderPin(_encoderPin),
//          interruptCallback(_interruptCallback),
//          mode(_mode) {
//}
//
//RPMSens::~RPMSens() = default;
//
//bool RPMSens::init() {
//    if (strcmp(name, "") == 0 && doc == nullptr) {
//        name = "RPMSens";
//        doc = new JsonDocument;
//    }
//    pinMode(encoderPin, INPUT_PULLUP);
//    attachInterrupt(digitalPinToInterrupt(encoderPin), interruptCallback, mode);
//    (*doc)[name] = 0;
//    return true;
//}
//
//bool RPMSens::update() {
//    if (millis() - sensorTimer >= interval) {
//        float rpmValue = (encoderCount / 20.0) * 60 * 10;
//        rpmValue = rpmValue < 0 ? 0 : rpmValue;
//        (*doc)[name] = rpmValue;
//        encoderCount = 0;
//        sensorTimer = millis();
//        return true;
//    }
//    return false;
//}
//
//void RPMSens::count() {
//    this->encoderCount++;
//}
//
//void RPMSens::setDocument(const char *objName) {
//    name = objName;
//}
//
//void RPMSens::setDocumentValue(JsonDocument *docBase) {
//    doc = docBase;
//}
//
//JsonDocument RPMSens::getDocument() {
//    return (*doc);
//}
//
//JsonVariant RPMSens::getVariant(const char *searchName) {
//    return (*doc)[searchName];
//}
//
//float RPMSens::getValueRPMSens() const {
//    return (*doc)[name].as<float>();
//}
//
//void RPMSens::setPins(uint8_t _pin) {
//    sensorPin = _pin;
//}
#endif
/*
 *  gm67-sens.cpp
 *
 *  gm67 sensor c
 *  Created on: 2023. 4. 3
 */
#ifdef USE_GM67_SENS
#include "gm67-sens.h"
#include "Arduino.h"

//#if defined(ESP32)
//
//GM67Sens::GM67Sens(HardwareSerial *_serialPtr, long baud, SerialConfig cfg,
//                   int8_t rxPin, int8_t txPin) {
//    serialPtr = _serialPtr;
//    serialPtr->begin(baud, cfg, rxPin, txPin);
//    serialPtr->setTimeout(100);
//}
//
//#endif
//
//GM67Sens::~GM67Sens() = default;
//
//bool GM67Sens::init() {
//    if (strcmp(name, "") == 0 && doc == nullptr) {
//        name = "GM67Sens";
//        doc = new JsonDocument;
//    }
//    (*doc)[name] = "";
//    return true;
//}
//
//bool GM67Sens::update() {
//#if defined(ESP32)
//    if (serialPtr->available()) {
//        String code = serialPtr->readStringUntil('\r');
//        (*doc)[name] = code;
//        return true;
//    }
//#endif
//    (*doc)[name] = "";
//    return false;
//}
//
//void GM67Sens::setDocument(const char *objName) {
//    name = objName;
//}
//
//void GM67Sens::setDocumentValue(JsonDocument *docBase) {
//    doc = docBase;
//}
//
//JsonDocument GM67Sens::getDocument() {
//    return (*doc);
//}
//
//JsonVariant GM67Sens::getVariant(const char *searchName) {
//    return (*doc)[searchName];
//}
//
//float GM67Sens::getValueGM67Sens() const {
//    return (*doc)[name].as<float>();
//}
//
//void GM67Sens::setPins(uint8_t _pin) {
//    sensorPin = _pin;
//}

#endif

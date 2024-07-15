/*
 *  aht-sens.cpp
 *
 *  aht sensor c
 *  Created on: 2023. 4. 3
 */


#ifdef USE_AHT_SENS

#include "aht-sens.h"
#include "Arduino.h"
//
//AHTSens::~AHTSens() = default;
//
//bool AHTSens::init() {
//    if (strcmp(name, "") == 0 && doc == nullptr) {
//        name = "AHTSens";
//        doc = new JsonDocument;
//    }
//    if (!Adafruit_AHTX0::begin()) {
//        return false;
//    }
//    (*doc)[name]["temp"] = 0.0;
//    (*doc)[name]["hum"] = 0.0;
//    return true;
//}
//
//bool AHTSens::update() {
//    if (millis() - sensorTimer >= 500) {
//        sensors_event_t humidity, temp;
//        Adafruit_AHTX0::getEvent(&humidity, &temp);
//        (*doc)[name]["temp"] = temp.temperature;
//        (*doc)[name]["hum"] = humidity.relative_humidity;
//        sensorTimer = millis();
//        return true;
//    }
//    return false;
//}
//
//void AHTSens::setDocument(const char *objName) {
//    name = objName;
//}
//
//void AHTSens::setDocumentValue(JsonDocument *docBase) {
//    doc = docBase;
//}
//
//JsonDocument AHTSens::getDocument() {
//    return (*doc);
//}
//
//JsonVariant AHTSens::getVariant(const char *searchName) {
//    return (*doc)[searchName];
//}
//
//float AHTSens::getValueAHTSens() const {
//    return (*doc)[name].as<float>();
//}
//
//void AHTSens::setPins(uint8_t _pin) {
//    sensorPin = _pin;
//}

#endif
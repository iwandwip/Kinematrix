/*
 *  scd30-sens.cpp
 *
 *  scd30 sensor c
 *  Created on: 2023. 4. 3
 */

#include "scd30-sens.h"

SCD30Sens::~SCD30Sens() = default;

bool SCD30Sens::init() {
    if (strcmp(name, "") == 0 && doc == nullptr) {
        name = "SCD30Sens";
        doc = new JsonDocument;
    }
    Wire.begin();
    SensirionI2cScd30::begin(Wire, SCD30_I2C_ADDR_61);

    SensirionI2cScd30::stopPeriodicMeasurement();
    SensirionI2cScd30::softReset();
    delay(2000);

    uint8_t major = 0;
    uint8_t minor = 0;
    error = SensirionI2cScd30::readFirmwareVersion(major, minor);
    if (error != NO_ERROR) {
        Serial.print("Error trying to execute readFirmwareVersion(): ");
        errorToString(error, errorMessage, sizeof errorMessage);
        Serial.println(errorMessage);
        return false;
    }
    Serial.print("firmware version major: ");
    Serial.print(major);
    Serial.print("\t");
    Serial.print("minor: ");
    Serial.print(minor);
    Serial.println();

    error = SensirionI2cScd30::startPeriodicMeasurement(0);
    if (error != NO_ERROR) {
        Serial.print("Error trying to execute startPeriodicMeasurement(): ");
        errorToString(error, errorMessage, sizeof errorMessage);
        Serial.println(errorMessage);
        return false;
    }

    (*doc)[name]["co2"] = 0;
    (*doc)[name]["temp"] = 0;
    (*doc)[name]["hum"] = 0;
    return true;
}

bool SCD30Sens::update() {
    float co2Concentration = 0.0;
    float temperature = 0.0;
    float humidity = 0.0;

    error = SensirionI2cScd30::blockingReadMeasurementData(co2Concentration, temperature, humidity);
    if (error != NO_ERROR) {
        errorToString(error, errorMessage, sizeof errorMessage);
        return false;
    }

    (*doc)[name]["co2"] = co2Concentration;
    (*doc)[name]["temp"] = temperature;
    (*doc)[name]["hum"] = humidity;

    return true;
}

void SCD30Sens::setDocument(const char *objName) {
    name = objName;
}

void SCD30Sens::setDocumentValue(JsonDocument *docBase) {
    doc = docBase;
}

JsonDocument SCD30Sens::getDocument() {
    return (*doc);
}

JsonVariant SCD30Sens::getVariant(const char *searchName) {
    return (*doc)[searchName];
}
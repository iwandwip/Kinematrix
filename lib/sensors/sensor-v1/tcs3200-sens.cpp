/*
 *  tcs3200-sens.cpp
 *
 *  tcs3200 sensor c
 *  Created on: 2023. 4. 3
 */

#include "tcs3200-sens.h"
#include "Arduino.h"

TCS3200Sens::TCS3200Sens(uint8_t S0, uint8_t S1, uint8_t S2, uint8_t S3, uint8_t output)
        : tcs3200(S0, S1, S2, S3, output) {
}

TCS3200Sens::TCS3200Sens(uint8_t S2, uint8_t S3, uint8_t output)
        : tcs3200(S2, S2, S2, S3, output) {
}

TCS3200Sens::~TCS3200Sens() = default;

bool TCS3200Sens::init() {
    if (strcmp(name, "") == 0 && doc == nullptr) {
        name = "TCS3200Sens";
        doc = new JsonDocument;
    }
    (*doc)[name]["r"] = 0;
    (*doc)[name]["g"] = 0;
    (*doc)[name]["b"] = 0;
    return true;
}

bool TCS3200Sens::update() {
    if (millis() - sensorTimer >= 200) {
        (*doc)[name]["r"] = tcs3200::colorRead('r', 100);
        (*doc)[name]["g"] = tcs3200::colorRead('g', 100);
        (*doc)[name]["b"] = tcs3200::colorRead('b', 100);
        sensorTimer = millis();
        return true;
    }
    return false;
}

void TCS3200Sens::setDocument(const char *objName) {
    name = objName;
}

void TCS3200Sens::setDocumentValue(JsonDocument *docBase) {
    doc = docBase;
}

JsonDocument TCS3200Sens::getDocument() {
    return (*doc);
}

JsonVariant TCS3200Sens::getVariant(const char *searchName) {
    return (*doc)[searchName];
}

String TCS3200Sens::closestColorCustomRGB(int r, int g, int b, int distinctRGB[][3], String distinctColors[], int num_of_colors) {
    int index = -1;
    int biggestDifference = 765;
    for (int i = 0; i < num_of_colors; i++) {
        int difference = (int) sqrt(pow(r - distinctRGB[i][0], 2) + pow(g - distinctRGB[i][1], 2) + pow(b - distinctRGB[i][2], 2));
        if (difference < biggestDifference) {
            index = i;
            biggestDifference = difference;
        }
    }
    String colorReturn = "NA";
    return (index == -1 ? colorReturn : distinctColors[index]);
}

float TCS3200Sens::getValueTCS3200Sens() const {
    return (*doc)[name].as<float>();
}

void TCS3200Sens::setPins(uint8_t _pin) {
    sensorPin = _pin;
}
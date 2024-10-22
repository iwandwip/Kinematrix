/*
 *  mlx90614-sens.cpp
 *
 *  mlx90614 sensor c
 *  Created on: 2023. 4. 3
 */

#include "mlx90614-sens.h"
#include "Arduino.h"

MLX90614Sens::MLX90614Sens()
        : doc(nullptr),
          name("") {}

MLX90614Sens::~MLX90614Sens() = default;

bool MLX90614Sens::init() {
    if (strcmp(name, "") == 0 && doc == nullptr) {
        name = "MLX90614Sens";
        doc = new JsonDocument;
    }
    if (!Adafruit_MLX90614::begin()) return false;
    Adafruit_MLX90614::readEmissivity();
    (*doc)[name] = 0;
    return true;
}

bool MLX90614Sens::update() {
    if (millis() - sensorTimer >= 500) {
        (*doc)[name] = Adafruit_MLX90614::readObjectTempC();
        sensorTimer = millis();
    }
    return true;
}

void MLX90614Sens::setDocument(const char *objName) {
    name = objName;
}

void MLX90614Sens::setDocumentValue(JsonDocument *docBase) {
    doc = docBase;
}

JsonDocument MLX90614Sens::getDocument() {
    return (*doc);
}

JsonVariant MLX90614Sens::getVariant(const char *searchName) {
    return (*doc)[searchName];
}

float MLX90614Sens::getValueMLX90614Sens() const {
    return (*doc)[name].as<float>();
}

void MLX90614Sens::setPins(uint8_t _pin) {
    sensorPin = _pin;
}
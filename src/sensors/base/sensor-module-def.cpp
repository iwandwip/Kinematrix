/*
 *  sensor-module-def.cpp
 *
 *  sensor module definition c
 *  Created on: 2023. 4. 3
 */

#include "sensor-module.h"

void BaseSens::getValue(float *output) {
    /*not implemented yet*/
}

void BaseSens::getValue(int *output) {
    /*not implemented yet*/
}

float BaseSens::getValueF() const {
    return 0;
}

void BaseSens::setDocument(const char *objName) {
    /*not implemented yet*/
}

void BaseSens::setDocumentValue(JsonDocument *docBase) {
    /*not implemented yet*/
}

JsonDocument BaseSens::getDocument() {
    return JsonDocument();
}

JsonVariant BaseSens::getVariant(const char *searchName) {
    return JsonVariant();
}

void BaseSens::process() {
    /*not implemented yet*/
}
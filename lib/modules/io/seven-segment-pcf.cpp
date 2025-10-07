/*
 *  seven-segment.cpp
 *
 *  seven segment c
 *  Created on: 2023. 4. 3
 */

#include "seven-segment-pcf.h"

uint8_t sevenSegmentMapPCF[12][8] = {
        {1, 1, 1, 1, 1, 1, 0, 0},  // 0
        {0, 1, 1, 0, 0, 0, 0, 0},  // 1
        {1, 1, 0, 1, 1, 0, 1, 0},  // 2
        {1, 1, 1, 1, 0, 0, 1, 0},  // 3
        {0, 1, 1, 0, 0, 1, 1, 0},  // 4
        {1, 0, 1, 1, 0, 1, 1, 0},  // 5
        {1, 0, 1, 1, 1, 1, 1, 0},  // 6
        {1, 1, 1, 0, 0, 0, 0, 0},  // 7
        {1, 1, 1, 1, 1, 1, 1, 0},  // 8
        {1, 1, 1, 1, 0, 1, 1, 0},  // 9
        {0, 0, 0, 0, 0, 0, 1, 0},  // -
        {0, 0, 0, 0, 0, 0, 0, 1},  // .
};

SevenSegmentPCF::SevenSegmentPCF(uint8_t *address, int numDisplays, bool reverse) {
    this->address = address;
    this->numDisplays = numDisplays;
    this->reverse = reverse;
}

SevenSegmentPCF::~SevenSegmentPCF() {
    delete[] address;
}

void SevenSegmentPCF::processNumber(const String &_number) {
    String numberStr = _number;
    numberStr.trim();

    float number = numberStr.toFloat();
    unsigned int length = numberStr.length();
    if (numberStr.indexOf('.') != -1 && numberStr[length - 1] == '0') length--;

    double maxNumber = pow(10, numDisplays) - 1;
    double minNumber = -maxNumber / 10;
    if (number < minNumber || number > maxNumber || length > numDisplays) return;

    for (int i = 0; i < numDisplays; i++) {
        char digit = i < length ? numberStr[i] : ' ';
        displayDigit(digit, address[i]);
    }
}

void SevenSegmentPCF::displayDigit(char digit, uint8_t _address) const {
    PCF8574 pcf(0x00);
    int index;

    if (digit == '-') index = 10;
    else if (digit == '.') index = 11;
    else if (digit >= '0' && digit <= '9') index = digit - '0';
    else index = -1;

    if (index >= 0 && index <= 11) {
        for (int i = 0; i < 8; ++i) {
            pcf.pinMode(i, OUTPUT, sevenSegmentMapPCF[index][i] ? reverse ? LOW : HIGH : reverse ? HIGH : LOW);
        }
    } else {
        for (int i = 0; i < 8; ++i) {
            pcf.pinMode(i, OUTPUT, reverse ? HIGH : LOW);
        }
    }
    pcf.begin(_address);
}
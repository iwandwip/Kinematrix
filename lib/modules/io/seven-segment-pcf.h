/*
 *  seven-segment.h
 *
 *  seven segment header
 *  Created on: 2023. 4. 3
 */

#pragma once

#ifndef KINEMATRIX_SEVEN_SEGMENT_PCF
#define KINEMATRIX_SEVEN_SEGMENT_PCF

#include "Arduino.h"
#include "PCF8574.h"

#pragma message("[COMPILED]: seven-segment-pcf.h")

class SevenSegmentPCF {
private:
    uint8_t *address;
    int numDisplays;
    bool reverse;
    void processNumber(const String &_number);
    void displayDigit(char digit, uint8_t _address) const;

public:
    SevenSegmentPCF(uint8_t *address, int numDisplays, bool reverse = false);
    ~SevenSegmentPCF();

    template<typename T = float>
    void displayNumber(T number) {
        processNumber(String(number));
    }
};

#endif  // KINEMATRIX_SEVEN_SEGMENT_PCF
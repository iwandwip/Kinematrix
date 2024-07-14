/*
 *  seven-segment.h
 *
 *  seven segment header
 *  Created on: 2023. 4. 3
 */
#ifdef USE_SEVEN_SEGMENT
#pragma once

#ifndef KINEMATRIX_SEVEN_SEGMENT
#define KINEMATRIX_SEVEN_SEGMENT

#include "Arduino.h"

typedef int SevenSegmentEnable;

class SevenSegment {
private:
    int pinA, pinB, pinC, pinD;
    int *enablePins;
    int numDisplays;
    void displayDigit(int digit);

public:
    SevenSegment(int pinA, int pinB, int pinC, int pinD, int *enablePins, int numDisplays);
    SevenSegment(int pinA, int pinB, int pinC, int pinD);
    ~SevenSegment();
    void displayNumber(int number);
};

#endif  // KINEMATRIX_SEVEN_SEGMENT
#endif
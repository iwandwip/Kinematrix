/*
 *  seven-segment-74hc595.h
 *
 *  seven segment 74hc595 header
 *  Created on: 2023. 4. 3
 */

#pragma once

#ifndef KINEMATRIX_SEVEN_SEGMENT_74HC595
#define KINEMATRIX_SEVEN_SEGMENT_74HC595

#include "Arduino.h"

#pragma message("[COMPILED]: seven-segment-74hc595.h")

class SevenSegment74HC595 {
private:
    int dataPin;
    int clockPin;
    int *enablePins;

    int numDisplays;
    bool reverse;

    void processNumber(const String &_number, uint32_t _time);
    void displayDigit(char digit, bool isThousand) const;

    static byte reverseByte(byte input);
    static void shiftArray(int *arr, int size);

public:
    SevenSegment74HC595(int dataPin, int clockPin, const int *enablePins, int numDisplays, bool reverse = false);
    ~SevenSegment74HC595();

    template<typename T = float>
    void displayNumber(T number, uint32_t _time = 5) {
        processNumber(String(number), _time);
    }
};

#endif  // KINEMATRIX_SEVEN_SEGMENT_74HC595
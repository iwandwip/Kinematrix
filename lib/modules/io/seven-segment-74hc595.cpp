/*
 *  seven-segment-74hc595.cpp
 *
 *  seven segment 74hc595 c
 *  Created on: 2023. 4. 3
 */

#include "seven-segment-74hc595.h"

const byte sevenSegmentMap74HC595[12] = { // MSB (Most Significant Bit)
        //H, G, F, E, D, C, B, A, DOT
        B01111110, // 0
        B00001100, // 1
        B10110110, // 2
        B10011110, // 3
        B11001100, // 4
        B11011010, // 5
        B11111010, // 6
        B00001110, // 7
        B11111110, // 8
        B11011110,  // 9,
        B10000000,  // -,
        B00000001,  // .,
};

SevenSegment74HC595::SevenSegment74HC595(int dataPin, int clockPin, const int *enablePins, int numDisplays, bool enableState, bool ledState) {
    this->dataPin = dataPin;
    this->clockPin = clockPin;
    this->numDisplays = numDisplays;

    this->enableState = enableState;
    this->ledState = ledState;

    this->enablePins = new int[numDisplays];
    for (int i = 0; i < numDisplays; i++) {
        this->enablePins[i] = enablePins[i];
        pinMode(this->enablePins[i], OUTPUT);

        if (this->enableState == COMMON_ANODE) {
            digitalWrite(this->enablePins[i], LOW);
        } else {
            digitalWrite(this->enablePins[i], HIGH);
        }
    }

    pinMode(dataPin, OUTPUT);
    pinMode(clockPin, OUTPUT);
}

SevenSegment74HC595::~SevenSegment74HC595() {
    delete[] enablePins;
}

void SevenSegment74HC595::processNumber(const String &_number, uint32_t _time) {
    String numberStr = _number;
    numberStr.trim();

    float number = numberStr.toFloat();
    int length = int(numberStr.length());

    if (numberStr.indexOf('.') != -1 && numberStr[length - 1] == '0') length--;

    double maxNumber = pow(10, numDisplays) - 1;
    double minNumber = -maxNumber / 10;
    if (number < minNumber || number > maxNumber || length > numDisplays) return;

    int thousand[length];
    int count = 0;

    for (int i = 0; i < length; i++) {
        count++;
        bool isThousand = (length - count) % 3 == 0 && i != length - 1;
        thousand[i] = isThousand;
    }

    shiftArray(thousand, length);

    for (int i = 0; i < length; i++) {
        char digit = i < length ? numberStr[i] : ' ';

        displayDigit(digit, thousand[i]);

        if (this->enableState == COMMON_ANODE) {
            digitalWrite(enablePins[i], HIGH);
            delay(_time);
            digitalWrite(enablePins[i], LOW);
        } else {
            digitalWrite(enablePins[i], LOW);
            delay(_time);
            digitalWrite(enablePins[i], HIGH);
        }
    }
}

void SevenSegment74HC595::displayDigit(char digit, bool isThousand) const {
    int value = -1;

    if (digit == '-') value = 10;
    else if (digit == '.') value = 11;
    else if (digit >= '0' && digit <= '9') value = digit - '0';

    if (value >= 0 && value < 12) {
        byte displayValue = sevenSegmentMap74HC595[value];
        if (isThousand) displayValue = displayValue | sevenSegmentMap74HC595[11];
//        displayValue = ~displayValue & 0xFF;
//        displayValue = reverseByte(displayValue);

        if (this->enableState == COMMON_ANODE) {
            shiftOut(dataPin, clockPin, MSBFIRST, ~displayValue);
        } else {
            shiftOut(dataPin, clockPin, MSBFIRST, displayValue);
        }
    } else {

        if (this->enableState == COMMON_ANODE) {
            shiftOut(dataPin, clockPin, MSBFIRST, B11111111);
        } else {
            shiftOut(dataPin, clockPin, MSBFIRST, B00000000);
        }
    }
}

byte SevenSegment74HC595::reverseByte(byte input) {
    byte reversed = 0;
    for (int i = 0; i < 8; i++) {
        reversed |= ((input >> i) & 1) << (7 - i);
    }
    return reversed;
}

void SevenSegment74HC595::shiftArray(int *arr, int size) {
    int temp = arr[0];
    for (int i = 0; i < size - 1; i++) {
        arr[i] = arr[i + 1];
    }
    arr[size - 1] = temp;
}
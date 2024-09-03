/*
 *  seven-segment.cpp
 *
 *  seven segment c
 *  Created on: 2023. 4. 3
 */

#include "seven-segment.h"

SevenSegment::SevenSegment(int pinA, int pinB, int pinC, int pinD, const int *enablePins, int numDisplays) {
    this->pinA = pinA;
    this->pinB = pinB;
    this->pinC = pinC;
    this->pinD = pinD;
    this->numDisplays = numDisplays;

    // Allocate memory for enable pins
    this->enablePins = new int[numDisplays];
    for (int i = 0; i < numDisplays; i++) {
        this->enablePins[i] = enablePins[i];
        pinMode(this->enablePins[i], OUTPUT);
        digitalWrite(this->enablePins[i], LOW); // Initialize enable pins to LOW
    }

    // Set the pins as output
    pinMode(pinA, OUTPUT);
    pinMode(pinB, OUTPUT);
    pinMode(pinC, OUTPUT);
    pinMode(pinD, OUTPUT);
}

// Constructor for single display (no enable pin)
SevenSegment::SevenSegment(int pinA, int pinB, int pinC, int pinD) {
    this->pinA = pinA;
    this->pinB = pinB;
    this->pinC = pinC;
    this->pinD = pinD;
    this->numDisplays = 1; // Set number of displays to 1

    // Set the pins as output
    pinMode(pinA, OUTPUT);
    pinMode(pinB, OUTPUT);
    pinMode(pinC, OUTPUT);
    pinMode(pinD, OUTPUT);

    this->enablePins = nullptr; // Set enablePins to nullptr since no enable pin is used
}

// Destructor
SevenSegment::~SevenSegment() {
    delete[] enablePins;
}

// Method to display a number
void SevenSegment::displayNumber(int number, uint32_t _delay_ms) {
    // Ensure the number is within the range that can be displayed
    double maxNumber = pow(10, numDisplays) - 1;
    double minNumber = -maxNumber / 10;
    if (number < minNumber || number > maxNumber) return;

    // Break the number into digits
    bool isNegative = number < 0;
    if (isNegative) number = -number;

    int digits[4] = {0, 0, 0, 0}; // Array to hold each digit
    for (int i = numDisplays - 1; i >= 0; i--) {
        digits[i] = number % 10;
        number /= 10;
    }

    // Display each digit one by one
    for (int i = 0; i < numDisplays; i++) {
        if (enablePins != nullptr) digitalWrite(enablePins[i], HIGH);
        if (isNegative && i == 0) {
            displayDigit(10); // Custom representation for '-'
        } else {
            displayDigit(digits[i]);
        }
        delay(_delay_ms); // Adjust delay for proper multiplexing
        if (enablePins != nullptr) digitalWrite(enablePins[i], LOW);
    }
}

void SevenSegment::displayDigit(int digit) const {
    if (digit < 0 || digit > 10) return; // Check for valid digit range (0-9 and custom '-')
    if (digit == 10) {
        // Custom representation for '-'
        digitalWrite(pinA, LOW);
        digitalWrite(pinB, LOW);
        digitalWrite(pinC, LOW);
        digitalWrite(pinD, HIGH);
    } else {
        digitalWrite(pinA, bitRead(digit, 0)); // LSB
        digitalWrite(pinB, bitRead(digit, 1));
        digitalWrite(pinC, bitRead(digit, 2));
        digitalWrite(pinD, bitRead(digit, 3)); // MSB
    }
}
/*
 *  seven-segment-raw.h
 *
 *  Raw 7-segment display library - Simple but powerful
 *  Created on: 2024. 12. 18
 */

#pragma once

#ifndef KINEMATRIX_SEVEN_SEGMENT_RAW
#define KINEMATRIX_SEVEN_SEGMENT_RAW

#include "Arduino.h"
#include <TimerOne.h>

#pragma message("[COMPILED]: seven-segment-raw.h")

#define SEGMENT_ACTIVE_HIGH  true
#define SEGMENT_ACTIVE_LOW   false
#define DIGIT_ACTIVE_HIGH    true
#define DIGIT_ACTIVE_LOW     false

// Constants for display patterns and limits
#define MAX_DIGITS           8
#define PATTERN_BLANK        16
#define PATTERN_DASH         17
#define DECIMAL_POINT_BIT    0x01
#define MIN_REFRESH_PERIOD   500
#define MAX_REFRESH_PERIOD   5000

// Common presets for convenience
#define COMMON_CATHODE SEGMENT_ACTIVE_LOW, DIGIT_ACTIVE_HIGH
#define COMMON_ANODE   SEGMENT_ACTIVE_HIGH, DIGIT_ACTIVE_LOW

// Timing modes for display refresh
enum TimingMode {
    TIMING_DELAY,      // Blocking delay (simple but blocks program)
    TIMING_MILLIS,     // Non-blocking millis() (default, compatible)
    TIMING_TIMER1      // Hardware Timer1 interrupt (smoothest)
};

// Padding modes for leading zeros/blanks
enum PaddingMode {
    PADDING_ZEROS,     // 0009
    PADDING_BLANKS,    // ___9  
    PADDING_DASHES,    // ---9
    PADDING_NONE       // Same as PADDING_BLANKS
};

class SevenSegmentRaw {
private:
    uint8_t *segmentPins;
    uint8_t *digitPins;
    uint8_t numDigits;
    bool segmentActiveHigh;
    bool digitActiveHigh;
    uint8_t brightness;
    uint32_t lastRefresh;
    volatile uint8_t currentDigit;
    PaddingMode paddingMode;
    TimingMode timingMode;

    static const uint8_t digitPatterns[18];
    
    // Timer1 integration (for TIMING_TIMER1 mode)
    static SevenSegmentRaw* instance;
    volatile uint8_t displayBuffer[MAX_DIGITS];
    volatile uint8_t dpPosition;
    volatile uint8_t decimals;
    static void timerISR();
    void refreshDisplayISR();
    void updateTimer();
    
    // Instance variables to replace static variables
    int32_t currentNumber;
    uint16_t currentHexNumber;
    float currentFloat;
    char currentText[MAX_DIGITS + 1];
    uint8_t currentDecimals;

    bool shouldPad(uint8_t position, int32_t number);
    void setSegment(uint8_t pattern);
    void selectDigit(uint8_t digit);
    void clearAll();
    void handleRefresh(); // Handle refresh based on timing mode

public:
    SevenSegmentRaw(uint8_t segA, uint8_t segB, uint8_t segC, uint8_t segD,
                    uint8_t segE, uint8_t segF, uint8_t segG, uint8_t segDP,
                    uint8_t *digitPins, uint8_t numDigits,
                    bool segmentActiveHigh = SEGMENT_ACTIVE_LOW,
                    bool digitActiveHigh = DIGIT_ACTIVE_HIGH);
    ~SevenSegmentRaw();

    void begin();
    void setTimingMode(TimingMode mode);
    TimingMode getTimingMode();
    
    void displayNumber(int32_t number);
    void displayFloat(float number, uint8_t decimals = 1);
    void displayHex(uint16_t number);
    void displayText(const char *text);
    void setBrightness(uint8_t level);
    void clear();
    void refresh();

    void displayInt(int value) { displayNumber(value); }

    void displayLong(long value) { displayNumber(value); }

    void displayByte(byte value) { displayNumber(value); }

    void setPaddingMode(PaddingMode mode);
    PaddingMode getPaddingMode();

    void testDigits(uint16_t delay_ms = 500, uint8_t loops = 3);
    void testSegments(uint8_t digit = 0, uint16_t delay_ms = 300);
    void testAllSegments(uint16_t delay_ms = 200);
    void runFullDiagnostics(uint16_t delay_ms = 400);
};

#endif  // KINEMATRIX_SEVEN_SEGMENT_RAW
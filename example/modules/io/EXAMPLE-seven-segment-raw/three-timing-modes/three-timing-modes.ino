/*
 * SevenSegmentRaw - 3 Timing Modes Example
 * 
 * Demonstrates all 3 timing modes with minimal code:
 * 1. TIMING_DELAY   - Blocking delay (simple but blocks program)
 * 2. TIMING_MILLIS  - Non-blocking millis() (default, compatible)  
 * 3. TIMING_TIMER1  - Hardware interrupt (smoothest, recommended)
 * 
 * Hardware: Connect 7-segment to pins 2-9 (segments) and 10-13 (digits)
 */

#define ENABLE_MODULE_SEVEN_SEGMENT_RAW
#include "Kinematrix.h"

const uint8_t digitPins[4] = {10, 11, 12, 13};
SevenSegmentRaw display(2,3,4,5,6,7,8,9, (uint8_t*)digitPins, 4, COMMON_CATHODE);

void setup() {
    Serial.begin(115200);
    Serial.println("SevenSegmentRaw - 3 Timing Modes Demo");
    Serial.println("Commands: '1'=DELAY, '2'=MILLIS, '3'=TIMER1");
    
    display.begin();
    display.setBrightness(80);
    display.setTimingMode(TIMING_TIMER1); // Start with best mode
    
    Serial.println("Started with TIMING_TIMER1 (smoothest)");
}

void loop() {
    // Display counter - works with all 3 modes!
    display.displayNumber(millis() / 1000);
    
    // Handle mode switching
    if (Serial.available()) {
        char cmd = Serial.read();
        switch (cmd) {
            case '1':
                display.setTimingMode(TIMING_DELAY);
                Serial.println("Mode: TIMING_DELAY (blocking, simple)");
                break;
            case '2': 
                display.setTimingMode(TIMING_MILLIS);
                Serial.println("Mode: TIMING_MILLIS (non-blocking, compatible)");
                break;
            case '3':
                display.setTimingMode(TIMING_TIMER1);
                Serial.println("Mode: TIMING_TIMER1 (hardware interrupt, smoothest)");
                break;
        }
    }
    
    delay(100); // Try different delays to see the difference!
    
    // Note: Only TIMING_TIMER1 stays smooth with delays
    // TIMING_DELAY will block here
    // TIMING_MILLIS may flicker with long delays
}
/*
 * SevenSegmentRaw - Basic Usage Example
 * 
 * Demonstrates basic usage of raw 7-segment display library
 * - Single digit display
 * - Multiple digit display
 * - Integer and float numbers
 * - Hexadecimal display
 * 
 * Hardware connections:
 * - Segments A,B,C,D,E,F,G,DP to Arduino pins 2-9
 * - Digit pins (common) to Arduino pins 10-13 for 4-digit display
 * 
 */

#define ENABLE_MODULE_SEVEN_SEGMENT_RAW
#include "Kinematrix.h"

// Pin definitions for segments A,B,C,D,E,F,G,DP
const uint8_t segmentPins[8] = {2, 3, 4, 5, 6, 7, 8, 9};

// Pin definitions for digit selection (common pins)
const uint8_t digitPins[4] = {10, 11, 12, 13};

// Create display object (4-digit, common cathode)
SevenSegmentRaw display(2, 3, 4, 5, 6, 7, 8, 9,  // A,B,C,D,E,F,G,DP
                        (uint8_t*)digitPins, 4,    // digit pins, count
                        COMMON_CATHODE);           // display type

int counter = 0;
float temperature = 23.5;
uint16_t hexValue = 0xABCD;
unsigned long lastUpdate = 0;

void setup() {
    Serial.begin(115200);
    Serial.println("SevenSegmentRaw Basic Usage - 3 Timing Modes");
    
    display.begin();
    display.setBrightness(75);
    
    // Set timing mode (TIMING_DELAY, TIMING_MILLIS, TIMING_TIMER1)
    display.setTimingMode(TIMING_TIMER1); // Smoothest mode
    Serial.print("Timing mode: ");
    Serial.println(display.getTimingMode());
    
    Serial.println("Display initialized!");
    Serial.println("Cycling through different display modes...");
    Serial.println("Commands: '1'=DELAY, '2'=MILLIS, '3'=TIMER1");
}

void loop() {
    // Update display every 2 seconds
    if (millis() - lastUpdate >= 2000) {
        lastUpdate = millis();
        
        static uint8_t mode = 0;
        
        switch (mode) {
            case 0:
                Serial.print("Displaying counter: ");
                Serial.println(counter);
                display.displayNumber(counter);
                counter++;
                if (counter > 9999) counter = 0;
                break;
                
            case 1:
                Serial.print("Displaying temperature: ");
                Serial.println(temperature, 1);
                display.displayFloat(temperature, 1);
                temperature += 0.5;
                if (temperature > 99.9) temperature = -9.9;
                break;
                
            case 2:
                Serial.print("Displaying hex: 0x");
                Serial.println(hexValue, HEX);
                display.displayHex(hexValue);
                hexValue += 0x111;
                if (hexValue == 0x0000) hexValue = 0x0001;
                break;
                
            case 3:
                Serial.println("Displaying text: CAFE");
                display.displayText("CAFE");
                break;
        }
        
        mode = (mode + 1) % 4;
    }
    
    // Handle serial commands for timing mode switching
    if (Serial.available()) {
        char cmd = Serial.read();
        switch (cmd) {
            case '1':
                display.setTimingMode(TIMING_DELAY);
                Serial.println("✓ TIMING_DELAY - Simple but blocking");
                break;
            case '2':
                display.setTimingMode(TIMING_MILLIS);
                Serial.println("✓ TIMING_MILLIS - Non-blocking, compatible");
                break;
            case '3':
                display.setTimingMode(TIMING_TIMER1);
                Serial.println("✓ TIMING_TIMER1 - Hardware interrupt, smoothest");
                break;
            case 's':
                Serial.print("Current mode: ");
                Serial.println(display.getTimingMode());
                break;
        }
    }
    
    // NOTE: Different timing modes handle refresh automatically:
    // TIMING_DELAY  - Blocks in display methods with delay()
    // TIMING_MILLIS - Non-blocking timing, may need manual refresh() 
    // TIMING_TIMER1 - Hardware interrupt, fully automatic
    
    // Manual refresh only needed for TIMING_MILLIS in some cases
    if (display.getTimingMode() == TIMING_MILLIS) {
        // display.refresh(); // Uncomment if display seems slow
    }
}
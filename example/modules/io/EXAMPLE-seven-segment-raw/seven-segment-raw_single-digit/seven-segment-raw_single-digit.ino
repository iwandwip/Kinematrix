/*
 * SevenSegmentRaw - Single Digit Example
 * 
 * Demonstrates usage with single digit 7-segment display
 * - Counts 0-9 repeatedly
 * - Shows hexadecimal digits 0-F
 * - Simple and clean implementation
 * 
 * Hardware connections:
 * - Segments A,B,C,D,E,F,G,DP to Arduino pins 2-9
 * - No digit selection pins needed for single digit
 * - Common pin to GND (CC) or VCC (CA)
 * 
 */

#define ENABLE_MODULE_SEVEN_SEGMENT_RAW  
#include "Kinematrix.h"

// No digit pins needed for single digit display
uint8_t dummyDigitPin = 255; // Unused pin

// Create single digit display object (common cathode)
SevenSegmentRaw display(2, 3, 4, 5, 6, 7, 8, 9,  // A,B,C,D,E,F,G,DP
                        &dummyDigitPin, 1,         // single digit
                        COMMON_CATHODE);           // display type

uint8_t counter = 0;
bool showHex = false;
unsigned long lastUpdate = 0;

void setup() {
    Serial.begin(115200);
    Serial.println("SevenSegmentRaw Single Digit Example");
    
    display.begin();
    display.setBrightness(80);
    
    Serial.println("Single digit display initialized!");
    Serial.println("Counting 0-9, then hex A-F...");
}

void loop() {
    // Update every 800ms
    if (millis() - lastUpdate >= 800) {
        lastUpdate = millis();
        
        if (!showHex) {
            // Show decimal digits 0-9
            Serial.print("Decimal: ");
            Serial.println(counter);
            display.displayNumber(counter);
            
            counter++;
            if (counter > 9) {
                counter = 10; // Start hex from A
                showHex = true;
                Serial.println("Switching to hexadecimal...");
            }
        } else {
            // Show hexadecimal digits A-F (10-15)
            Serial.print("Hex: ");
            Serial.print(counter, HEX);
            Serial.print(" (");
            Serial.print(counter);
            Serial.println(")");
            display.displayHex(counter);
            
            counter++;
            if (counter > 15) {
                counter = 0;
                showHex = false;
                Serial.println("Back to decimal...");
            }
        }
    }
    
    // Keep displaying current value
    if (!showHex) {
        display.displayNumber(counter - 1);
    } else {
        display.displayHex(counter - 1);
    }
}
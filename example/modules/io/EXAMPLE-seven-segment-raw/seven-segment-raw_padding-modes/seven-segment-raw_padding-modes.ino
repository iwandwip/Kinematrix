/*
 * SevenSegmentRaw - Padding Modes Example
 * 
 * Demonstrates different padding modes for leading zero control
 * - PADDING_ZEROS: Shows leading zeros (0009, 0123)
 * - PADDING_BLANKS: Shows blanks for leading zeros (   9,  123)
 * - PADDING_DASHES: Shows dashes for leading zeros (---9, -123)
 * - PADDING_NONE: Same as PADDING_BLANKS
 * 
 * Hardware connections:
 * - Segments A,B,C,D,E,F,G,DP to Arduino pins 2-9
 * - Digit pins (common) to Arduino pins 10-13 for 4-digit display
 * 
 */

#define ENABLE_MODULE_SEVEN_SEGMENT_RAW
#include "Kinematrix.h"

// Pin definitions
const uint8_t digitPins[4] = {10, 11, 12, 13};

// Create display object (4-digit, common cathode)
SevenSegmentRaw display(2, 3, 4, 5, 6, 7, 8, 9,  // A,B,C,D,E,F,G,DP
                        (uint8_t*)digitPins, 4,    // digit pins, count
                        COMMON_CATHODE);           // display type

void setup() {
    Serial.begin(115200);
    Serial.println("SevenSegmentRaw Padding Modes Example");
    Serial.println("====================================");
    
    display.begin();
    display.setBrightness(80);
    
    Serial.println("Demonstrating different padding modes...");
    Serial.println();
    
    // Test numbers for demonstration
    int testNumbers[] = {9, 42, 123, 1234, -7, -56};
    const char* testNames[] = {"9", "42", "123", "1234", "-7", "-56"};
    uint8_t numTests = 6;
    
    // Test each padding mode
    PaddingMode modes[] = {PADDING_ZEROS, PADDING_BLANKS, PADDING_DASHES, PADDING_NONE};
    const char* modeNames[] = {"ZEROS", "BLANKS", "DASHES", "NONE"};
    uint8_t numModes = 4;
    
    for (uint8_t mode = 0; mode < numModes; mode++) {
        Serial.print("=== PADDING_");
        Serial.print(modeNames[mode]);
        Serial.println(" Mode ===");
        
        display.setPaddingMode(modes[mode]);
        
        for (uint8_t test = 0; test < numTests; test++) {
            Serial.print("Number ");
            Serial.print(testNames[test]);
            Serial.print(" displays as: ");
            
            display.displayNumber(testNumbers[test]);
            
            // Show what it should look like
            switch (modes[mode]) {
                case PADDING_ZEROS:
                    if (testNumbers[test] == 9) Serial.print("0009");
                    else if (testNumbers[test] == 42) Serial.print("0042"); 
                    else if (testNumbers[test] == 123) Serial.print("0123");
                    else if (testNumbers[test] == 1234) Serial.print("1234");
                    else if (testNumbers[test] == -7) Serial.print("-007");
                    else if (testNumbers[test] == -56) Serial.print("-056");
                    break;
                case PADDING_BLANKS:
                case PADDING_NONE:
                    if (testNumbers[test] == 9) Serial.print("___9");
                    else if (testNumbers[test] == 42) Serial.print("__42");
                    else if (testNumbers[test] == 123) Serial.print("_123");
                    else if (testNumbers[test] == 1234) Serial.print("1234");
                    else if (testNumbers[test] == -7) Serial.print("__-7");
                    else if (testNumbers[test] == -56) Serial.print("_-56");
                    break;
                case PADDING_DASHES:
                    if (testNumbers[test] == 9) Serial.print("---9");
                    else if (testNumbers[test] == 42) Serial.print("--42");
                    else if (testNumbers[test] == 123) Serial.print("-123");
                    else if (testNumbers[test] == 1234) Serial.print("1234");
                    else if (testNumbers[test] == -7) Serial.print("---7");
                    else if (testNumbers[test] == -56) Serial.print("--56");
                    break;
            }
            Serial.println();
            
            delay(2000);  // Display for 2 seconds
        }
        
        Serial.println();
        delay(1000);
    }
    
    Serial.println("Padding modes demonstration completed!");
    Serial.println("Starting interactive mode...");
}

void loop() {
    // Interactive demonstration
    static uint32_t lastUpdate = 0;
    static uint8_t currentMode = 0;
    static int16_t counter = 0;
    
    PaddingMode modes[] = {PADDING_ZEROS, PADDING_BLANKS, PADDING_DASHES};
    const char* modeNames[] = {"ZEROS", "BLANKS", "DASHES"};
    
    if (millis() - lastUpdate >= 3000) {
        lastUpdate = millis();
        
        // Switch padding mode every few cycles
        if (counter % 20 == 0) {
            currentMode = (currentMode + 1) % 3;
            display.setPaddingMode(modes[currentMode]);
            
            Serial.print("Switched to PADDING_");
            Serial.println(modeNames[currentMode]);
        }
        
        display.displayNumber(counter);
        
        Serial.print("Counter: ");
        Serial.print(counter);
        Serial.print(" (Mode: ");
        Serial.print(modeNames[currentMode]);
        Serial.println(")");
        
        counter++;
        if (counter > 100) counter = -20;  // Show negative numbers too
    }
    
    // Keep display refreshed
    display.displayNumber(counter);
}
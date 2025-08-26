/*
 * SevenSegmentRaw - Diagnostics & Hardware Testing Example
 * 
 * Demonstrates the diagnostic and testing capabilities
 * - Full hardware diagnostics
 * - Individual digit testing  
 * - Segment wiring verification
 * - Troubleshooting helper functions
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

void setup() {
    Serial.begin(115200);
    Serial.println("SevenSegmentRaw Hardware Diagnostics Example");
    Serial.println("===========================================");
    
    display.begin();
    
    Serial.println("Starting comprehensive hardware test...");
    Serial.println();
    
    // Run complete diagnostic suite
    Serial.println("1. Running Full Diagnostics...");
    display.runFullDiagnostics(400);
    
    delay(2000);
    
    // Individual tests for detailed troubleshooting
    Serial.println("2. Testing Digit Order (5 loops)...");
    display.testDigits(600, 5);
    
    delay(1500);
    
    Serial.println("3. Testing Segments on Each Digit...");
    for (uint8_t digit = 0; digit < 4; digit++) {
        Serial.print("   Testing digit ");
        Serial.print(digit + 1);
        Serial.println(" segments (A-B-C-D-E-F-G-DP)...");
        display.testSegments(digit, 250);
        delay(1000);
    }
    
    Serial.println("4. Testing All Segments Uniformity...");
    display.testAllSegments(300);
    
    delay(1500);
    
    Serial.println("5. Quick brightness test...");
    for (uint8_t brightness = 20; brightness <= 90; brightness += 20) {
        display.setBrightness(brightness);
        display.displayText("8888");
        Serial.print("   Brightness: ");
        Serial.print(brightness);
        Serial.println("%");
        delay(1000);
    }
    
    display.clear();
    
    Serial.println();
    Serial.println("Hardware diagnostics completed!");
    Serial.println("If all segments and digits work correctly, your wiring is OK.");
    Serial.println();
    Serial.println("Normal operation starting...");
}

void loop() {
    // Normal operation - display counter
    static uint32_t counter = 0;
    static uint32_t lastUpdate = 0;
    
    if (millis() - lastUpdate >= 1000) {
        lastUpdate = millis();
        display.displayNumber(counter);
        counter++;
        
        if (counter > 9999) {
            counter = 0;
            
            // Repeat diagnostics every 10000 counts
            Serial.println();
            Serial.println("Running periodic diagnostics...");
            display.runFullDiagnostics(300);
            delay(1000);
        }
    }
    
    // Keep display refreshed
    display.displayNumber(counter);
}
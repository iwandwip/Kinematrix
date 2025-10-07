/*
 * SevenSegmentRaw - Troubleshooting Helper Example
 * 
 * Step-by-step troubleshooting guide for hardware issues
 * - Interactive testing via Serial commands
 * - Individual component verification
 * - Common problem diagnosis
 * 
 * Hardware connections:
 * - Segments A,B,C,D,E,F,G,DP to Arduino pins 2-9
 * - Digit pins (common) to Arduino pins 10-13 for 4-digit display
 * 
 * Serial Commands:
 * - 'd' : Test digits
 * - 's' : Test segments  
 * - 'a' : Test all segments
 * - 'f' : Full diagnostics
 * - 'b' : Brightness test
 * - 'h' : Help
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

void printHelp() {
    Serial.println();
    Serial.println("=== SevenSegmentRaw Troubleshooting Menu ===");
    Serial.println("Commands:");
    Serial.println("  d - Test digit order and connections");
    Serial.println("  s - Test segment wiring (A,B,C,D,E,F,G,DP)");
    Serial.println("  a - Test all segments uniformity");
    Serial.println("  f - Run full diagnostics suite");
    Serial.println("  b - Test brightness levels");
    Serial.println("  c - Clear display");
    Serial.println("  t - Display test pattern (8888)");
    Serial.println("  h - Show this help menu");
    Serial.println("  q - Quit testing mode");
    Serial.println();
    Serial.println("Hardware Check Tips:");
    Serial.println("- If digits don't light up: check digit pin connections");
    Serial.println("- If wrong segments light up: check A-G,DP pin order");
    Serial.println("- If dim/bright issues: check current limiting resistors");
    Serial.println("- If all segments dim: check common anode/cathode setting");
    Serial.println();
    Serial.print("Enter command: ");
}

void setup() {
    Serial.begin(115200);
    while (!Serial) { delay(10); }
    
    Serial.println("SevenSegmentRaw Troubleshooting Helper");
    Serial.println("=====================================");
    Serial.println();
    
    display.begin();
    display.setBrightness(70);
    
    Serial.println("Display initialized!");
    Serial.println("Ready for interactive troubleshooting...");
    
    printHelp();
}

void loop() {
    if (Serial.available()) {
        char command = Serial.read();
        Serial.println(command);
        
        switch (command) {
            case 'd':
            case 'D':
                Serial.println("Testing digit order...");
                Serial.println("You should see: 1-2-3-4 repeating 3 times");
                display.testDigits(800, 3);
                Serial.println("Digit test completed.");
                break;
                
            case 's':
            case 'S':
                Serial.println("Testing segments on each digit...");
                for (uint8_t digit = 0; digit < 4; digit++) {
                    Serial.print("Digit ");
                    Serial.print(digit + 1);
                    Serial.println(": A-B-C-D-E-F-G-DP");
                    display.testSegments(digit, 400);
                    delay(500);
                }
                Serial.println("Segment test completed.");
                break;
                
            case 'a':
            case 'A':
                Serial.println("Testing all segments uniformity...");
                Serial.println("Each segment should light on all digits simultaneously");
                display.testAllSegments(500);
                Serial.println("All segments test completed.");
                break;
                
            case 'f':
            case 'F':
                Serial.println("Running full diagnostic suite...");
                display.runFullDiagnostics(400);
                Serial.println("Full diagnostics completed.");
                break;
                
            case 'b':
            case 'B':
                Serial.println("Testing brightness levels...");
                display.displayText("8888");
                for (uint8_t brightness = 10; brightness <= 90; brightness += 20) {
                    display.setBrightness(brightness);
                    Serial.print("Brightness: ");
                    Serial.print(brightness);
                    Serial.println("%");
                    delay(1500);
                }
                display.setBrightness(70);
                Serial.println("Brightness test completed.");
                break;
                
            case 'c':
            case 'C':
                Serial.println("Clearing display...");
                display.clear();
                break;
                
            case 't':
            case 'T':
                Serial.println("Displaying test pattern (8888)...");
                display.displayText("8888");
                break;
                
            case 'h':
            case 'H':
                printHelp();
                break;
                
            case 'q':
            case 'Q':
                Serial.println("Exiting troubleshooting mode...");
                Serial.println("Starting normal counter display...");
                break;
                
            default:
                Serial.println("Unknown command. Press 'h' for help.");
                break;
        }
        
        if (command != 'q' && command != 'Q') {
            Serial.println();
            Serial.print("Enter command (h for help): ");
        }
    }
    
    // Normal display mode after quit
    static bool normalMode = false;
    if (Serial.available() && Serial.peek() == 'q') {
        normalMode = true;
    }
    
    if (normalMode) {
        static uint16_t counter = 0;
        static uint32_t lastUpdate = 0;
        
        if (millis() - lastUpdate >= 1000) {
            lastUpdate = millis();
            display.displayNumber(counter);
            counter++;
            if (counter > 9999) counter = 0;
        }
        
        display.displayNumber(counter);
    }
}
/*
 * SevenSegmentRaw - Digital Clock Display Example
 * 
 * Demonstrates practical usage of padding modes for clock display
 * - PADDING_ZEROS for proper time format (01:05, 12:34)
 * - PADDING_BLANKS for clean counter display
 * - Real-time clock simulation with different formats
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

// Time variables
uint8_t hours = 12;
uint8_t minutes = 0;
uint8_t seconds = 0;
uint32_t lastSecond = 0;
bool displayMode = true; // true = clock, false = counter

void setup() {
    Serial.begin(115200);
    Serial.println("SevenSegmentRaw Clock Display Example");
    Serial.println("====================================");
    Serial.println("Modes:");
    Serial.println("- Clock mode: PADDING_ZEROS for proper time format");
    Serial.println("- Counter mode: PADDING_BLANKS for clean display");
    Serial.println();
    
    display.begin();
    display.setBrightness(85);
    
    // Set initial time (can be set via Serial or RTC)
    hours = 9;    // 09:00:00
    minutes = 0;
    seconds = 0;
    
    Serial.println("Clock started at 09:00:00");
    Serial.println("Format: HH:MM (HHMM on display)");
    Serial.println();
}

void loop() {
    // Update time every second
    if (millis() - lastSecond >= 1000) {
        lastSecond = millis();
        
        seconds++;
        if (seconds >= 60) {
            seconds = 0;
            minutes++;
            if (minutes >= 60) {
                minutes = 0;
                hours++;
                if (hours >= 24) {
                    hours = 0;
                }
            }
        }
        
        // Switch display mode every 30 seconds
        if (seconds == 0 || seconds == 30) {
            displayMode = !displayMode;
            
            if (displayMode) {
                Serial.println("Switched to Clock Mode (PADDING_ZEROS)");
                display.setPaddingMode(PADDING_ZEROS);
            } else {
                Serial.println("Switched to Counter Mode (PADDING_BLANKS)");  
                display.setPaddingMode(PADDING_BLANKS);
            }
        }
        
        if (displayMode) {
            // Clock mode: Display time as HHMM with zero padding
            uint16_t timeDisplay = hours * 100 + minutes;
            display.displayNumber(timeDisplay);
            
            Serial.print("Time: ");
            if (hours < 10) Serial.print("0");
            Serial.print(hours);
            Serial.print(":");
            if (minutes < 10) Serial.print("0");  
            Serial.print(minutes);
            Serial.print(":");
            if (seconds < 10) Serial.print("0");
            Serial.print(seconds);
            Serial.println(" (Clock Mode)");
            
        } else {
            // Counter mode: Display elapsed seconds with blank padding
            uint32_t elapsedSeconds = hours * 3600 + minutes * 60 + seconds;
            uint16_t displayValue = elapsedSeconds % 10000; // Keep within 4 digits
            display.displayNumber(displayValue);
            
            Serial.print("Elapsed: ");
            Serial.print(elapsedSeconds);
            Serial.println(" seconds (Counter Mode)");
        }
    }
    
    // Keep display refreshed with current mode
    if (displayMode) {
        uint16_t timeDisplay = hours * 100 + minutes;
        display.displayNumber(timeDisplay);
    } else {
        uint32_t elapsedSeconds = hours * 3600 + minutes * 60 + seconds;
        uint16_t displayValue = elapsedSeconds % 10000;
        display.displayNumber(displayValue);
    }
    
    // Handle serial commands for time setting
    if (Serial.available()) {
        String command = Serial.readStringUntil('\n');
        command.trim();
        
        if (command.startsWith("set ")) {
            // Format: "set HH:MM" or "set HH:MM:SS"
            int firstColon = command.indexOf(':', 4);
            int secondColon = command.indexOf(':', firstColon + 1);
            
            if (firstColon > 0) {
                hours = command.substring(4, firstColon).toInt();
                if (secondColon > 0) {
                    minutes = command.substring(firstColon + 1, secondColon).toInt();
                    seconds = command.substring(secondColon + 1).toInt();
                } else {
                    minutes = command.substring(firstColon + 1).toInt();
                    seconds = 0;
                }
                
                // Validate time values
                if (hours > 23) hours = 23;
                if (minutes > 59) minutes = 59; 
                if (seconds > 59) seconds = 59;
                
                Serial.print("Time set to: ");
                if (hours < 10) Serial.print("0");
                Serial.print(hours);
                Serial.print(":");
                if (minutes < 10) Serial.print("0");
                Serial.print(minutes);
                Serial.print(":");
                if (seconds < 10) Serial.print("0");
                Serial.println(seconds);
            }
        } else if (command == "help") {
            Serial.println("Commands:");
            Serial.println("  set HH:MM     - Set time (hours:minutes)");
            Serial.println("  set HH:MM:SS  - Set time (hours:minutes:seconds)");
            Serial.println("  help          - Show this help");
        }
    }
}
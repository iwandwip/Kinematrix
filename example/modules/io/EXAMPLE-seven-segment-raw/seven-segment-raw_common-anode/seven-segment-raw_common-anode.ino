/*
 * SevenSegmentRaw - Common Anode Example
 * 
 * Demonstrates usage with common anode 7-segment display
 * - Shows configuration for CA displays
 * - Brightness control demonstration
 * - Real-time clock simulation
 * 
 * Hardware connections:
 * - Segments A,B,C,D,E,F,G,DP to Arduino pins 2-9
 * - Common anode pins to Arduino pins 10-13 via PNP transistors
 * - Remember to use appropriate current limiting resistors
 * 
 */

#define ENABLE_MODULE_SEVEN_SEGMENT_RAW
#include "Kinematrix.h"

// Pin definitions
const uint8_t digitPins[4] = {10, 11, 12, 13};

// Create display object (4-digit, common anode)
SevenSegmentRaw display(2, 3, 4, 5, 6, 7, 8, 9,  // A,B,C,D,E,F,G,DP
                        (uint8_t*)digitPins, 4,    // digit pins, count  
                        COMMON_ANODE);             // display type

unsigned long lastSecond = 0;
uint16_t timeValue = 0;  // MMSS format (0000-5959)
uint8_t brightness = 10;
bool brightnessUp = true;

void setup() {
    Serial.begin(115200);
    Serial.println("SevenSegmentRaw Common Anode Example");
    Serial.println("Displaying time in MM:SS format with brightness control");
    
    display.begin();
    display.setBrightness(brightness);
    
    Serial.println("Display initialized!");
}

void loop() {
    // Update time every second
    if (millis() - lastSecond >= 1000) {
        lastSecond = millis();
        
        // Increment seconds
        uint8_t seconds = timeValue % 100;
        uint8_t minutes = timeValue / 100;
        
        seconds++;
        if (seconds >= 60) {
            seconds = 0;
            minutes++;
            if (minutes >= 60) {
                minutes = 0;
            }
        }
        
        timeValue = minutes * 100 + seconds;
        
        // Update brightness
        if (brightnessUp) {
            brightness += 5;
            if (brightness >= 95) {
                brightness = 95;
                brightnessUp = false;
            }
        } else {
            brightness -= 5;
            if (brightness <= 10) {
                brightness = 10;
                brightnessUp = true;
            }
        }
        
        display.setBrightness(brightness);
        
        Serial.print("Time: ");
        Serial.print(minutes);
        Serial.print(":");
        if (seconds < 10) Serial.print("0");
        Serial.print(seconds);
        Serial.print(" | Brightness: ");
        Serial.println(brightness);
    }
    
    // Display time with colon effect using DP on digit 1 (index 1)
    // For proper colon display, you'd need to modify the library or use external LED
    display.displayNumber(timeValue);
}
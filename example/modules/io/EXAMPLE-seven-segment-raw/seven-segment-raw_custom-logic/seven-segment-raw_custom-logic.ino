/*
 * SevenSegmentRaw - Custom Logic Levels Example
 * 
 * Demonstrates the flexible control of segment and digit logic levels
 * - Common Cathode standard setup
 * - Common Anode standard setup  
 * - Custom logic level combinations
 * - Comparison between different configurations
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

void setup() {
    Serial.begin(115200);
    Serial.println("SevenSegmentRaw Custom Logic Levels Example");
    Serial.println("==========================================");
    Serial.println();
    
    Serial.println("Available configuration options:");
    Serial.println("1. Common Cathode (Standard): Segments Active Low, Digits Active High");  
    Serial.println("2. Common Anode (Standard): Segments Active High, Digits Active Low");
    Serial.println("3. Custom combinations for special hardware");
    Serial.println();
    
    // Example 1: Standard Common Cathode
    Serial.println("=== Example 1: Common Cathode (Standard) ===");
    SevenSegmentRaw displayCC(2, 3, 4, 5, 6, 7, 8, 9, (uint8_t*)digitPins, 4, 
                              COMMON_CATHODE);  // Expands to: SEGMENT_ACTIVE_LOW, DIGIT_ACTIVE_HIGH
    
    displayCC.begin();
    displayCC.setBrightness(70);
    
    Serial.println("Displaying 'CC-1' for Common Cathode...");
    displayCC.displayText("CC-1");
    delay(3000);
    displayCC.clear();
    
    delay(1000);
    
    // Example 2: Standard Common Anode
    Serial.println("=== Example 2: Common Anode (Standard) ===");
    SevenSegmentRaw displayCA(2, 3, 4, 5, 6, 7, 8, 9, (uint8_t*)digitPins, 4, 
                              COMMON_ANODE);   // Expands to: SEGMENT_ACTIVE_HIGH, DIGIT_ACTIVE_LOW
    
    displayCA.begin();
    displayCA.setBrightness(70);
    
    Serial.println("Displaying 'CA-2' for Common Anode...");  
    displayCA.displayText("CA-2");
    delay(3000);
    displayCA.clear();
    
    delay(1000);
    
    // Example 3: Custom Logic - Segments High, Digits High
    Serial.println("=== Example 3: Custom Logic (Both Active High) ===");
    SevenSegmentRaw displayCustom1(2, 3, 4, 5, 6, 7, 8, 9, (uint8_t*)digitPins, 4,
                                   SEGMENT_ACTIVE_HIGH, DIGIT_ACTIVE_HIGH);
    
    displayCustom1.begin();
    displayCustom1.setBrightness(70);
    
    Serial.println("Displaying '3333' with custom logic (Both High)...");
    displayCustom1.displayNumber(3333);
    delay(3000);
    displayCustom1.clear();
    
    delay(1000);
    
    // Example 4: Custom Logic - Segments Low, Digits Low  
    Serial.println("=== Example 4: Custom Logic (Both Active Low) ===");
    SevenSegmentRaw displayCustom2(2, 3, 4, 5, 6, 7, 8, 9, (uint8_t*)digitPins, 4,
                                   SEGMENT_ACTIVE_LOW, DIGIT_ACTIVE_LOW);
    
    displayCustom2.begin(); 
    displayCustom2.setBrightness(70);
    
    Serial.println("Displaying '4444' with custom logic (Both Low)...");
    displayCustom2.displayNumber(4444);
    delay(3000);
    displayCustom2.clear();
    
    Serial.println();
    Serial.println("All configuration examples completed!");
    Serial.println("Choose the configuration that matches your hardware.");
}

void loop() {
    // Demonstrate all configurations in rotation
    static uint8_t configMode = 0;
    static uint32_t lastChange = 0;
    static uint16_t counter = 0;
    
    if (millis() - lastChange >= 5000) {
        lastChange = millis();
        counter++;
        
        switch (configMode) {
            case 0: {
                Serial.println("Demo: Common Cathode");
                SevenSegmentRaw display(2,3,4,5,6,7,8,9, (uint8_t*)digitPins, 4, COMMON_CATHODE);
                display.begin();
                display.setBrightness(80);
                
                for (uint8_t i = 0; i < 20; i++) {
                    display.displayNumber(counter + i);
                    delay(100);
                }
                display.clear();
                break;
            }
            
            case 1: {
                Serial.println("Demo: Common Anode"); 
                SevenSegmentRaw display(2,3,4,5,6,7,8,9, (uint8_t*)digitPins, 4, COMMON_ANODE);
                display.begin();
                display.setBrightness(80);
                
                for (uint8_t i = 0; i < 20; i++) {
                    display.displayNumber(counter + i + 1000);
                    delay(100);  
                }
                display.clear();
                break;
            }
            
            case 2: {
                Serial.println("Demo: Custom - Both High");
                SevenSegmentRaw display(2,3,4,5,6,7,8,9, (uint8_t*)digitPins, 4, 
                                      SEGMENT_ACTIVE_HIGH, DIGIT_ACTIVE_HIGH);
                display.begin();
                display.setBrightness(80);
                
                for (uint8_t i = 0; i < 20; i++) {
                    display.displayNumber(counter + i + 2000);
                    delay(100);
                }  
                display.clear();
                break;
            }
            
            case 3: {
                Serial.println("Demo: Custom - Both Low");
                SevenSegmentRaw display(2,3,4,5,6,7,8,9, (uint8_t*)digitPins, 4,
                                      SEGMENT_ACTIVE_LOW, DIGIT_ACTIVE_LOW);
                display.begin();
                display.setBrightness(80);
                
                for (uint8_t i = 0; i < 20; i++) {
                    display.displayNumber(counter + i + 3000);
                    delay(100);
                }
                display.clear();
                break;
            }
        }
        
        configMode = (configMode + 1) % 4;
    }
}
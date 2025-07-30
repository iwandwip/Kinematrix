/*
 *  EXAMPLE_USAGE.ino - AutoLightV4 Usage Examples
 *
 *  Kastara Electronics Embedded Development
 *  Created on: 2024. 7. 23
 *  
 *  This file demonstrates the fully dynamic and user-friendly API
 *  of AutoLightV4 with various usage scenarios.
 */

#define ENABLE_ADDONS_AUTOLIGHT_V4
#include "Kinematrix.h"

using namespace AutoLight;

AutoLightV4 led(true);

void setup() {
    Serial.begin(115200);
    delay(1000);
    
    Serial.println("=== AutoLightV4 Usage Examples ===");
    
    // Example 1: Fully Automatic Mode
    Serial.println("\n1. FULLY AUTOMATIC MODE");
    automaticExample();
    
    // Example 2: Manual Configuration
    Serial.println("\n2. MANUAL CONFIGURATION MODE");
    manualExample();
    
    // Example 3: Hybrid Mode (Auto-detect + Manual config)
    Serial.println("\n3. HYBRID MODE");
    hybridExample();
    
    // Example 4: Runtime Dynamic Changes
    Serial.println("\n4. RUNTIME DYNAMIC CHANGES");
    runtimeExample();
    
    // Example 5: Advanced Features
    Serial.println("\n5. ADVANCED FEATURES");
    advancedExample();
}

void loop() {
    led.update();
    
    static uint32_t last_demo = 0;
    if (millis() - last_demo > 10000) {
        demoSequenceCycle();
        last_demo = millis();
    }
    
    delay(20);
}

void automaticExample() {
    Serial.println("Auto-scanning hardware...");
    
    auto scan_result = led.autoScan();
    if (scan_result == SCAN_RESULT_SUCCESS) {
        Serial.println("✓ Hardware detected successfully");
        
        led.autoDistribute();
        Serial.println("✓ Channels distributed automatically");
        
        led.begin();
        Serial.println("✓ System ready!");
        
        led.printConfiguration();
    } else {
        Serial.println("✗ No hardware detected");
    }
}

void manualExample() {
    Serial.println("Manual configuration...");
    
    bool success = led.configure()
        .channels(12)
        .pcf(2)
        .addresses(0x38, 0x21)
        .distribute(6, 6)
        .buttonMode(BUTTON_MODE_SINGLE)
        .sequences(15)
        .autoScan(false)
        .autoFix(true)
        .hotSwap(false)
        .debugMode(true)
        .apply();
    
    if (success) {
        Serial.println("✓ Manual configuration applied");
        led.begin();
        Serial.println("✓ System ready with manual config!");
    } else {
        Serial.println("✗ Manual configuration failed");
    }
}

void hybridExample() {
    Serial.println("Hybrid mode (auto-detect + manual override)...");
    
    led.autoScan();
    
    led.configure()
        .buttonMode(BUTTON_MODE_DUAL)
        .sequences(20)
        .autoFix(true)
        .hotSwap(true)
        .apply();
    
    led.begin();
    Serial.println("✓ Hybrid configuration complete!");
}

void runtimeExample() {
    Serial.println("Demonstrating runtime changes...");
    
    Serial.println("Adding new PCF at runtime...");
    if (led.addPCF(0x22, 8)) {
        Serial.println("✓ PCF added successfully");
        led.redistributeChannels();
        Serial.println("✓ Channels redistributed");
    }
    
    Serial.println("Switching button mode...");
    led.setButtonMode(BUTTON_MODE_TRIPLE);
    Serial.printf("✓ Button mode changed to: %d\n", led.getButtonMode());
    
    Serial.println("Adding custom sequence...");
    led.addSequence(10, "Custom Blink", customBlinkSequence, 100);
    Serial.println("✓ Custom sequence added");
    
    Serial.println("Runtime validation...");
    auto validation = led.validate();
    if (validation != VALIDATION_OK) {
        Serial.println("⚠ Validation issues detected, auto-fixing...");
        if (led.autoFix()) {
            Serial.println("✓ Auto-fix successful");
        }
    }
}

void advancedExample() {
    Serial.println("Advanced features demonstration...");
    
    led.enableHotSwap(true);
    led.enableAutoScan(true, 3000);
    led.enableAutoFix(true);
    led.enableAutoCycle(true, 8000);
    
    Serial.println("✓ Advanced features enabled");
    
    led.onHardwareChange([](uint8_t address, bool connected) {
        Serial.printf("Hardware change: PCF 0x%02X %s\n", 
                      address, connected ? "connected" : "disconnected");
    });
    
    led.onSequenceChange([](uint8_t old_seq, uint8_t new_seq) {
        Serial.printf("Sequence changed: %d → %d\n", old_seq, new_seq);
    });
    
    led.onChannelChange([](uint8_t channel, bool state) {
        Serial.printf("Channel %d: %s\n", channel, state ? "ON" : "OFF");
    });
    
    Serial.println("✓ Event callbacks registered");
    
    Serial.println("\nTesting all channels...");
    if (led.testAllChannels()) {
        Serial.println("✓ All channels responding");
    } else {
        Serial.println("⚠ Some channels not responding");
    }
    
    Serial.println("\nSystem statistics:");
    led.printRuntimeStats();
}

void demoSequenceCycle() {
    static uint8_t demo_sequence = 0;
    
    Serial.printf("Demo: Switching to sequence %d\n", demo_sequence);
    led.setSequenceIndex(demo_sequence);
    
    demo_sequence++;
    if (demo_sequence > 15) {
        demo_sequence = 0;
    }
}

void customBlinkSequence(uint8_t* channels, uint8_t count, uint32_t timing) {
    static bool blink_state = false;
    static uint32_t last_blink = 0;
    
    uint32_t now = millis();
    if ((now - last_blink) >= timing) {
        for (uint8_t i = 0; i < count; i += 2) {
            // Custom blink pattern: alternate channels
        }
        blink_state = !blink_state;
        last_blink = now;
    }
}

void buttonExample() {
    Serial.println("\n=== Button Examples ===");
    
    Serial.println("Single button mode:");
    led.setButtonMode(BUTTON_MODE_SINGLE);
    led.singleButtonCycle();
    
    delay(1000);
    
    Serial.println("Smart button press:");
    led.smartButtonPress(0);
    
    delay(1000);
    
    Serial.println("Toggle ON/OFF:");
    led.toggleOnOff();
}

void channelControlExample() {
    Serial.println("\n=== Channel Control Examples ===");
    
    Serial.println("Individual channel control:");
    for (uint8_t i = 0; i < led.getTotalChannels(); i++) {
        led.set(i, true);
        delay(100);
        led.set(i, false);
    }
    
    Serial.println("All channels ON:");
    led.setAll(true);
    delay(1000);
    
    Serial.println("All channels OFF:");
    led.clearAll();
    
    Serial.println("Channel states:");
    led.printChannelStates();
}

void validationExample() {
    Serial.println("\n=== Validation Examples ===");
    
    Serial.println("Configuration validation:");
    auto result = led.validate();
    
    switch (result) {
        case VALIDATION_OK:
            Serial.println("✓ Configuration valid");
            break;
        case VALIDATION_WARNING:
            Serial.println("⚠ Configuration warnings");
            led.printValidationReport();
            break;
        case VALIDATION_ERROR:
            Serial.println("✗ Configuration errors");
            led.printValidationReport();
            if (led.autoFix()) {
                Serial.println("✓ Auto-fix applied");
            }
            break;
        case VALIDATION_CRITICAL:
            Serial.println("💥 Critical configuration errors");
            led.printValidationReport();
            break;
    }
}
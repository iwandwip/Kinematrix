#define ENABLE_MODULE_LOGIC_CONDITION_MANAGER
#include "Kinematrix.h"

LogicConditionManager logic;

void setup() {
        Serial.begin(9600);
        pinMode(LED_BUILTIN, OUTPUT);

        Serial.println("=== Minimal Timer Conditions Example ===");

        // Timer 1: One-time timer (5 seconds)
        logic.addTimerCondition(
            "oneTimeTimer",
            5000,   // 5 seconds
            false,  // No auto reset
            []() {
                    Serial.println("⏰ One-time timer completed (5 seconds)");
                    digitalWrite(LED_BUILTIN, HIGH);
            });

        // Timer 2: Repeating timer (3 seconds)
        logic.addTimerCondition(
            "repeatingTimer",
            3000,  // 3 seconds
            true,  // Auto reset
            []() {
                    Serial.println("🔄 Repeating timer triggered (every 3 seconds)");
                    digitalWrite(LED_BUILTIN, !digitalRead(LED_BUILTIN));
            });

        // Timer 3: Limited repetitions
        logic.addTimerCondition(
            "limitedTimer",
            2000,  // 2 seconds
            true,  // Auto reset
            []() {
                    Serial.println("🔢 Limited timer triggered");
            },
            5  // Only 5 times
        );

        Serial.println("Timers started:");
        Serial.println("- One-time: 5 seconds");
        Serial.println("- Repeating: every 3 seconds");
        Serial.println("- Limited: 5 times every 2 seconds");
}

void loop() {
        logic.update();

        // Show timer status every 10 seconds
        static unsigned long lastStatus = 0;
        if (millis() - lastStatus > 10000) {
                Serial.println("\n--- Timer Status ---");
                logic.printStatus("oneTimeTimer");
                logic.printStatus("repeatingTimer");
                logic.printStatus("limitedTimer");
                Serial.println("------------------\n");
                lastStatus = millis();
        }
}
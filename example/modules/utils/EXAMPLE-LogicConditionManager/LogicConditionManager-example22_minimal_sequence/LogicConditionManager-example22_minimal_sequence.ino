#define ENABLE_MODULE_LOGIC_CONDITION_MANAGER
#include "Kinematrix.h"

LogicConditionManager logic;

bool step1 = false;
bool step2 = false;
bool step3 = false;

void setup() {
        Serial.begin(9600);

        pinMode(2, INPUT_PULLUP);  // Step 1 button
        pinMode(3, INPUT_PULLUP);  // Step 2 button
        pinMode(4, INPUT_PULLUP);  // Step 3 button
        pinMode(LED_BUILTIN, OUTPUT);

        Serial.println("=== Minimal Sequence Conditions Example ===");
        Serial.println("Press buttons in order: Pin 2 → Pin 3 → Pin 4");

        // Create individual step conditions
        int step1Id = logic.addCondition(
            "step1",
            &step1,
            LogicConditionManager::TO_TRUE,
            []() { Serial.println("✅ Step 1 completed"); });

        int step2Id = logic.addCondition(
            "step2",
            &step2,
            LogicConditionManager::TO_TRUE,
            []() { Serial.println("✅ Step 2 completed"); });

        int step3Id = logic.addCondition(
            "step3",
            &step3,
            LogicConditionManager::TO_TRUE,
            []() { Serial.println("✅ Step 3 completed"); });

        // Simple sequence: 1 → 2 → 3
        logic.addSequenceCondition(
            "simpleSequence",
            new int[3]{step1Id, step2Id, step3Id},
            3,
            10000,  // Must complete within 10 seconds
            3,      // All 3 steps required
            []() {
                    Serial.println("🎉 SEQUENCE COMPLETED! All steps done in order");
                    digitalWrite(LED_BUILTIN, HIGH);
                    delay(2000);
                    digitalWrite(LED_BUILTIN, LOW);
            });

        // Quick sequence: just 1 → 2
        logic.addSequenceCondition(
            "quickSequence",
            new int[2]{step1Id, step2Id},
            2,
            5000,  // Within 5 seconds
            2,     // Both steps required
            []() {
                    Serial.println("⚡ Quick sequence completed!");
                    for (int i = 0; i < 3; i++) {
                            digitalWrite(LED_BUILTIN, HIGH);
                            delay(200);
                            digitalWrite(LED_BUILTIN, LOW);
                            delay(200);
                    }
            });
}

void loop() {
        // Read button states
        step1 = digitalRead(2) == LOW;
        step2 = digitalRead(3) == LOW;
        step3 = digitalRead(4) == LOW;

        logic.update();

        // Reset sequences if any step is released
        static bool wasPressed = false;
        bool anyPressed = step1 || step2 || step3;

        if (wasPressed && !anyPressed) {
                Serial.println("🔄 Resetting sequences (all buttons released)");
                logic.resetCondition("simpleSequence");
                logic.resetCondition("quickSequence");
        }
        wasPressed = anyPressed;

        // Show current state
        static unsigned long lastDisplay = 0;
        if (millis() - lastDisplay > 2000) {
                Serial.print("Current: ");
                Serial.print(step1 ? "[1]" : " 1 ");
                Serial.print(step2 ? "[2]" : " 2 ");
                Serial.print(step3 ? "[3]" : " 3 ");
                Serial.println();
                lastDisplay = millis();
        }
}
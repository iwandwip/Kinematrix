#define ENABLE_MODULE_LOGIC_CONDITION_MANAGER
#include "Kinematrix.h"

LogicConditionManager logic;

bool buttonPressed = false;

void setup() {
        Serial.begin(9600);

        pinMode(2, INPUT_PULLUP);  // Button for counting
        pinMode(LED_BUILTIN, OUTPUT);

        Serial.println("=== Minimal Edge Counter Example ===");
        Serial.println("Press button on pin 2 to trigger edge counting");

        // Create basic button condition
        int buttonId = logic.addCondition(
            "button",
            &buttonPressed,
            LogicConditionManager::TO_TRUE,
            []() {
                    Serial.println("👆 Button pressed");
                    digitalWrite(LED_BUILTIN, HIGH);
                    delay(100);
                    digitalWrite(LED_BUILTIN, LOW);
            });

        // Count 3 presses within 5 seconds
        logic.addEdgeCountCondition(
            "tripleClick",
            buttonId,
            3,     // 3 button presses
            5000,  // Within 5 seconds
            []() {
                    Serial.println("🎯 TRIPLE CLICK detected! (3 presses in 5 seconds)");
                    for (int i = 0; i < 5; i++) {
                            digitalWrite(LED_BUILTIN, HIGH);
                            delay(200);
                            digitalWrite(LED_BUILTIN, LOW);
                            delay(200);
                    }
            });

        // Count 5 presses within 10 seconds
        logic.addEdgeCountCondition(
            "rapidFire",
            buttonId,
            5,      // 5 button presses
            10000,  // Within 10 seconds
            []() {
                    Serial.println("🔥 RAPID FIRE detected! (5 presses in 10 seconds)");
                    for (int i = 0; i < 10; i++) {
                            digitalWrite(LED_BUILTIN, HIGH);
                            delay(100);
                            digitalWrite(LED_BUILTIN, LOW);
                            delay(100);
                    }
            });

        // Count 2 presses within 2 seconds (double click)
        logic.addEdgeCountCondition(
            "doubleClick",
            buttonId,
            2,     // 2 button presses
            2000,  // Within 2 seconds
            []() {
                    Serial.println("⚡ DOUBLE CLICK detected!");
                    tone(8, 1000, 300);
            });

        Serial.println("Edge counters active:");
        Serial.println("- Double click: 2 presses in 2 seconds");
        Serial.println("- Triple click: 3 presses in 5 seconds");
        Serial.println("- Rapid fire: 5 presses in 10 seconds");
}

void loop() {
        buttonPressed = digitalRead(2) == LOW;

        logic.update();

        // Show edge counter status
        static unsigned long lastStatus = 0;
        if (millis() - lastStatus > 5000) {
                Serial.println("\n--- Edge Counter Status ---");
                Serial.print("Double click executions: ");
                Serial.println(logic.getExecutionCount("doubleClick"));

                Serial.print("Triple click executions: ");
                Serial.println(logic.getExecutionCount("tripleClick"));

                Serial.print("Rapid fire executions: ");
                Serial.println(logic.getExecutionCount("rapidFire"));

                Serial.println("-------------------------\n");
                lastStatus = millis();
        }
}
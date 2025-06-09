#define ENABLE_MODULE_LOGIC_CONDITION_MANAGER
#include "Kinematrix.h"

LogicConditionManager logic;

// Test variables for different trigger types
bool switchState = false;
bool buttonPressed = false;
bool sensorActive = false;
bool systemReady = false;
bool alarmTriggered = false;

void setup() {
        Serial.begin(9600);

        pinMode(2, INPUT_PULLUP);  // Switch for WHEN_TRUE/WHEN_FALSE
        pinMode(3, INPUT_PULLUP);  // Button for TO_TRUE/TO_FALSE
        pinMode(4, INPUT_PULLUP);  // Sensor for ON_CHANGE
        pinMode(LED_BUILTIN, OUTPUT);

        Serial.println("=== TriggerType Demonstration System ===");
        Serial.println("Pin 2: Switch (WHEN_TRUE/WHEN_FALSE)");
        Serial.println("Pin 3: Button (TO_TRUE/TO_FALSE)");
        Serial.println("Pin 4: Sensor (ON_CHANGE)");
        Serial.println();

        initializeTriggerTypeExamples();
}

void initializeTriggerTypeExamples() {
        // 1. WHEN_TRUE - Triggers whenever condition is true
        logic.addCondition(
            "switch_WHEN_TRUE",
            &switchState,
            LogicConditionManager::WHEN_TRUE,
            []() {
                    Serial.println("✅ WHEN_TRUE: Switch is currently ON");
                    digitalWrite(LED_BUILTIN, HIGH);
            });

        // 2. WHEN_FALSE - Triggers whenever condition is false
        logic.addCondition(
            "switch_WHEN_FALSE",
            &switchState,
            LogicConditionManager::WHEN_FALSE,
            []() {
                    Serial.println("❌ WHEN_FALSE: Switch is currently OFF");
                    digitalWrite(LED_BUILTIN, LOW);
            });

        // 3. TO_TRUE - Triggers only when condition changes from false to true
        logic.addCondition(
            "button_TO_TRUE",
            &buttonPressed,
            LogicConditionManager::TO_TRUE,
            []() {
                    Serial.println("🔼 TO_TRUE: Button was just pressed (false→true)");
                    tone(8, 1000, 200);
            });

        // 4. TO_FALSE - Triggers only when condition changes from true to false
        logic.addCondition(
            "button_TO_FALSE",
            &buttonPressed,
            LogicConditionManager::TO_FALSE,
            []() {
                    Serial.println("🔽 TO_FALSE: Button was just released (true→false)");
                    tone(8, 500, 200);
            });

        // 5. ON_CHANGE - Triggers on any change (both true→false and false→true)
        logic.addCondition(
            "sensor_ON_CHANGE",
            &sensorActive,
            LogicConditionManager::ON_CHANGE,
            []() {
                    Serial.println("🔄 ON_CHANGE: Sensor state changed (any direction)");
                    // Blink LED to indicate change
                    for (int i = 0; i < 3; i++) {
                            digitalWrite(LED_BUILTIN, HIGH);
                            delay(100);
                            digitalWrite(LED_BUILTIN, LOW);
                            delay(100);
                    }
            });

        // Additional examples with function-based conditions
        logic.addCondition(
            "system_WHEN_TRUE",
            []() { return systemReady; },
            LogicConditionManager::WHEN_TRUE,
            []() {
                    static unsigned long lastMsg = 0;
                    if (millis() - lastMsg > 3000) {
                            Serial.println("🟢 WHEN_TRUE (function): System is ready");
                            lastMsg = millis();
                    }
            });

        logic.addCondition(
            "alarm_TO_TRUE",
            []() { return alarmTriggered; },
            LogicConditionManager::TO_TRUE,
            []() {
                    Serial.println("🚨 TO_TRUE (function): Alarm just triggered!");
                    for (int i = 0; i < 5; i++) {
                            tone(8, 2000, 100);
                            delay(150);
                    }
            });

        Serial.println("All TriggerType examples initialized!");
        Serial.println("Try changing the pin states to see different trigger behaviors");
        Serial.println();
}

void loop() {
        // Update input states
        switchState = digitalRead(2) == LOW;    // Active when pressed (pullup)
        buttonPressed = digitalRead(3) == LOW;  // Active when pressed (pullup)
        sensorActive = digitalRead(4) == LOW;   // Active when pressed (pullup)

        // Simulate system ready condition
        static bool lastSystemReady = false;
        systemReady = (millis() / 5000) % 2 == 0;  // Toggle every 5 seconds
        if (systemReady != lastSystemReady) {
                if (systemReady) {
                        Serial.println("📡 System became ready");
                } else {
                        Serial.println("📡 System became not ready");
                }
                lastSystemReady = systemReady;
        }

        // Simulate alarm condition
        static unsigned long lastAlarmToggle = 0;
        if (millis() - lastAlarmToggle > 15000) {  // Every 15 seconds
                alarmTriggered = !alarmTriggered;
                Serial.print("🚨 Alarm state changed to: ");
                Serial.println(alarmTriggered ? "TRIGGERED" : "CLEARED");
                lastAlarmToggle = millis();
        }

        // Update logic manager
        logic.update();

        // Status display every 10 seconds
        static unsigned long lastStatus = 0;
        if (millis() - lastStatus > 10000) {
                Serial.println("\n--- Current States ---");
                Serial.print("Switch (pin 2): ");
                Serial.println(switchState ? "ON" : "OFF");
                Serial.print("Button (pin 3): ");
                Serial.println(buttonPressed ? "PRESSED" : "RELEASED");
                Serial.print("Sensor (pin 4): ");
                Serial.println(sensorActive ? "ACTIVE" : "INACTIVE");
                Serial.print("System Ready: ");
                Serial.println(systemReady ? "YES" : "NO");
                Serial.print("Alarm: ");
                Serial.println(alarmTriggered ? "TRIGGERED" : "NORMAL");
                Serial.println("-------------------\n");
                lastStatus = millis();
        }

        // Manual trigger simulation
        static unsigned long lastDemo = 0;
        if (millis() - lastDemo > 30000) {  // Every 30 seconds
                Serial.println("\n🎭 DEMO: Manual state changes to show trigger differences");

                // Demonstrate the difference between trigger types
                Serial.println("Setting all states to FALSE...");
                switchState = false;
                buttonPressed = false;
                sensorActive = false;
                logic.update();
                delay(1000);

                Serial.println("Setting all states to TRUE...");
                switchState = true;
                buttonPressed = true;
                sensorActive = true;
                logic.update();
                delay(1000);

                Serial.println("Setting all states back to FALSE...");
                switchState = false;
                buttonPressed = false;
                sensorActive = false;
                logic.update();
                delay(1000);

                Serial.println("Demo complete!\n");
                lastDemo = millis();
        }
}
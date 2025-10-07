#define ENABLE_MODULE_LOGIC_CONDITION_MANAGER
#include "Kinematrix.h"

LogicConditionManager logic;

void setup() {
        Serial.begin(9600);
        pinMode(LED_BUILTIN, OUTPUT);

        logic.addTimerCondition(
            "startupDelay",
            5000,   // 5 seconds
            false,  // No auto reset
            []() {
                    Serial.println("System startup complete!");
                    digitalWrite(LED_BUILTIN, HIGH);
            });

        logic.addTimerCondition(
            "heartbeat",
            10000,  // 10 seconds
            true,   // Auto reset
            []() {
                    Serial.println("System heartbeat");
                    digitalWrite(LED_BUILTIN, !digitalRead(LED_BUILTIN));
            },
            -1  // Infinite repetitions
        );

        logic.addTimerCondition(
            "maintenance",
            30000,  // 30 seconds for demo (normally 5 minutes)
            true,
            []() { Serial.println("Time for maintenance check"); });

        logic.addTimerCondition(
            "shortBlink",
            1000,  // 1 second
            true,
            []() {
                    digitalWrite(LED_BUILTIN, HIGH);
                    delay(100);
                    digitalWrite(LED_BUILTIN, LOW);
            },
            10,   // 10 times
            2000  // Every 2 seconds
        );

        Serial.println("Timer system initialized");
        Serial.println("Startup delay: 5 seconds");
        Serial.println("Heartbeat: every 10 seconds");
        Serial.println("Maintenance: every 30 seconds");
        Serial.println("Blink: 10 times, every 2 seconds");
}

void loop() {
        logic.update();

        unsigned long lastHeartbeat = logic.getLastTriggerTime("heartbeat");
        if (lastHeartbeat > 0) {
                static unsigned long lastPrint = 0;
                if (millis() - lastPrint > 5000) {
                        Serial.print("Last heartbeat: ");
                        Serial.print(millis() - lastHeartbeat);
                        Serial.println(" ms ago");

                        Serial.print("Heartbeat executions: ");
                        Serial.println(logic.getExecutionCount("heartbeat"));

                        Serial.print("Blink executions: ");
                        Serial.println(logic.getExecutionCount("shortBlink"));

                        lastPrint = millis();
                }
        }
}
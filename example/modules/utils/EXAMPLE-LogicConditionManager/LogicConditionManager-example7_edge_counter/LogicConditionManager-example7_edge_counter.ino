#define ENABLE_MODULE_LOGIC_CONDITION_MANAGER
#include "Kinematrix.h"

LogicConditionManager logic;

bool pulseSignal() {
        return digitalRead(9) == HIGH;
}

bool knockSignal() {
        return digitalRead(10) == HIGH;
}

void setup() {
        Serial.begin(9600);
        pinMode(9, INPUT);
        pinMode(10, INPUT);
        pinMode(LED_BUILTIN, OUTPUT);

        int pulseId = logic.addCondition(
            "pulseInput",
            pulseSignal,
            LogicConditionManager::TO_TRUE);

        int knockId = logic.addCondition(
            "knockInput",
            knockSignal,
            LogicConditionManager::TO_TRUE);

        logic.addEdgeCountCondition(
            "rpm_monitor",
            pulseId,
            10,    // Count 10 pulses
            1000,  // Within 1 second
            []() {
                    Serial.println("RPM threshold reached! High speed detected");
                    digitalWrite(LED_BUILTIN, HIGH);
                    delay(500);
                    digitalWrite(LED_BUILTIN, LOW);
            });

        logic.addEdgeCountCondition(
            "pulse_burst",
            pulseId,
            5,    // Count 5 pulses
            500,  // Within 500ms
            []() {
                    Serial.println("Pulse burst detected!");
                    tone(8, 2000, 200);
            });

        logic.addEdgeCountCondition(
            "knock_pattern",
            knockId,
            3,     // 3 knocks
            2000,  // Within 2 seconds
            []() {
                    Serial.println("Knock pattern recognized!");
                    digitalWrite(LED_BUILTIN, HIGH);
                    delay(100);
                    digitalWrite(LED_BUILTIN, LOW);
                    delay(100);
                    digitalWrite(LED_BUILTIN, HIGH);
                    delay(100);
                    digitalWrite(LED_BUILTIN, LOW);
            });

        logic.addEdgeCountCondition(
            "rapid_knock",
            knockId,
            6,     // 6 rapid knocks
            1000,  // Within 1 second
            []() { Serial.println("RAPID KNOCK SEQUENCE DETECTED!"); });

        Serial.println("Edge counter monitoring initialized");
        Serial.println("Pulse input on pin 9");
        Serial.println("Knock input on pin 10");
        Serial.println("RPM: 10 pulses in 1 second");
        Serial.println("Burst: 5 pulses in 500ms");
        Serial.println("Knock: 3 knocks in 2 seconds");
        Serial.println("Rapid: 6 knocks in 1 second");
}

void loop() {
        logic.update();

        static unsigned long lastStatus = 0;
        if (millis() - lastStatus > 3000) {
                Serial.println("--- Edge Counter Status ---");

                if (logic.isConditionMet("rpm_monitor")) {
                        Serial.print("RPM executions: ");
                        Serial.println(logic.getExecutionCount("rpm_monitor"));
                }

                if (logic.isConditionMet("pulse_burst")) {
                        Serial.print("Burst executions: ");
                        Serial.println(logic.getExecutionCount("pulse_burst"));
                }

                if (logic.isConditionMet("knock_pattern")) {
                        Serial.print("Knock executions: ");
                        Serial.println(logic.getExecutionCount("knock_pattern"));
                }

                lastStatus = millis();
        }

        static bool lastPulse = false;
        static bool lastKnock = false;
        bool currentPulse = pulseSignal();
        bool currentKnock = knockSignal();

        if (currentPulse != lastPulse) {
                Serial.println(currentPulse ? "Pulse HIGH" : "Pulse LOW");
                lastPulse = currentPulse;
        }

        if (currentKnock != lastKnock) {
                Serial.println(currentKnock ? "Knock HIGH" : "Knock LOW");
                lastKnock = currentKnock;
        }
}
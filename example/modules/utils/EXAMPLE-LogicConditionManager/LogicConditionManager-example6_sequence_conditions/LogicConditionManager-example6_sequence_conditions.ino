#define ENABLE_MODULE_LOGIC_CONDITION_MANAGER
#include "Kinematrix.h"
LogicConditionManager logic;

bool step1Complete = false;
bool step2Complete = false;
bool step3Complete = false;
bool unlockSequence = false;

void setup() {
        Serial.begin(9600);
        pinMode(6, INPUT_PULLUP);
        pinMode(7, INPUT_PULLUP);
        pinMode(8, INPUT_PULLUP);
        pinMode(9, INPUT_PULLUP);
        pinMode(LED_BUILTIN, OUTPUT);

        int step1Id = logic.addCondition(
            "step1",
            &step1Complete,
            LogicConditionManager::TO_TRUE,
            []() { Serial.println("Step 1 completed"); });

        int step2Id = logic.addCondition(
            "step2",
            &step2Complete,
            LogicConditionManager::TO_TRUE,
            []() { Serial.println("Step 2 completed"); });

        int step3Id = logic.addCondition(
            "step3",
            &step3Complete,
            LogicConditionManager::TO_TRUE,
            []() { Serial.println("Step 3 completed"); });

        logic.addSequenceCondition(
            "initSequence",
            new int[3]{step1Id, step2Id, step3Id},
            3,
            15000,  // Must complete within 15 seconds
            3,      // All 3 steps required
            []() {
                    Serial.println("Initialization sequence completed successfully!");
                    digitalWrite(LED_BUILTIN, HIGH);
            });

        logic.addSequenceCondition(
            "quickSequence",
            new int[2]{step1Id, step3Id},
            2,
            5000,  // Within 5 seconds
            2,     // Both steps required
            []() { Serial.println("Quick sequence activated!"); });

        int unlockId = logic.addCondition(
            "unlockTrigger",
            &unlockSequence,
            LogicConditionManager::TO_TRUE);

        logic.addSequenceCondition(
            "unlockSequence",
            new int[4]{step1Id, step2Id, step1Id, unlockId},
            4,
            10000,  // Within 10 seconds
            4,      // All steps in order
            []() { Serial.println("UNLOCK SEQUENCE ACTIVATED!"); });

        Serial.println("Sequence monitoring initialized");
        Serial.println("Press buttons 6,7,8,9 to trigger steps");
        Serial.println("Init sequence: Step1 -> Step2 -> Step3 (15s)");
        Serial.println("Quick sequence: Step1 -> Step3 (5s)");
        Serial.println("Unlock sequence: Step1 -> Step2 -> Step1 -> Unlock (10s)");
}

void loop() {
        step1Complete = digitalRead(6) == LOW;
        step2Complete = digitalRead(7) == LOW;
        step3Complete = digitalRead(8) == LOW;
        unlockSequence = digitalRead(9) == LOW;

        logic.update();

        static unsigned long lastStatus = 0;
        if (millis() - lastStatus > 2000) {
                if (logic.getConditionState("initSequence") == LogicConditionManager::TRIGGERED) {
                        Serial.println("Initialization sequence in progress...");
                }

                if (logic.getConditionState("quickSequence") == LogicConditionManager::TRIGGERED) {
                        Serial.println("Quick sequence in progress...");
                }

                if (logic.getConditionState("unlockSequence") == LogicConditionManager::TRIGGERED) {
                        Serial.println("Unlock sequence in progress...");
                }

                lastStatus = millis();
        }

        if (logic.isConditionMet("initSequence") &&
            logic.getConditionState("initSequence") == LogicConditionManager::COMPLETED) {
                logic.resetCondition("initSequence");
                delay(2000);
                digitalWrite(LED_BUILTIN, LOW);
        }
}
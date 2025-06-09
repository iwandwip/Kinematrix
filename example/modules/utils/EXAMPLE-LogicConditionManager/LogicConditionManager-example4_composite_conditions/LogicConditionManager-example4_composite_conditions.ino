#define ENABLE_MODULE_LOGIC_CONDITION_MANAGER
#include "Kinematrix.h"

LogicConditionManager logic;

bool isDoorOpen() { return digitalRead(4) == HIGH; }
bool isMotionDetected() { return digitalRead(5) == HIGH; }
bool isNightTime() { return analogRead(A1) < 100; }

void securityAlert() {
        Serial.println("SECURITY ALERT: Motion detected with door open!");
        tone(8, 1000, 500);
}

void setup() {
        Serial.begin(9600);
        pinMode(4, INPUT);
        pinMode(5, INPUT);
        pinMode(8, OUTPUT);

        int doorId = logic.addCondition(
            "doorOpen",
            isDoorOpen,
            LogicConditionManager::WHEN_TRUE);

        int motionId = logic.addCondition(
            "motionDetected",
            isMotionDetected,
            LogicConditionManager::WHEN_TRUE);

        int nightId = logic.addCondition(
            "nightTime",
            isNightTime,
            LogicConditionManager::WHEN_TRUE);

        logic.addAndCondition(
            "securityThreat",
            new int[2]{doorId, motionId},
            2,
            securityAlert);

        logic.addOrCondition(
            "anyActivity",
            new int[2]{doorId, motionId},
            2,
            []() { Serial.println("Activity detected"); });

        logic.addAndCondition(
            "nightSecurity",
            new int[3]{doorId, motionId, nightId},
            3,
            []() { Serial.println("Night security breach!"); });

        logic.addNotCondition(
            "dayTime",
            nightId,
            []() { Serial.println("Daytime mode activated"); });
}

void loop() {
        logic.update();

        if (logic.isConditionMet("anyActivity")) {
                Serial.print("Security status: ");
                Serial.println(logic.getConditionState("securityThreat"));
        }

        static unsigned long lastStatus = 0;
        if (millis() - lastStatus > 3000) {
                Serial.println("--- System Status ---");
                logic.printStatus("doorOpen");
                logic.printStatus("motionDetected");
                logic.printStatus("securityThreat");
                lastStatus = millis();
        }
}
#define ENABLE_MODULE_LOGIC_CONDITION_MANAGER
#include "Kinematrix.h"

LogicConditionManager logic;
bool ledState = false;

void setup() {
        Serial.begin(9600);
        pinMode(2, INPUT);

        logic.addCondition(
            "ledMonitor",
            &ledState,
            LogicConditionManager::TO_TRUE,
            []() { Serial.println("LED turned ON!"); });
}

void loop() {
        ledState = digitalRead(2);
        logic.update();
}
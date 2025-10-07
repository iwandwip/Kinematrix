#define ENABLE_MODULE_LOGIC_CONDITION_MANAGER
#include "Kinematrix.h"

LogicConditionManager logic;

bool isButtonPressed() {
        return digitalRead(3) == LOW;
}

void buttonPressedAction() {
        Serial.println("Button was pressed!");
        digitalWrite(LED_BUILTIN, HIGH);
        delay(500);
        digitalWrite(LED_BUILTIN, LOW);
}

void setup() {
        Serial.begin(9600);
        pinMode(3, INPUT_PULLUP);
        pinMode(LED_BUILTIN, OUTPUT);

        logic.addCondition(
            "buttonPress",
            isButtonPressed,
            LogicConditionManager::TO_TRUE,
            buttonPressedAction,
            1,   // Execute once per trigger
            0,   // No interval
            200  // 200ms debounce
        );
}

void loop() {
        logic.update();

        if (logic.isConditionMet("buttonPress")) {
                Serial.println("Button condition is currently met");
        }
}
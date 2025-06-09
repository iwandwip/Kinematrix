#define ENABLE_MODULE_LOGIC_CONDITION_MANAGER
#include "Kinematrix.h"
LogicConditionManager logic;

float getTemperature() {
        return analogRead(A0) * 0.1;
}

void setup() {
        Serial.begin(9600);
        pinMode(13, OUTPUT);

        logic.addValueCondition(
            "overheating",
            getTemperature,
            [](float temp) { return temp > 25.0; },
            LogicConditionManager::WHEN_TRUE,
            []() { Serial.println("WARNING: Overheating detected!"); },
            3,    // Execute 3 times
            2000  // Every 2 seconds
        );

        logic.addValueCondition(
            "tempNormal",
            getTemperature,
            [](float temp) { return temp <= 20.0; },
            LogicConditionManager::TO_TRUE,
            []() { Serial.println("Temperature back to normal"); });

        logic.addValueCondition(
            "tempChange",
            getTemperature,
            [](float temp) { return temp > 15.0; },
            LogicConditionManager::ON_CHANGE,
            []() { Serial.println("Temperature threshold changed"); });
}

void loop() {
        logic.update();

        if (logic.getCurrentBooleanValue("overheating")) {
                digitalWrite(13, HIGH);
        } else {
                digitalWrite(13, LOW);
        }

        static unsigned long lastPrint = 0;
        if (millis() - lastPrint > 5000) {
                Serial.print("Current temperature: ");
                Serial.println(getTemperature());
                lastPrint = millis();
        }
}
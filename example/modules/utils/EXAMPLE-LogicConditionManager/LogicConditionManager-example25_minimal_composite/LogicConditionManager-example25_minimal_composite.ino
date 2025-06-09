#define ENABLE_MODULE_LOGIC_CONDITION_MANAGER
#include "Kinematrix.h"

LogicConditionManager logic;

bool switch1 = false;
bool switch2 = false;
bool button1 = false;
bool sensor1 = false;

void setup() {
        Serial.begin(9600);

        pinMode(2, INPUT_PULLUP);  // Switch 1
        pinMode(3, INPUT_PULLUP);  // Switch 2
        pinMode(4, INPUT_PULLUP);  // Button 1
        pinMode(5, INPUT_PULLUP);  // Sensor 1
        pinMode(LED_BUILTIN, OUTPUT);

        Serial.println("=== Minimal Composite Conditions Example ===");
        Serial.println("Pins: 2=Switch1, 3=Switch2, 4=Button, 5=Sensor");

        // Create basic conditions
        int switch1Id = logic.addCondition(
            "switch1",
            &switch1,
            LogicConditionManager::WHEN_TRUE,
            []() { Serial.println("Switch 1 ON"); });

        int switch2Id = logic.addCondition(
            "switch2",
            &switch2,
            LogicConditionManager::WHEN_TRUE,
            []() { Serial.println("Switch 2 ON"); });

        int buttonId = logic.addCondition(
            "button1",
            &button1,
            LogicConditionManager::WHEN_TRUE,
            []() { Serial.println("Button pressed"); });

        int sensorId = logic.addCondition(
            "sensor1",
            &sensor1,
            LogicConditionManager::WHEN_TRUE,
            []() { Serial.println("Sensor active"); });

        // AND: Both switches must be ON
        logic.addAndCondition(
            "bothSwitches",
            new int[2]{switch1Id, switch2Id},
            2,
            []() {
                    Serial.println("✅ AND: Both switches are ON");
                    digitalWrite(LED_BUILTIN, HIGH);
            });

        // OR: Either switch OR button
        logic.addOrCondition(
            "anyInput",
            new int[3]{switch1Id, switch2Id, buttonId},
            3,
            []() {
                    Serial.println("✅ OR: At least one input is active");
            });

        // NOT: Switch1 is OFF
        logic.addNotCondition(
            "switch1Off",
            switch1Id,
            []() {
                    Serial.println("✅ NOT: Switch 1 is OFF");
                    digitalWrite(LED_BUILTIN, LOW);
            });

        // XOR: Switch1 and Switch2 are different
        logic.addCompositeCondition(
            "switchesDifferent",
            LogicConditionManager::XOR,
            new int[2]{switch1Id, switch2Id},
            2,
            []() {
                    Serial.println("✅ XOR: Switches are in different states");
                    tone(8, 1500, 300);
            });

        // Complex: (Switch1 AND Switch2) OR (Button AND Sensor)
        int bothSwitchesId = logic.addAndCondition(
            "switchesAnd",
            new int[2]{switch1Id, switch2Id},
            2);

        int buttonSensorId = logic.addAndCondition(
            "buttonSensorAnd",
            new int[2]{buttonId, sensorId},
            2);

        logic.addOrCondition(
            "complexCondition",
            new int[2]{bothSwitchesId, buttonSensorId},
            2,
            []() {
                    Serial.println("🧠 COMPLEX: (Switch1 AND Switch2) OR (Button AND Sensor)");
                    for (int i = 0; i < 3; i++) {
                            digitalWrite(LED_BUILTIN, HIGH);
                            delay(200);
                            digitalWrite(LED_BUILTIN, LOW);
                            delay(200);
                    }
            });

        Serial.println("Composite conditions active:");
        Serial.println("- AND: Both switches ON");
        Serial.println("- OR: Any input active");
        Serial.println("- NOT: Switch1 OFF");
        Serial.println("- XOR: Switches different");
        Serial.println("- COMPLEX: (S1&S2) OR (Button&Sensor)");
}

void loop() {
        // Read all inputs
        switch1 = digitalRead(2) == LOW;
        switch2 = digitalRead(3) == LOW;
        button1 = digitalRead(4) == LOW;
        sensor1 = digitalRead(5) == LOW;

        logic.update();

        // Display current input states
        static unsigned long lastDisplay = 0;
        if (millis() - lastDisplay > 3000) {
                Serial.print("Inputs: S1=");
                Serial.print(switch1 ? "1" : "0");
                Serial.print(" S2=");
                Serial.print(switch2 ? "1" : "0");
                Serial.print(" B=");
                Serial.print(button1 ? "1" : "0");
                Serial.print(" Sen=");
                Serial.println(sensor1 ? "1" : "0");

                Serial.print("Results: AND=");
                Serial.print(logic.getCurrentBooleanValue("bothSwitches") ? "1" : "0");
                Serial.print(" OR=");
                Serial.print(logic.getCurrentBooleanValue("anyInput") ? "1" : "0");
                Serial.print(" NOT=");
                Serial.print(logic.getCurrentBooleanValue("switch1Off") ? "1" : "0");
                Serial.print(" XOR=");
                Serial.print(logic.getCurrentBooleanValue("switchesDifferent") ? "1" : "0");
                Serial.print(" COMPLEX=");
                Serial.println(logic.getCurrentBooleanValue("complexCondition") ? "1" : "0");

                lastDisplay = millis();
        }
}
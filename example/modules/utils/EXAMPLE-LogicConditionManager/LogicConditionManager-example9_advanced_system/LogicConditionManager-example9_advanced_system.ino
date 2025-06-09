#define ENABLE_MODULE_LOGIC_CONDITION_MANAGER
#include "Kinematrix.h"

LogicConditionManager logic;

float sensorValue = 0;
bool emergencyStop = false;
bool maintenanceMode = false;
bool systemEnabled = true;
bool coolingActive = false;

void emergencyShutdown() {
        Serial.println("EMERGENCY SHUTDOWN INITIATED!");
        digitalWrite(10, LOW);
        systemEnabled = false;

        for (int i = 0; i < 5; i++) {
                digitalWrite(LED_BUILTIN, HIGH);
                tone(12, 2000, 200);
                delay(200);
                digitalWrite(LED_BUILTIN, LOW);
                delay(200);
        }
}

void activateCooling() {
        Serial.println("Cooling system activated");
        coolingActive = true;
        digitalWrite(11, HIGH);
}

void deactivateCooling() {
        Serial.println("Cooling system deactivated");
        coolingActive = false;
        digitalWrite(11, LOW);
}

void systemStartup() {
        Serial.println("System startup initiated");
        systemEnabled = true;
        digitalWrite(10, HIGH);
        logic.resetCondition("emergencyShutdown");
}

void maintenanceAlert() {
        Serial.println("Maintenance mode requested");
}

void setup() {
        Serial.begin(9600);
        pinMode(10, OUTPUT);  // System power
        pinMode(11, OUTPUT);  // Cooling
        pinMode(12, OUTPUT);  // Buzzer
        pinMode(LED_BUILTIN, OUTPUT);
        pinMode(2, INPUT_PULLUP);  // Emergency button
        pinMode(3, INPUT_PULLUP);  // Maintenance switch

        digitalWrite(10, HIGH);  // System initially on

        int tempId = logic.addValueCondition(
            "criticalTemp",
            []() { return sensorValue; },
            [](float val) { return val > 80.0; },
            LogicConditionManager::WHEN_TRUE,
            []() { Serial.println("Critical temperature reached!"); });

        int highTempId = logic.addValueCondition(
            "highTemp",
            []() { return sensorValue; },
            [](float val) { return val > 50.0 && val <= 80.0; },
            LogicConditionManager::TO_TRUE,
            activateCooling);

        int normalTempId = logic.addValueCondition(
            "normalTemp",
            []() { return sensorValue; },
            [](float val) { return val <= 45.0; },
            LogicConditionManager::TO_TRUE,
            deactivateCooling);

        int emergencyId = logic.addCondition(
            "emergencyButton",
            &emergencyStop,
            LogicConditionManager::TO_TRUE,
            []() { Serial.println("Emergency button pressed!"); });

        int maintenanceId = logic.addCondition(
            "maintenanceMode",
            &maintenanceMode,
            LogicConditionManager::TO_TRUE,
            maintenanceAlert);

        int systemEnabledId = logic.addCondition(
            "systemEnabled",
            &systemEnabled,
            LogicConditionManager::WHEN_TRUE);

        logic.addOrCondition(
            "emergencyShutdown",
            new int[2]{tempId, emergencyId},
            2,
            emergencyShutdown);

        logic.addAndCondition(
            "normalOperation",
            new int[3]{
                logic.addNotCondition("notCriticalTemp", tempId),
                logic.addNotCondition("notEmergency", emergencyId),
                logic.addNotCondition("notMaintenance", maintenanceId)},
            3,
            []() {
                    static unsigned long lastNormalMsg = 0;
                    if (millis() - lastNormalMsg > 10000) {
                            Serial.println("System operating normally");
                            lastNormalMsg = millis();
                    }
            });

        logic.addAndCondition(
            "coolingRequired",
            new int[2]{highTempId, systemEnabledId},
            2,
            []() { Serial.println("Cooling required and system enabled"); });

        logic.addTimerCondition(
            "systemHeartbeat",
            5000,
            true,
            []() {
                    Serial.println("System heartbeat - All systems operational");
                    digitalWrite(LED_BUILTIN, HIGH);
                    delay(50);
                    digitalWrite(LED_BUILTIN, LOW);
            });

        logic.addSequenceCondition(
            "restartSequence",
            new int[3]{emergencyId, maintenanceId, emergencyId},
            3,
            10000,
            3,
            systemStartup);

        Serial.println("Advanced system monitoring initialized");
        Serial.println("Emergency button: pin 2");
        Serial.println("Maintenance switch: pin 3");
        Serial.println("Critical temp: >80, High temp: 50-80, Normal: <=45");
}

void loop() {
        sensorValue = analogRead(A0) * 0.2;
        emergencyStop = digitalRead(2) == LOW;
        maintenanceMode = digitalRead(3) == LOW;

        logic.update();

        logic.setEnabled("emergencyShutdown", !maintenanceMode);
        logic.setEnabled("systemHeartbeat", systemEnabled && !maintenanceMode);

        if (logic.isConditionMet("normalOperation")) {
                digitalWrite(LED_BUILTIN, systemEnabled ? HIGH : LOW);
        }

        if (maintenanceMode) {
                logic.setEnabled("criticalTemp", false);
                logic.setEnabled("highTemp", false);
                logic.setEnabled("normalTemp", false);
        } else {
                logic.setEnabled("criticalTemp", true);
                logic.setEnabled("highTemp", true);
                logic.setEnabled("normalTemp", true);
        }

        static unsigned long lastStatus = 0;
        if (millis() - lastStatus > 8000) {
                Serial.println("\n=== SYSTEM STATUS ===");
                Serial.print("Temperature: ");
                Serial.println(sensorValue);
                Serial.print("System Enabled: ");
                Serial.println(systemEnabled ? "YES" : "NO");
                Serial.print("Emergency: ");
                Serial.println(emergencyStop ? "ACTIVE" : "NORMAL");
                Serial.print("Maintenance: ");
                Serial.println(maintenanceMode ? "ACTIVE" : "NORMAL");
                Serial.print("Cooling: ");
                Serial.println(coolingActive ? "ON" : "OFF");

                logic.printStatus("normalOperation");
                logic.printStatus("emergencyShutdown");
                logic.printStatus("coolingRequired");

                Serial.println("==================\n");
                lastStatus = millis();
        }
}
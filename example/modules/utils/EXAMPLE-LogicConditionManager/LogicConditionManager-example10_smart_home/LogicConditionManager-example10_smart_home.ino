#define ENABLE_MODULE_LOGIC_CONDITION_MANAGER
#include "Kinematrix.h"

LogicConditionManager logic;

class SmartHomeSystem {
       private:
        bool doorOpen = false;
        bool windowOpen = false;
        bool motionDetected = false;
        bool lightOn = false;
        bool presenceDetected = false;
        float temperature = 22.0;
        float humidity = 45.0;
        bool awayMode = false;
        bool securityArmed = false;

       public:
        void setup() {
                Serial.begin(9600);
                pinMode(2, INPUT);          // Door sensor
                pinMode(3, INPUT);          // Window sensor
                pinMode(4, INPUT);          // Motion sensor
                pinMode(5, INPUT);          // Presence sensor
                pinMode(6, OUTPUT);         // Light control
                pinMode(7, OUTPUT);         // Ventilation
                pinMode(8, OUTPUT);         // Security alarm
                pinMode(9, INPUT_PULLUP);   // Away mode switch
                pinMode(10, INPUT_PULLUP);  // Security arm switch

                initializeConditions();

                Serial.println("Smart Home System Initialized");
                Serial.println("Sensors: Door(2), Window(3), Motion(4), Presence(5)");
                Serial.println("Controls: Light(6), Ventilation(7), Alarm(8)");
                Serial.println("Switches: Away(9), Security(10)");
        }

        void initializeConditions() {
                int doorId = logic.addCondition(
                    "frontDoor",
                    &doorOpen,
                    LogicConditionManager::ON_CHANGE,
                    []() { Serial.println("Door status changed"); },
                    1, 0, 500);

                int windowId = logic.addCondition(
                    "windowSensor",
                    &windowOpen,
                    LogicConditionManager::WHEN_TRUE,
                    []() { Serial.println("Window opened"); });

                int motionId = logic.addCondition(
                    "motionSensor",
                    &motionDetected,
                    LogicConditionManager::TO_TRUE,
                    [this]() {
                            Serial.println("Motion detected - turning on lights");
                            lightOn = true;
                            digitalWrite(6, HIGH);
                    });

                int presenceId = logic.addCondition(
                    "presenceDetector",
                    &presenceDetected,
                    LogicConditionManager::WHEN_TRUE);

                int tempId = logic.addValueCondition(
                    "comfortableTemp",
                    [this]() { return temperature; },
                    [](float t) { return t >= 20.0 && t <= 25.0; },
                    LogicConditionManager::WHEN_FALSE,
                    []() { Serial.println("Temperature out of comfort range"); });

                int humidityId = logic.addValueCondition(
                    "highHumidity",
                    [this]() { return humidity; },
                    [](float h) { return h > 60.0; },
                    LogicConditionManager::WHEN_TRUE,
                    []() {
                            Serial.println("High humidity - starting ventilation");
                            digitalWrite(7, HIGH);
                    });

                int lowHumidityId = logic.addValueCondition(
                    "lowHumidity",
                    [this]() { return humidity; },
                    [](float h) { return h <= 40.0; },
                    LogicConditionManager::WHEN_TRUE,
                    []() {
                            Serial.println("Humidity normalized - stopping ventilation");
                            digitalWrite(7, LOW);
                    });

                logic.addAndCondition(
                    "ventilationNeeded",
                    new int[2]{humidityId, tempId},
                    2,
                    []() { Serial.println("Climate control: Ventilation system activated"); });

                logic.addOrCondition(
                    "securityBreach",
                    new int[2]{doorId, windowId},
                    2,
                    [this]() {
                            if (securityArmed) {
                                    Serial.println("SECURITY ALERT: Perimeter breach detected!");
                                    digitalWrite(8, HIGH);
                                    delay(5000);
                                    digitalWrite(8, LOW);
                            }
                    });

                logic.addAndCondition(
                    "awayModeActive",
                    new int[3]{
                        logic.addNotCondition("noMotion", motionId),
                        logic.addNotCondition("noPresence", presenceId),
                        logic.addNotCondition("doorClosed", doorId)},
                    3,
                    [this]() {
                            Serial.println("Away mode conditions met - House secured");
                            awayMode = true;
                    });

                logic.addTimerCondition(
                    "energySaver",
                    60000,  // 1 minute for demo
                    true,
                    [this]() {
                            if (!presenceDetected && !motionDetected) {
                                    Serial.println("Energy saving mode activated");
                                    lightOn = false;
                                    digitalWrite(6, LOW);
                            }
                    });

                int lightOffId = logic.addCondition(
                    "lightsOff",
                    &lightOn,
                    LogicConditionManager::WHEN_FALSE);

                logic.addSequenceCondition(
                    "nightRoutine",
                    new int[3]{
                        doorId,
                        lightOffId,
                        logic.addCondition("noMotion5min", [this]() { return !motionDetected; }, LogicConditionManager::WHEN_TRUE)},
                    3,
                    300000,  // 5 minutes
                    3, []() { 
                Serial.println("Night routine completed - Security mode ARMED");
                digitalWrite(8, HIGH);
                delay(100);
                digitalWrite(8, LOW); });

                logic.addEdgeCountCondition(
                    "doorKnocking",
                    doorId,
                    3,     // 3 door activations
                    5000,  // Within 5 seconds
                    []() { Serial.println("Knock pattern detected at door"); });

                logic.addTimerCondition(
                    "systemStatus",
                    30000,  // Every 30 seconds
                    true,
                    [this]() { printSystemStatus(); });
        }

        void loop() {
                updateSensors();
                logic.update();

                if (logic.isConditionMet("motionSensor")) {
                        logic.resetCondition("energySaver");
                }

                if (logic.isConditionMet("awayModeActive")) {
                        Serial.println("All systems secured for away mode");
                }

                if (logic.checkAndTrigger("ventilationNeeded",
                                          []() { Serial.println("Custom climate control callback"); })) {
                        Serial.println("Advanced climate control activated");
                }

                if (digitalRead(9) == LOW && !awayMode) {
                        Serial.println("Manual away mode activated");
                        awayMode = true;
                        securityArmed = true;
                } else if (digitalRead(9) == HIGH && awayMode) {
                        Serial.println("Away mode deactivated");
                        awayMode = false;
                }

                securityArmed = digitalRead(10) == LOW;

                manageLighting();
        }

       private:
        void updateSensors() {
                doorOpen = digitalRead(2);
                windowOpen = digitalRead(3);
                motionDetected = digitalRead(4);
                presenceDetected = digitalRead(5);
                temperature = 20.0 + (analogRead(A0) * 0.02);
                humidity = 30.0 + (analogRead(A1) * 0.05);
        }

        void manageLighting() {
                if (motionDetected || presenceDetected) {
                        if (!lightOn) {
                                lightOn = true;
                                digitalWrite(6, HIGH);
                        }
                }
        }

        void printSystemStatus() {
                Serial.println("\n=== SMART HOME STATUS ===");
                Serial.print("Door: ");
                Serial.println(doorOpen ? "OPEN" : "CLOSED");
                Serial.print("Window: ");
                Serial.println(windowOpen ? "OPEN" : "CLOSED");
                Serial.print("Motion: ");
                Serial.println(motionDetected ? "DETECTED" : "NONE");
                Serial.print("Presence: ");
                Serial.println(presenceDetected ? "YES" : "NO");
                Serial.print("Lights: ");
                Serial.println(lightOn ? "ON" : "OFF");
                Serial.print("Temperature: ");
                Serial.print(temperature);
                Serial.println("°C");
                Serial.print("Humidity: ");
                Serial.print(humidity);
                Serial.println("%");
                Serial.print("Away Mode: ");
                Serial.println(awayMode ? "ACTIVE" : "INACTIVE");
                Serial.print("Security: ");
                Serial.println(securityArmed ? "ARMED" : "DISARMED");

                Serial.println("\n--- Condition Status ---");
                logic.printStatus("securityBreach");
                logic.printStatus("ventilationNeeded");
                logic.printStatus("awayModeActive");
                logic.printStatus("energySaver");

                Serial.println("========================\n");
        }
};

SmartHomeSystem smartHome;

void setup() {
        smartHome.setup();
}

void loop() {
        smartHome.loop();
}
#define ENABLE_MODULE_LOGIC_CONDITION_MANAGER
#include "Kinematrix.h"

LogicConditionManager logic;

class VehicleSystem {
       private:
        float engineTemp = 90.0;
        float oilPressure = 4.0;
        float fuelLevel = 75.0;
        float speed = 0.0;
        float rpm = 800.0;
        bool engineRunning = false;
        bool doorOpen = false;
        bool seatBeltFastened = true;
        bool handbrakeEngaged = true;
        bool headlightsOn = false;
        bool turnSignalLeft = false;
        bool turnSignalRight = false;
        bool brakePressed = false;

       public:
        void setup() {
                Serial.begin(9600);

                pinMode(2, INPUT);    // Engine start button
                pinMode(3, INPUT);    // Door sensor
                pinMode(4, INPUT);    // Seatbelt sensor
                pinMode(5, INPUT);    // Handbrake sensor
                pinMode(6, INPUT);    // Brake pedal
                pinMode(7, OUTPUT);   // Engine warning
                pinMode(8, OUTPUT);   // Fuel warning
                pinMode(9, OUTPUT);   // Safety warning
                pinMode(10, OUTPUT);  // Dashboard LED
                pinMode(11, OUTPUT);  // Buzzer

                initializeVehicleSystem();
                Serial.println("Vehicle Monitoring System Initialized");
        }

        void initializeVehicleSystem() {
                int engineTempId = logic.addValueCondition(
                    "engineOverheat",
                    [this]() { return engineTemp; },
                    [](float temp) { return temp > 110.0; },
                    LogicConditionManager::WHEN_TRUE,
                    [this]() {
                            Serial.println("ENGINE WARNING: Overheating detected!");
                            digitalWrite(7, HIGH);
                            tone(11, 1000, 500);
                    });

                int oilPressureId = logic.addValueCondition(
                    "lowOilPressure",
                    [this]() { return oilPressure; },
                    [](float pressure) { return pressure < 1.5; },
                    LogicConditionManager::WHEN_TRUE,
                    [this]() {
                            Serial.println("ENGINE WARNING: Low oil pressure!");
                            digitalWrite(7, HIGH);
                            for (int i = 0; i < 3; i++) {
                                    tone(11, 2000, 200);
                                    delay(300);
                            }
                    });

                int fuelLowId = logic.addValueCondition(
                    "fuelLow",
                    [this]() { return fuelLevel; },
                    [](float fuel) { return fuel < 15.0; },
                    LogicConditionManager::TO_TRUE,
                    [this]() {
                            Serial.println("FUEL WARNING: Low fuel level");
                            digitalWrite(8, HIGH);
                            tone(11, 800, 300);
                    });

                int fuelCriticalId = logic.addValueCondition(
                    "fuelCritical",
                    [this]() { return fuelLevel; },
                    [](float fuel) { return fuel < 5.0; },
                    LogicConditionManager::WHEN_TRUE,
                    [this]() {
                            Serial.println("FUEL CRITICAL: Immediate refueling required!");
                            for (int i = 0; i < 5; i++) {
                                    digitalWrite(8, HIGH);
                                    tone(11, 1500, 100);
                                    delay(150);
                                    digitalWrite(8, LOW);
                                    delay(150);
                            }
                    });

                int speedingId = logic.addValueCondition(
                    "speeding",
                    [this]() { return speed; },
                    [](float spd) { return spd > 120.0; },
                    LogicConditionManager::WHEN_TRUE,
                    []() { Serial.println("SPEED WARNING: Exceeding speed limit!"); });

                int engineId = logic.addCondition(
                    "engineRunning",
                    &engineRunning,
                    LogicConditionManager::WHEN_TRUE);

                int doorId = logic.addCondition(
                    "doorOpen",
                    &doorOpen,
                    LogicConditionManager::WHEN_TRUE,
                    []() { Serial.println("Door opened while vehicle running"); });

                int seatbeltId = logic.addCondition(
                    "seatbeltUnfastened",
                    &seatBeltFastened,
                    LogicConditionManager::WHEN_FALSE);

                int handbrakeId = logic.addCondition(
                    "handbrakeEngaged",
                    &handbrakeEngaged,
                    LogicConditionManager::WHEN_TRUE);

                logic.addAndCondition(
                    "safetyViolation",
                    new int[3]{engineId, doorId, seatbeltId},
                    3,
                    [this]() {
                            Serial.println("SAFETY VIOLATION: Multiple safety issues detected!");
                            digitalWrite(9, HIGH);
                            tone(11, 2500, 1000);
                            delay(1000);
                            digitalWrite(9, LOW);
                    });

                logic.addAndCondition(
                    "drivingUnsafe",
                    new int[2]{seatbeltId, speedingId},
                    2,
                    []() { Serial.println("UNSAFE DRIVING: Seatbelt unfastened while speeding!"); });

                logic.addSequenceCondition(
                    "startupSequence",
                    new int[3]{
                        logic.addCondition("seatbeltCheck", &seatBeltFastened, LogicConditionManager::WHEN_TRUE),
                        logic.addCondition("handbrakeCheck", &handbrakeEngaged, LogicConditionManager::WHEN_TRUE),
                        logic.addCondition("doorCheck", &doorOpen, LogicConditionManager::WHEN_FALSE)},
                    3,
                    10000,  // 10 seconds
                    3,
                    [this]() {
                            Serial.println("Pre-start safety checks passed - Ready to start");
                            digitalWrite(10, HIGH);
                    });

                logic.addTimerCondition(
                    "engineIdleWarning",
                    300000,  // 5 minutes
                    false,
                    [this]() {
                            if (engineRunning && speed < 5.0) {
                                    Serial.println("Engine idling too long - Consider turning off");
                            }
                    });

                logic.addEdgeCountCondition(
                    "rapidBraking",
                    logic.addCondition("brakePressed", &brakePressed, LogicConditionManager::TO_TRUE),
                    3,     // 3 brake applications
                    5000,  // Within 5 seconds
                    []() { Serial.println("DRIVING ALERT: Rapid braking pattern detected"); });

                logic.addOrCondition(
                    "criticalWarning",
                    new int[3]{engineTempId, oilPressureId, fuelCriticalId},
                    3,
                    [this]() {
                            Serial.println("CRITICAL SYSTEM WARNING - Pull over safely!");
                            digitalWrite(7, HIGH);
                            digitalWrite(8, HIGH);
                            digitalWrite(9, HIGH);
                    });

                logic.addAndCondition(
                    "readyToDrive",
                    new int[4]{
                        engineId,
                        logic.addNotCondition("seatbeltOK", seatbeltId),
                        logic.addNotCondition("doorClosed", doorId),
                        logic.addNotCondition("handbrakeReleased", handbrakeId)},
                    4,
                    [this]() {
                            Serial.println("Vehicle ready to drive - All safety checks passed");
                            digitalWrite(10, HIGH);
                    });

                logic.addTimerCondition(
                    "dashboardUpdate",
                    2000,  // Every 2 seconds
                    true,
                    [this]() { updateDashboard(); });
        }

        void loop() {
                updateSensors();
                logic.update();

                engineRunning = digitalRead(2);
                doorOpen = digitalRead(3);
                seatBeltFastened = digitalRead(4);
                handbrakeEngaged = digitalRead(5);
                brakePressed = digitalRead(6);

                if (!logic.isConditionMet("fuelLow")) {
                        digitalWrite(8, LOW);
                }

                if (engineTemp < 105.0 && oilPressure > 2.0) {
                        digitalWrite(7, LOW);
                }

                if (logic.isConditionMet("readyToDrive")) {
                        digitalWrite(10, HIGH);
                } else {
                        static unsigned long lastBlink = 0;
                        if (millis() - lastBlink > 500) {
                                digitalWrite(10, !digitalRead(10));
                                lastBlink = millis();
                        }
                }

                handleTurnSignals();
        }

       private:
        void updateSensors() {
                engineTemp = 85.0 + (analogRead(A0) * 0.05);
                oilPressure = 2.0 + (analogRead(A1) * 0.01);
                fuelLevel = analogRead(A2) * 0.1;
                speed = analogRead(A3) * 0.2;

                if (engineRunning) {
                        rpm = 800 + (speed * 50);
                        fuelLevel -= 0.001;  // Fuel consumption
                } else {
                        rpm = 0;
                }
        }

        void handleTurnSignals() {
                static unsigned long lastBlink = 0;
                static bool blinkState = false;

                if (millis() - lastBlink > 500) {
                        if (turnSignalLeft || turnSignalRight) {
                                blinkState = !blinkState;
                                Serial.print("Turn signal: ");
                                Serial.println(turnSignalLeft ? "LEFT" : "RIGHT");
                        }
                        lastBlink = millis();
                }
        }

        void updateDashboard() {
                Serial.println("\n=== VEHICLE DASHBOARD ===");
                Serial.print("Engine: ");
                Serial.println(engineRunning ? "RUNNING" : "OFF");
                Serial.print("Speed: ");
                Serial.print(speed);
                Serial.println(" km/h");
                Serial.print("RPM: ");
                Serial.println(rpm);
                Serial.print("Engine Temp: ");
                Serial.print(engineTemp);
                Serial.println(" °C");
                Serial.print("Oil Pressure: ");
                Serial.print(oilPressure);
                Serial.println(" bar");
                Serial.print("Fuel Level: ");
                Serial.print(fuelLevel);
                Serial.println(" %");
                Serial.print("Seatbelt: ");
                Serial.println(seatBeltFastened ? "FASTENED" : "UNFASTENED");
                Serial.print("Door: ");
                Serial.println(doorOpen ? "OPEN" : "CLOSED");
                Serial.print("Handbrake: ");
                Serial.println(handbrakeEngaged ? "ENGAGED" : "RELEASED");

                Serial.println("\n--- Active Warnings ---");
                if (logic.getCurrentBooleanValue("engineOverheat")) {
                        Serial.println("⚠️  ENGINE OVERHEATING");
                }
                if (logic.getCurrentBooleanValue("lowOilPressure")) {
                        Serial.println("⚠️  LOW OIL PRESSURE");
                }
                if (logic.getCurrentBooleanValue("fuelLow")) {
                        Serial.println("⚠️  LOW FUEL");
                }
                if (logic.getCurrentBooleanValue("safetyViolation")) {
                        Serial.println("🚨 SAFETY VIOLATION");
                }

                Serial.println("=======================\n");
        }
};

VehicleSystem vehicle;

void setup() {
        vehicle.setup();
}

void loop() {
        vehicle.loop();
}
#define ENABLE_MODULE_LOGIC_CONDITION_MANAGER
#include "Kinematrix.h"

LogicConditionManager logic;

class IndustrialProcessControl {
       private:
        float pressure = 0.0;
        float temperature = 0.0;
        float flowRate = 0.0;
        float level = 0.0;
        bool motorRunning = false;
        bool valveOpen = false;
        bool pumpActive = false;
        bool emergencyStop = false;
        int processStage = 0;

       public:
        void setup() {
                Serial.begin(9600);

                pinMode(2, INPUT_PULLUP);  // Emergency stop
                pinMode(3, OUTPUT);        // Motor control
                pinMode(4, OUTPUT);        // Valve control
                pinMode(5, OUTPUT);        // Pump control
                pinMode(6, OUTPUT);        // Alarm
                pinMode(7, OUTPUT);        // Status LED

                initializeProcessControl();
                Serial.println("Industrial Process Control System Initialized");
        }

        void initializeProcessControl() {
                int pressureHighId = logic.addValueCondition(
                    "pressureHigh",
                    [this]() { return pressure; },
                    [](float p) { return p > 8.5; },
                    LogicConditionManager::WHEN_TRUE,
                    [this]() {
                            Serial.println("ALERT: High pressure detected - Emergency valve release");
                            valveOpen = true;
                            digitalWrite(4, HIGH);
                            digitalWrite(6, HIGH);
                    });

                int pressureCriticalId = logic.addValueCondition(
                    "pressureCritical",
                    [this]() { return pressure; },
                    [](float p) { return p > 10.0; },
                    LogicConditionManager::WHEN_TRUE,
                    [this]() {
                            Serial.println("CRITICAL: Pressure limit exceeded - EMERGENCY SHUTDOWN");
                            emergencyShutdown();
                    });

                int temperatureOptimalId = logic.addValueCondition(
                    "tempOptimal",
                    [this]() { return temperature; },
                    [](float t) { return t >= 80.0 && t <= 120.0; },
                    LogicConditionManager::WHEN_TRUE,
                    []() { Serial.println("Temperature in optimal range"); });

                int temperatureHighId = logic.addValueCondition(
                    "tempHigh",
                    [this]() { return temperature; },
                    [](float t) { return t > 120.0; },
                    LogicConditionManager::TO_TRUE,
                    [this]() {
                            Serial.println("High temperature - Activating cooling");
                            pumpActive = true;
                            digitalWrite(5, HIGH);
                    });

                int flowRateNormalId = logic.addValueCondition(
                    "flowNormal",
                    [this]() { return flowRate; },
                    [](float f) { return f >= 15.0 && f <= 25.0; },
                    LogicConditionManager::WHEN_TRUE);

                int levelOptimalId = logic.addValueCondition(
                    "levelOptimal",
                    [this]() { return level; },
                    [](float l) { return l >= 40.0 && l <= 80.0; },
                    LogicConditionManager::WHEN_TRUE);

                int emergencyId = logic.addCondition(
                    "emergencyStop",
                    &emergencyStop,
                    LogicConditionManager::TO_TRUE,
                    [this]() { emergencyShutdown(); });

                logic.addAndCondition(
                    "processStable",
                    new int[3]{temperatureOptimalId, flowRateNormalId, levelOptimalId},
                    3,
                    [this]() {
                            if (processStage < 3) {
                                    processStage++;
                                    Serial.print("Process advancing to stage: ");
                                    Serial.println(processStage);
                            }
                    });

                logic.addOrCondition(
                    "processUnsafe",
                    new int[3]{pressureHighId, temperatureHighId, emergencyId},
                    3,
                    [this]() {
                            Serial.println("Process conditions unsafe - Implementing safety measures");
                            digitalWrite(6, HIGH);
                            delay(1000);
                            digitalWrite(6, LOW);
                    });

                logic.addSequenceCondition(
                    "startupSequence",
                    new int[4]{
                        logic.addCondition("initPressure", [this]() { return pressure > 2.0; }, LogicConditionManager::WHEN_TRUE), logic.addCondition("initTemp", [this]() { return temperature > 50.0; }, LogicConditionManager::WHEN_TRUE), logic.addCondition("initFlow", [this]() { return flowRate > 5.0; }, LogicConditionManager::WHEN_TRUE), logic.addCondition("initLevel", [this]() { return level > 20.0; }, LogicConditionManager::WHEN_TRUE)},
                    4,
                    30000,  // 30 seconds startup window
                    4, [this]() { 
                Serial.println("Startup sequence completed - Process ready");
                motorRunning = true;
                digitalWrite(3, HIGH);
                digitalWrite(7, HIGH); });

                logic.addEdgeCountCondition(
                    "pressureFluctuation",
                    pressureHighId,
                    5,      // 5 pressure spikes
                    10000,  // Within 10 seconds
                    []() { Serial.println("WARNING: Pressure instability detected"); });

                logic.addTimerCondition(
                    "processMonitor",
                    5000,  // Every 5 seconds
                    true,
                    [this]() { logProcessData(); });

                logic.addTimerCondition(
                    "maintenanceCheck",
                    60000,  // Every minute
                    true,
                    []() { Serial.println("Maintenance check: All systems operational"); });

                logic.addNotCondition(
                    "safeConditions",
                    logic.findConditionIdByName("processUnsafe"),
                    [this]() {
                            static unsigned long lastSafe = 0;
                            if (millis() - lastSafe > 15000) {
                                    Serial.println("All safety conditions met - Normal operation");
                                    digitalWrite(7, HIGH);
                                    lastSafe = millis();
                            }
                    });
        }

        void loop() {
                updateSensors();
                logic.update();

                emergencyStop = digitalRead(2) == LOW;

                if (logic.isConditionMet("processStable") && processStage >= 3) {
                        digitalWrite(7, HIGH);
                }

                if (pressure < 6.0 && valveOpen) {
                        Serial.println("Pressure normalized - Closing emergency valve");
                        valveOpen = false;
                        digitalWrite(4, LOW);
                        digitalWrite(6, LOW);
                }

                if (temperature < 100.0 && pumpActive) {
                        Serial.println("Temperature normalized - Stopping cooling pump");
                        pumpActive = false;
                        digitalWrite(5, LOW);
                }

                processControl();
        }

       private:
        void updateSensors() {
                pressure = 5.0 + (analogRead(A0) * 0.01);
                temperature = 70.0 + (analogRead(A1) * 0.1);
                flowRate = 10.0 + (analogRead(A2) * 0.05);
                level = 30.0 + (analogRead(A3) * 0.1);
        }

        void emergencyShutdown() {
                Serial.println("=== EMERGENCY SHUTDOWN ACTIVATED ===");
                motorRunning = false;
                valveOpen = true;
                pumpActive = false;
                processStage = 0;

                digitalWrite(3, LOW);   // Stop motor
                digitalWrite(4, HIGH);  // Open valve
                digitalWrite(5, LOW);   // Stop pump
                digitalWrite(7, LOW);   // Status off

                for (int i = 0; i < 10; i++) {
                        digitalWrite(6, HIGH);
                        delay(100);
                        digitalWrite(6, LOW);
                        delay(100);
                }

                logic.setEnabled("processStable", false);
                logic.setEnabled("startupSequence", false);
        }

        void processControl() {
                if (logic.isConditionMet("processStable")) {
                        if (!motorRunning && !emergencyStop) {
                                Serial.println("Conditions stable - Starting motor");
                                motorRunning = true;
                                digitalWrite(3, HIGH);
                        }
                }
        }

        void logProcessData() {
                Serial.println("\n=== PROCESS DATA LOG ===");
                Serial.print("Stage: ");
                Serial.println(processStage);
                Serial.print("Pressure: ");
                Serial.print(pressure);
                Serial.println(" bar");
                Serial.print("Temperature: ");
                Serial.print(temperature);
                Serial.println(" °C");
                Serial.print("Flow Rate: ");
                Serial.print(flowRate);
                Serial.println(" L/min");
                Serial.print("Level: ");
                Serial.print(level);
                Serial.println(" %");
                Serial.print("Motor: ");
                Serial.println(motorRunning ? "RUNNING" : "STOPPED");
                Serial.print("Valve: ");
                Serial.println(valveOpen ? "OPEN" : "CLOSED");
                Serial.print("Pump: ");
                Serial.println(pumpActive ? "ACTIVE" : "INACTIVE");
                Serial.println("=====================\n");
        }
};

IndustrialProcessControl processControl;

void setup() {
        processControl.setup();
}

void loop() {
        processControl.loop();
}
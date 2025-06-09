#define ENABLE_MODULE_LOGIC_CONDITION_MANAGER
#include "Kinematrix.h"
LogicConditionManager logic;

class BatteryManagementSystem {
       private:
        // Battery parameters
        float batteryVoltage = 12.6;
        float batteryCurrent = 0.0;
        float batteryTemperature = 25.0;
        float batteryCapacity = 100.0;  // Ah
        float stateOfCharge = 85.0;     // %
        float stateOfHealth = 95.0;     // %

        // Charging parameters
        bool chargingActive = false;
        bool dischargingActive = false;
        bool balancingActive = false;
        float chargingCurrent = 0.0;
        float maxChargingCurrent = 10.0;

        // Cell monitoring (simplified to 4 cells)
        float cellVoltages[4] = {3.15, 3.14, 3.16, 3.13};
        float cellTemperatures[4] = {24.5, 25.2, 24.8, 25.0};

        // Protection flags
        bool overVoltageProtection = false;
        bool underVoltageProtection = false;
        bool overCurrentProtection = false;
        bool overTemperatureProtection = false;
        bool underTemperatureProtection = false;

        // System state
        int chargingStage = 0;  // 0=off, 1=CC, 2=CV, 3=float
        bool safetyShutdown = false;
        int cycleCount = 156;

       public:
        void setup() {
                Serial.begin(9600);

                pinMode(2, OUTPUT);        // Charging control
                pinMode(3, OUTPUT);        // Load disconnect
                pinMode(4, OUTPUT);        // Balancing control
                pinMode(5, OUTPUT);        // Cooling fan
                pinMode(6, OUTPUT);        // Warning LED
                pinMode(7, OUTPUT);        // Error LED
                pinMode(8, OUTPUT);        // Status LED
                pinMode(9, INPUT_PULLUP);  // Manual reset
                pinMode(10, OUTPUT);       // Buzzer

                initializeBatterySystem();
                Serial.println("Advanced Battery Management System Initialized");
        }

        void initializeBatterySystem() {
                logic.addValueCondition(
                    "overVoltage",
                    [this]() { return batteryVoltage; },
                    [](float voltage) { return voltage > 14.4; },
                    LogicConditionManager::WHEN_TRUE,
                    [this]() {
                            Serial.println("⚠️  OVERVOLTAGE PROTECTION ACTIVATED");
                            overVoltageProtection = true;
                            stopCharging();
                            digitalWrite(6, HIGH);
                    });

                logic.addValueCondition(
                    "underVoltage",
                    [this]() { return batteryVoltage; },
                    [](float voltage) { return voltage < 10.5; },
                    LogicConditionManager::WHEN_TRUE,
                    [this]() {
                            Serial.println("⚠️  UNDERVOLTAGE PROTECTION ACTIVATED");
                            underVoltageProtection = true;
                            disconnectLoad();
                            digitalWrite(6, HIGH);
                    });

                logic.addValueCondition(
                    "overCurrent",
                    [this]() { return abs(batteryCurrent); },
                    [](float current) { return current > 50.0; },
                    LogicConditionManager::WHEN_TRUE,
                    [this]() {
                            Serial.println("⚠️  OVERCURRENT PROTECTION ACTIVATED");
                            overCurrentProtection = true;
                            emergencyShutdown();
                            digitalWrite(7, HIGH);
                    });

                logic.addValueCondition(
                    "overTemperature",
                    [this]() { return batteryTemperature; },
                    [](float temp) { return temp > 60.0; },
                    LogicConditionManager::WHEN_TRUE,
                    [this]() {
                            Serial.println("🔥 OVERTEMPERATURE PROTECTION ACTIVATED");
                            overTemperatureProtection = true;
                            activateCooling();
                            stopCharging();
                            digitalWrite(7, HIGH);
                    });

                logic.addValueCondition(
                    "underTemperature",
                    [this]() { return batteryTemperature; },
                    [](float temp) { return temp < -10.0; },
                    LogicConditionManager::WHEN_TRUE,
                    [this]() {
                            Serial.println("🧊 UNDERTEMPERATURE PROTECTION ACTIVATED");
                            underTemperatureProtection = true;
                            stopCharging();
                            digitalWrite(6, HIGH);
                    });

                logic.addValueCondition(
                    "lowStateOfCharge",
                    [this]() { return stateOfCharge; },
                    [](float soc) { return soc < 20.0; },
                    LogicConditionManager::TO_TRUE,
                    [this]() {
                            Serial.println("🔋 LOW BATTERY - Charging recommended");
                            startCharging();
                    });

                logic.addValueCondition(
                    "fullCharge",
                    [this]() { return stateOfCharge; },
                    [](float soc) { return soc > 95.0; },
                    LogicConditionManager::TO_TRUE,
                    [this]() {
                            Serial.println("✅ BATTERY FULLY CHARGED");
                            completeCharging();
                    });

                logic.addValueCondition(
                    "cellImbalance",
                    [this]() { return getCellImbalance(); },
                    [](float imbalance) { return imbalance > 0.1; },
                    LogicConditionManager::WHEN_TRUE,
                    [this]() {
                            Serial.println("⚖️  CELL IMBALANCE DETECTED - Starting balancing");
                            startCellBalancing();
                    });

                logic.addSequenceCondition(
                    "chargingSequence",
                    new int[3]{
                        logic.addCondition("preCharge", [this]() { return batteryVoltage > 10.0; }, LogicConditionManager::WHEN_TRUE), logic.addCondition("constantCurrent", [this]() { return chargingStage == 1; }, LogicConditionManager::WHEN_TRUE), logic.addCondition("constantVoltage", [this]() { return chargingStage == 2; }, LogicConditionManager::WHEN_TRUE)},
                    3,
                    18000000,  // 5 hours maximum charging time
                    3, [this]() {
                            Serial.println("✅ Charging sequence completed successfully");
                            chargingStage = 3;  // Float stage
                    });

                logic.addAndCondition(
                    "safeToCharge",
                    new int[4]{
                        logic.addNotCondition("noOverVoltage", logic.findConditionIdByName("overVoltage")),
                        logic.addNotCondition("noOverTemp", logic.findConditionIdByName("overTemperature")),
                        logic.addNotCondition("noUnderTemp", logic.findConditionIdByName("underTemperature")),
                        logic.addCondition("connectionOK", [this]() { return !safetyShutdown; }, LogicConditionManager::WHEN_TRUE)},
                    4, [this]() { 
                if (!chargingActive && stateOfCharge < 90.0) {
                    Serial.println("✅ Safe charging conditions met");
                } });

                logic.addOrCondition(
                    "criticalCondition",
                    new int[5]{
                        logic.findConditionIdByName("overVoltage"),
                        logic.findConditionIdByName("underVoltage"),
                        logic.findConditionIdByName("overCurrent"),
                        logic.findConditionIdByName("overTemperature"),
                        logic.findConditionIdByName("underTemperature")},
                    5,
                    [this]() {
                            Serial.println("🚨 CRITICAL BATTERY CONDITION - Safety measures activated");
                            tone(10, 2000, 1000);
                    });

                logic.addTimerCondition(
                    "batteryMonitoring",
                    5000,  // Every 5 seconds
                    true,
                    [this]() {
                            updateBatteryParameters();
                            monitorCellHealth();
                    });

                logic.addTimerCondition(
                    "chargingControl",
                    1000,  // Every second during charging
                    true,
                    [this]() {
                            if (chargingActive) {
                                    controlChargingProcess();
                            }
                    });

                logic.addEdgeCountCondition(
                    "rapidDischarge",
                    logic.addCondition("heavyLoad", [this]() { return batteryCurrent < -20.0; }, LogicConditionManager::TO_TRUE),
                    3,      // 3 heavy load events
                    10000,  // Within 10 seconds
                    [this]() { Serial.println("⚡ RAPID DISCHARGE DETECTED - Monitoring load"); });

                logic.addValueCondition(
                    "degradedHealth",
                    [this]() { return stateOfHealth; },
                    [](float soh) { return soh < 80.0; },
                    LogicConditionManager::TO_TRUE,
                    [this]() {
                            Serial.println("🔄 BATTERY HEALTH DEGRADED - Maintenance recommended");
                            tone(10, 500, 200);
                    });

                logic.addTimerCondition(
                    "maintenanceCheck",
                    3600000,  // Every hour
                    true,
                    [this]() {
                            performMaintenanceCheck();
                    });

                logic.addCondition(
                    "manualReset",
                    []() { return digitalRead(9) == LOW; },
                    LogicConditionManager::TO_TRUE,
                    [this]() {
                            Serial.println("🔧 Manual reset triggered");
                            resetProtectionFlags();
                    },
                    1, 0, 1000);

                logic.addTimerCondition(
                    "balancingControl",
                    30000,  // Check balancing every 30 seconds
                    true,
                    [this]() {
                            if (balancingActive) {
                                    updateCellBalancing();
                            }
                    });
        }

        void loop() {
                simulateBatteryConditions();
                logic.update();

                updateDisplays();
                handleProtectionLogic();

                static unsigned long lastReport = 0;
                if (millis() - lastReport > 10000) {
                        generateBatteryReport();
                        lastReport = millis();
                }
        }

       private:
        void simulateBatteryConditions() {
                static unsigned long lastSimulation = 0;
                if (millis() - lastSimulation > 2000) {
                        // Simulate battery behavior
                        if (chargingActive) {
                                if (stateOfCharge < 95.0) {
                                        stateOfCharge += 0.5;
                                        batteryVoltage += 0.01;
                                        batteryCurrent = chargingCurrent;
                                        batteryTemperature += 0.1;
                                }
                        } else if (dischargingActive) {
                                if (stateOfCharge > 5.0) {
                                        stateOfCharge -= 0.3;
                                        batteryVoltage -= 0.005;
                                        batteryCurrent = -15.0 + random(-10, 10);
                                }
                        } else {
                                // Idle state
                                batteryCurrent = 0.0;
                                if (batteryTemperature > 25.0) {
                                        batteryTemperature -= 0.05;
                                }
                        }

                        // Simulate cell variations
                        for (int i = 0; i < 4; i++) {
                                cellVoltages[i] = batteryVoltage / 4.0 + random(-20, 20) * 0.001;
                                cellTemperatures[i] = batteryTemperature + random(-10, 10) * 0.1;
                        }

                        // Random discharge simulation
                        dischargingActive = random(0, 100) < 30;

                        lastSimulation = millis();
                }
        }

        void updateBatteryParameters() {
                // Calculate state of health based on cycle count and other factors
                stateOfHealth = 100.0 - (cycleCount * 0.02) - (batteryTemperature > 40 ? 0.5 : 0);
                if (stateOfHealth < 0) stateOfHealth = 0;

                Serial.print("📊 Battery Update - SOC: ");
                Serial.print(stateOfCharge);
                Serial.print("% SOH: ");
                Serial.print(stateOfHealth);
                Serial.print("% V: ");
                Serial.print(batteryVoltage);
                Serial.print("V I: ");
                Serial.print(batteryCurrent);
                Serial.print("A T: ");
                Serial.print(batteryTemperature);
                Serial.println("°C");
        }

        void monitorCellHealth() {
                float maxVoltage = cellVoltages[0];
                float minVoltage = cellVoltages[0];

                for (int i = 1; i < 4; i++) {
                        if (cellVoltages[i] > maxVoltage) maxVoltage = cellVoltages[i];
                        if (cellVoltages[i] < minVoltage) minVoltage = cellVoltages[i];
                }

                float imbalance = maxVoltage - minVoltage;
                if (imbalance > 0.05) {
                        Serial.print("⚖️  Cell imbalance: ");
                        Serial.print(imbalance * 1000);
                        Serial.println(" mV");
                }
        }

        void startCharging() {
                if (logic.isConditionMet("safeToCharge")) {
                        chargingActive = true;
                        chargingStage = 1;  // Constant current
                        chargingCurrent = min(maxChargingCurrent, batteryCapacity * 0.3);
                        digitalWrite(2, HIGH);
                        Serial.println("🔌 Charging started - Constant Current phase");
                } else {
                        Serial.println("❌ Cannot start charging - Safety conditions not met");
                }
        }

        void stopCharging() {
                chargingActive = false;
                chargingStage = 0;
                chargingCurrent = 0.0;
                digitalWrite(2, LOW);
                Serial.println("🔌 Charging stopped");
        }

        void completeCharging() {
                chargingStage = 3;                         // Float
                chargingCurrent = batteryCapacity * 0.05;  // 5% of capacity
                Serial.println("✅ Charging complete - Float mode");
                cycleCount++;
        }

        void controlChargingProcess() {
                switch (chargingStage) {
                        case 1:  // Constant Current
                                if (batteryVoltage >= 14.2) {
                                        chargingStage = 2;
                                        Serial.println("🔄 Switching to Constant Voltage phase");
                                }
                                break;

                        case 2:  // Constant Voltage
                                chargingCurrent = max(0.5, chargingCurrent - 0.1);
                                if (chargingCurrent <= batteryCapacity * 0.05) {
                                        completeCharging();
                                }
                                break;

                        case 3:  // Float
                                if (stateOfCharge < 95.0) {
                                        chargingStage = 1;
                                        Serial.println("🔄 Resuming charging from float");
                                }
                                break;
                }
        }

        void startCellBalancing() {
                balancingActive = true;
                digitalWrite(4, HIGH);
                Serial.println("⚖️  Cell balancing activated");
        }

        void updateCellBalancing() {
                float imbalance = getCellImbalance();

                if (imbalance < 0.02) {
                        balancingActive = false;
                        digitalWrite(4, LOW);
                        Serial.println("✅ Cell balancing complete");
                } else {
                        Serial.print("⚖️  Balancing in progress - Imbalance: ");
                        Serial.print(imbalance * 1000);
                        Serial.println(" mV");
                }
        }

        void disconnectLoad() {
                digitalWrite(3, HIGH);
                Serial.println("⚡ Load disconnected for protection");
        }

        void emergencyShutdown() {
                safetyShutdown = true;
                stopCharging();
                disconnectLoad();
                digitalWrite(7, HIGH);
                Serial.println("🚨 EMERGENCY SHUTDOWN ACTIVATED");

                for (int i = 0; i < 5; i++) {
                        tone(10, 2500, 200);
                        delay(300);
                }
        }

        void activateCooling() {
                digitalWrite(5, HIGH);
                Serial.println("🌪️  Cooling fan activated");
        }

        void resetProtectionFlags() {
                overVoltageProtection = false;
                underVoltageProtection = false;
                overCurrentProtection = false;
                overTemperatureProtection = false;
                underTemperatureProtection = false;
                safetyShutdown = false;

                digitalWrite(6, LOW);
                digitalWrite(7, LOW);
                digitalWrite(3, LOW);  // Reconnect load

                Serial.println("🔄 Protection flags reset");
        }

        void performMaintenanceCheck() {
                Serial.println("\n🔧 MAINTENANCE CHECK");
                Serial.print("Cycle count: ");
                Serial.println(cycleCount);
                Serial.print("State of Health: ");
                Serial.print(stateOfHealth);
                Serial.println("%");

                if (cycleCount > 500) {
                        Serial.println("⚠️  High cycle count - Consider battery replacement");
                }

                if (stateOfHealth < 80) {
                        Serial.println("⚠️  Battery health degraded - Performance monitoring recommended");
                }
        }

        void handleProtectionLogic() {
                // Turn off cooling if temperature is normal
                if (batteryTemperature < 35.0) {
                        digitalWrite(5, LOW);
                }

                // Status LED management
                if (safetyShutdown) {
                        // Fast blink for emergency
                        static unsigned long lastBlink = 0;
                        if (millis() - lastBlink > 200) {
                                digitalWrite(8, !digitalRead(8));
                                lastBlink = millis();
                        }
                } else if (chargingActive) {
                        // Slow blink while charging
                        static unsigned long lastBlink = 0;
                        if (millis() - lastBlink > 1000) {
                                digitalWrite(8, !digitalRead(8));
                                lastBlink = millis();
                        }
                } else {
                        digitalWrite(8, HIGH);  // Solid on when idle
                }
        }

        void updateDisplays() {
                // Update various displays based on conditions
                static unsigned long lastUpdate = 0;
                if (millis() - lastUpdate > 3000) {
                        // Display current status
                        lastUpdate = millis();
                }
        }

        float getCellImbalance() {
                float maxV = cellVoltages[0];
                float minV = cellVoltages[0];

                for (int i = 1; i < 4; i++) {
                        if (cellVoltages[i] > maxV) maxV = cellVoltages[i];
                        if (cellVoltages[i] < minV) minV = cellVoltages[i];
                }

                return maxV - minV;
        }

        void generateBatteryReport() {
                Serial.println("\n=== BATTERY MANAGEMENT REPORT ===");
                Serial.print("Battery Voltage: ");
                Serial.print(batteryVoltage);
                Serial.println(" V");
                Serial.print("Battery Current: ");
                Serial.print(batteryCurrent);
                Serial.println(" A");
                Serial.print("Battery Temperature: ");
                Serial.print(batteryTemperature);
                Serial.println(" °C");
                Serial.print("State of Charge: ");
                Serial.print(stateOfCharge);
                Serial.println(" %");
                Serial.print("State of Health: ");
                Serial.print(stateOfHealth);
                Serial.println(" %");
                Serial.print("Cycle Count: ");
                Serial.println(cycleCount);

                Serial.print("Charging Stage: ");
                switch (chargingStage) {
                        case 0:
                                Serial.println("OFF");
                                break;
                        case 1:
                                Serial.println("Constant Current");
                                break;
                        case 2:
                                Serial.println("Constant Voltage");
                                break;
                        case 3:
                                Serial.println("Float");
                                break;
                }

                Serial.println("--- Cell Voltages ---");
                for (int i = 0; i < 4; i++) {
                        Serial.print("Cell ");
                        Serial.print(i + 1);
                        Serial.print(": ");
                        Serial.print(cellVoltages[i]);
                        Serial.println(" V");
                }

                Serial.print("Cell Imbalance: ");
                Serial.print(getCellImbalance() * 1000);
                Serial.println(" mV");

                Serial.println("--- Protection Status ---");
                Serial.print("Overvoltage: ");
                Serial.println(overVoltageProtection ? "ACTIVE" : "OK");
                Serial.print("Undervoltage: ");
                Serial.println(underVoltageProtection ? "ACTIVE" : "OK");
                Serial.print("Overcurrent: ");
                Serial.println(overCurrentProtection ? "ACTIVE" : "OK");
                Serial.print("Overtemperature: ");
                Serial.println(overTemperatureProtection ? "ACTIVE" : "OK");
                Serial.print("Safety Shutdown: ");
                Serial.println(safetyShutdown ? "ACTIVE" : "OK");

                Serial.println("==============================\n");
        }
};

BatteryManagementSystem batterySystem;

void setup() {
        batterySystem.setup();
}

void loop() {
        batterySystem.loop();
}
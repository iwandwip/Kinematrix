#define ENABLE_MODULE_LOGIC_CONDITION_MANAGER
#include "Kinematrix.h"
LogicConditionManager logic;

class HVACSystem {
       private:
        // Zone definitions
        struct Zone {
                String name;
                float currentTemp;
                float targetTemp;
                float humidity;
                bool occupied;
                bool damperOpen;
                int airflowRate;  // CFM
                bool heatingActive;
                bool coolingActive;
                bool ventilationActive;
        };

        Zone zones[4] = {
            {"Living Room", 22.0, 23.0, 45.0, true, true, 150, false, false, false},
            {"Bedroom", 20.0, 21.0, 50.0, false, false, 100, false, false, false},
            {"Kitchen", 25.0, 22.0, 60.0, true, true, 200, false, false, false},
            {"Office", 24.0, 23.0, 40.0, true, true, 120, false, false, false}};

        // System parameters
        float outdoorTemp = 15.0;
        float outdoorHumidity = 70.0;
        bool systemPower = true;
        bool maintenanceMode = false;
        bool energySaveMode = false;
        bool filterDirty = false;

        // Equipment status
        bool heatPumpActive = false;
        bool auxHeaterActive = false;
        bool airConditionerActive = false;
        bool ventilationFanActive = false;
        bool humidifierActive = false;
        bool dehumidifierActive = false;

        // Control parameters
        float deadband = 1.0;  // Temperature deadband in °C
        int scheduleMode = 1;  // 1=normal, 2=away, 3=sleep
        unsigned long systemRuntime = 0;
        int maintenanceHours = 2450;

       public:
        void setup() {
                Serial.begin(9600);

                // Zone control outputs
                pinMode(2, OUTPUT);  // Living room damper
                pinMode(3, OUTPUT);  // Bedroom damper
                pinMode(4, OUTPUT);  // Kitchen damper
                pinMode(5, OUTPUT);  // Office damper

                // Equipment control
                pinMode(6, OUTPUT);   // Heat pump
                pinMode(7, OUTPUT);   // Aux heater
                pinMode(8, OUTPUT);   // Air conditioner
                pinMode(9, OUTPUT);   // Ventilation fan
                pinMode(10, OUTPUT);  // Humidifier
                pinMode(11, OUTPUT);  // Dehumidifier

                // Status indicators
                pinMode(12, OUTPUT);  // System status LED
                pinMode(13, OUTPUT);  // Maintenance LED

                initializeHVACSystem();
                Serial.println("HVAC Multi-Zone Climate Control System Initialized");
        }

        void initializeHVACSystem() {
                // Zone temperature controls
                for (int i = 0; i < 4; i++) {
                        String heatingName = zones[i].name + "_heating";
                        String coolingName = zones[i].name + "_cooling";

                        logic.addValueCondition(
                            heatingName,
                            [this, i]() { return zones[i].currentTemp; },
                            [this, i](float temp) { return temp < (zones[i].targetTemp - deadband); },
                            LogicConditionManager::WHEN_TRUE,
                            [this, i]() {
                                    Serial.print("🔥 Heating required in ");
                                    Serial.println(zones[i].name);
                                    activateZoneHeating(i);
                            });

                        logic.addValueCondition(
                            coolingName,
                            [this, i]() { return zones[i].currentTemp; },
                            [this, i](float temp) { return temp > (zones[i].targetTemp + deadband); },
                            LogicConditionManager::WHEN_TRUE,
                            [this, i]() {
                                    Serial.print("❄️  Cooling required in ");
                                    Serial.println(zones[i].name);
                                    activateZoneCooling(i);
                            });
                }

                // System-wide equipment control
                logic.addCondition(
                    "anyZoneHeating",
                    [this]() {
                            for (int i = 0; i < 4; i++) {
                                    if (zones[i].heatingActive && zones[i].occupied) return true;
                            }
                            return false;
                    },
                    LogicConditionManager::WHEN_TRUE,
                    [this]() {
                            Serial.println("🔥 Activating heat pump for heating demand");
                            heatPumpActive = true;
                            digitalWrite(6, HIGH);
                    });

                logic.addCondition(
                    "anyZoneCooling",
                    [this]() {
                            for (int i = 0; i < 4; i++) {
                                    if (zones[i].coolingActive && zones[i].occupied) return true;
                            }
                            return false;
                    },
                    LogicConditionManager::WHEN_TRUE,
                    [this]() {
                            Serial.println("❄️  Activating air conditioner for cooling demand");
                            airConditionerActive = true;
                            digitalWrite(8, HIGH);
                    });

                logic.addValueCondition(
                    "extremeCold",
                    [this]() { return outdoorTemp; },
                    [](float temp) { return temp < -10.0; },
                    LogicConditionManager::WHEN_TRUE,
                    [this]() {
                            Serial.println("🔥 Extreme cold - Activating auxiliary heater");
                            auxHeaterActive = true;
                            digitalWrite(7, HIGH);
                    });

                logic.addValueCondition(
                    "highHumidity",
                    [this]() { return getAverageHumidity(); },
                    [](float humidity) { return humidity > 60.0; },
                    LogicConditionManager::WHEN_TRUE,
                    [this]() {
                            Serial.println("💧 High humidity detected - Activating dehumidifier");
                            dehumidifierActive = true;
                            digitalWrite(11, HIGH);
                    });

                logic.addValueCondition(
                    "lowHumidity",
                    [this]() { return getAverageHumidity(); },
                    [](float humidity) { return humidity < 30.0; },
                    LogicConditionManager::WHEN_TRUE,
                    [this]() {
                            Serial.println("💧 Low humidity detected - Activating humidifier");
                            humidifierActive = true;
                            digitalWrite(10, HIGH);
                    });

                logic.addTimerCondition(
                    "ventilationCycle",
                    1800000,  // 30 minutes
                    true,
                    [this]() {
                            Serial.println("🌪️  Fresh air ventilation cycle");
                            activateVentilation();
                    });

                logic.addSequenceCondition(
                    "systemStartup",
                    new int[3]{
                        logic.addCondition("powerOn", &systemPower, LogicConditionManager::WHEN_TRUE),
                        logic.addCondition("selfTest", [this]() { return !maintenanceMode; }, LogicConditionManager::WHEN_TRUE), logic.addCondition("zonesReady", [this]() { return checkZoneReadiness(); }, LogicConditionManager::WHEN_TRUE)},
                    3,
                    60000,  // 1 minute startup sequence
                    3, [this]() { 
                Serial.println("✅ HVAC system startup sequence completed");
                digitalWrite(12, HIGH); });

                logic.addAndCondition(
                    "energyOptimization",
                    new int[3]{
                        logic.addCondition("unoccupiedZones", [this]() { return getUnoccupiedZoneCount() > 2; }, LogicConditionManager::WHEN_TRUE), logic.addCondition("mildWeather", [this]() { return outdoorTemp > 15.0 && outdoorTemp < 25.0; }, LogicConditionManager::WHEN_TRUE), logic.addCondition("lowDemand", [this]() { return getTotalEnergyDemand() < 30.0; }, LogicConditionManager::WHEN_TRUE)},
                    3, [this]() { 
                Serial.println("💚 Energy optimization conditions met - Activating eco mode");
                energySaveMode = true;
                optimizeEnergyUsage(); });

                logic.addEdgeCountCondition(
                    "frequentCycling",
                    logic.addCondition("equipmentStart", &heatPumpActive, LogicConditionManager::TO_TRUE),
                    5,        // 5 starts
                    1800000,  // Within 30 minutes
                    [this]() {
                            Serial.println("⚠️  Frequent equipment cycling detected - Check system balance");
                    });

                logic.addTimerCondition(
                    "maintenanceCheck",
                    86400000,  // Daily
                    true,
                    [this]() {
                            checkMaintenanceNeeds();
                    });

                logic.addValueCondition(
                    "filterMaintenance",
                    [this]() { return (float)maintenanceHours; },
                    [](float hours) { return hours > 2400; },
                    LogicConditionManager::TO_TRUE,
                    [this]() {
                            Serial.println("🔧 Filter maintenance required");
                            filterDirty = true;
                            digitalWrite(13, HIGH);
                    });

                logic.addTimerCondition(
                    "scheduleControl",
                    3600000,  // Every hour
                    true,
                    [this]() {
                            updateScheduleSettings();
                    });

                logic.addNotCondition(
                    "noHeatingDemand",
                    logic.findConditionIdByName("anyZoneHeating"),
                    [this]() {
                            if (heatPumpActive && !auxHeaterActive) {
                                    Serial.println("🔥 No heating demand - Shutting down heat pump");
                                    heatPumpActive = false;
                                    digitalWrite(6, LOW);
                            }
                    });

                logic.addNotCondition(
                    "noCoolingDemand",
                    logic.findConditionIdByName("anyZoneCooling"),
                    [this]() {
                            if (airConditionerActive) {
                                    Serial.println("❄️  No cooling demand - Shutting down air conditioner");
                                    airConditionerActive = false;
                                    digitalWrite(8, LOW);
                            }
                    });

                logic.addTimerCondition(
                    "systemMonitoring",
                    30000,  // Every 30 seconds
                    true,
                    [this]() {
                            monitorSystemPerformance();
                    });
        }

        void loop() {
                simulateEnvironmentalConditions();
                logic.update();

                updateZoneDampers();
                balanceAirflow();

                static unsigned long lastReport = 0;
                if (millis() - lastReport > 60000) {
                        generateHVACReport();
                        lastReport = millis();
                }

                systemRuntime += 30;  // Increment runtime (simplified)
                maintenanceHours = systemRuntime / 3600;
        }

       private:
        void simulateEnvironmentalConditions() {
                static unsigned long lastSimulation = 0;
                if (millis() - lastSimulation > 5000) {
                        // Simulate temperature changes
                        for (int i = 0; i < 4; i++) {
                                // Temperature drifts based on heating/cooling and external factors
                                if (zones[i].heatingActive) {
                                        zones[i].currentTemp += 0.1;
                                } else if (zones[i].coolingActive) {
                                        zones[i].currentTemp -= 0.1;
                                } else {
                                        // Natural drift toward outdoor temperature
                                        float drift = (outdoorTemp - zones[i].currentTemp) * 0.01;
                                        zones[i].currentTemp += drift;
                                }

                                // Humidity changes
                                if (humidifierActive) {
                                        zones[i].humidity += 0.5;
                                } else if (dehumidifierActive) {
                                        zones[i].humidity -= 0.5;
                                } else {
                                        zones[i].humidity += random(-10, 10) * 0.1;
                                }

                                // Clamp values
                                zones[i].humidity = constrain(zones[i].humidity, 20.0, 80.0);

                                // Simulate occupancy changes
                                if (random(0, 100) < 5) {  // 5% chance to change occupancy
                                        zones[i].occupied = !zones[i].occupied;
                                }
                        }

                        // Outdoor conditions
                        outdoorTemp += random(-10, 10) * 0.1;
                        outdoorTemp = constrain(outdoorTemp, -20.0, 40.0);

                        outdoorHumidity += random(-5, 5) * 0.5;
                        outdoorHumidity = constrain(outdoorHumidity, 30.0, 90.0);

                        lastSimulation = millis();
                }
        }

        void activateZoneHeating(int zoneIndex) {
                zones[zoneIndex].heatingActive = true;
                zones[zoneIndex].coolingActive = false;
                zones[zoneIndex].damperOpen = true;
                digitalWrite(2 + zoneIndex, HIGH);  // Open damper
        }

        void activateZoneCooling(int zoneIndex) {
                zones[zoneIndex].coolingActive = true;
                zones[zoneIndex].heatingActive = false;
                zones[zoneIndex].damperOpen = true;
                digitalWrite(2 + zoneIndex, HIGH);  // Open damper
        }

        void activateVentilation() {
                ventilationFanActive = true;
                digitalWrite(9, HIGH);

                // Open dampers for occupied zones
                for (int i = 0; i < 4; i++) {
                        if (zones[i].occupied) {
                                zones[i].ventilationActive = true;
                                zones[i].damperOpen = true;
                                digitalWrite(2 + i, HIGH);
                        }
                }

                // Run for 10 minutes
                logic.addTimerCondition(
                    "ventilationOff",
                    600000,  // 10 minutes
                    false,
                    [this]() {
                            Serial.println("🌪️  Ventilation cycle complete");
                            ventilationFanActive = false;
                            digitalWrite(9, LOW);
                            for (int i = 0; i < 4; i++) {
                                    zones[i].ventilationActive = false;
                            }
                    });
        }

        void optimizeEnergyUsage() {
                Serial.println("💚 Implementing energy optimization strategies");

                // Close dampers for unoccupied zones
                for (int i = 0; i < 4; i++) {
                        if (!zones[i].occupied) {
                                zones[i].damperOpen = false;
                                digitalWrite(2 + i, LOW);
                                Serial.print("💚 Closing damper for unoccupied ");
                                Serial.println(zones[i].name);
                        }
                }

                // Adjust temperature setpoints
                for (int i = 0; i < 4; i++) {
                        if (zones[i].occupied) {
                                if (scheduleMode == 2) {  // Away mode
                                        zones[i].targetTemp += (zones[i].heatingActive ? -2.0 : 2.0);
                                }
                        }
                }
        }

        void updateZoneDampers() {
                for (int i = 0; i < 4; i++) {
                        bool shouldOpen = zones[i].occupied &&
                                          (zones[i].heatingActive || zones[i].coolingActive || zones[i].ventilationActive);

                        if (shouldOpen != zones[i].damperOpen) {
                                zones[i].damperOpen = shouldOpen;
                                digitalWrite(2 + i, shouldOpen ? HIGH : LOW);
                        }
                }
        }

        void balanceAirflow() {
                // Simple airflow balancing
                int totalDemand = 0;
                for (int i = 0; i < 4; i++) {
                        if (zones[i].damperOpen) {
                                totalDemand += zones[i].airflowRate;
                        }
                }

                // Adjust fan speed based on total demand (simplified)
                if (totalDemand > 400) {
                        Serial.println("🌪️  High airflow demand - Increasing fan speed");
                }
        }

        void checkMaintenanceNeeds() {
                Serial.println("🔧 Performing daily maintenance check");

                if (maintenanceHours > 2400) {
                        Serial.println("🔧 Filter replacement due");
                        filterDirty = true;
                }

                if (systemRuntime > 8760) {  // Annual service
                        Serial.println("🔧 Annual service recommended");
                }

                // Check equipment performance
                if (getTotalEnergyDemand() > 80.0) {
                        Serial.println("⚠️  High energy usage - System efficiency check recommended");
                }
        }

        void updateScheduleSettings() {
                // Simple schedule simulation based on time
                unsigned long currentHour = (millis() / 3600000) % 24;

                if (currentHour >= 6 && currentHour <= 8) {
                        scheduleMode = 1;  // Morning normal
                        for (int i = 0; i < 4; i++) {
                                zones[i].targetTemp = (i == 0) ? 23.0 : 21.0;  // Living room warmer
                        }
                } else if (currentHour >= 9 && currentHour <= 17) {
                        scheduleMode = 2;  // Away mode
                        for (int i = 0; i < 4; i++) {
                                zones[i].occupied = (i == 2);  // Only kitchen occupied (cooking)
                                zones[i].targetTemp -= 2.0;    // Lower setpoints
                        }
                } else if (currentHour >= 22 || currentHour <= 6) {
                        scheduleMode = 3;            // Sleep mode
                        zones[1].targetTemp = 19.0;  // Cooler bedroom
                        zones[0].targetTemp = 21.0;  // Cooler living room
                }
        }

        void monitorSystemPerformance() {
                float totalPower = 0;

                if (heatPumpActive) totalPower += 3.5;  // kW
                if (auxHeaterActive) totalPower += 8.0;
                if (airConditionerActive) totalPower += 4.2;
                if (ventilationFanActive) totalPower += 0.5;
                if (humidifierActive) totalPower += 0.3;
                if (dehumidifierActive) totalPower += 0.8;

                if (totalPower > 10.0) {
                        Serial.println("⚡ High power consumption detected");
                }

                // Check temperature satisfaction
                int satisfiedZones = 0;
                for (int i = 0; i < 4; i++) {
                        if (abs(zones[i].currentTemp - zones[i].targetTemp) <= deadband) {
                                satisfiedZones++;
                        }
                }

                if (satisfiedZones == 4) {
                        Serial.println("✅ All zones at target temperature");
                }
        }

        bool checkZoneReadiness() {
                // Check if all zone dampers are responding
                for (int i = 0; i < 4; i++) {
                        if (zones[i].occupied && !zones[i].damperOpen) {
                                return false;
                        }
                }
                return true;
        }

        float getAverageHumidity() {
                float total = 0;
                int count = 0;
                for (int i = 0; i < 4; i++) {
                        if (zones[i].occupied) {
                                total += zones[i].humidity;
                                count++;
                        }
                }
                return count > 0 ? total / count : 50.0;
        }

        int getUnoccupiedZoneCount() {
                int count = 0;
                for (int i = 0; i < 4; i++) {
                        if (!zones[i].occupied) count++;
                }
                return count;
        }

        float getTotalEnergyDemand() {
                float demand = 0;
                for (int i = 0; i < 4; i++) {
                        if (zones[i].heatingActive || zones[i].coolingActive) {
                                demand += abs(zones[i].currentTemp - zones[i].targetTemp) * 10.0;
                        }
                }
                return demand;
        }

        void generateHVACReport() {
                Serial.println("\n=== HVAC SYSTEM REPORT ===");
                Serial.print("System Runtime: ");
                Serial.print(systemRuntime / 3600);
                Serial.println(" hours");
                Serial.print("Outdoor Temp: ");
                Serial.print(outdoorTemp);
                Serial.println(" °C");
                Serial.print("Outdoor Humidity: ");
                Serial.print(outdoorHumidity);
                Serial.println(" %");
                Serial.print("Schedule Mode: ");
                switch (scheduleMode) {
                        case 1:
                                Serial.println("Normal");
                                break;
                        case 2:
                                Serial.println("Away");
                                break;
                        case 3:
                                Serial.println("Sleep");
                                break;
                }

                Serial.println("\n--- Zone Status ---");
                for (int i = 0; i < 4; i++) {
                        Serial.print(zones[i].name);
                        Serial.print(": ");
                        Serial.print(zones[i].currentTemp);
                        Serial.print("°C (target: ");
                        Serial.print(zones[i].targetTemp);
                        Serial.print("°C) ");
                        Serial.print(zones[i].humidity);
                        Serial.print("% ");
                        Serial.print(zones[i].occupied ? "OCCUPIED" : "VACANT");
                        Serial.print(" Damper: ");
                        Serial.print(zones[i].damperOpen ? "OPEN" : "CLOSED");

                        if (zones[i].heatingActive) Serial.print(" HEATING");
                        if (zones[i].coolingActive) Serial.print(" COOLING");
                        if (zones[i].ventilationActive) Serial.print(" VENTILATING");
                        Serial.println();
                }

                Serial.println("\n--- Equipment Status ---");
                Serial.print("Heat Pump: ");
                Serial.println(heatPumpActive ? "ON" : "OFF");
                Serial.print("Aux Heater: ");
                Serial.println(auxHeaterActive ? "ON" : "OFF");
                Serial.print("Air Conditioner: ");
                Serial.println(airConditionerActive ? "ON" : "OFF");
                Serial.print("Ventilation Fan: ");
                Serial.println(ventilationFanActive ? "ON" : "OFF");
                Serial.print("Humidifier: ");
                Serial.println(humidifierActive ? "ON" : "OFF");
                Serial.print("Dehumidifier: ");
                Serial.println(dehumidifierActive ? "ON" : "OFF");

                Serial.print("Energy Save Mode: ");
                Serial.println(energySaveMode ? "ACTIVE" : "INACTIVE");
                Serial.print("Filter Status: ");
                Serial.println(filterDirty ? "DIRTY" : "CLEAN");

                Serial.println("========================\n");
        }
};

HVACSystem hvacSystem;

void setup() {
        hvacSystem.setup();
}

void loop() {
        hvacSystem.loop();
}
#define ENABLE_MODULE_LOGIC_CONDITION_MANAGER
#include "Kinematrix.h"

LogicConditionManager logic;

class GreenhouseSystem {
       private:
        float soilMoisture = 45.0;
        float airTemperature = 24.0;
        float humidity = 60.0;
        float lightIntensity = 300.0;
        float co2Level = 400.0;
        float phLevel = 6.8;
        bool waterPumpActive = false;
        bool ventilationActive = false;
        bool heatingActive = false;
        bool coolingActive = false;
        bool growLightsOn = false;
        bool irrigationValve = false;
        int growthStage = 1;  // 1=seedling, 2=vegetative, 3=flowering, 4=harvest

       public:
        void setup() {
                Serial.begin(9600);

                pinMode(2, OUTPUT);         // Water pump
                pinMode(3, OUTPUT);         // Ventilation fan
                pinMode(4, OUTPUT);         // Heating element
                pinMode(5, OUTPUT);         // Cooling system
                pinMode(6, OUTPUT);         // Grow lights
                pinMode(7, OUTPUT);         // Irrigation valve
                pinMode(8, OUTPUT);         // CO2 generator
                pinMode(9, OUTPUT);         // pH adjustment
                pinMode(10, INPUT_PULLUP);  // Growth stage selector

                initializeGreenhouseSystem();
                Serial.println("Smart Greenhouse System Initialized");
        }

        void initializeGreenhouseSystem() {
                int soilDryId = logic.addValueCondition(
                    "soilDry",
                    [this]() { return soilMoisture; },
                    [this](float moisture) {
                            float threshold = (growthStage == 1) ? 70.0 : (growthStage == 2) ? 60.0
                                                                      : (growthStage == 3)   ? 55.0
                                                                                             : 50.0;
                            return moisture < threshold;
                    },
                    LogicConditionManager::TO_TRUE,
                    [this]() {
                            Serial.println("Soil moisture low - Activating irrigation");
                            irrigationValve = true;
                            waterPumpActive = true;
                            digitalWrite(2, HIGH);
                            digitalWrite(7, HIGH);
                    });

                int soilWetId = logic.addValueCondition(
                    "soilWet",
                    [this]() { return soilMoisture; },
                    [this](float moisture) {
                            float threshold = (growthStage == 1) ? 85.0 : (growthStage == 2) ? 80.0
                                                                      : (growthStage == 3)   ? 75.0
                                                                                             : 70.0;
                            return moisture > threshold;
                    },
                    LogicConditionManager::TO_TRUE,
                    [this]() {
                            Serial.println("Soil moisture optimal - Stopping irrigation");
                            irrigationValve = false;
                            waterPumpActive = false;
                            digitalWrite(2, LOW);
                            digitalWrite(7, LOW);
                    });

                int tempLowId = logic.addValueCondition(
                    "tempLow",
                    [this]() { return airTemperature; },
                    [this](float temp) {
                            float minTemp = (growthStage == 1) ? 22.0 : (growthStage == 2) ? 24.0
                                                                    : (growthStage == 3)   ? 26.0
                                                                                           : 20.0;
                            return temp < minTemp;
                    },
                    LogicConditionManager::WHEN_TRUE,
                    [this]() {
                            Serial.println("Temperature too low - Activating heating");
                            heatingActive = true;
                            digitalWrite(4, HIGH);
                    });

                int tempHighId = logic.addValueCondition(
                    "tempHigh",
                    [this]() { return airTemperature; },
                    [this](float temp) {
                            float maxTemp = (growthStage == 1) ? 26.0 : (growthStage == 2) ? 28.0
                                                                    : (growthStage == 3)   ? 30.0
                                                                                           : 25.0;
                            return temp > maxTemp;
                    },
                    LogicConditionManager::WHEN_TRUE,
                    [this]() {
                            Serial.println("Temperature too high - Activating cooling");
                            coolingActive = true;
                            ventilationActive = true;
                            digitalWrite(5, HIGH);
                            digitalWrite(3, HIGH);
                    });

                int humidityLowId = logic.addValueCondition(
                    "humidityLow",
                    [this]() { return humidity; },
                    [](float h) { return h < 50.0; },
                    LogicConditionManager::WHEN_TRUE,
                    []() { Serial.println("Humidity low - Increasing misting"); });

                int humidityHighId = logic.addValueCondition(
                    "humidityHigh",
                    [this]() { return humidity; },
                    [](float h) { return h > 80.0; },
                    LogicConditionManager::WHEN_TRUE,
                    [this]() {
                            Serial.println("Humidity too high - Increasing ventilation");
                            ventilationActive = true;
                            digitalWrite(3, HIGH);
                    });

                int lightLowId = logic.addValueCondition(
                    "lightLow",
                    [this]() { return lightIntensity; },
                    [this](float light) {
                            float minLight = (growthStage == 1) ? 200.0 : (growthStage == 2) ? 400.0
                                                                      : (growthStage == 3)   ? 600.0
                                                                                             : 100.0;
                            return light < minLight;
                    },
                    LogicConditionManager::WHEN_TRUE,
                    [this]() {
                            Serial.println("Light intensity low - Activating grow lights");
                            growLightsOn = true;
                            digitalWrite(6, HIGH);
                    });

                int co2LowId = logic.addValueCondition(
                    "co2Low",
                    [this]() { return co2Level; },
                    [](float co2) { return co2 < 800.0; },
                    LogicConditionManager::WHEN_TRUE,
                    [this]() {
                            Serial.println("CO2 level low - Activating CO2 generator");
                            digitalWrite(8, HIGH);
                    });

                int phImbalanceId = logic.addValueCondition(
                    "phImbalance",
                    [this]() { return phLevel; },
                    [](float ph) { return ph < 6.0 || ph > 7.5; },
                    LogicConditionManager::WHEN_TRUE,
                    [this]() {
                            Serial.print("pH imbalance detected: ");
                            Serial.println(phLevel);
                            digitalWrite(9, HIGH);
                            delay(5000);
                            digitalWrite(9, LOW);
                    });

                logic.addAndCondition(
                    "optimalGrowth",
                    new int[4]{
                        logic.addNotCondition("soilNotDry", soilDryId),
                        logic.addNotCondition("tempOK", tempLowId),
                        logic.addNotCondition("tempOK2", tempHighId),
                        logic.addNotCondition("lightOK", lightLowId)},
                    4,
                    [this]() {
                            static unsigned long lastOptimal = 0;
                            if (millis() - lastOptimal > 30000) {
                                    Serial.println("✅ Optimal growing conditions achieved!");
                                    lastOptimal = millis();
                            }
                    });

                logic.addOrCondition(
                    "stressConditions",
                    new int[4]{tempLowId, tempHighId, humidityHighId, phImbalanceId},
                    4,
                    []() { Serial.println("⚠️  Plant stress conditions detected!"); });

                logic.addSequenceCondition(
                    "wateringCycle",
                    new int[3]{
                        soilDryId,
                        logic.addCondition("pumpRunning", &waterPumpActive, LogicConditionManager::WHEN_TRUE),
                        soilWetId},
                    3,
                    600000,  // 10 minutes max watering cycle
                    3,
                    []() { Serial.println("Watering cycle completed successfully"); });

                logic.addTimerCondition(
                    "lightCycle",
                    43200000,  // 12 hours (normally would be based on growth stage)
                    true,
                    [this]() {
                            growLightsOn = !growLightsOn;
                            digitalWrite(6, growLightsOn);
                            Serial.print("Light cycle: ");
                            Serial.println(growLightsOn ? "DAY" : "NIGHT");
                    });

                logic.addTimerCondition(
                    "nutrientSchedule",
                    3600000,  // Every hour
                    true,
                    []() { Serial.println("Nutrient feeding schedule check"); });

                logic.addEdgeCountCondition(
                    "frequentWatering",
                    soilDryId,
                    3,        // 3 watering cycles
                    7200000,  // Within 2 hours
                    []() { Serial.println("WARNING: Frequent watering detected - Check drainage"); });

                logic.addTimerCondition(
                    "environmentalLog",
                    30000,  // Every 30 seconds
                    true,
                    [this]() { logEnvironmentalData(); });

                logic.addValueCondition(
                    "heatStress",
                    [this]() { return airTemperature; },
                    [](float temp) { return temp > 35.0; },
                    LogicConditionManager::WHEN_TRUE,
                    [this]() {
                            Serial.println("🚨 HEAT STRESS ALERT - Emergency cooling!");
                            coolingActive = true;
                            ventilationActive = true;
                            digitalWrite(3, HIGH);
                            digitalWrite(5, HIGH);

                            for (int i = 0; i < 3; i++) {
                                    tone(11, 1000, 200);
                                    delay(300);
                            }
                    });
        }

        void loop() {
                updateSensors();
                logic.update();

                updateGrowthStage();
                controlSystems();

                if (humidity < 60.0 && !humidityHighId) {
                        digitalWrite(3, LOW);
                        ventilationActive = false;
                }

                if (airTemperature > 22.0 && airTemperature < 32.0) {
                        digitalWrite(4, LOW);
                        digitalWrite(5, LOW);
                        heatingActive = false;
                        coolingActive = false;
                }

                if (co2Level > 1000.0) {
                        digitalWrite(8, LOW);
                }

                if (lightIntensity > 500.0 && growthStage != 3) {
                        digitalWrite(6, LOW);
                        growLightsOn = false;
                }
        }

       private:
        void updateSensors() {
                soilMoisture = 30.0 + (analogRead(A0) * 0.1);
                airTemperature = 20.0 + (analogRead(A1) * 0.03);
                humidity = 40.0 + (analogRead(A2) * 0.08);
                lightIntensity = analogRead(A3) * 2.0;
                co2Level = 350.0 + (analogRead(A4) * 1.5);
                phLevel = 5.5 + (analogRead(A5) * 0.005);
        }

        void updateGrowthStage() {
                static unsigned long lastStageChange = 0;

                // Auto-advance growth stages (simplified)
                if (millis() - lastStageChange > 120000) {  // 2 minutes per stage for demo
                        if (growthStage < 4) {
                                growthStage++;
                                Serial.print("Growth stage advanced to: ");
                                Serial.println(growthStage);

                                switch (growthStage) {
                                        case 2:
                                                Serial.println("Entering vegetative growth phase");
                                                break;
                                        case 3:
                                                Serial.println("Entering flowering phase");
                                                break;
                                        case 4:
                                                Serial.println("Ready for harvest!");
                                                break;
                                }
                                lastStageChange = millis();
                        }
                }
        }

        void controlSystems() {
                // Additional system controls based on conditions
                if (logic.isConditionMet("optimalGrowth")) {
                        // All systems running optimally
                }

                if (logic.isConditionMet("stressConditions")) {
                        // Implement stress mitigation
                }
        }

        void logEnvironmentalData() {
                Serial.println("\n=== GREENHOUSE ENVIRONMENTAL DATA ===");
                Serial.print("Growth Stage: ");
                switch (growthStage) {
                        case 1:
                                Serial.println("Seedling");
                                break;
                        case 2:
                                Serial.println("Vegetative");
                                break;
                        case 3:
                                Serial.println("Flowering");
                                break;
                        case 4:
                                Serial.println("Harvest Ready");
                                break;
                }

                Serial.print("Soil Moisture: ");
                Serial.print(soilMoisture);
                Serial.println(" %");
                Serial.print("Air Temperature: ");
                Serial.print(airTemperature);
                Serial.println(" °C");
                Serial.print("Humidity: ");
                Serial.print(humidity);
                Serial.println(" %");
                Serial.print("Light Intensity: ");
                Serial.print(lightIntensity);
                Serial.println(" lux");
                Serial.print("CO2 Level: ");
                Serial.print(co2Level);
                Serial.println(" ppm");
                Serial.print("pH Level: ");
                Serial.println(phLevel);

                Serial.println("\n--- System Status ---");
                Serial.print("Water Pump: ");
                Serial.println(waterPumpActive ? "ON" : "OFF");
                Serial.print("Ventilation: ");
                Serial.println(ventilationActive ? "ON" : "OFF");
                Serial.print("Heating: ");
                Serial.println(heatingActive ? "ON" : "OFF");
                Serial.print("Cooling: ");
                Serial.println(coolingActive ? "ON" : "OFF");
                Serial.print("Grow Lights: ");
                Serial.println(growLightsOn ? "ON" : "OFF");
                Serial.print("Irrigation: ");
                Serial.println(irrigationValve ? "OPEN" : "CLOSED");

                Serial.println("================================\n");
        }
};

GreenhouseSystem greenhouse;

void setup() {
        greenhouse.setup();
}

void loop() {
        greenhouse.loop();
}
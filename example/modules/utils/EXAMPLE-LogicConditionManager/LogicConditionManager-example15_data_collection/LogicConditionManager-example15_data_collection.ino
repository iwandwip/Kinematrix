#define ENABLE_MODULE_LOGIC_CONDITION_MANAGER
#include "Kinematrix.h"

LogicConditionManager logic;

class DataCollectionSystem {
       private:
        struct SensorReading {
                float value;
                unsigned long timestamp;
                bool isValid;
        };

        SensorReading temperatureHistory[50];
        SensorReading pressureHistory[50];
        SensorReading vibrationHistory[50];

        int tempIndex = 0;
        int pressureIndex = 0;
        int vibrationIndex = 0;

        float currentTemp = 25.0;
        float currentPressure = 1013.25;
        float currentVibration = 0.0;

        bool dataLoggingActive = true;
        bool patternDetectionActive = true;
        bool anomalyDetected = false;
        bool trendAnalysisComplete = false;

        float tempTrend = 0.0;
        float pressureTrend = 0.0;
        int anomalyCount = 0;

       public:
        void setup() {
                Serial.begin(9600);

                pinMode(2, OUTPUT);        // Data logging LED
                pinMode(3, OUTPUT);        // Pattern detection LED
                pinMode(4, OUTPUT);        // Anomaly alert
                pinMode(5, OUTPUT);        // Trend indicator
                pinMode(6, INPUT_PULLUP);  // Start/stop logging

                initializeDataSystem();
                initializeArrays();

                Serial.println("Data Collection & Pattern Recognition System Initialized");
                Serial.println("Monitoring: Temperature, Pressure, Vibration");
        }

        void initializeDataSystem() {
                logic.addTimerCondition(
                    "dataCollection",
                    2000,  // Collect data every 2 seconds
                    true,
                    [this]() {
                            if (dataLoggingActive) {
                                    collectSensorData();
                                    digitalWrite(2, HIGH);
                                    delay(100);
                                    digitalWrite(2, LOW);
                            }
                    });

                logic.addTimerCondition(
                    "patternAnalysis",
                    10000,  // Analyze patterns every 10 seconds
                    true,
                    [this]() {
                            if (patternDetectionActive) {
                                    analyzePatterns();
                                    digitalWrite(3, HIGH);
                                    delay(100);
                                    digitalWrite(3, LOW);
                            }
                    });

                logic.addValueCondition(
                    "temperatureAnomaly",
                    [this]() { return currentTemp; },
                    [this](float temp) {
                            return isTemperatureAnomalous(temp);
                    },
                    LogicConditionManager::TO_TRUE,
                    [this]() {
                            Serial.println("🚨 TEMPERATURE ANOMALY DETECTED!");
                            anomalyDetected = true;
                            anomalyCount++;
                            digitalWrite(4, HIGH);
                    });

                logic.addValueCondition(
                    "pressureAnomaly",
                    [this]() { return currentPressure; },
                    [this](float pressure) {
                            return isPressureAnomalous(pressure);
                    },
                    LogicConditionManager::TO_TRUE,
                    [this]() {
                            Serial.println("🚨 PRESSURE ANOMALY DETECTED!");
                            anomalyDetected = true;
                            anomalyCount++;
                            digitalWrite(4, HIGH);
                    });

                logic.addValueCondition(
                    "vibrationSpike",
                    [this]() { return currentVibration; },
                    [](float vib) { return vib > 5.0; },
                    LogicConditionManager::TO_TRUE,
                    [this]() {
                            Serial.println("⚡ VIBRATION SPIKE DETECTED!");
                            anomalyDetected = true;
                            digitalWrite(4, HIGH);
                    });

                logic.addEdgeCountCondition(
                    "frequentAnomalies",
                    logic.addCondition("anyAnomaly", &anomalyDetected, LogicConditionManager::TO_TRUE),
                    3,      // 3 anomalies
                    30000,  // Within 30 seconds
                    [this]() {
                            Serial.println("⚠️  PATTERN: Frequent anomalies detected - System instability!");
                            triggerSystemAlert();
                    });

                logic.addSequenceCondition(
                    "trendConfirmation",
                    new int[3]{
                        logic.addCondition("trend1", [this]() { return abs(tempTrend) > 0.5; }, LogicConditionManager::WHEN_TRUE), logic.addCondition("trend2", [this]() { return abs(tempTrend) > 1.0; }, LogicConditionManager::WHEN_TRUE), logic.addCondition("trend3", [this]() { return abs(tempTrend) > 1.5; }, LogicConditionManager::WHEN_TRUE)},
                    3,
                    60000,  // Within 1 minute
                    3, [this]() { 
                Serial.print("📈 TREND CONFIRMED: Temperature ");
                Serial.print(tempTrend > 0 ? "RISING" : "FALLING");
                Serial.print(" at rate: ");
                Serial.println(tempTrend);
                trendAnalysisComplete = true;
                digitalWrite(5, HIGH); });

                logic.addTimerCondition(
                    "dataExport",
                    30000,  // Export data every 30 seconds
                    true,
                    [this]() {
                            exportDataSummary();
                    });

                logic.addValueCondition(
                    "dataBufferFull",
                    [this]() { return (float)getBufferUtilization(); },
                    [](float util) { return util > 90.0; },
                    LogicConditionManager::TO_TRUE,
                    [this]() {
                            Serial.println("📊 Data buffer near full - Consider data archival");
                    });

                logic.addAndCondition(
                    "stableConditions",
                    new int[3]{
                        logic.addNotCondition("noTempAnomaly", logic.findConditionIdByName("temperatureAnomaly")),
                        logic.addNotCondition("noPressureAnomaly", logic.findConditionIdByName("pressureAnomaly")),
                        logic.addNotCondition("noVibration", logic.findConditionIdByName("vibrationSpike"))},
                    3,
                    [this]() {
                            static unsigned long lastStable = 0;
                            if (millis() - lastStable > 20000) {
                                    Serial.println("✅ System conditions stable - Normal operation");
                                    anomalyDetected = false;
                                    digitalWrite(4, LOW);
                                    lastStable = millis();
                            }
                    });

                logic.addTimerCondition(
                    "correlationAnalysis",
                    15000,  // Every 15 seconds
                    true,
                    [this]() {
                            analyzeCrossSensorCorrelation();
                    });

                logic.addCondition(
                    "manualTrigger",
                    []() { return digitalRead(6) == LOW; },
                    LogicConditionManager::TO_TRUE,
                    [this]() {
                            Serial.println("Manual data snapshot triggered");
                            takeDataSnapshot();
                    },
                    1, 0, 1000  // 1 second debounce
                );
        }

        void loop() {
                updateSensors();
                logic.update();

                static unsigned long lastDisplay = 0;
                if (millis() - lastDisplay > 5000) {
                        displayCurrentReadings();
                        lastDisplay = millis();
                }
        }

       private:
        void initializeArrays() {
                for (int i = 0; i < 50; i++) {
                        temperatureHistory[i] = {0.0, 0, false};
                        pressureHistory[i] = {0.0, 0, false};
                        vibrationHistory[i] = {0.0, 0, false};
                }
        }

        void updateSensors() {
                currentTemp = 20.0 + (analogRead(A0) * 0.02) + (sin(millis() * 0.001) * 2.0);
                currentPressure = 1000.0 + (analogRead(A1) * 0.05) + (cos(millis() * 0.0015) * 5.0);
                currentVibration = (analogRead(A2) * 0.01) + (random(-100, 100) * 0.01);
        }

        void collectSensorData() {
                temperatureHistory[tempIndex] = {currentTemp, millis(), true};
                tempIndex = (tempIndex + 1) % 50;

                pressureHistory[pressureIndex] = {currentPressure, millis(), true};
                pressureIndex = (pressureIndex + 1) % 50;

                vibrationHistory[vibrationIndex] = {currentVibration, millis(), true};
                vibrationIndex = (vibrationIndex + 1) % 50;

                Serial.print("Data collected - T:");
                Serial.print(currentTemp);
                Serial.print(" P:");
                Serial.print(currentPressure);
                Serial.print(" V:");
                Serial.println(currentVibration);
        }

        void analyzePatterns() {
                calculateTrends();
                detectPatterns();

                Serial.println("📊 Pattern analysis completed");
                Serial.print("Temp trend: ");
                Serial.println(tempTrend);
                Serial.print("Pressure trend: ");
                Serial.println(pressureTrend);
        }

        void calculateTrends() {
                float tempSum = 0, pressureSum = 0;
                int validCount = 0;

                for (int i = 0; i < 50; i++) {
                        if (temperatureHistory[i].isValid) {
                                tempSum += temperatureHistory[i].value;
                                pressureSum += pressureHistory[i].value;
                                validCount++;
                        }
                }

                if (validCount > 10) {
                        float tempAvg = tempSum / validCount;
                        float pressureAvg = pressureSum / validCount;

                        tempTrend = currentTemp - tempAvg;
                        pressureTrend = currentPressure - pressureAvg;
                }
        }

        void detectPatterns() {
                // Simple pattern detection - look for cycles
                int tempPeaks = 0;
                int pressurePeaks = 0;

                for (int i = 2; i < 48; i++) {
                        if (temperatureHistory[i].isValid &&
                            temperatureHistory[i - 1].isValid &&
                            temperatureHistory[i + 1].isValid) {
                                if (temperatureHistory[i].value > temperatureHistory[i - 1].value &&
                                    temperatureHistory[i].value > temperatureHistory[i + 1].value) {
                                        tempPeaks++;
                                }
                        }

                        if (pressureHistory[i].isValid &&
                            pressureHistory[i - 1].isValid &&
                            pressureHistory[i + 1].isValid) {
                                if (pressureHistory[i].value > pressureHistory[i - 1].value &&
                                    pressureHistory[i].value > pressureHistory[i + 1].value) {
                                        pressurePeaks++;
                                }
                        }
                }

                if (tempPeaks > 5) {
                        Serial.println("🔄 PATTERN: Cyclic temperature variations detected");
                }

                if (pressurePeaks > 5) {
                        Serial.println("🔄 PATTERN: Cyclic pressure variations detected");
                }
        }

        bool isTemperatureAnomalous(float temp) {
                float avg = 0;
                int count = 0;

                for (int i = 0; i < 50; i++) {
                        if (temperatureHistory[i].isValid) {
                                avg += temperatureHistory[i].value;
                                count++;
                        }
                }

                if (count < 5) return false;

                avg /= count;
                float threshold = 3.0;  // 3 degree threshold

                return abs(temp - avg) > threshold;
        }

        bool isPressureAnomalous(float pressure) {
                float avg = 0;
                int count = 0;

                for (int i = 0; i < 50; i++) {
                        if (pressureHistory[i].isValid) {
                                avg += pressureHistory[i].value;
                                count++;
                        }
                }

                if (count < 5) return false;

                avg /= count;
                float threshold = 20.0;  // 20 hPa threshold

                return abs(pressure - avg) > threshold;
        }

        void analyzeCrossSensorCorrelation() {
                // Simple correlation analysis
                float tempPressureCorr = 0;
                int validPairs = 0;

                for (int i = 0; i < 50; i++) {
                        if (temperatureHistory[i].isValid && pressureHistory[i].isValid) {
                                tempPressureCorr += temperatureHistory[i].value * pressureHistory[i].value;
                                validPairs++;
                        }
                }

                if (validPairs > 10) {
                        Serial.print("🔗 Cross-sensor correlation analysis - Valid pairs: ");
                        Serial.println(validPairs);

                        if (tempPressureCorr > 50000) {
                                Serial.println("Strong positive correlation between temperature and pressure");
                        }
                }
        }

        void triggerSystemAlert() {
                Serial.println("🚨 SYSTEM ALERT: Multiple anomalies indicate system instability!");

                for (int i = 0; i < 5; i++) {
                        digitalWrite(4, HIGH);
                        delay(200);
                        digitalWrite(4, LOW);
                        delay(200);
                }
        }

        void exportDataSummary() {
                Serial.println("\n=== DATA SUMMARY EXPORT ===");
                Serial.print("Collection timestamp: ");
                Serial.println(millis());
                Serial.print("Total anomalies: ");
                Serial.println(anomalyCount);
                Serial.print("Buffer utilization: ");
                Serial.print(getBufferUtilization());
                Serial.println("%");

                Serial.println("--- Recent Statistics ---");
                printStatistics();
                Serial.println("========================\n");
        }

        void printStatistics() {
                float tempMin = 999, tempMax = -999, tempAvg = 0;
                float pressureMin = 9999, pressureMax = -9999, pressureAvg = 0;
                int validCount = 0;

                for (int i = 0; i < 50; i++) {
                        if (temperatureHistory[i].isValid) {
                                float temp = temperatureHistory[i].value;
                                if (temp < tempMin) tempMin = temp;
                                if (temp > tempMax) tempMax = temp;
                                tempAvg += temp;

                                float pressure = pressureHistory[i].value;
                                if (pressure < pressureMin) pressureMin = pressure;
                                if (pressure > pressureMax) pressureMax = pressure;
                                pressureAvg += pressure;

                                validCount++;
                        }
                }

                if (validCount > 0) {
                        tempAvg /= validCount;
                        pressureAvg /= validCount;

                        Serial.print("Temperature - Min:");
                        Serial.print(tempMin);
                        Serial.print(" Max:");
                        Serial.print(tempMax);
                        Serial.print(" Avg:");
                        Serial.println(tempAvg);

                        Serial.print("Pressure - Min:");
                        Serial.print(pressureMin);
                        Serial.print(" Max:");
                        Serial.print(pressureMax);
                        Serial.print(" Avg:");
                        Serial.println(pressureAvg);
                }
        }

        float getBufferUtilization() {
                int validTemp = 0, validPressure = 0, validVibration = 0;

                for (int i = 0; i < 50; i++) {
                        if (temperatureHistory[i].isValid) validTemp++;
                        if (pressureHistory[i].isValid) validPressure++;
                        if (vibrationHistory[i].isValid) validVibration++;
                }

                return ((validTemp + validPressure + validVibration) / 150.0) * 100.0;
        }

        void takeDataSnapshot() {
                Serial.println("\n=== MANUAL DATA SNAPSHOT ===");
                Serial.print("Timestamp: ");
                Serial.println(millis());
                Serial.print("Temperature: ");
                Serial.println(currentTemp);
                Serial.print("Pressure: ");
                Serial.println(currentPressure);
                Serial.print("Vibration: ");
                Serial.println(currentVibration);
                Serial.print("Anomaly status: ");
                Serial.println(anomalyDetected ? "DETECTED" : "NORMAL");
                Serial.println("==========================\n");
        }

        void displayCurrentReadings() {
                Serial.println("\n--- CURRENT READINGS ---");
                Serial.print("T: ");
                Serial.print(currentTemp);
                Serial.print("°C ");
                Serial.print("P: ");
                Serial.print(currentPressure);
                Serial.print("hPa ");
                Serial.print("V: ");
                Serial.print(currentVibration);
                Serial.println("g");
                Serial.print("Status: ");
                Serial.println(anomalyDetected ? "ANOMALY" : "NORMAL");
                Serial.println("----------------------");
        }
};

DataCollectionSystem dataSystem;

void setup() {
        dataSystem.setup();
}

void loop() {
        dataSystem.loop();
}
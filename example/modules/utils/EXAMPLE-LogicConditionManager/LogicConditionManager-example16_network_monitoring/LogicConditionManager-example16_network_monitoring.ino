#define ENABLE_MODULE_LOGIC_CONDITION_MANAGER
#include "Kinematrix.h"
LogicConditionManager logic;

class NetworkMonitoringSystem {
       private:
        // Network status simulation
        bool primaryWiFiConnected = true;
        bool secondaryWiFiConnected = false;
        bool ethernetConnected = false;
        bool cellularConnected = false;
        bool internetReachable = true;

        // Connection quality metrics
        int wifiSignalStrength = -45;  // dBm
        int cellularSignalStrength = -85;
        float packetLossRate = 0.0;
        int latencyMs = 25;
        float dataUsageMB = 0.0;

        // System state
        int activeConnection = 1;  // 1=WiFi1, 2=WiFi2, 3=Ethernet, 4=Cellular
        bool failoverActive = false;
        bool dataThrottling = false;
        int reconnectAttempts = 0;
        unsigned long lastSuccessfulConnection = 0;

        // Monitoring flags
        bool connectionStable = true;
        bool bandwidthExceeded = false;
        bool networkCongestion = false;

       public:
        void setup() {
                Serial.begin(9600);

                pinMode(2, OUTPUT);        // Primary connection LED
                pinMode(3, OUTPUT);        // Secondary connection LED
                pinMode(4, OUTPUT);        // Failover indicator
                pinMode(5, OUTPUT);        // Network error LED
                pinMode(6, OUTPUT);        // Data usage warning
                pinMode(7, INPUT_PULLUP);  // Manual failover button
                pinMode(8, OUTPUT);        // Status buzzer

                initializeNetworkMonitoring();
                Serial.println("Network Connection Monitoring & Failover System Initialized");
        }

        void initializeNetworkMonitoring() {
                int primaryWiFiId = logic.addCondition(
                    "primaryWiFiConnected",
                    &primaryWiFiConnected,
                    LogicConditionManager::WHEN_TRUE,
                    [this]() {
                            Serial.println("✅ Primary WiFi connection established");
                            activeConnection = 1;
                            digitalWrite(2, HIGH);
                            lastSuccessfulConnection = millis();
                    });

                int primaryWiFiLostId = logic.addCondition(
                    "primaryWiFiLost",
                    &primaryWiFiConnected,
                    LogicConditionManager::TO_FALSE,
                    [this]() {
                            Serial.println("❌ Primary WiFi connection lost - Initiating failover");
                            digitalWrite(2, LOW);
                            initiateFailover();
                    });

                logic.addValueCondition(
                    "weakSignal",
                    [this]() { return (float)wifiSignalStrength; },
                    [](float signal) { return signal < -70.0; },
                    LogicConditionManager::TO_TRUE,
                    [this]() {
                            Serial.println("⚠️  WiFi signal strength weak - Consider switching");
                            digitalWrite(5, HIGH);
                    });

                logic.addValueCondition(
                    "highLatency",
                    [this]() { return (float)latencyMs; },
                    [](float latency) { return latency > 200.0; },
                    LogicConditionManager::WHEN_TRUE,
                    [this]() {
                            Serial.println("🐌 High network latency detected");
                            networkCongestion = true;
                    });

                logic.addValueCondition(
                    "packetLoss",
                    [this]() { return packetLossRate; },
                    [](float loss) { return loss > 5.0; },
                    LogicConditionManager::WHEN_TRUE,
                    [this]() {
                            Serial.println("📉 Packet loss detected - Network quality degraded");
                            networkCongestion = true;
                            digitalWrite(5, HIGH);
                    });

                logic.addValueCondition(
                    "dataUsageHigh",
                    [this]() { return dataUsageMB; },
                    [](float usage) { return usage > 500.0; },
                    LogicConditionManager::TO_TRUE,
                    [this]() {
                            Serial.println("📊 High data usage detected - Enabling throttling");
                            dataThrottling = true;
                            digitalWrite(6, HIGH);
                    });

                logic.addValueCondition(
                    "dataUsageCritical",
                    [this]() { return dataUsageMB; },
                    [](float usage) { return usage > 1000.0; },
                    LogicConditionManager::TO_TRUE,
                    [this]() {
                            Serial.println("🚨 CRITICAL: Data usage limit approached!");
                            bandwidthExceeded = true;
                            tone(8, 1000, 500);
                    });

                logic.addSequenceCondition(
                    "failoverSequence",
                    new int[4]{
                        primaryWiFiLostId,
                        logic.addCondition("trySecondary", &secondaryWiFiConnected, LogicConditionManager::TO_TRUE),
                        logic.addCondition("tryEthernet", &ethernetConnected, LogicConditionManager::TO_TRUE),
                        logic.addCondition("tryCellular", &cellularConnected, LogicConditionManager::TO_TRUE)},
                    4,
                    30000,  // 30 seconds failover window
                    1,      // At least one backup connection
                    [this]() {
                            Serial.println("✅ Failover sequence completed successfully");
                            failoverActive = true;
                            digitalWrite(4, HIGH);
                    });

                logic.addEdgeCountCondition(
                    "connectionFlapping",
                    primaryWiFiLostId,
                    3,      // 3 disconnections
                    60000,  // Within 1 minute
                    [this]() {
                            Serial.println("⚠️  Connection flapping detected - Switching to stable backup");
                            forceStableConnection();
                    });

                logic.addTimerCondition(
                    "reconnectAttempt",
                    10000,  // Try reconnect every 10 seconds
                    true,
                    [this]() {
                            if (!primaryWiFiConnected && reconnectAttempts < 5) {
                                    attemptReconnection();
                            }
                    });

                logic.addTimerCondition(
                    "networkHealthCheck",
                    30000,  // Health check every 30 seconds
                    true,
                    [this]() {
                            performNetworkHealthCheck();
                    });

                logic.addAndCondition(
                    "networkDegraded",
                    new int[3]{
                        logic.addCondition("weakSignalCheck", [this]() { return wifiSignalStrength < -70; }, LogicConditionManager::WHEN_TRUE), logic.addCondition("highLatencyCheck", [this]() { return latencyMs > 150; }, LogicConditionManager::WHEN_TRUE), logic.addCondition("packetLossCheck", [this]() { return packetLossRate > 3.0; }, LogicConditionManager::WHEN_TRUE)},
                    3, [this]() { 
                Serial.println("📉 Network performance severely degraded - Consider failover");
                connectionStable = false; });

                logic.addOrCondition(
                    "anyConnectionAvailable",
                    new int[4]{
                        primaryWiFiId,
                        logic.addCondition("secondaryWiFi", &secondaryWiFiConnected, LogicConditionManager::WHEN_TRUE),
                        logic.addCondition("ethernet", &ethernetConnected, LogicConditionManager::WHEN_TRUE),
                        logic.addCondition("cellular", &cellularConnected, LogicConditionManager::WHEN_TRUE)},
                    4,
                    [this]() {
                            static unsigned long lastConnectedMsg = 0;
                            if (millis() - lastConnectedMsg > 60000) {
                                    Serial.println("✅ Network connectivity maintained");
                                    lastConnectedMsg = millis();
                            }
                    });

                logic.addNotCondition(
                    "noConnections",
                    logic.findConditionIdByName("anyConnectionAvailable"),
                    [this]() {
                            Serial.println("🚨 NETWORK OUTAGE: No connections available!");
                            digitalWrite(5, HIGH);
                            tone(8, 2000, 1000);
                    });

                logic.addTimerCondition(
                    "dataUsageReset",
                    86400000,  // Reset daily (24 hours)
                    true,
                    [this]() {
                            Serial.println("📊 Daily data usage reset");
                            dataUsageMB = 0.0;
                            dataThrottling = false;
                            bandwidthExceeded = false;
                            digitalWrite(6, LOW);
                    });

                logic.addCondition(
                    "manualFailover",
                    []() { return digitalRead(7) == LOW; },
                    LogicConditionManager::TO_TRUE,
                    [this]() {
                            Serial.println("Manual failover triggered");
                            initiateManualFailover();
                    },
                    1, 0, 1000  // 1 second debounce
                );

                logic.addTimerCondition(
                    "connectionReport",
                    60000,  // Report every minute
                    true,
                    [this]() {
                            generateConnectionReport();
                    });
        }

        void loop() {
                simulateNetworkConditions();
                logic.update();

                updateStatusLEDs();
                updateDataUsage();

                if (latencyMs < 100 && packetLossRate < 2.0) {
                        digitalWrite(5, LOW);
                        networkCongestion = false;
                }

                if (wifiSignalStrength > -60) {
                        connectionStable = true;
                }
        }

       private:
        void simulateNetworkConditions() {
                static unsigned long lastSimulation = 0;
                if (millis() - lastSimulation > 3000) {
                        // Simulate changing network conditions
                        wifiSignalStrength = -45 + random(-30, 10);
                        cellularSignalStrength = -85 + random(-20, 15);
                        latencyMs = 25 + random(0, 200);
                        packetLossRate = random(0, 100) * 0.1;

                        // Simulate random disconnections
                        if (random(0, 100) < 5) {  // 5% chance
                                primaryWiFiConnected = false;
                        } else if (random(0, 100) < 10) {  // 10% chance to reconnect
                                primaryWiFiConnected = true;
                                reconnectAttempts = 0;
                        }

                        // Simulate backup connections
                        secondaryWiFiConnected = random(0, 100) < 70;  // 70% available
                        ethernetConnected = random(0, 100) < 80;       // 80% available
                        cellularConnected = random(0, 100) < 90;       // 90% available

                        lastSimulation = millis();
                }
        }

        void initiateFailover() {
                Serial.println("🔄 Initiating automatic failover...");
                failoverActive = true;
                digitalWrite(4, HIGH);

                if (secondaryWiFiConnected) {
                        activeConnection = 2;
                        Serial.println("✅ Switched to secondary WiFi");
                        digitalWrite(3, HIGH);
                } else if (ethernetConnected) {
                        activeConnection = 3;
                        Serial.println("✅ Switched to Ethernet");
                } else if (cellularConnected) {
                        activeConnection = 4;
                        Serial.println("✅ Switched to Cellular");
                } else {
                        Serial.println("❌ No backup connections available");
                }
        }

        void initiateManualFailover() {
                Serial.println("🔧 Manual failover initiated");

                activeConnection++;
                if (activeConnection > 4) activeConnection = 1;

                switch (activeConnection) {
                        case 1:
                                Serial.println("Attempting primary WiFi");
                                break;
                        case 2:
                                Serial.println("Attempting secondary WiFi");
                                break;
                        case 3:
                                Serial.println("Attempting Ethernet");
                                break;
                        case 4:
                                Serial.println("Attempting Cellular");
                                break;
                }

                digitalWrite(4, HIGH);
                delay(1000);
                digitalWrite(4, LOW);
        }

        void attemptReconnection() {
                reconnectAttempts++;
                Serial.print("🔄 Reconnection attempt #");
                Serial.println(reconnectAttempts);

                // Simulate reconnection attempt
                if (random(0, 100) < 30) {  // 30% success rate
                        primaryWiFiConnected = true;
                        reconnectAttempts = 0;
                        Serial.println("✅ Reconnection successful");
                        digitalWrite(2, HIGH);
                        if (failoverActive) {
                                Serial.println("🔄 Switching back to primary connection");
                                failoverActive = false;
                                digitalWrite(4, LOW);
                                activeConnection = 1;
                        }
                }

                if (reconnectAttempts >= 5) {
                        Serial.println("❌ Max reconnection attempts reached");
                        reconnectAttempts = 0;
                }
        }

        void forceStableConnection() {
                Serial.println("🔒 Forcing stable connection - Disabling primary WiFi temporarily");

                if (ethernetConnected) {
                        activeConnection = 3;
                        Serial.println("✅ Locked to Ethernet connection");
                } else if (secondaryWiFiConnected) {
                        activeConnection = 2;
                        Serial.println("✅ Locked to secondary WiFi");
                }

                // Disable primary WiFi monitoring temporarily
                logic.setEnabled("primaryWiFiConnected", false);

                logic.addTimerCondition(
                    "reEnablePrimary",
                    300000,  // 5 minutes
                    false,
                    [this]() {
                            logic.setEnabled("primaryWiFiConnected", true);
                            Serial.println("🔄 Re-enabled primary WiFi monitoring");
                    });
        }

        void performNetworkHealthCheck() {
                Serial.println("🔍 Performing network health check...");

                float healthScore = 100.0;

                if (wifiSignalStrength < -70) healthScore -= 20;
                if (latencyMs > 100) healthScore -= 15;
                if (packetLossRate > 2.0) healthScore -= 25;
                if (!internetReachable) healthScore -= 40;

                Serial.print("Network health score: ");
                Serial.println(healthScore);

                if (healthScore < 50) {
                        Serial.println("⚠️  Network health poor - Consider failover");
                } else if (healthScore > 80) {
                        Serial.println("✅ Network health excellent");
                }
        }

        void updateStatusLEDs() {
                // Primary connection LED
                if (primaryWiFiConnected && activeConnection == 1) {
                        digitalWrite(2, HIGH);
                } else {
                        static unsigned long lastBlink = 0;
                        if (millis() - lastBlink > 500) {
                                digitalWrite(2, !digitalRead(2));
                                lastBlink = millis();
                        }
                }

                // Secondary connection LED
                digitalWrite(3, (activeConnection == 2 || activeConnection == 3 || activeConnection == 4) ? HIGH : LOW);
        }

        void updateDataUsage() {
                static unsigned long lastUsageUpdate = 0;
                if (millis() - lastUsageUpdate > 5000) {
                        dataUsageMB += random(1, 10) * 0.1;  // Simulate data usage
                        lastUsageUpdate = millis();
                }
        }

        void generateConnectionReport() {
                Serial.println("\n=== NETWORK CONNECTION REPORT ===");
                Serial.print("Active Connection: ");
                switch (activeConnection) {
                        case 1:
                                Serial.println("Primary WiFi");
                                break;
                        case 2:
                                Serial.println("Secondary WiFi");
                                break;
                        case 3:
                                Serial.println("Ethernet");
                                break;
                        case 4:
                                Serial.println("Cellular");
                                break;
                }

                Serial.print("Primary WiFi: ");
                Serial.println(primaryWiFiConnected ? "CONNECTED" : "DISCONNECTED");
                Serial.print("Secondary WiFi: ");
                Serial.println(secondaryWiFiConnected ? "AVAILABLE" : "UNAVAILABLE");
                Serial.print("Ethernet: ");
                Serial.println(ethernetConnected ? "AVAILABLE" : "UNAVAILABLE");
                Serial.print("Cellular: ");
                Serial.println(cellularConnected ? "AVAILABLE" : "UNAVAILABLE");

                Serial.print("WiFi Signal: ");
                Serial.print(wifiSignalStrength);
                Serial.println(" dBm");
                Serial.print("Latency: ");
                Serial.print(latencyMs);
                Serial.println(" ms");
                Serial.print("Packet Loss: ");
                Serial.print(packetLossRate);
                Serial.println(" %");
                Serial.print("Data Usage: ");
                Serial.print(dataUsageMB);
                Serial.println(" MB");

                Serial.print("Failover Active: ");
                Serial.println(failoverActive ? "YES" : "NO");
                Serial.print("Connection Stable: ");
                Serial.println(connectionStable ? "YES" : "NO");
                Serial.print("Reconnect Attempts: ");
                Serial.println(reconnectAttempts);

                Serial.println("==============================\n");
        }
};

NetworkMonitoringSystem networkSystem;

void setup() {
        networkSystem.setup();
}

void loop() {
        networkSystem.loop();
}
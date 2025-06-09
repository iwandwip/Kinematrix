#define ENABLE_MODULE_LOGIC_CONDITION_MANAGER
#include "Kinematrix.h"
LogicConditionManager logic;

class SecurityAuthentication {
       private:
        String rfidCode = "";
        String enteredPin = "";
        String correctPin = "1234";
        String masterCode = "9999";
        bool rfidValid = false;
        bool pinValid = false;
        bool biometricValid = false;
        bool motionDetected = false;
        bool doorUnlocked = false;
        bool alarmActive = false;
        bool systemArmed = true;
        int authLevel = 0;  // 0=none, 1=RFID, 2=PIN, 3=biometric, 4=full access
        int failedAttempts = 0;
        unsigned long lastActivity = 0;

       public:
        void setup() {
                Serial.begin(9600);

                pinMode(2, INPUT);         // RFID reader trigger
                pinMode(3, INPUT);         // Motion sensor
                pinMode(4, OUTPUT);        // Door lock solenoid
                pinMode(5, OUTPUT);        // Access granted LED
                pinMode(6, OUTPUT);        // Access denied LED
                pinMode(7, OUTPUT);        // Alarm buzzer
                pinMode(8, OUTPUT);        // Status LED
                pinMode(9, INPUT_PULLUP);  // System arm/disarm
                pinMode(10, INPUT);        // Biometric sensor

                initializeSecuritySystem();
                Serial.println("Multi-Stage Security Authentication System Initialized");
                Serial.println("Authentication stages: RFID -> PIN -> Biometric -> Access");
        }

        void initializeSecuritySystem() {
                int rfidReadId = logic.addCondition(
                    "rfidDetected",
                    []() { return digitalRead(2) == HIGH; },
                    LogicConditionManager::TO_TRUE,
                    [this]() {
                            Serial.println("RFID card detected - Enter PIN");
                            rfidCode = "CARD_123";  // Simulated RFID read
                            validateRFID();
                    },
                    1, 0, 1000  // 1 second debounce
                );

                int pinEntryId = logic.addCondition(
                    "pinEntered",
                    &pinValid,
                    LogicConditionManager::TO_TRUE,
                    [this]() {
                            Serial.println("PIN verified - Biometric scan required");
                            authLevel = 2;
                    });

                int biometricId = logic.addCondition(
                    "biometricScanned",
                    []() { return digitalRead(10) == HIGH; },
                    LogicConditionManager::TO_TRUE,
                    [this]() {
                            Serial.println("Biometric scan initiated");
                            validateBiometric();
                    },
                    1, 0, 2000  // 2 second debounce for biometric
                );

                int motionId = logic.addCondition(
                    "motionDetected",
                    &motionDetected,
                    LogicConditionManager::TO_TRUE,
                    [this]() {
                            if (systemArmed && !doorUnlocked) {
                                    Serial.println("INTRUSION: Motion detected while system armed!");
                                    triggerAlarm();
                            }
                    });

                logic.addSequenceCondition(
                    "fullAuthentication",
                    new int[3]{rfidReadId, pinEntryId, biometricId},
                    3,
                    30000,  // 30 seconds to complete full sequence
                    3,
                    [this]() {
                            Serial.println("✅ FULL AUTHENTICATION SUCCESSFUL - ACCESS GRANTED");
                            grantAccess();
                            authLevel = 4;
                    });

                logic.addSequenceCondition(
                    "quickAccess",
                    new int[2]{rfidReadId, pinEntryId},
                    2,
                    10000,  // 10 seconds for quick access
                    2,
                    [this]() {
                            Serial.println("✅ QUICK ACCESS GRANTED - Limited permissions");
                            grantLimitedAccess();
                            authLevel = 2;
                    });

                logic.addTimerCondition(
                    "authTimeout",
                    15000,  // 15 seconds timeout
                    false,
                    [this]() {
                            if (authLevel > 0 && authLevel < 4) {
                                    Serial.println("❌ Authentication timeout - Resetting");
                                    resetAuthentication();
                            }
                    });

                logic.addEdgeCountCondition(
                    "multipleFailures",
                    logic.addCondition("authFailed", [this]() { return failedAttempts > 0; }, LogicConditionManager::WHEN_TRUE),
                    3,      // 3 failed attempts
                    60000,  // Within 1 minute
                    [this]() { 
                Serial.println("🚨 SECURITY BREACH: Multiple failed attempts detected!");
                triggerLockdown(); });

                logic.addAndCondition(
                    "unauthorizedEntry",
                    new int[2]{
                        motionId,
                        logic.addCondition("systemArmed", &systemArmed, LogicConditionManager::WHEN_TRUE)},
                    2,
                    [this]() {
                            Serial.println("🚨 UNAUTHORIZED ENTRY ATTEMPT!");
                            triggerAlarm();
                    });

                logic.addTimerCondition(
                    "accessExpiry",
                    300000,  // 5 minutes access
                    false,
                    [this]() {
                            if (doorUnlocked) {
                                    Serial.println("Access expired - Locking door");
                                    lockDoor();
                            }
                    });

                logic.addValueCondition(
                    "tooManyFailures",
                    [this]() { return (float)failedAttempts; },
                    [](float attempts) { return attempts >= 5; },
                    LogicConditionManager::WHEN_TRUE,
                    [this]() {
                            Serial.println("🚨 CRITICAL: Too many failed attempts - System lockdown!");
                            triggerLockdown();
                    });

                logic.addTimerCondition(
                    "activityMonitor",
                    5000,  // Every 5 seconds
                    true,
                    [this]() {
                            if (millis() - lastActivity > 60000 && authLevel > 0) {
                                    Serial.println("No activity detected - Resetting authentication");
                                    resetAuthentication();
                            }
                    });

                logic.addCondition(
                    "emergencyOverride",
                    [this]() { return enteredPin == masterCode; },
                    LogicConditionManager::TO_TRUE,
                    [this]() {
                            Serial.println("🔓 EMERGENCY OVERRIDE ACTIVATED");
                            grantEmergencyAccess();
                    });

                logic.addNotCondition(
                    "systemDisarmed",
                    logic.addCondition("systemArmedCheck", &systemArmed, LogicConditionManager::WHEN_TRUE),
                    [this]() {
                            Serial.println("Security system disarmed");
                            alarmActive = false;
                            digitalWrite(7, LOW);
                    });
        }

        void loop() {
                updateSensors();
                logic.update();

                systemArmed = digitalRead(9) == LOW;
                motionDetected = digitalRead(3) == HIGH;

                handleSerialInput();
                updateStatusLED();

                if (authLevel > 0) {
                        lastActivity = millis();
                }

                if (alarmActive) {
                        static unsigned long lastAlarmBeep = 0;
                        if (millis() - lastAlarmBeep > 500) {
                                digitalWrite(7, !digitalRead(7));
                                lastAlarmBeep = millis();
                        }
                }
        }

       private:
        void updateSensors() {
                // Simulated sensor updates
                static unsigned long lastUpdate = 0;
                if (millis() - lastUpdate > 1000) {
                        // Update sensor readings
                        lastUpdate = millis();
                }
        }

        void handleSerialInput() {
                if (Serial.available()) {
                        String input = Serial.readString();
                        input.trim();

                        if (input.startsWith("PIN:")) {
                                enteredPin = input.substring(4);
                                validatePIN();
                        } else if (input == "RESET") {
                                resetAuthentication();
                                Serial.println("System reset manually");
                        } else if (input == "STATUS") {
                                printSystemStatus();
                        }
                }
        }

        void validateRFID() {
                if (rfidCode == "CARD_123" || rfidCode == "CARD_456") {
                        rfidValid = true;
                        authLevel = 1;
                        Serial.println("✅ RFID validated - Level 1 access");
                        digitalWrite(5, HIGH);
                        logic.resetCondition("authTimeout");
                } else {
                        rfidValid = false;
                        failedAttempts++;
                        Serial.println("❌ Invalid RFID card");
                        digitalWrite(6, HIGH);
                        delay(1000);
                        digitalWrite(6, LOW);
                }
        }

        void validatePIN() {
                if (enteredPin == correctPin || enteredPin == masterCode) {
                        pinValid = true;
                        Serial.println("✅ PIN validated - Level 2 access");
                        digitalWrite(5, HIGH);

                        if (enteredPin == masterCode) {
                                logic.resetCondition("emergencyOverride");
                        }
                } else {
                        pinValid = false;
                        failedAttempts++;
                        Serial.println("❌ Invalid PIN");
                        digitalWrite(6, HIGH);
                        delay(1000);
                        digitalWrite(6, LOW);
                }
        }

        void validateBiometric() {
                // Simulated biometric validation
                static int biometricAttempts = 0;
                biometricAttempts++;

                if (biometricAttempts % 3 == 0) {  // Every 3rd attempt succeeds for demo
                        biometricValid = true;
                        authLevel = 3;
                        Serial.println("✅ Biometric validated - Level 3 access");
                        digitalWrite(5, HIGH);
                } else {
                        biometricValid = false;
                        failedAttempts++;
                        Serial.println("❌ Biometric scan failed");
                        digitalWrite(6, HIGH);
                        delay(1000);
                        digitalWrite(6, LOW);
                }
        }

        void grantAccess() {
                doorUnlocked = true;
                digitalWrite(4, HIGH);
                digitalWrite(5, HIGH);
                Serial.println("🔓 FULL ACCESS GRANTED - All systems unlocked");
                logic.resetCondition("accessExpiry");
        }

        void grantLimitedAccess() {
                doorUnlocked = true;
                digitalWrite(4, HIGH);
                digitalWrite(5, HIGH);
                Serial.println("🔓 LIMITED ACCESS GRANTED - Basic systems unlocked");
                logic.setInterval("accessExpiry", 180000);  // 3 minutes for limited access
                logic.resetCondition("accessExpiry");
        }

        void grantEmergencyAccess() {
                doorUnlocked = true;
                authLevel = 4;
                systemArmed = false;
                alarmActive = false;
                digitalWrite(4, HIGH);
                digitalWrite(5, HIGH);
                digitalWrite(7, LOW);
                Serial.println("🚨 EMERGENCY ACCESS - All security bypassed");
        }

        void lockDoor() {
                doorUnlocked = false;
                digitalWrite(4, LOW);
                digitalWrite(5, LOW);
                Serial.println("🔒 Door locked - Access revoked");
                resetAuthentication();
        }

        void triggerAlarm() {
                alarmActive = true;
                digitalWrite(7, HIGH);
                digitalWrite(6, HIGH);
                Serial.println("🚨 SECURITY ALARM ACTIVATED!");
        }

        void triggerLockdown() {
                alarmActive = true;
                systemArmed = true;
                doorUnlocked = false;
                authLevel = 0;
                digitalWrite(4, LOW);
                digitalWrite(5, LOW);
                digitalWrite(6, HIGH);
                digitalWrite(7, HIGH);

                Serial.println("🔐 SYSTEM LOCKDOWN - All access revoked");
                Serial.println("Contact administrator for reset");
        }

        void resetAuthentication() {
                rfidValid = false;
                pinValid = false;
                biometricValid = false;
                authLevel = 0;
                enteredPin = "";
                rfidCode = "";

                digitalWrite(5, LOW);
                digitalWrite(6, LOW);

                logic.resetCondition("fullAuthentication");
                logic.resetCondition("quickAccess");
                logic.resetCondition("authTimeout");

                Serial.println("Authentication reset - Ready for new attempt");
        }

        void updateStatusLED() {
                static unsigned long lastBlink = 0;

                if (authLevel == 0) {
                        // Slow blink when ready
                        if (millis() - lastBlink > 1000) {
                                digitalWrite(8, !digitalRead(8));
                                lastBlink = millis();
                        }
                } else if (authLevel < 4) {
                        // Fast blink during authentication
                        if (millis() - lastBlink > 250) {
                                digitalWrite(8, !digitalRead(8));
                                lastBlink = millis();
                        }
                } else {
                        // Solid on when fully authenticated
                        digitalWrite(8, HIGH);
                }
        }

        void printSystemStatus() {
                Serial.println("\n=== SECURITY SYSTEM STATUS ===");
                Serial.print("Authentication Level: ");
                Serial.println(authLevel);
                Serial.print("System Armed: ");
                Serial.println(systemArmed ? "YES" : "NO");
                Serial.print("Door Unlocked: ");
                Serial.println(doorUnlocked ? "YES" : "NO");
                Serial.print("Alarm Active: ");
                Serial.println(alarmActive ? "YES" : "NO");
                Serial.print("Failed Attempts: ");
                Serial.println(failedAttempts);
                Serial.print("RFID Valid: ");
                Serial.println(rfidValid ? "YES" : "NO");
                Serial.print("PIN Valid: ");
                Serial.println(pinValid ? "YES" : "NO");
                Serial.print("Biometric Valid: ");
                Serial.println(biometricValid ? "YES" : "NO");
                Serial.println("============================\n");
        }
};

SecurityAuthentication security;

void setup() {
        security.setup();
}

void loop() {
        security.loop();
}
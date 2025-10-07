#define ENABLE_MODULE_LOGIC_CONDITION_MANAGER
#include "Kinematrix.h"

LogicConditionManager logic;

float sensorValue = 50.0;
float temperature = 25.0;

// Simple sensor reading function
float readSensorValue() {
        return analogRead(A0) * 0.1;  // Convert to 0-100 range
}

// Simple temperature reading function
float readTemperature() {
        return 20.0 + (analogRead(A1) * 0.05);  // 20-70°C range
}

void setup() {
        Serial.begin(9600);
        pinMode(LED_BUILTIN, OUTPUT);

        Serial.println("=== Minimal Value Conditions Example ===");
        Serial.println("Connect potentiometers to A0 and A1");

        // Value condition with function
        logic.addValueCondition(
            "highSensor",
            readSensorValue,
            [](float value) { return value > 75.0; },
            LogicConditionManager::WHEN_TRUE,
            []() {
                    Serial.println("📈 Sensor value HIGH (>75)");
                    digitalWrite(LED_BUILTIN, HIGH);
            });

        // Value condition with pointer
        logic.addValueCondition(
            "lowSensor",
            &sensorValue,
            [](float value) { return value < 25.0; },
            LogicConditionManager::WHEN_TRUE,
            []() {
                    Serial.println("📉 Sensor value LOW (<25)");
                    digitalWrite(LED_BUILTIN, LOW);
            });

        // Temperature monitoring
        logic.addValueCondition(
            "tempHot",
            readTemperature,
            [](float temp) { return temp > 40.0; },
            LogicConditionManager::TO_TRUE,
            []() {
                    Serial.println("🔥 Temperature became HOT (>40°C)");
                    tone(8, 2000, 500);
            });

        // Temperature with pointer
        logic.addValueCondition(
            "tempCold",
            &temperature,
            [](float temp) { return temp < 22.0; },
            LogicConditionManager::TO_TRUE,
            []() {
                    Serial.println("🧊 Temperature became COLD (<22°C)");
                    tone(8, 500, 500);
            });

        // Value change detection
        logic.addValueCondition(
            "sensorChange",
            readSensorValue,
            [](float value) { return value > 30.0 && value < 70.0; },
            LogicConditionManager::ON_CHANGE,
            []() {
                    Serial.println("🔄 Sensor in middle range (30-70) - state changed");
            });

        Serial.println("Value conditions active:");
        Serial.println("- High sensor: >75 (A0)");
        Serial.println("- Low sensor: <25 (A0)");
        Serial.println("- Hot temp: >40°C (A1)");
        Serial.println("- Cold temp: <22°C");
        Serial.println("- Middle range change: 30-70");
}

void loop() {
        // Update pointer values
        sensorValue = readSensorValue();
        temperature = readTemperature();

        logic.update();

        // Display current values
        static unsigned long lastDisplay = 0;
        if (millis() - lastDisplay > 2000) {
                Serial.print("Sensor: ");
                Serial.print(sensorValue, 1);
                Serial.print("  Temp: ");
                Serial.print(temperature, 1);
                Serial.println("°C");
                lastDisplay = millis();
        }

        // Turn off LED if not in high range
        if (sensorValue <= 75.0 && sensorValue >= 25.0) {
                digitalWrite(LED_BUILTIN, LOW);
        }
}
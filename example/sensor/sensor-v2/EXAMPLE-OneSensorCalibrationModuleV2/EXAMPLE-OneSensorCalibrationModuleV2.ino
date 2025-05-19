#define ENABLE_INTERACTIVE_SERIAL_GENERAL_SENSOR_CALIBRATOR_V2
#define ENABLE_SENSOR_CALIBRATION_MODULE_V2
#define ENABLE_SENSOR_MODULE_V2
#define ENABLE_SENSOR_UTILITY_V2
#define ENABLE_SENSOR_ANALOG_V2

#include "Kinematrix.h"

SensorCalibrationModuleV2 sensorModule;
unsigned long lastDisplayTime = 0;
const unsigned long displayInterval = 1000;
bool calibrationModeActive = false;

void setup() {
    Serial.begin(115200);
    while (!Serial) {}

    Serial.println("\n===== Light Sensor Calibration Demo =====");

    sensorModule.addSensor("light", []() -> BaseSensV2 * {
        auto lightSensor = new AnalogSensV2(36, 3.3, 4095, [](BaseSensV2 *sensor, int analogValue, float voltageValue) {
            float lightPercentage = map(analogValue, 0, 4095, 0, 100);
            sensor->updateValue("level", lightPercentage);

            float lux = analogValue * 2.5;
            sensor->updateValue("lux", lux);

            int uvIndex = map(analogValue, 0, 4095, 0, 12);
            sensor->updateValue("uv", uvIndex);

            const char *category;
            if (lightPercentage < 20) category = "Dark";
            else if (lightPercentage < 50) category = "Dim";
            else if (lightPercentage < 80) category = "Bright";
            else category = "Very Bright";
            sensor->updateValue("category", category);
        });

        lightSensor->addCustomValue("level", "Light Level", "%", 1, TYPE_FLOAT);
        lightSensor->addCustomValue("lux", "Illuminance", "lx", 0, TYPE_FLOAT);
        lightSensor->addCustomValue("uv", "UV Index", "", 0, TYPE_INT);
        lightSensor->addCustomValue("category", "Light Category", "", 0, TYPE_STRING);
        lightSensor->setUpdateInterval(100);

        return lightSensor;
    });

    sensorModule.init();
    sensorModule.discoverSensorValues();
    sensorModule.setEEPROMStartAddress(100);
    sensorModule.setAutoSaveCalibration(true);
    sensorModule.setCalibrationTimeout(300000);
    sensorModule.setSerialOutput(&Serial);

    if (sensorModule.loadAllCalibrations()) {
        Serial.println("Calibration data loaded successfully");
    } else {
        Serial.println("No calibration data found or error loading");
    }

    Serial.println("Press 'c' to enter calibration mode");
    Serial.println("Press 'd' to display detailed sensor information");
    Serial.println("Press 's' to save calibrations");
    Serial.println("Press 'l' to load calibrations");
    Serial.println("Press 'i' to show detailed calibration status");
    Serial.println("Press 'v' to list all available sensor values");
    Serial.println("Press '1' for manual one-point calibration");
    Serial.println("Monitoring started...");
}

void loop() {
    sensorModule.update();

    if (!calibrationModeActive && millis() - lastDisplayTime >= displayInterval) {
        float levelRaw = sensorModule.getRawValue("light", "level");
        float levelCal = sensorModule.getCalibratedValue("light", "level");

        float luxRaw = sensorModule.getRawValue("light", "lux");
        float luxCal = sensorModule.getCalibratedValue("light", "lux");

        int uvRaw = sensorModule.getSensorByName("light")->getIntValue("uv");
        int uvCal = sensorModule.getCalibratedValue("light", "uv");

        const char *category = sensorModule.getSensorByName("light")->getStringValue("category");

        Serial.println("\n----- Current Readings -----");
        Serial.print("Light Level: Raw = ");
        Serial.print(levelRaw, 1);
        Serial.print("%, Calibrated = ");
        Serial.print(levelCal, 1);
        Serial.println("%");

        Serial.print("Illuminance: Raw = ");
        Serial.print(luxRaw, 0);
        Serial.print(" lx, Calibrated = ");
        Serial.print(luxCal, 0);
        Serial.println(" lx");

        Serial.print("UV Index: Raw = ");
        Serial.print(uvRaw);
        Serial.print(", Calibrated = ");
        Serial.println(uvCal);

        Serial.print("Light Category: ");
        Serial.println(category);

        lastDisplayTime = millis();
    }

    if (Serial.available() > 0) {
        char cmd = Serial.read();

        switch (cmd) {
            case 'c':
                if (!sensorModule.isInCalibrationMode()) {
                    Serial.println("\nEntering calibration mode...");
                    sensorModule.startCalibrationMode(300000);
                    calibrationModeActive = true;
                }
                break;

            case 'x':
                if (sensorModule.isInCalibrationMode()) {
                    sensorModule.stopCalibrationMode();
                    calibrationModeActive = false;
                    Serial.println("\nExited calibration mode");
                }
                break;

            case 'd':
                Serial.println("\n----- Detailed Sensor Information -----");
                sensorModule.debugAll();
                break;

            case 's':
                if (sensorModule.saveAllCalibrations()) {
                    Serial.println("\nAll calibrations saved successfully");
                } else {
                    Serial.println("\nError saving calibrations");
                }
                break;

            case 'l':
                if (sensorModule.loadAllCalibrations()) {
                    Serial.println("\nCalibration data loaded successfully");
                } else {
                    Serial.println("\nNo calibration data found or error loading");
                }
                break;

            case 'i':
                sensorModule.printCalibrationStatus();
                break;

            case 'v':
                sensorModule.listAllSensorValues();
                break;

            case '1':
                if (!calibrationModeActive) {
                    Serial.println("\n----- Manual One-Point Calibration for Light Level -----");
                    float currentRaw = sensorModule.getRawValue("light", "level");
                    Serial.print("Current raw reading: ");
                    Serial.print(currentRaw, 1);
                    Serial.println("%");

                    Serial.print("Enter known light level value: ");
                    while (!Serial.available()) {
                        delay(10);
                    }

                    String input = Serial.readStringUntil('\n');
                    float knownValue = input.toFloat();

                    sensorModule.calibrateOnePoint("light", "level", knownValue);
                    Serial.println("One-point calibration completed!");
                }
                break;
        }

        while (Serial.available()) {
            Serial.read();
        }
    }

    delay(10);
}
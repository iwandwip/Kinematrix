#define ENABLE_INTERACTIVE_SERIAL_GENERAL_SENSOR_CALIBRATOR_V2
#define ENABLE_SENSOR_ALERT_SYSTEM_V2
#define ENABLE_SENSOR_CALIBRATION_MODULE_V2
#define ENABLE_SENSOR_FILTER_V2
#define ENABLE_SENSOR_MODULE_V2
#define ENABLE_SENSOR_UTILITY_V2

#define ENABLE_SENSOR_ANALOG_V2
#define ENABLE_SENSOR_DHT_V2

#include "Kinematrix.h"

SensorCalibrationModuleV2 sensorModule;
unsigned long lastDisplayTime = 0;
const unsigned long displayInterval = 1000;
bool calibrationModeActive = false;

void setup() {
    Serial.begin(115200);
    while (!Serial) {}

    Serial.println("\n===== Sensor Calibration Module V2 Demo =====");

    sensorModule.addSensor("soil", []() -> BaseSensV2 * {
        auto soilSensor = new AnalogSensV2(36, 3.3, 4095, [](BaseSensV2 *sensor, int analogValue, float voltageValue) {
            float moisturePercentage = map(analogValue, 0, 4095, 0, 100);
            sensor->updateValue("moist", moisturePercentage);
        });

        soilSensor->addCustomValue("moist", "Moisture", "%", 1, TYPE_FLOAT);
        soilSensor->setUpdateInterval(100);

        return soilSensor;
    });

    sensorModule.addSensor("dht", []() -> BaseSensV2 * {
        auto dhtSensor = new DHTSensV2(21, DHT22);
        dhtSensor->setUpdateInterval(2000);
        return dhtSensor;
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

    FilterParams soilFilterParams;
    soilFilterParams.movingAverage.windowSize = 10;
    sensorModule.attachFilter("soil", "moist", FILTER_MOVING_AVERAGE, soilFilterParams);

    Serial.println("Press 'c' to enter calibration mode");
    Serial.println("Press 'd' to display detailed sensor information");
    Serial.println("Press 's' to save calibrations");
    Serial.println("Press 'l' to load calibrations");
    Serial.println("Press 'i' to show detailed calibration status");
    Serial.println("Press 'v' to list all available sensor values");
    Serial.println("Monitoring started...");
}

void loop() {
    sensorModule.update();

    if (!calibrationModeActive && millis() - lastDisplayTime >= displayInterval) {
        float soilRaw = sensorModule.getRawValue("soil", "moist");
        float soilCal = sensorModule.getCalibratedValue("soil", "moist");

        float tempRaw = sensorModule.getRawValue("dht", "temp");
        float tempCal = sensorModule.getCalibratedValue("dht", "temp");

        float humRaw = sensorModule.getRawValue("dht", "hum");
        float humCal = sensorModule.getCalibratedValue("dht", "hum");

        Serial.println("\n----- Current Readings -----");
        Serial.print("Soil Moisture: Raw = ");
        Serial.print(soilRaw, 1);
        Serial.print("%, Calibrated = ");
        Serial.print(soilCal, 1);
        Serial.println("%");

        Serial.print("Temperature: Raw = ");
        Serial.print(tempRaw, 1);
        Serial.print("°C, Calibrated = ");
        Serial.print(tempCal, 1);
        Serial.println("°C");

        Serial.print("Humidity: Raw = ");
        Serial.print(humRaw, 1);
        Serial.print("%, Calibrated = ");
        Serial.print(humCal, 1);
        Serial.println("%");

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

                Serial.print("Soil Moisture: Raw = ");
                Serial.print(sensorModule.getRawValue("soil", "moist"), 1);
                Serial.print("%, Calibrated = ");
                Serial.print(sensorModule.getCalibratedValue("soil", "moist"), 1);
                Serial.println("%");

                Serial.print("Temperature: Raw = ");
                Serial.print(sensorModule.getRawValue("dht", "temp"), 1);
                Serial.print("°C, Calibrated = ");
                Serial.print(sensorModule.getCalibratedValue("dht", "temp"), 1);
                Serial.println("°C");

                Serial.print("Humidity: Raw = ");
                Serial.print(sensorModule.getRawValue("dht", "hum"), 1);
                Serial.print("%, Calibrated = ");
                Serial.print(sensorModule.getCalibratedValue("dht", "hum"), 1);
                Serial.println("%");
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
                    Serial.println("\n----- Manual One-Point Calibration for Soil Moisture -----");
                    float currentRaw = sensorModule.getRawValue("soil", "moist");
                    Serial.print("Current raw reading: ");
                    Serial.print(currentRaw, 1);
                    Serial.println("%");

                    Serial.print("Enter known moisture value: ");
                    while (!Serial.available()) {
                        delay(10);
                    }

                    String input = Serial.readStringUntil('\n');
                    float knownValue = input.toFloat();

                    sensorModule.calibrateOnePoint("soil", "moist", knownValue);
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
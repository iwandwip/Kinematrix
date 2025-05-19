#define ENABLE_SENSOR_ALERT_SYSTEM_V2
#define ENABLE_SENSOR_FILTER_V2
#define ENABLE_SENSOR_MODULE_V2
#define ENABLE_SENSOR_UTILITY_V2
#define ENABLE_SENSOR_ANALOG_V2

#include "Kinematrix.h"

SensorModuleV2 sensorModule;
unsigned long lastDisplayTime = 0;
const unsigned long displayInterval = 1000;

void setup() {
    Serial.begin(115200);
    while (!Serial) {}

    Serial.println("\n===== Light Sensor Filtering & Alerts Demo =====");

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

    FilterParams levelFilterParams;
    levelFilterParams.movingAverage.windowSize = 10;
    sensorModule.attachFilter("light", "level", FILTER_MOVING_AVERAGE, levelFilterParams);

    FilterParams luxFilterParams;
    luxFilterParams.kalman.processNoise = 0.01f;
    luxFilterParams.kalman.measurementNoise = 0.1f;
    luxFilterParams.kalman.estimateError = 0.1f;
    sensorModule.attachFilter("light", "lux", FILTER_KALMAN, luxFilterParams);

    FilterParams uvFilterParams;
    uvFilterParams.median.windowSize = 5;
    sensorModule.attachFilter("light", "uv", FILTER_MEDIAN, uvFilterParams);

    sensorModule.setThreshold("light", "level", 20.0, 80.0, ALERT_OUTSIDE);
    sensorModule.setThreshold("light", "lux", 500.0, 2000.0, ALERT_OUTSIDE);
    sensorModule.setThreshold("light", "uv", 5, 10, ALERT_ABOVE);

    sensorModule.setGlobalAlertCallback([](AlertInfo alert) {
        Serial.print("ALERT: ");

        if (strcmp(alert.valueKey, "level") == 0) {
            Serial.print("Light level ");
            if (alert.currentValue < alert.lowThreshold) {
                Serial.print("too low! ");
            } else if (alert.currentValue > alert.highThreshold) {
                Serial.print("too high! ");
            }
            Serial.print("Current: ");
            Serial.print(alert.currentValue);
            Serial.print("%, Threshold: ");
            Serial.print(alert.lowThreshold);
            Serial.print("% - ");
            Serial.print(alert.highThreshold);
            Serial.println("%");
        } else if (strcmp(alert.valueKey, "lux") == 0) {
            Serial.print("Illuminance ");
            if (alert.currentValue < alert.lowThreshold) {
                Serial.print("too low! ");
            } else if (alert.currentValue > alert.highThreshold) {
                Serial.print("too high! ");
            }
            Serial.print("Current: ");
            Serial.print(alert.currentValue);
            Serial.print(" lx, Threshold: ");
            Serial.print(alert.lowThreshold);
            Serial.print(" lx - ");
            Serial.print(alert.highThreshold);
            Serial.println(" lx");
        } else if (strcmp(alert.valueKey, "uv") == 0) {
            Serial.print("UV index too high! ");
            Serial.print("Current: ");
            Serial.print(alert.currentValue);
            Serial.print(", Threshold: ");
            Serial.println(alert.highThreshold);
        }
    });

    sensorModule.setDefaultHysteresis(5000);
    sensorModule.setDefaultDebounceTime(1000);

    Serial.println("Press 'a' to acknowledge all alerts");
    Serial.println("Press 'r' to reset all alerts");
    Serial.println("Press 'f' to reset all filters");
    Serial.println("Press 'd' to display detailed information");
    Serial.println("Monitoring started...");
}

void loop() {
    sensorModule.update();

    if (millis() - lastDisplayTime >= displayInterval) {
        float rawLevel = sensorModule.getSensorByName("light")->getFloatValue("level");
        float filteredLevel = sensorModule.getFilteredValue("light", "level");

        float rawLux = sensorModule.getSensorByName("light")->getFloatValue("lux");
        float filteredLux = sensorModule.getFilteredValue("light", "lux");

        int rawUv = sensorModule.getSensorByName("light")->getIntValue("uv");
        float filteredUv = sensorModule.getFilteredValue("light", "uv");

        const char *category = sensorModule.getSensorByName("light")->getStringValue("category");

        Serial.println("\n----- Current Readings -----");
        Serial.print("Light Level: Raw = ");
        Serial.print(rawLevel, 1);
        Serial.print("% → Filtered = ");
        Serial.print(filteredLevel, 1);
        Serial.println("%");

        Serial.print("Illuminance: Raw = ");
        Serial.print(rawLux, 0);
        Serial.print(" lx → Filtered = ");
        Serial.print(filteredLux, 0);
        Serial.println(" lx");

        Serial.print("UV Index: Raw = ");
        Serial.print(rawUv);
        Serial.print(" → Filtered = ");
        Serial.print(filteredUv, 1);
        Serial.println("");

        Serial.print("Light Category: ");
        Serial.println(category);

        Serial.println("\n----- Alert Status -----");
        Serial.print("Light Level: ");
        Serial.println(sensorModule.isAlertActive("light", "level") ? "ACTIVE" : "INACTIVE");

        Serial.print("Illuminance: ");
        Serial.println(sensorModule.isAlertActive("light", "lux") ? "ACTIVE" : "INACTIVE");

        Serial.print("UV Index: ");
        Serial.println(sensorModule.isAlertActive("light", "uv") ? "ACTIVE" : "INACTIVE");

        lastDisplayTime = millis();
    }

    if (Serial.available() > 0) {
        char cmd = Serial.read();

        switch (cmd) {
            case 'a':
                sensorModule.acknowledgeAllAlerts();
                Serial.println("All alerts acknowledged");
                break;

            case 'r':
                sensorModule.resetAllAlerts();
                Serial.println("All alerts reset");
                break;

            case 'f':
                sensorModule.resetAllFilters();
                Serial.println("All filters reset");
                break;

            case 'd':
                Serial.println("\n----- Detailed Sensor Information -----");

                Serial.println("Filter Types:");
                Serial.print("Light Level: ");
                printFilterType(sensorModule.getFilterType("light", "level"));
                Serial.print("Illuminance: ");
                printFilterType(sensorModule.getFilterType("light", "lux"));
                Serial.print("UV Index: ");
                printFilterType(sensorModule.getFilterType("light", "uv"));

                Serial.println("\nDebug Information:");
                sensorModule.debugAll(true);
                break;
        }

        while (Serial.available()) {
            Serial.read();
        }
    }

    delay(10);
}

void printFilterType(FilterType type) {
    switch (type) {
        case FILTER_NONE:
            Serial.println("None");
            break;
        case FILTER_MOVING_AVERAGE:
            Serial.println("Moving Average");
            break;
        case FILTER_MEDIAN:
            Serial.println("Median");
            break;
        case FILTER_KALMAN:
            Serial.println("Kalman");
            break;
        case FILTER_EXPONENTIAL:
            Serial.println("Exponential");
            break;
        default:
            Serial.println("Unknown");
            break;
    }
}
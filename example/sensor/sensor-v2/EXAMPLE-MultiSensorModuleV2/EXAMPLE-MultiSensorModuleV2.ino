#define ENABLE_SENSOR_ALERT_SYSTEM_V2
#define ENABLE_SENSOR_FILTER_V2
#define ENABLE_SENSOR_MODULE_V2
#define ENABLE_SENSOR_UTILITY_V2

#define ENABLE_SENSOR_ANALOG_V2
#define ENABLE_SENSOR_DHT_V2

#include "Kinematrix.h"

SensorModuleV2 sensorModule;
unsigned long lastDisplayTime = 0;
const unsigned long displayInterval = 1000;

void setup() {
    Serial.begin(115200);
    while (!Serial) {}

    Serial.println("\n===== Sensor Module V2 Demo =====");

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

    sensorModule.setThreshold("soil", "moist", 20.0, 80.0, ALERT_OUTSIDE);
    sensorModule.setThresholdParams("soil", "moist", 5000, 1000);
    sensorModule.setSensorAlertCallback("soil", [](AlertInfo alert) {
        Serial.print("ALERT: Soil moisture ");
        if (alert.currentValue < alert.lowThreshold) {
            Serial.print("too low! ");
        } else {
            Serial.print("too high! ");
        }
        Serial.print("Current: ");
        Serial.print(alert.currentValue);
        Serial.print("%, Threshold: ");
        Serial.print(alert.lowThreshold);
        Serial.print("% - ");
        Serial.print(alert.highThreshold);
        Serial.println("%");
    });

    sensorModule.setThreshold("dht", "temp", 15.0, 30.0, ALERT_OUTSIDE);
    sensorModule.setThresholdParams("dht", "temp", 5000, 2000);
    sensorModule.setSensorAlertCallback("dht", [](AlertInfo alert) {
        Serial.print("ALERT: ");
        if (strcmp(alert.valueKey, "temp") == 0) {
            Serial.print("Temperature ");
            if (alert.currentValue < alert.lowThreshold) {
                Serial.print("too cold! ");
            } else {
                Serial.print("too hot! ");
            }
            Serial.print("Current: ");
            Serial.print(alert.currentValue);
            Serial.print("°C, Threshold: ");
            Serial.print(alert.lowThreshold);
            Serial.print("°C - ");
            Serial.print(alert.highThreshold);
            Serial.println("°C");
        } else if (strcmp(alert.valueKey, "hum") == 0) {
            Serial.print("Humidity ");
            if (alert.currentValue < alert.lowThreshold) {
                Serial.print("too low! ");
            } else {
                Serial.print("too high! ");
            }
            Serial.print("Current: ");
            Serial.print(alert.currentValue);
            Serial.print("%, Threshold: ");
            Serial.print(alert.lowThreshold);
            Serial.print("% - ");
            Serial.print(alert.highThreshold);
            Serial.println("%");
        }
    });

    sensorModule.setThreshold("dht", "hum", 30.0, 70.0, ALERT_OUTSIDE);
    sensorModule.setThresholdParams("dht", "hum", 5000, 2000);

    FilterParams soilFilterParams;
    soilFilterParams.movingAverage.windowSize = 10;
    sensorModule.attachFilter("soil", "moist", FILTER_MOVING_AVERAGE, soilFilterParams);

    FilterParams tempFilterParams;
    tempFilterParams.kalman.processNoise = 0.01f;
    tempFilterParams.kalman.measurementNoise = 0.1f;
    tempFilterParams.kalman.estimateError = 0.1f;
    sensorModule.attachFilter("dht", "temp", FILTER_KALMAN, tempFilterParams);

    FilterParams humFilterParams;
    humFilterParams.exponential.alpha = 0.3f;
    sensorModule.attachFilter("dht", "hum", FILTER_EXPONENTIAL, humFilterParams);

    Serial.println("Monitoring started...");
}

void loop() {
    sensorModule.update();

    if (millis() - lastDisplayTime >= displayInterval) {
        float rawMoisture = sensorModule.getSensorByName("soil")->getFloatValue("moist");
        float filteredMoisture = sensorModule.getFilteredValue("soil", "moist");

        float rawTemp = sensorModule.getSensorByName("dht")->getFloatValue("temp");
        float filteredTemp = sensorModule.getFilteredValue("dht", "temp");

        float rawHum = sensorModule.getSensorByName("dht")->getFloatValue("hum");
        float filteredHum = sensorModule.getFilteredValue("dht", "hum");

        Serial.print("Soil: ");
        Serial.print(rawMoisture, 1);
        Serial.print("% -> ");
        Serial.print(filteredMoisture, 1);
        Serial.print("% | Temp: ");
        Serial.print(rawTemp, 1);
        Serial.print("°C -> ");
        Serial.print(filteredTemp, 1);
        Serial.print("°C | Hum: ");
        Serial.print(rawHum, 1);
        Serial.print("% -> ");
        Serial.print(filteredHum, 1);
        Serial.println("%");

        sensorModule.debugHorizontal(true);

        lastDisplayTime = millis();
    }

    if (Serial.available() > 0) {
        char cmd = Serial.read();

        switch (cmd) {
            case 'a':
                sensorModule.acknowledgeAlert("soil", "moist");
                sensorModule.acknowledgeAlert("dht", "temp");
                sensorModule.acknowledgeAlert("dht", "hum");
                Serial.println("All alerts acknowledged");
                break;

            case 'r':
                sensorModule.resetAlert("soil", "moist");
                sensorModule.resetAlert("dht", "temp");
                sensorModule.resetAlert("dht", "hum");
                Serial.println("All alerts reset");
                break;

            case 'f':
                sensorModule.resetAllFilters();
                Serial.println("All filters reset");
                break;

            case 'd':
                Serial.println("\n----- Detailed Sensor Information -----");

                Serial.println("Raw and Filtered Values:");

                Serial.print("Soil Moisture: ");
                Serial.print(sensorModule.getSensorByName("soil")->getFloatValue("moist"), 1);
                Serial.print("% -> ");
                Serial.print(sensorModule.getFilteredValue("soil", "moist"), 1);
                Serial.println("%");

                Serial.print("Temperature: ");
                Serial.print(sensorModule.getSensorByName("dht")->getFloatValue("temp"), 1);
                Serial.print("°C -> ");
                Serial.print(sensorModule.getFilteredValue("dht", "temp"), 1);
                Serial.println("°C");

                Serial.print("Humidity: ");
                Serial.print(sensorModule.getSensorByName("dht")->getFloatValue("hum"), 1);
                Serial.print("% -> ");
                Serial.print(sensorModule.getFilteredValue("dht", "hum"), 1);
                Serial.println("%");

                Serial.println("\nAlert Status:");
                Serial.print("Soil Moisture: ");
                Serial.println(sensorModule.isAlertActive("soil", "moist") ? "ACTIVE" : "INACTIVE");

                Serial.print("Temperature: ");
                Serial.println(sensorModule.isAlertActive("dht", "temp") ? "ACTIVE" : "INACTIVE");

                Serial.print("Humidity: ");
                Serial.println(sensorModule.isAlertActive("dht", "hum") ? "ACTIVE" : "INACTIVE");

                Serial.println("\nFilter Types:");
                Serial.print("Soil Moisture: ");
                Serial.println(sensorModule.getFilterType("soil", "moist"));

                Serial.print("Temperature: ");
                Serial.println(sensorModule.getFilterType("dht", "temp"));

                Serial.print("Humidity: ");
                Serial.println(sensorModule.getFilterType("dht", "hum"));
                break;

            case 's':
                Serial.println("\n----- Sensor Status -----");
                sensorModule.debugAll();
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
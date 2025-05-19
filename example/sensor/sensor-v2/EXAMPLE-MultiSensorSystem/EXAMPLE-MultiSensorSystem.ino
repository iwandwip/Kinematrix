#define ENABLE_INTERACTIVE_SERIAL_GENERAL_SENSOR_CALIBRATOR_V2
#define ENABLE_SENSOR_ALERT_SYSTEM_V2
#define ENABLE_SENSOR_CALIBRATION_MODULE_V2
#define ENABLE_SENSOR_FILTER_V2
#define ENABLE_SENSOR_MODULE_V2
#define ENABLE_SENSOR_UTILITY_V2

#define ENABLE_SENSOR_ANALOG_V2
#define ENABLE_SENSOR_DHT_V2

#include "Kinematrix.h"

void displayCompactReadings();
void displayDetailedReadings();
String alertStateToString(AlertState state);
void handleCommand(char cmd);
void displayQuickStatistics();
void printFilterType(FilterType type);
void performManualSoilCalibration();
void performManualWeightCalibration();
void waitForSerialEnter();
float readSerialFloat(float defaultValue);

SensorCalibrationModuleV2 sensorModule;
unsigned long lastDisplayTime = 0;
const unsigned long displayInterval = 1000;
bool calibrationModeActive = false;

void setup() {
    Serial.begin(115200);
    while (!Serial) {}

    Serial.println("\n===== COMPREHENSIVE SENSOR SYSTEM DEMO =====");

    sensorModule.addSensor("soil", []() -> BaseSensV2 * {
        auto soilSensor = new AnalogSensV2(36, 3.3, 4095, [](BaseSensV2 *sensor, int analogValue, float voltageValue) {
            float moisturePercentage = map(analogValue, 0, 4095, 0, 100);
            sensor->updateValue("moist", moisturePercentage);

            float moistureVolumetric = map(analogValue, 0, 4095, 0, 50);
            sensor->updateValue("vol", moistureVolumetric);
        });

        soilSensor->addCustomValue("moist", "Moisture", "%", 1, TYPE_FLOAT);
        soilSensor->addCustomValue("vol", "Volumetric Water", "m³/m³", 3, TYPE_FLOAT);
        soilSensor->setUpdateInterval(100);
        return soilSensor;
    });

    sensorModule.addSensor("dht", []() -> BaseSensV2 * {
        auto dhtSensor = new DHTSensV2(21, DHT22);
        dhtSensor->setUpdateInterval(2000);
        return dhtSensor;
    });

    sensorModule.addSensor("light", []() -> BaseSensV2 * {
        auto lightSensor = new AnalogSensV2(39, 3.3, 4095, [](BaseSensV2 *sensor, int analogValue, float voltageValue) {
            float lightPercentage = map(analogValue, 0, 4095, 0, 100);
            sensor->updateValue("level", lightPercentage);

            float lux = analogValue * 1.5;
            sensor->updateValue("lux", lux);
        });

        lightSensor->addCustomValue("level", "Light Level", "%", 1, TYPE_FLOAT);
        lightSensor->addCustomValue("lux", "Illuminance", "lx", 0, TYPE_FLOAT);
        lightSensor->setUpdateInterval(500);
        return lightSensor;
    });

    sensorModule.addSensor("weight", []() -> BaseSensV2 * {
        auto weightSensor = new AnalogSensV2(34, 3.3, 4095, [](BaseSensV2 *sensor, int analogValue, float voltageValue) {
            float weight = analogValue * 0.01;
            sensor->updateValue("mass", weight);

            float force = weight * 9.81;
            sensor->updateValue("force", force);
        });

        weightSensor->addCustomValue("mass", "Weight", "kg", 2, TYPE_FLOAT);
        weightSensor->addCustomValue("force", "Force", "N", 2, TYPE_FLOAT);
        weightSensor->setUpdateInterval(200);
        return weightSensor;
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

    FilterParams tempFilterParams;
    tempFilterParams.kalman.processNoise = 0.01f;
    tempFilterParams.kalman.measurementNoise = 0.1f;
    tempFilterParams.kalman.estimateError = 0.1f;
    sensorModule.attachFilter("dht", "temp", FILTER_KALMAN, tempFilterParams);

    FilterParams humFilterParams;
    humFilterParams.exponential.alpha = 0.3f;
    sensorModule.attachFilter("dht", "hum", FILTER_EXPONENTIAL, humFilterParams);

    FilterParams lightFilterParams;
    lightFilterParams.median.windowSize = 5;
    sensorModule.attachFilter("light", "level", FILTER_MEDIAN, lightFilterParams);

    FilterParams weightFilterParams;
    weightFilterParams.movingAverage.windowSize = 20;
    sensorModule.attachFilter("weight", "mass", FILTER_MOVING_AVERAGE, weightFilterParams);

    sensorModule.setThreshold("soil", "moist", 20.0, 80.0, ALERT_OUTSIDE);
    sensorModule.setThresholdParams("soil", "moist", 5000, 1000);
    sensorModule.setSensorAlertCallback("soil", [](AlertInfo alert) {
        Serial.print("ALERT: Soil moisture ");
        if (alert.currentValue < alert.lowThreshold) {
            Serial.print("too dry! ");
        } else {
            Serial.print("too wet! ");
        }
        Serial.print("Current: ");
        Serial.print(alert.currentValue);
        Serial.print("%, Threshold: ");
        Serial.print(alert.lowThreshold);
        Serial.print("% - ");
        Serial.print(alert.highThreshold);
        Serial.println("%");
    });

    sensorModule.setThreshold("dht", "temp", 18.0, 28.0, ALERT_OUTSIDE);
    sensorModule.setThresholdParams("dht", "temp", 5000, 2000);

    sensorModule.setThreshold("dht", "hum", 30.0, 70.0, ALERT_OUTSIDE);
    sensorModule.setThresholdParams("dht", "hum", 5000, 2000);

    sensorModule.setThreshold("light", "level", 10.0, 100.0, ALERT_BELOW);
    sensorModule.setThresholdParams("light", "level", 3000, 1000);

    sensorModule.setThreshold("weight", "mass", 0.0, 5.0, ALERT_ABOVE);
    sensorModule.setThresholdParams("weight", "mass", 1000, 500);

    sensorModule.setGlobalAlertCallback([](AlertInfo alert) {
        Serial.print("SYSTEM ALERT: ");
        Serial.print(alert.sensorName);
        Serial.print(" - ");
        Serial.print(alert.valueKey);
        Serial.print(" = ");
        Serial.print(alert.currentValue);

        if (alert.type == ALERT_ABOVE) {
            Serial.print(" is above threshold ");
            Serial.println(alert.highThreshold);
        } else if (alert.type == ALERT_BELOW) {
            Serial.print(" is below threshold ");
            Serial.println(alert.lowThreshold);
        } else if (alert.type == ALERT_OUTSIDE) {
            Serial.print(" is outside range ");
            Serial.print(alert.lowThreshold);
            Serial.print(" - ");
            Serial.println(alert.highThreshold);
        } else if (alert.type == ALERT_BETWEEN) {
            Serial.print(" is between range ");
            Serial.print(alert.lowThreshold);
            Serial.print(" - ");
            Serial.println(alert.highThreshold);
        }
    });

    sensorModule.setDefaultHysteresis(3000);
    sensorModule.setDefaultDebounceTime(1000);

    Serial.println("\nCOMMAND MENU:");
    Serial.println("c - Enter calibration mode");
    Serial.println("x - Exit calibration mode");
    Serial.println("d - Display detailed sensor information");
    Serial.println("s - Save all calibrations");
    Serial.println("l - Load all calibrations");
    Serial.println("i - Show calibration status");
    Serial.println("v - List all available sensor values");
    Serial.println("a - Acknowledge all alerts");
    Serial.println("r - Reset all alerts");
    Serial.println("f - Reset all filters");
    Serial.println("1 - Perform manual calibration for soil sensor");
    Serial.println("2 - Perform manual calibration for weight sensor");
    Serial.println("m - Toggle display mode");
    Serial.println("q - Quick statistics");
    Serial.println("\nMonitoring started...");
}

bool compactDisplay = false;

void loop() {
    sensorModule.update();

    if (!calibrationModeActive && millis() - lastDisplayTime >= displayInterval) {
        if (compactDisplay) {
            displayCompactReadings();
        } else {
            displayDetailedReadings();
        }
        lastDisplayTime = millis();
    }

    if (Serial.available() > 0) {
        char cmd = Serial.read();
        handleCommand(cmd);

        while (Serial.available()) {
            Serial.read();
        }
    }

    delay(10);
}

void displayCompactReadings() {
    Serial.print("SOIL:");
    Serial.print(sensorModule.getCalibratedValue("soil", "moist"), 1);
    Serial.print("% ");

    Serial.print("TEMP:");
    Serial.print(sensorModule.getCalibratedValue("dht", "temp"), 1);
    Serial.print("C ");

    Serial.print("HUM:");
    Serial.print(sensorModule.getCalibratedValue("dht", "hum"), 1);
    Serial.print("% ");

    Serial.print("LIGHT:");
    Serial.print(sensorModule.getCalibratedValue("light", "level"), 1);
    Serial.print("% ");

    Serial.print("WEIGHT:");
    Serial.print(sensorModule.getCalibratedValue("weight", "mass"), 2);
    Serial.println("kg");
}

void displayDetailedReadings() {
    Serial.println("\n----- CURRENT READINGS -----");

    float soilRaw = sensorModule.getRawValue("soil", "moist");
    float soilCal = sensorModule.getCalibratedValue("soil", "moist");
    float soilVol = sensorModule.getCalibratedValue("soil", "vol");

    Serial.print("Soil Moisture: Raw = ");
    Serial.print(soilRaw, 1);
    Serial.print("%, Calibrated = ");
    Serial.print(soilCal, 1);
    Serial.print("%, Volumetric = ");
    Serial.print(soilVol, 3);
    Serial.println(" m³/m³");

    float tempRaw = sensorModule.getRawValue("dht", "temp");
    float tempCal = sensorModule.getCalibratedValue("dht", "temp");
    float humRaw = sensorModule.getRawValue("dht", "hum");
    float humCal = sensorModule.getCalibratedValue("dht", "hum");

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

    float lightRaw = sensorModule.getRawValue("light", "level");
    float lightCal = sensorModule.getCalibratedValue("light", "level");
    float luxCal = sensorModule.getCalibratedValue("light", "lux");

    Serial.print("Light Level: Raw = ");
    Serial.print(lightRaw, 1);
    Serial.print("%, Calibrated = ");
    Serial.print(lightCal, 1);
    Serial.print("%, Illuminance = ");
    Serial.print(luxCal, 0);
    Serial.println(" lx");

    float weightRaw = sensorModule.getRawValue("weight", "mass");
    float weightCal = sensorModule.getCalibratedValue("weight", "mass");
    float forceRaw = sensorModule.getRawValue("weight", "force");
    float forceCal = sensorModule.getCalibratedValue("weight", "force");

    Serial.print("Weight: Raw = ");
    Serial.print(weightRaw, 2);
    Serial.print("kg, Calibrated = ");
    Serial.print(weightCal, 2);
    Serial.println("kg");

    Serial.print("Force: Raw = ");
    Serial.print(forceRaw, 2);
    Serial.print("N, Calibrated = ");
    Serial.print(forceCal, 2);
    Serial.println("N");

    Serial.println("\n----- ALERT STATUS -----");
    Serial.print("Soil Moisture: ");
    Serial.println(alertStateToString(sensorModule.getAlertState("soil", "moist")));

    Serial.print("Temperature: ");
    Serial.println(alertStateToString(sensorModule.getAlertState("dht", "temp")));

    Serial.print("Humidity: ");
    Serial.println(alertStateToString(sensorModule.getAlertState("dht", "hum")));

    Serial.print("Light Level: ");
    Serial.println(alertStateToString(sensorModule.getAlertState("light", "level")));

    Serial.print("Weight: ");
    Serial.println(alertStateToString(sensorModule.getAlertState("weight", "mass")));
}

String alertStateToString(AlertState state) {
    switch (state) {
        case ALERT_INACTIVE:
            return "NORMAL";
        case ALERT_ACTIVE:
            return "ALERT!";
        case ALERT_ACKNOWLEDGED:
            return "ACKNOWLEDGED";
        default:
            return "UNKNOWN";
    }
}

void handleCommand(char cmd) {
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
            Serial.println("\n----- DETAILED SENSOR STATUS -----");
            sensorModule.debugAll(true);
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

        case 'a':
            sensorModule.acknowledgeAllAlerts();
            Serial.println("\nAll alerts acknowledged");
            break;

        case 'r':
            sensorModule.resetAllAlerts();
            Serial.println("\nAll alerts reset");
            break;

        case 'f':
            sensorModule.resetAllFilters();
            Serial.println("\nAll filters reset");
            break;

        case 'm':
            compactDisplay = !compactDisplay;
            Serial.print("\nDisplay mode set to ");
            Serial.println(compactDisplay ? "COMPACT" : "DETAILED");
            break;

        case 'q':
            displayQuickStatistics();
            break;

        case '1':
            if (!calibrationModeActive) {
                performManualSoilCalibration();
            }
            break;

        case '2':
            if (!calibrationModeActive) {
                performManualWeightCalibration();
            }
            break;
    }
}

void displayQuickStatistics() {
    Serial.println("\n----- QUICK STATISTICS -----");

    float soilRaw = sensorModule.getRawValue("soil", "moist");
    float soilFilter = sensorModule.getFilteredValue("soil", "moist");
    float soilCal = sensorModule.getCalibratedValue("soil", "moist");

    Serial.println("Soil Moisture:");
    Serial.print("  Raw: ");
    Serial.print(soilRaw, 1);
    Serial.print("% → Filtered: ");
    Serial.print(soilFilter, 1);
    Serial.print("% → Calibrated: ");
    Serial.print(soilCal, 1);
    Serial.println("%");

    float tempRaw = sensorModule.getRawValue("dht", "temp");
    float tempFilter = sensorModule.getFilteredValue("dht", "temp");
    float tempCal = sensorModule.getCalibratedValue("dht", "temp");

    Serial.println("Temperature:");
    Serial.print("  Raw: ");
    Serial.print(tempRaw, 1);
    Serial.print("°C → Filtered: ");
    Serial.print(tempFilter, 1);
    Serial.print("°C → Calibrated: ");
    Serial.print(tempCal, 1);
    Serial.println("°C");

    float weightRaw = sensorModule.getRawValue("weight", "mass");
    float weightFilter = sensorModule.getFilteredValue("weight", "mass");
    float weightCal = sensorModule.getCalibratedValue("weight", "mass");

    Serial.println("Weight:");
    Serial.print("  Raw: ");
    Serial.print(weightRaw, 2);
    Serial.print("kg → Filtered: ");
    Serial.print(weightFilter, 2);
    Serial.print("kg → Calibrated: ");
    Serial.print(weightCal, 2);
    Serial.println("kg");

    Serial.println("\nFilter Types:");
    Serial.print("  Soil: ");
    printFilterType(sensorModule.getFilterType("soil", "moist"));
    Serial.print("  Temperature: ");
    printFilterType(sensorModule.getFilterType("dht", "temp"));
    Serial.print("  Weight: ");
    printFilterType(sensorModule.getFilterType("weight", "mass"));

    Serial.println("\nActive Alerts:");
    int alertCount = 0;

    if (sensorModule.isAlertActive("soil", "moist")) {
        Serial.println("  - Soil Moisture");
        alertCount++;
    }

    if (sensorModule.isAlertActive("dht", "temp")) {
        Serial.println("  - Temperature");
        alertCount++;
    }

    if (sensorModule.isAlertActive("dht", "hum")) {
        Serial.println("  - Humidity");
        alertCount++;
    }

    if (sensorModule.isAlertActive("light", "level")) {
        Serial.println("  - Light Level");
        alertCount++;
    }

    if (sensorModule.isAlertActive("weight", "mass")) {
        Serial.println("  - Weight");
        alertCount++;
    }

    if (alertCount == 0) {
        Serial.println("  None");
    }
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

void performManualSoilCalibration() {
    Serial.println("\n----- MANUAL SOIL MOISTURE CALIBRATION -----");
    Serial.println("This will perform a two-point calibration.");

    Serial.println("\nSTEP 1: Set up your MINIMUM moisture reference point (dry soil)");
    Serial.println("Press Enter when ready...");
    waitForSerialEnter();

    float dryRaw = sensorModule.getRawValue("soil", "moist");
    Serial.print("Minimum point raw reading: ");
    Serial.print(dryRaw, 1);
    Serial.println("%");

    Serial.print("Enter the known moisture value for DRY soil (usually 0%): ");
    float dryValue = readSerialFloat(0.0);

    Serial.println("\nSTEP 2: Set up your MAXIMUM moisture reference point (wet soil)");
    Serial.println("Press Enter when ready...");
    waitForSerialEnter();

    float wetRaw = sensorModule.getRawValue("soil", "moist");
    Serial.print("Maximum point raw reading: ");
    Serial.print(wetRaw, 1);
    Serial.println("%");

    Serial.print("Enter the known moisture value for WET soil (usually 100%): ");
    float wetValue = readSerialFloat(100.0);

    sensorModule.calibrateTwoPoint("soil", "moist", dryValue, dryRaw, wetValue, wetRaw);
    Serial.println("\nTwo-point calibration completed!");

    float currentValue = sensorModule.getCalibratedValue("soil", "moist");
    Serial.print("Current calibrated moisture: ");
    Serial.print(currentValue, 1);
    Serial.println("%");
}

void performManualWeightCalibration() {
    Serial.println("\n----- MANUAL WEIGHT CALIBRATION -----");
    Serial.println("This will perform a multi-point calibration.");

    Serial.print("How many calibration points (2-5)? ");
    int numPoints = (int) readSerialFloat(3);

    if (numPoints < 2) numPoints = 2;
    if (numPoints > 5) numPoints = 5;

    float rawValues[5];
    float knownValues[5];

    for (int i = 0; i < numPoints; i++) {
        Serial.print("\nSTEP ");
        Serial.print(i + 1);
        Serial.print(": Place a known weight on the sensor");
        Serial.println("\nPress Enter when ready...");
        waitForSerialEnter();

        rawValues[i] = sensorModule.getRawValue("weight", "mass");
        Serial.print("Raw reading: ");
        Serial.print(rawValues[i], 3);
        Serial.println("kg");

        Serial.print("Enter the actual known weight in kg: ");
        knownValues[i] = readSerialFloat(i * 0.5);
    }

    // Perbaikan: Ganti metode kalibrasi karena fungsi yang tidak tersedia di library
    // Kita akan menggunakan calibrateTwoPoint untuk dua titik pertama,
    // dan kemudian melaporkan bahwa kalibrasi multi-point tidak tersedia
    sensorModule.calibrateTwoPoint("weight", "mass", knownValues[0], rawValues[0], knownValues[1], rawValues[1]);
    Serial.println("\nTwo-point calibration completed!");

    if (numPoints > 2) {
        Serial.println("Note: Multi-point calibration with more than 2 points is not available in this library version.");
        Serial.println("Only the first two points were used for calibration.");
    }

    float currentValue = sensorModule.getCalibratedValue("weight", "mass");
    Serial.print("Current calibrated weight: ");
    Serial.print(currentValue, 3);
    Serial.println("kg");
}

void waitForSerialEnter() {
    while (Serial.available()) Serial.read();

    Serial.println("Press Enter...");
    while (!Serial.available()) delay(10);

    while (Serial.available()) Serial.read();
}

float readSerialFloat(float defaultValue) {
    while (Serial.available()) Serial.read();

    String input = "";
    unsigned long startTime = millis();
    bool timeout = false;

    while (!timeout) {
        if (Serial.available()) {
            char c = Serial.read();
            if (c == '\n' || c == '\r') {
                break;
            }
            input += c;
        }

        if (millis() - startTime > 10000) {
            timeout = true;
        }

        delay(10);
    }

    Serial.println(input.length() > 0 ? input : String(defaultValue));

    if (input.length() > 0) {
        return input.toFloat();
    } else {
        return defaultValue;
    }
}
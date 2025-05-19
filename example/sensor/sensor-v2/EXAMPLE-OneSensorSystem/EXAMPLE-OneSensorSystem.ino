#define ENABLE_INTERACTIVE_SERIAL_GENERAL_SENSOR_CALIBRATOR_V2
#define ENABLE_SENSOR_ALERT_SYSTEM_V2
#define ENABLE_SENSOR_CALIBRATION_MODULE_V2
#define ENABLE_SENSOR_FILTER_V2
#define ENABLE_SENSOR_MODULE_V2
#define ENABLE_SENSOR_UTILITY_V2
#define ENABLE_SENSOR_ANALOG_V2

#include "Kinematrix.h"

void displayCompactReadings();
void displayDetailedReadings();
String alertStateToString(AlertState state);
void handleCommand(char cmd);
void displayQuickStatistics();
void printFilterType(FilterType type);
void performManualCalibration();
void waitForSerialEnter();
float readSerialFloat(float defaultValue);

SensorCalibrationModuleV2 sensorModule;
unsigned long lastDisplayTime = 0;
const unsigned long displayInterval = 1000;
bool calibrationModeActive = false;
bool compactDisplay = false;

void setup() {
    Serial.begin(115200);
    while (!Serial) {}

    Serial.println("\n===== COMPREHENSIVE LIGHT SENSOR SYSTEM =====");

    sensorModule.addSensor("light", []() -> BaseSensV2 * {
        auto lightSensor = new AnalogSensV2(36, 3.3, 4095, [](BaseSensV2 *sensor, int analogValue, float voltageValue) {
            float lightPercentage = map(analogValue, 0, 4095, 0, 100);
            sensor->updateValue("level", lightPercentage);

            float lux = analogValue * 2.5;
            sensor->updateValue("lux", lux);

            int uvIndex = map(analogValue, 0, 4095, 0, 12);
            sensor->updateValue("uv", uvIndex);

            float energyEfficiency = 0;
            if (lightPercentage > 0) {
                energyEfficiency = lux / (voltageValue * 10); // Simulated lumens per watt
            }
            sensor->updateValue("efficiency", energyEfficiency);
        });

        lightSensor->addCustomValue("level", "Light Level", "%", 1, TYPE_FLOAT);
        lightSensor->addCustomValue("lux", "Illuminance", "lx", 0, TYPE_FLOAT);
        lightSensor->addCustomValue("uv", "UV Index", "", 0, TYPE_INT);
        lightSensor->addCustomValue("efficiency", "Energy Efficiency", "lm/W", 1, TYPE_FLOAT);
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

    FilterParams efficiencyFilterParams;
    efficiencyFilterParams.exponential.alpha = 0.2f;
    sensorModule.attachFilter("light", "efficiency", FILTER_EXPONENTIAL, efficiencyFilterParams);

    sensorModule.setThreshold("light", "level", 20.0, 80.0, ALERT_OUTSIDE);
    sensorModule.setThresholdParams("light", "level", 5000, 1000);

    sensorModule.setThreshold("light", "lux", 500.0, 2000.0, ALERT_OUTSIDE);
    sensorModule.setThresholdParams("light", "lux", 5000, 1000);

    sensorModule.setThreshold("light", "uv", 0, 7, ALERT_ABOVE);
    sensorModule.setThresholdParams("light", "uv", 5000, 1000);

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
    Serial.println("1 - Perform manual two-point calibration");
    Serial.println("m - Toggle display mode");
    Serial.println("q - Quick statistics");
    Serial.println("\nMonitoring started...");
}

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
    Serial.print("LIGHT:");
    Serial.print(sensorModule.getCalibratedValue("light", "level"), 1);
    Serial.print("% ");

    Serial.print("LUX:");
    Serial.print(sensorModule.getCalibratedValue("light", "lux"), 0);
    Serial.print(" ");

    Serial.print("UV:");
    Serial.print(sensorModule.getCalibratedValue("light", "uv"), 0);
    Serial.print(" ");

    Serial.print("EFF:");
    Serial.print(sensorModule.getCalibratedValue("light", "efficiency"), 1);
    Serial.println("lm/W");
}

void displayDetailedReadings() {
    Serial.println("\n----- CURRENT READINGS -----");

    float levelRaw = sensorModule.getRawValue("light", "level");
    float levelFiltered = sensorModule.getFilteredValue("light", "level");
    float levelCal = sensorModule.getCalibratedValue("light", "level");

    Serial.print("Light Level: Raw = ");
    Serial.print(levelRaw, 1);
    Serial.print("% → Filtered = ");
    Serial.print(levelFiltered, 1);
    Serial.print("% → Calibrated = ");
    Serial.print(levelCal, 1);
    Serial.println("%");

    float luxRaw = sensorModule.getRawValue("light", "lux");
    float luxFiltered = sensorModule.getFilteredValue("light", "lux");
    float luxCal = sensorModule.getCalibratedValue("light", "lux");

    Serial.print("Illuminance: Raw = ");
    Serial.print(luxRaw, 0);
    Serial.print(" lx → Filtered = ");
    Serial.print(luxFiltered, 0);
    Serial.print(" lx → Calibrated = ");
    Serial.print(luxCal, 0);
    Serial.println(" lx");

    int uvRaw = sensorModule.getSensorByName("light")->getIntValue("uv");
    float uvFiltered = sensorModule.getFilteredValue("light", "uv");
    float uvCal = sensorModule.getCalibratedValue("light", "uv");

    Serial.print("UV Index: Raw = ");
    Serial.print(uvRaw);
    Serial.print(" → Filtered = ");
    Serial.print(uvFiltered, 1);
    Serial.print(" → Calibrated = ");
    Serial.println(uvCal, 1);

    float effRaw = sensorModule.getRawValue("light", "efficiency");
    float effFiltered = sensorModule.getFilteredValue("light", "efficiency");
    float effCal = sensorModule.getCalibratedValue("light", "efficiency");

    Serial.print("Energy Efficiency: Raw = ");
    Serial.print(effRaw, 1);
    Serial.print(" lm/W → Filtered = ");
    Serial.print(effFiltered, 1);
    Serial.print(" lm/W → Calibrated = ");
    Serial.print(effCal, 1);
    Serial.println(" lm/W");

    Serial.println("\n----- ALERT STATUS -----");
    Serial.print("Light Level: ");
    Serial.println(alertStateToString(sensorModule.getAlertState("light", "level")));

    Serial.print("Illuminance: ");
    Serial.println(alertStateToString(sensorModule.getAlertState("light", "lux")));

    Serial.print("UV Index: ");
    Serial.println(alertStateToString(sensorModule.getAlertState("light", "uv")));
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
                performManualCalibration();
            }
            break;
    }
}

void displayQuickStatistics() {
    Serial.println("\n----- QUICK STATISTICS -----");

    float levelRaw = sensorModule.getRawValue("light", "level");
    float levelFilter = sensorModule.getFilteredValue("light", "level");
    float levelCal = sensorModule.getCalibratedValue("light", "level");

    Serial.println("Light Level:");
    Serial.print("  Raw: ");
    Serial.print(levelRaw, 1);
    Serial.print("% → Filtered: ");
    Serial.print(levelFilter, 1);
    Serial.print("% → Calibrated: ");
    Serial.print(levelCal, 1);
    Serial.println("%");

    float luxRaw = sensorModule.getRawValue("light", "lux");
    float luxFilter = sensorModule.getFilteredValue("light", "lux");
    float luxCal = sensorModule.getCalibratedValue("light", "lux");

    Serial.println("Illuminance:");
    Serial.print("  Raw: ");
    Serial.print(luxRaw, 0);
    Serial.print(" lx → Filtered: ");
    Serial.print(luxFilter, 0);
    Serial.print(" lx → Calibrated: ");
    Serial.print(luxCal, 0);
    Serial.println(" lx");

    Serial.println("\nFilter Types:");
    Serial.print("  Light Level: ");
    printFilterType(sensorModule.getFilterType("light", "level"));
    Serial.print("  Illuminance: ");
    printFilterType(sensorModule.getFilterType("light", "lux"));
    Serial.print("  UV Index: ");
    printFilterType(sensorModule.getFilterType("light", "uv"));
    Serial.print("  Energy Efficiency: ");
    printFilterType(sensorModule.getFilterType("light", "efficiency"));

    Serial.println("\nActive Alerts:");
    int alertCount = 0;

    if (sensorModule.isAlertActive("light", "level")) {
        Serial.println("  - Light Level");
        alertCount++;
    }

    if (sensorModule.isAlertActive("light", "lux")) {
        Serial.println("  - Illuminance");
        alertCount++;
    }

    if (sensorModule.isAlertActive("light", "uv")) {
        Serial.println("  - UV Index");
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

void performManualCalibration() {
    Serial.println("\n----- MANUAL LIGHT SENSOR CALIBRATION -----");
    Serial.println("This will perform a two-point calibration.");

    Serial.println("\nSTEP 1: Set up your MINIMUM light reference point");
    Serial.println("Press Enter when ready...");
    waitForSerialEnter();

    float darkRaw = sensorModule.getRawValue("light", "level");
    Serial.print("Minimum point raw reading: ");
    Serial.print(darkRaw, 1);
    Serial.println("%");

    Serial.print("Enter the known light value for minimum point: ");
    float darkValue = readSerialFloat(0.0);

    Serial.println("\nSTEP 2: Set up your MAXIMUM light reference point");
    Serial.println("Press Enter when ready...");
    waitForSerialEnter();

    float brightRaw = sensorModule.getRawValue("light", "level");
    Serial.print("Maximum point raw reading: ");
    Serial.print(brightRaw, 1);
    Serial.println("%");

    Serial.print("Enter the known light value for maximum point: ");
    float brightValue = readSerialFloat(100.0);

    sensorModule.calibrateTwoPoint("light", "level", darkValue, darkRaw, brightValue, brightRaw);
    Serial.println("\nTwo-point calibration completed!");

    float currentValue = sensorModule.getCalibratedValue("light", "level");
    Serial.print("Current calibrated light level: ");
    Serial.print(currentValue, 1);
    Serial.println("%");
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
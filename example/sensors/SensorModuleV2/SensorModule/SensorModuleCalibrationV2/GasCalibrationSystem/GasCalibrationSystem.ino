#define ENABLE_SENSOR_MODULE_V2
#define ENABLE_SENSOR_CALIBRATION_MODULE_V2
#define ENABLE_INTERACTIVE_SERIAL_GENERAL_SENSOR_CALIBRATOR_V2
#define ENABLE_SENSOR_ALERT_SYSTEM_V2
#define ENABLE_SENSOR_FILTER_V2
#define ENABLE_SENSOR_UTILITY_V2
#define ENABLE_SENSOR_MQ_V2
#include "Kinematrix.h"

SensorCalibrationModuleV2 calibrationModule;

void setup() {
    Serial.begin(115200);
    Serial.println("Gas Sensor Calibration System Example");
    
    calibrationModule.addSensor("mq2", new MQSensV2("Arduino", 5.0, 10, A0, "MQ-2"));
    calibrationModule.addSensor("mq135", new MQSensV2("Arduino", 5.0, 10, A1, "MQ-135"));
    calibrationModule.addSensor("mq7", new MQSensV2("Arduino", 5.0, 10, A2, "MQ-7"));
    calibrationModule.addSensor("mq9", new MQSensV2("Arduino", 5.0, 10, A3, "MQ-9"));
    
    calibrationModule.init();
    
    calibrationModule.addCalibrationEntry("mq2", "lpg");
    calibrationModule.addCalibrationEntry("mq2", "smoke");
    calibrationModule.addCalibrationEntry("mq135", "co2");
    calibrationModule.addCalibrationEntry("mq135", "nh3");
    calibrationModule.addCalibrationEntry("mq7", "co");
    calibrationModule.addCalibrationEntry("mq9", "methane");
    
    calibrationModule.calibrateTwoPoint("mq2", "lpg", 200.0, 185.0, 5000.0, 5150.0);
    calibrationModule.calibrateOnePoint("mq2", "smoke", 100.0);
    calibrationModule.calibrateOnePoint("mq135", "co2", 400.0);
    calibrationModule.calibrateTwoPoint("mq135", "nh3", 10.0, 9.5, 300.0, 315.0);
    calibrationModule.calibrateOnePoint("mq7", "co", 30.0);
    calibrationModule.calibrateTwoPoint("mq9", "methane", 200.0, 195.0, 10000.0, 10200.0);
    
    Serial.println("Gas sensor calibration system ready");
}

void loop() {
    calibrationModule.update();
    
    if (calibrationModule.isUpdated("mq2")) {
        Serial.println("=== MQ-2 Gas Sensor ===");
        Serial.print("LPG - Raw: ");
        Serial.print((float)calibrationModule["mq2"]["lpg"]);
        Serial.print(" ppm, Cal: ");
        Serial.print((float)calibrationModule.getCalibratedValue("mq2", "lpg"));
        Serial.println(" ppm");
        
        Serial.print("Smoke - Raw: ");
        Serial.print((float)calibrationModule["mq2"]["smoke"]);
        Serial.print(" ppm, Cal: ");
        Serial.print((float)calibrationModule.getCalibratedValue("mq2", "smoke"));
        Serial.println(" ppm");
    }
    
    if (calibrationModule.isUpdated("mq135")) {
        Serial.println("=== MQ-135 Air Quality ===");
        Serial.print("CO2 - Raw: ");
        Serial.print((float)calibrationModule["mq135"]["co2"]);
        Serial.print(" ppm, Cal: ");
        Serial.print((float)calibrationModule.getCalibratedValue("mq135", "co2"));
        Serial.println(" ppm");
    }
    
    if (calibrationModule.isUpdated("mq7")) {
        Serial.print("CO - Raw: ");
        Serial.print((float)calibrationModule["mq7"]["co"]);
        Serial.print(" ppm, Cal: ");
        Serial.print((float)calibrationModule.getCalibratedValue("mq7", "co"));
        Serial.println(" ppm");
    }
    
    delay(3000);
}
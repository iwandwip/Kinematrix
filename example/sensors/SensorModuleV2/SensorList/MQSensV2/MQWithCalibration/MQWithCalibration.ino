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
    Serial.println("MQSensV2 with Calibration Example");
    
    calibrationModule.addSensor("mq2_kitchen", new MQSensV2("Arduino", 5.0, 10, A0, "MQ-2"));
    calibrationModule.addSensor("mq135_office", new MQSensV2("Arduino", 5.0, 10, A1, "MQ-135"));
    calibrationModule.addSensor("mq7_garage", new MQSensV2("Arduino", 5.0, 10, A2, "MQ-7"));
    calibrationModule.addSensor("mq9_basement", new MQSensV2("Arduino", 5.0, 10, A3, "MQ-9"));
    
    calibrationModule.init();
    
    calibrationModule.addCalibrationEntry("mq2_kitchen", "lpg");
    calibrationModule.addCalibrationEntry("mq2_kitchen", "smoke");
    calibrationModule.addCalibrationEntry("mq135_office", "co2");
    calibrationModule.addCalibrationEntry("mq135_office", "nh3");
    calibrationModule.addCalibrationEntry("mq7_garage", "co");
    calibrationModule.addCalibrationEntry("mq9_basement", "methane");
    
    calibrationModule.calibrateTwoPoint("mq2_kitchen", "lpg", 200.0, 185.0, 1000.0, 1020.0);
    calibrationModule.calibrateOnePoint("mq2_kitchen", "smoke", 50.0);
    calibrationModule.calibrateTwoPoint("mq135_office", "co2", 400.0, 385.0, 1000.0, 1015.0);
    calibrationModule.calibrateOnePoint("mq135_office", "nh3", 25.0);
    calibrationModule.calibrateOnePoint("mq7_garage", "co", 30.0);
    calibrationModule.calibrateTwoPoint("mq9_basement", "methane", 300.0, 285.0, 5000.0, 5100.0);
    
    Serial.println("Gas sensors with calibration ready");
    Serial.println("Warming up sensors... please wait 60 seconds");
    delay(60000);
    Serial.println("Calibrated gas detection system ready");
}

void loop() {
    calibrationModule.update();
    
    if (calibrationModule.isUpdated("mq2_kitchen")) {
        float rawLpg = calibrationModule["mq2_kitchen"]["lpg"];
        float calLpg = calibrationModule.getCalibratedValue("mq2_kitchen", "lpg");
        float rawSmoke = calibrationModule["mq2_kitchen"]["smoke"];
        float calSmoke = calibrationModule.getCalibratedValue("mq2_kitchen", "smoke");
        
        Serial.println("=== Kitchen MQ-2 ===");
        Serial.print("LPG - Raw: ");
        Serial.print(rawLpg);
        Serial.print(" ppm, Cal: ");
        Serial.print(calLpg);
        Serial.println(" ppm");
        Serial.print("Smoke - Raw: ");
        Serial.print(rawSmoke);
        Serial.print(" ppm, Cal: ");
        Serial.print(calSmoke);
        Serial.println(" ppm");
    }
    
    if (calibrationModule.isUpdated("mq135_office")) {
        float calCo2 = calibrationModule.getCalibratedValue("mq135_office", "co2");
        float calNh3 = calibrationModule.getCalibratedValue("mq135_office", "nh3");
        Serial.print("Office Air Quality - CO2: ");
        Serial.print(calCo2);
        Serial.print(" ppm, NH3: ");
        Serial.print(calNh3);
        Serial.println(" ppm");
    }
    
    if (calibrationModule.isUpdated("mq7_garage")) {
        float calCo = calibrationModule.getCalibratedValue("mq7_garage", "co");
        Serial.print("Garage CO - Calibrated: ");
        Serial.print(calCo);
        Serial.println(" ppm");
    }
    
    if (calibrationModule.isUpdated("mq9_basement")) {
        float calMethane = calibrationModule.getCalibratedValue("mq9_basement", "methane");
        Serial.print("Basement Methane - Calibrated: ");
        Serial.print(calMethane);
        Serial.println(" ppm");
    }
    
    delay(5000);
}
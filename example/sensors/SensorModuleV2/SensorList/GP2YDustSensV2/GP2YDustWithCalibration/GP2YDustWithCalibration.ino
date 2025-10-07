#define ENABLE_SENSOR_MODULE_V2
#define ENABLE_SENSOR_CALIBRATION_MODULE_V2
#define ENABLE_INTERACTIVE_SERIAL_GENERAL_SENSOR_CALIBRATOR_V2
#define ENABLE_SENSOR_ALERT_SYSTEM_V2
#define ENABLE_SENSOR_FILTER_V2
#define ENABLE_SENSOR_UTILITY_V2
#define ENABLE_SENSOR_GP2Y_DUST_V2
#include "Kinematrix.h"

SensorCalibrationModuleV2 calibrationModule;

void setup() {
    Serial.begin(115200);
    Serial.println("GP2YDustSensV2 with Calibration Example");
    
    calibrationModule.addSensor("indoor_dust", new GP2YDustSensV2(GP2Y1010AU0F, 7, A0));
    calibrationModule.addSensor("outdoor_dust", new GP2YDustSensV2(GP2Y1010AU0F, 8, A1));
    calibrationModule.addSensor("exhaust_dust", new GP2YDustSensV2(GP2Y1014AU0F, 9, A2));
    
    calibrationModule.init();
    
    calibrationModule.addCalibrationEntry("indoor_dust", "density");
    calibrationModule.addCalibrationEntry("indoor_dust", "voltage");
    calibrationModule.addCalibrationEntry("outdoor_dust", "density");
    calibrationModule.addCalibrationEntry("outdoor_dust", "voltage");
    calibrationModule.addCalibrationEntry("exhaust_dust", "density");
    
    calibrationModule.calibrateTwoPoint("indoor_dust", "density", 25.0, 23.5, 100.0, 102.8);
    calibrationModule.calibrateOnePoint("indoor_dust", "voltage", 2.5);
    calibrationModule.calibrateTwoPoint("outdoor_dust", "density", 50.0, 48.2, 200.0, 205.5);
    calibrationModule.calibrateOnePoint("outdoor_dust", "voltage", 3.0);
    calibrationModule.calibrateOnePoint("exhaust_dust", "density", 150.0);
    
    Serial.println("Dust sensors with calibration ready");
    Serial.println("Warming up sensors... please wait 30 seconds");
    delay(30000);
    Serial.println("Calibrated dust monitoring system ready");
}

void loop() {
    calibrationModule.update();
    
    if (calibrationModule.isUpdated("indoor_dust")) {
        float rawDensity = calibrationModule["indoor_dust"]["density"];
        float calDensity = calibrationModule.getCalibratedValue("indoor_dust", "density");
        float rawVoltage = calibrationModule["indoor_dust"]["voltage"];
        float calVoltage = calibrationModule.getCalibratedValue("indoor_dust", "voltage");
        
        Serial.println("=== Indoor Dust Sensor ===");
        Serial.print("Density - Raw: ");
        Serial.print(rawDensity);
        Serial.print(" μg/m³, Cal: ");
        Serial.print(calDensity);
        Serial.println(" μg/m³");
        Serial.print("Voltage - Raw: ");
        Serial.print(rawVoltage);
        Serial.print("V, Cal: ");
        Serial.print(calVoltage);
        Serial.println("V");
    }
    
    if (calibrationModule.isUpdated("outdoor_dust")) {
        float calDensity = calibrationModule.getCalibratedValue("outdoor_dust", "density");
        float calVoltage = calibrationModule.getCalibratedValue("outdoor_dust", "voltage");
        Serial.print("Outdoor Dust - Density: ");
        Serial.print(calDensity);
        Serial.print(" μg/m³, Voltage: ");
        Serial.print(calVoltage);
        Serial.println("V");
    }
    
    if (calibrationModule.isUpdated("exhaust_dust")) {
        float calDensity = calibrationModule.getCalibratedValue("exhaust_dust", "density");
        Serial.print("Exhaust Dust - Calibrated: ");
        Serial.print(calDensity);
        Serial.println(" μg/m³");
    }
    
    delay(3000);
}
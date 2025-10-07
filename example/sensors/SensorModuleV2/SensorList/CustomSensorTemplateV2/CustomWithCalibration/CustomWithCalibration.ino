#define ENABLE_SENSOR_MODULE_V2
#define ENABLE_SENSOR_CALIBRATION_MODULE_V2
#define ENABLE_INTERACTIVE_SERIAL_GENERAL_SENSOR_CALIBRATOR_V2
#define ENABLE_SENSOR_ALERT_SYSTEM_V2
#define ENABLE_SENSOR_FILTER_V2
#define ENABLE_SENSOR_UTILITY_V2
#define ENABLE_SENSOR_CUSTOM_TEMPLATE_V2
#include "Kinematrix.h"

SensorCalibrationModuleV2 calibrationModule;

void setup() {
    Serial.begin(115200);
    Serial.println("CustomSensorTemplateV2 with Calibration Example");
    
    CustomSensorTemplateV2* sensor1 = new CustomSensorTemplateV2(A0);
    sensor1->setCalibrationFactor(2.0);
    sensor1->setSensorMode(0);
    sensor1->addCustomSensorValue("pressure", "Pressure", "bar", 2, true);
    
    CustomSensorTemplateV2* sensor2 = new CustomSensorTemplateV2(A1);
    sensor2->setCalibrationFactor(0.5);
    sensor2->setSensorMode(1);
    sensor2->addCustomSensorValue("level", "Level", "cm", 1, true);
    
    CustomSensorTemplateV2* sensor3 = new CustomSensorTemplateV2(A2);
    sensor3->setCalibrationFactor(1.8);
    sensor3->setSensorMode(0);
    sensor3->addCustomSensorValue("force", "Force", "N", 2, true);
    
    calibrationModule.addSensor("custom1", sensor1);
    calibrationModule.addSensor("custom2", sensor2);
    calibrationModule.addSensor("custom3", sensor3);
    
    calibrationModule.init();
    
    calibrationModule.addCalibrationEntry("custom1", "value");
    calibrationModule.addCalibrationEntry("custom1", "pressure");
    calibrationModule.addCalibrationEntry("custom2", "value");
    calibrationModule.addCalibrationEntry("custom2", "level");
    calibrationModule.addCalibrationEntry("custom3", "force");
    
    calibrationModule.calibrateTwoPoint("custom1", "value", 5.0, 4.8, 15.0, 15.3);
    calibrationModule.calibrateOnePoint("custom1", "pressure", 10.0);
    calibrationModule.calibrateOnePoint("custom2", "value", 25.0);
    calibrationModule.calibrateTwoPoint("custom2", "level", 10.0, 9.8, 100.0, 101.2);
    calibrationModule.calibrateOnePoint("custom3", "force", 50.0);
    
    Serial.println("Custom sensors with calibration ready");
}

void loop() {
    calibrationModule.update();
    
    if (calibrationModule.isUpdated("custom1")) {
        float rawValue = calibrationModule["custom1"]["value"];
        float calValue = calibrationModule.getCalibratedValue("custom1", "value");
        float rawPressure = calibrationModule["custom1"]["pressure"];
        float calPressure = calibrationModule.getCalibratedValue("custom1", "pressure");
        
        Serial.println("=== Custom Sensor 1 ===");
        Serial.print("Value - Raw: ");
        Serial.print(rawValue);
        Serial.print(", Cal: ");
        Serial.println(calValue);
        Serial.print("Pressure - Raw: ");
        Serial.print(rawPressure);
        Serial.print(" bar, Cal: ");
        Serial.print(calPressure);
        Serial.println(" bar");
    }
    
    if (calibrationModule.isUpdated("custom2")) {
        float calLevel = calibrationModule.getCalibratedValue("custom2", "level");
        Serial.print("Custom2 Level - Calibrated: ");
        Serial.print(calLevel);
        Serial.println(" cm");
    }
    
    if (calibrationModule.isUpdated("custom3")) {
        float calForce = calibrationModule.getCalibratedValue("custom3", "force");
        Serial.print("Custom3 Force - Calibrated: ");
        Serial.print(calForce);
        Serial.println(" N");
    }
    
    delay(2000);
}
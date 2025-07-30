#define ENABLE_SENSOR_MODULE_V2
#define ENABLE_SENSOR_CALIBRATION_MODULE_V2
#define ENABLE_INTERACTIVE_SERIAL_GENERAL_SENSOR_CALIBRATOR_V2
#define ENABLE_SENSOR_ALERT_SYSTEM_V2
#define ENABLE_SENSOR_FILTER_V2
#define ENABLE_SENSOR_UTILITY_V2
#define ENABLE_SENSOR_ABSTRACT_V2
#include "Kinematrix.h"

SensorCalibrationModuleV2 calibrationModule;

void setup() {
    Serial.begin(115200);
    Serial.println("AbstractSensV2 with Calibration Example");
    
    calibrationModule.addSensor("abstract1", new AbstractSensV2(3, 10.0, 50.0));
    calibrationModule.addSensor("abstract2", new AbstractSensV2(2, 100.0));
    
    calibrationModule.init();
    
    calibrationModule.addCalibrationEntry("abstract1", "value");
    calibrationModule.addCalibrationEntry("abstract1", "sine");
    calibrationModule.addCalibrationEntry("abstract2", "sine");
    
    calibrationModule.calibrateOnePoint("abstract1", "value", 50.0);
    calibrationModule.calibrateTwoPoint("abstract1", "sine", 0.0, -5.0, 100.0, 105.0);
    calibrationModule.calibrateTwoPoint("abstract2", "sine", 0.0, -5.0, 100.0, 105.0);
    
    Serial.println("Abstract sensors with calibration ready");
}

void loop() {
    calibrationModule.update();
    
    if (calibrationModule.isUpdated("abstract1")) {
        float rawValue = calibrationModule["abstract1"]["value"];
        float calValue = calibrationModule.getCalibratedValue("abstract1", "value");
        float rawSine = calibrationModule["abstract1"]["sine"];
        float calSine = calibrationModule.getCalibratedValue("abstract1", "sine");
        
        Serial.print("Abstract1 - Value Raw: ");
        Serial.print(rawValue);
        Serial.print(", Cal: ");
        Serial.print(calValue);
        Serial.print(" | Sine Raw: ");
        Serial.print(rawSine);
        Serial.print(", Cal: ");
        Serial.println(calSine);
    }
    
    if (calibrationModule.isUpdated("abstract2")) {
        float rawSine = calibrationModule["abstract2"]["sine"];
        float calSine = calibrationModule.getCalibratedValue("abstract2", "sine");
        Serial.print("Abstract2 Sine - Raw: ");
        Serial.print(rawSine);
        Serial.print(", Cal: ");
        Serial.println(calSine);
    }
    
    delay(1500);
}
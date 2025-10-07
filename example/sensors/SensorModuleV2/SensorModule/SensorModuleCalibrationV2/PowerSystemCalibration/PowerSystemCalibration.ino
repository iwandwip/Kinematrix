#define ENABLE_SENSOR_MODULE_V2
#define ENABLE_SENSOR_CALIBRATION_MODULE_V2
#define ENABLE_INTERACTIVE_SERIAL_GENERAL_SENSOR_CALIBRATOR_V2
#define ENABLE_SENSOR_ALERT_SYSTEM_V2
#define ENABLE_SENSOR_FILTER_V2
#define ENABLE_SENSOR_UTILITY_V2
#define ENABLE_SENSOR_INA219_V2
#define ENABLE_SENSOR_ANALOG_V2
#include "Kinematrix.h"

SensorCalibrationModuleV2 calibrationModule;

void setup() {
    Serial.begin(115200);
    Serial.println("Power System Calibration Example");
    
    calibrationModule.addSensor("ina219_1", new INA219SensV2());
    calibrationModule.addSensor("ina219_2", new INA219SensV2(0x41));
    calibrationModule.addSensor("voltage1", new AnalogSensV2(A0, 5.0, 1023));
    calibrationModule.addSensor("voltage2", new AnalogSensV2(A1, 3.3, 1023));
    
    calibrationModule.init();
    
    calibrationModule.addCalibrationEntry("ina219_1", "current");
    calibrationModule.addCalibrationEntry("ina219_1", "voltage");
    calibrationModule.addCalibrationEntry("ina219_2", "current");
    calibrationModule.addCalibrationEntry("voltage1", "volt");
    
    calibrationModule.calibrateOnePoint("ina219_1", "current", 1.0);
    calibrationModule.calibrateOnePoint("ina219_1", "voltage", 12.0);
    calibrationModule.calibrateTwoPoint("ina219_2", "current", 0.5, 0.485, 2.0, 1.995);
    calibrationModule.calibrateOnePoint("voltage1", "volt", 5.0);
    
    Serial.println("Power system calibration ready");
}

void loop() {
    calibrationModule.update();
    
    if (calibrationModule.isUpdated("ina219_1")) {
        float rawCurrent = calibrationModule["ina219_1"]["current"];
        float calCurrent = calibrationModule.getCalibratedValue("ina219_1", "current");
        float rawVoltage = calibrationModule["ina219_1"]["voltage"];
        float calVoltage = calibrationModule.getCalibratedValue("ina219_1", "voltage");
        
        Serial.print("INA219_1 - Current Raw: ");
        Serial.print(rawCurrent);
        Serial.print("A, Cal: ");
        Serial.print(calCurrent);
        Serial.print("A | Voltage Raw: ");
        Serial.print(rawVoltage);
        Serial.print("V, Cal: ");
        Serial.print(calVoltage);
        Serial.println("V");
    }
    
    if (calibrationModule.isUpdated("ina219_2")) {
        float calCurrent2 = calibrationModule.getCalibratedValue("ina219_2", "current");
        Serial.print("INA219_2 Calibrated Current: ");
        Serial.print(calCurrent2);
        Serial.println("A");
    }
    
    delay(1500);
}
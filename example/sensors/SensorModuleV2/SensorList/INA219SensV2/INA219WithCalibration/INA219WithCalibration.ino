#define ENABLE_SENSOR_MODULE_V2
#define ENABLE_SENSOR_CALIBRATION_MODULE_V2
#define ENABLE_INTERACTIVE_SERIAL_GENERAL_SENSOR_CALIBRATOR_V2
#define ENABLE_SENSOR_ALERT_SYSTEM_V2
#define ENABLE_SENSOR_FILTER_V2
#define ENABLE_SENSOR_UTILITY_V2
#define ENABLE_SENSOR_INA219_V2
#include "Kinematrix.h"

SensorCalibrationModuleV2 calibrationModule;

void setup() {
    Serial.begin(115200);
    Serial.println("INA219SensV2 with Calibration Example");
    
    calibrationModule.addSensor("main_power", new INA219SensV2());
    calibrationModule.addSensor("backup_power", new INA219SensV2(0x41));
    calibrationModule.addSensor("load_power", new INA219SensV2(0x44));
    calibrationModule.addSensor("solar_power", new INA219SensV2(0x45));
    
    calibrationModule.init();
    
    calibrationModule.addCalibrationEntry("main_power", "voltage");
    calibrationModule.addCalibrationEntry("main_power", "current");
    calibrationModule.addCalibrationEntry("main_power", "power");
    calibrationModule.addCalibrationEntry("backup_power", "voltage");
    calibrationModule.addCalibrationEntry("backup_power", "current");
    calibrationModule.addCalibrationEntry("load_power", "current");
    calibrationModule.addCalibrationEntry("solar_power", "power");
    
    calibrationModule.calibrateTwoPoint("main_power", "voltage", 12.0, 11.95, 24.0, 24.08);
    calibrationModule.calibrateTwoPoint("main_power", "current", 1.0, 0.998, 5.0, 5.02);
    calibrationModule.calibrateOnePoint("main_power", "power", 60.0);
    calibrationModule.calibrateOnePoint("backup_power", "voltage", 12.0);
    calibrationModule.calibrateOnePoint("backup_power", "current", 2.0);
    calibrationModule.calibrateOnePoint("load_power", "current", 3.0);
    calibrationModule.calibrateOnePoint("solar_power", "power", 100.0);
    
    Serial.println("Power monitoring system with calibration ready");
}

void loop() {
    calibrationModule.update();
    
    if (calibrationModule.isUpdated("main_power")) {
        float rawVoltage = calibrationModule["main_power"]["voltage"];
        float calVoltage = calibrationModule.getCalibratedValue("main_power", "voltage");
        float rawCurrent = calibrationModule["main_power"]["current"];
        float calCurrent = calibrationModule.getCalibratedValue("main_power", "current");
        float rawPower = calibrationModule["main_power"]["power"];
        float calPower = calibrationModule.getCalibratedValue("main_power", "power");
        
        Serial.println("=== Main Power Supply ===");
        Serial.print("Voltage - Raw: ");
        Serial.print(rawVoltage);
        Serial.print("V, Cal: ");
        Serial.print(calVoltage);
        Serial.println("V");
        Serial.print("Current - Raw: ");
        Serial.print(rawCurrent);
        Serial.print("A, Cal: ");
        Serial.print(calCurrent);
        Serial.println("A");
        Serial.print("Power - Raw: ");
        Serial.print(rawPower);
        Serial.print("W, Cal: ");
        Serial.print(calPower);
        Serial.println("W");
    }
    
    if (calibrationModule.isUpdated("backup_power")) {
        float calVoltage = calibrationModule.getCalibratedValue("backup_power", "voltage");
        float calCurrent = calibrationModule.getCalibratedValue("backup_power", "current");
        Serial.print("Backup Power - Voltage: ");
        Serial.print(calVoltage);
        Serial.print("V, Current: ");
        Serial.print(calCurrent);
        Serial.println("A");
    }
    
    if (calibrationModule.isUpdated("load_power")) {
        float calCurrent = calibrationModule.getCalibratedValue("load_power", "current");
        Serial.print("Load Current - Calibrated: ");
        Serial.print(calCurrent);
        Serial.println("A");
    }
    
    if (calibrationModule.isUpdated("solar_power")) {
        float calPower = calibrationModule.getCalibratedValue("solar_power", "power");
        Serial.print("Solar Power - Calibrated: ");
        Serial.print(calPower);
        Serial.println("W");
    }
    
    delay(2000);
}
#define ENABLE_SENSOR_MODULE_V2
#define ENABLE_SENSOR_CALIBRATION_MODULE_V2
#define ENABLE_INTERACTIVE_SERIAL_GENERAL_SENSOR_CALIBRATOR_V2
#define ENABLE_SENSOR_ALERT_SYSTEM_V2
#define ENABLE_SENSOR_FILTER_V2
#define ENABLE_SENSOR_UTILITY_V2
#define ENABLE_SENSOR_MLX90614_V2
#include "Kinematrix.h"

SensorCalibrationModuleV2 calibrationModule;

void setup() {
    Serial.begin(115200);
    Serial.println("MLX90614SensV2 with Calibration Example");
    
    calibrationModule.addSensor("infrared1", new MLX90614SensV2());
    calibrationModule.addSensor("infrared2", new MLX90614SensV2());
    calibrationModule.addSensor("infrared3", new MLX90614SensV2());
    
    calibrationModule.init();
    
    calibrationModule.addCalibrationEntry("infrared1", "objectTemp");
    calibrationModule.addCalibrationEntry("infrared1", "ambientTemp");
    calibrationModule.addCalibrationEntry("infrared2", "objectTemp");
    calibrationModule.addCalibrationEntry("infrared2", "ambientTemp");
    calibrationModule.addCalibrationEntry("infrared3", "objectTemp");
    
    calibrationModule.calibrateTwoPoint("infrared1", "objectTemp", 25.0, 24.8, 100.0, 100.5);
    calibrationModule.calibrateTwoPoint("infrared1", "ambientTemp", 20.0, 19.9, 30.0, 30.2);
    calibrationModule.calibrateTwoPoint("infrared2", "objectTemp", 30.0, 29.7, 80.0, 80.8);
    calibrationModule.calibrateOnePoint("infrared2", "ambientTemp", 25.0);
    calibrationModule.calibrateOnePoint("infrared3", "objectTemp", 37.0);
    
    Serial.println("Infrared temperature sensors with calibration ready");
}

void loop() {
    calibrationModule.update();
    
    if (calibrationModule.isUpdated("infrared1")) {
        float rawObjectTemp = calibrationModule["infrared1"]["objectTemp"];
        float calObjectTemp = calibrationModule.getCalibratedValue("infrared1", "objectTemp");
        float rawAmbientTemp = calibrationModule["infrared1"]["ambientTemp"];
        float calAmbientTemp = calibrationModule.getCalibratedValue("infrared1", "ambientTemp");
        
        Serial.println("=== Infrared Sensor 1 ===");
        Serial.print("Object - Raw: ");
        Serial.print(rawObjectTemp);
        Serial.print("°C, Cal: ");
        Serial.print(calObjectTemp);
        Serial.println("°C");
        Serial.print("Ambient - Raw: ");
        Serial.print(rawAmbientTemp);
        Serial.print("°C, Cal: ");
        Serial.print(calAmbientTemp);
        Serial.println("°C");
    }
    
    if (calibrationModule.isUpdated("infrared2")) {
        float calObjectTemp = calibrationModule.getCalibratedValue("infrared2", "objectTemp");
        float calAmbientTemp = calibrationModule.getCalibratedValue("infrared2", "ambientTemp");
        Serial.print("Infrared2 - Object: ");
        Serial.print(calObjectTemp);
        Serial.print("°C, Ambient: ");
        Serial.print(calAmbientTemp);
        Serial.println("°C");
    }
    
    if (calibrationModule.isUpdated("infrared3")) {
        float calObjectTemp = calibrationModule.getCalibratedValue("infrared3", "objectTemp");
        Serial.print("Infrared3 - Calibrated Object: ");
        Serial.print(calObjectTemp);
        Serial.println("°C");
    }
    
    delay(2000);
}
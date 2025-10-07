#define ENABLE_SENSOR_MODULE_V2
#define ENABLE_SENSOR_CALIBRATION_MODULE_V2
#define ENABLE_INTERACTIVE_SERIAL_GENERAL_SENSOR_CALIBRATOR_V2
#define ENABLE_SENSOR_ALERT_SYSTEM_V2
#define ENABLE_SENSOR_FILTER_V2
#define ENABLE_SENSOR_UTILITY_V2
#define ENABLE_SENSOR_ANALOG_V2
#define ENABLE_SENSOR_MLX90614_V2
#define ENABLE_SENSOR_INA219_V2
#include "Kinematrix.h"

SensorCalibrationModuleV2 calibrationModule;

void setup() {
    Serial.begin(115200);
    Serial.println("Multiple Calibration Types Example");
    
    calibrationModule.addSensor("analog1", new AnalogSensV2(A0, 5.0, 1023));
    calibrationModule.addSensor("analog2", new AnalogSensV2(A1, 3.3, 1023));
    calibrationModule.addSensor("analog3", new AnalogSensV2(A2, 5.0, 1023));
    calibrationModule.addSensor("mlx90614", new MLX90614SensV2());
    calibrationModule.addSensor("ina219", new INA219SensV2());
    
    calibrationModule.init();
    
    calibrationModule.addCalibrationEntry("analog1", "volt");
    calibrationModule.addCalibrationEntry("analog2", "volt");
    calibrationModule.addCalibrationEntry("analog3", "volt");
    calibrationModule.addCalibrationEntry("mlx90614", "objectTemp");
    calibrationModule.addCalibrationEntry("mlx90614", "ambientTemp");
    calibrationModule.addCalibrationEntry("ina219", "power");
    
    calibrationModule.calibrateOnePoint("analog1", "volt", 3.3);
    calibrationModule.calibrateTwoPoint("analog2", "volt", 1.0, 0.95, 4.0, 4.08);
    calibrationModule.calibrateOnePoint("analog3", "volt", 2.5);
    calibrationModule.calibrateTwoPoint("mlx90614", "objectTemp", 25.0, 24.8, 80.0, 80.5);
    calibrationModule.calibrateTwoPoint("mlx90614", "ambientTemp", 25.0, 24.8, 80.0, 80.5);
    calibrationModule.calibrateOnePoint("ina219", "power", 5.0);
    
    Serial.println("Multiple calibration types configured");
}

void loop() {
    calibrationModule.update();
    
    if (calibrationModule.isUpdated("analog1")) {
        Serial.print("A1 Linear - Raw: ");
        Serial.print((float)calibrationModule["analog1"]["volt"]);
        Serial.print("V, Cal: ");
        Serial.print((float)calibrationModule.getCalibratedValue("analog1", "volt"));
        Serial.println("V");
    }
    
    if (calibrationModule.isUpdated("analog2")) {
        Serial.print("A2 TwoPoint - Raw: ");
        Serial.print((float)calibrationModule["analog2"]["volt"]);
        Serial.print("V, Cal: ");
        Serial.print((float)calibrationModule.getCalibratedValue("analog2", "volt"));
        Serial.println("V");
    }
    
    if (calibrationModule.isUpdated("analog3")) {
        Serial.print("A3 Polynomial - Raw: ");
        Serial.print((float)calibrationModule["analog3"]["volt"]);
        Serial.print("V, Cal: ");
        Serial.print((float)calibrationModule.getCalibratedValue("analog3", "volt"));
        Serial.println("V");
    }
    
    if (calibrationModule.isUpdated("mlx90614")) {
        Serial.print("MLX Temp - Raw: ");
        Serial.print((float)calibrationModule["mlx90614"]["objectTemp"]);
        Serial.print("°C, Cal: ");
        Serial.print((float)calibrationModule.getCalibratedValue("mlx90614", "objectTemp"));
        Serial.println("°C");
    }
    
    delay(2000);
}
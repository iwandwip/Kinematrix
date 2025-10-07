#define ENABLE_SENSOR_MODULE_V2
#define ENABLE_SENSOR_CALIBRATION_MODULE_V2
#define ENABLE_INTERACTIVE_SERIAL_GENERAL_SENSOR_CALIBRATOR_V2
#define ENABLE_SENSOR_ALERT_SYSTEM_V2
#define ENABLE_SENSOR_FILTER_V2
#define ENABLE_SENSOR_UTILITY_V2
#define ENABLE_SENSOR_BME680_V2
#include "Kinematrix.h"

SensorCalibrationModuleV2 calibrationModule;

void setup() {
    Serial.begin(115200);
    Serial.println("BME680SensV2 with Calibration Example");
    
    calibrationModule.addSensor("bme680", new BME680SensV2());
    
    calibrationModule.init();
    
    calibrationModule.addCalibrationEntry("bme680", "temperature");
    calibrationModule.addCalibrationEntry("bme680", "humidity");
    calibrationModule.addCalibrationEntry("bme680", "pressure");
    
    calibrationModule.calibrateTwoPoint("bme680", "temperature", 22.0, 21.8, 28.0, 28.2);
    calibrationModule.calibrateOnePoint("bme680", "humidity", 60.0);
    calibrationModule.calibrateOnePoint("bme680", "pressure", 1013.25);
    
    Serial.println("BME680 with calibration ready");
}

void loop() {
    calibrationModule.update();
    
    if (calibrationModule.isUpdated("bme680")) {
        Serial.println("=== BME680 Calibrated ===");
        Serial.print("Temperature: ");
        Serial.print((float)calibrationModule.getCalibratedValue("bme680", "temperature"));
        Serial.println("°C");
        
        Serial.print("Humidity: ");
        Serial.print((float)calibrationModule.getCalibratedValue("bme680", "humidity"));
        Serial.println("%");
        
        Serial.print("Pressure: ");
        Serial.print((float)calibrationModule.getCalibratedValue("bme680", "pressure"));
        Serial.println(" hPa");
        
        Serial.print("Gas Resistance: ");
        Serial.print((float)calibrationModule["bme680"]["gas"]);
        Serial.println(" Ohms");
    }
    
    delay(3000);
}
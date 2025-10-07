#define ENABLE_SENSOR_MODULE_V2
#define ENABLE_SENSOR_CALIBRATION_MODULE_V2
#define ENABLE_INTERACTIVE_SERIAL_GENERAL_SENSOR_CALIBRATOR_V2
#define ENABLE_SENSOR_ALERT_SYSTEM_V2
#define ENABLE_SENSOR_FILTER_V2
#define ENABLE_SENSOR_UTILITY_V2
#define ENABLE_SENSOR_DHT_V2
#define ENABLE_SENSOR_ANALOG_V2
#define ENABLE_SENSOR_BME680_V2
#include "Kinematrix.h"

SensorCalibrationModuleV2 calibrationModule;

void setup() {
    Serial.begin(115200);
    Serial.println("Basic Calibration Example");
    
    calibrationModule.addSensor("dht", new DHTSensV2(2, DHT22));
    calibrationModule.addSensor("analog", new AnalogSensV2(A0, 5.0, 1023));
    calibrationModule.addSensor("bme680", new BME680SensV2());
    
    calibrationModule.init();
    
    calibrationModule.addCalibrationEntry("dht", "temperature");
    calibrationModule.addCalibrationEntry("analog", "volt");
    calibrationModule.addCalibrationEntry("bme680", "humidity");
    
    calibrationModule.calibrateTwoPoint("dht", "temperature", 20.0, 19.8, 25.0, 25.3);
    calibrationModule.calibrateOnePoint("analog", "volt", 3.3);
    calibrationModule.calibrateTwoPoint("bme680", "humidity", 50.0, 48.5, 80.0, 82.1);
    
    Serial.println("Calibration setup complete");
}

void loop() {
    calibrationModule.update();
    
    if (calibrationModule.isUpdated("dht")) {
        float rawTemp = calibrationModule["dht"]["temperature"];
        float calTemp = calibrationModule.getCalibratedValue("dht", "temperature");
        Serial.print("DHT Temp - Raw: ");
        Serial.print(rawTemp);
        Serial.print("°C, Calibrated: ");
        Serial.print(calTemp);
        Serial.println("°C");
    }
    
    if (calibrationModule.isUpdated("analog")) {
        float rawVolt = calibrationModule["analog"]["volt"];
        float calVolt = calibrationModule.getCalibratedValue("analog", "volt");
        Serial.print("Voltage - Raw: ");
        Serial.print(rawVolt);
        Serial.print("V, Calibrated: ");
        Serial.print(calVolt);
        Serial.println("V");
    }
    
    delay(2000);
}
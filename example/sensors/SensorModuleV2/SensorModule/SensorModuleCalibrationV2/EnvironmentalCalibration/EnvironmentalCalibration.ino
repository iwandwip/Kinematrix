#define ENABLE_SENSOR_MODULE_V2
#define ENABLE_SENSOR_CALIBRATION_MODULE_V2
#define ENABLE_INTERACTIVE_SERIAL_GENERAL_SENSOR_CALIBRATOR_V2
#define ENABLE_SENSOR_ALERT_SYSTEM_V2
#define ENABLE_SENSOR_FILTER_V2
#define ENABLE_SENSOR_UTILITY_V2
#define ENABLE_SENSOR_BME680_V2
#define ENABLE_SENSOR_DHT_V2
#define ENABLE_SENSOR_GP2Y_DUST_V2
#define ENABLE_SENSOR_MQ_V2
#include "Kinematrix.h"

SensorCalibrationModuleV2 calibrationModule;

void setup() {
    Serial.begin(115200);
    Serial.println("Environmental Sensor Calibration Example");
    
    calibrationModule.addSensor("bme680", new BME680SensV2());
    calibrationModule.addSensor("dht22", new DHTSensV2(2, DHT22));
    calibrationModule.addSensor("dust", new GP2YDustSensV2(GP2Y1010AU0F, 7, A3));
    calibrationModule.addSensor("mq135", new MQSensV2("Arduino", 5.0, 10, A2, "MQ-135"));
    
    calibrationModule.init();
    
    calibrationModule.addCalibrationEntry("bme680", "temperature");
    calibrationModule.addCalibrationEntry("dht22", "temperature");
    calibrationModule.addCalibrationEntry("bme680", "humidity");
    calibrationModule.addCalibrationEntry("dht22", "humidity");
    calibrationModule.addCalibrationEntry("bme680", "pressure");
    calibrationModule.addCalibrationEntry("dust", "density");
    calibrationModule.addCalibrationEntry("mq135", "co2");
    
    calibrationModule.calibrateTwoPoint("bme680", "temperature", 22.0, 21.8, 28.0, 28.2);
    calibrationModule.calibrateTwoPoint("dht22", "temperature", 22.0, 21.8, 28.0, 28.2);
    calibrationModule.calibrateOnePoint("bme680", "humidity", 60.0);
    calibrationModule.calibrateOnePoint("dht22", "humidity", 60.0);
    calibrationModule.calibrateOnePoint("bme680", "pressure", 1013.25);
    calibrationModule.calibrateOnePoint("dust", "density", 25.0);
    calibrationModule.calibrateTwoPoint("mq135", "co2", 400.0, 385.0, 1000.0, 1020.0);
    
    Serial.println("Environmental calibration setup complete");
}

void loop() {
    calibrationModule.update();
    
    if (calibrationModule.isUpdated("bme680")) {
        Serial.println("=== BME680 ===");
        Serial.print("Temp - Raw: ");
        Serial.print((float)calibrationModule["bme680"]["temperature"]);
        Serial.print("°C, Cal: ");
        Serial.print((float)calibrationModule.getCalibratedValue("bme680", "temperature"));
        Serial.println("°C");
        
        Serial.print("Humidity - Raw: ");
        Serial.print((float)calibrationModule["bme680"]["humidity"]);
        Serial.print("%, Cal: ");
        Serial.print((float)calibrationModule.getCalibratedValue("bme680", "humidity"));
        Serial.println("%");
    }
    
    if (calibrationModule.isUpdated("dust")) {
        Serial.print("Dust - Raw: ");
        Serial.print((float)calibrationModule["dust"]["density"]);
        Serial.print(" μg/m³, Cal: ");
        Serial.print((float)calibrationModule.getCalibratedValue("dust", "density"));
        Serial.println(" μg/m³");
    }
    
    delay(3000);
}
#define ENABLE_SENSOR_MODULE_V2
#define ENABLE_SENSOR_CALIBRATION_MODULE_V2
#define ENABLE_INTERACTIVE_SERIAL_GENERAL_SENSOR_CALIBRATOR_V2
#define ENABLE_SENSOR_ALERT_SYSTEM_V2
#define ENABLE_SENSOR_FILTER_V2
#define ENABLE_SENSOR_UTILITY_V2
#define ENABLE_SENSOR_DHT_V2
#include "Kinematrix.h"

SensorCalibrationModuleV2 calibrationModule;

void setup() {
    Serial.begin(115200);
    Serial.println("DHTSensV2 with Calibration Example");
    
    calibrationModule.addSensor("dht22", new DHTSensV2(2, DHT22));
    calibrationModule.addSensor("dht11", new DHTSensV2(3, DHT11));
    
    calibrationModule.init();
    
    calibrationModule.addCalibrationEntry("dht22", "temperature");
    calibrationModule.addCalibrationEntry("dht11", "temperature");
    calibrationModule.addCalibrationEntry("dht22", "humidity");
    calibrationModule.addCalibrationEntry("dht11", "humidity");
    
    calibrationModule.calibrateTwoPoint("dht22", "temperature", 20.0, 19.8, 30.0, 30.3);
    calibrationModule.calibrateTwoPoint("dht11", "temperature", 20.0, 19.8, 30.0, 30.3);
    calibrationModule.calibrateOnePoint("dht22", "humidity", 60.0);
    calibrationModule.calibrateOnePoint("dht11", "humidity", 60.0);
    
    Serial.println("DHT sensors with calibration ready");
}

void loop() {
    calibrationModule.update();
    
    if (calibrationModule.isUpdated("dht22")) {
        float rawTemp = calibrationModule["dht22"]["temperature"];
        float calTemp = calibrationModule.getCalibratedValue("dht22", "temperature");
        float rawHum = calibrationModule["dht22"]["humidity"];
        float calHum = calibrationModule.getCalibratedValue("dht22", "humidity");
        
        Serial.println("=== DHT22 ===");
        Serial.print("Temp - Raw: ");
        Serial.print(rawTemp);
        Serial.print("°C, Cal: ");
        Serial.print(calTemp);
        Serial.println("°C");
        Serial.print("Humidity - Raw: ");
        Serial.print(rawHum);
        Serial.print("%, Cal: ");
        Serial.print(calHum);
        Serial.println("%");
    }
    
    if (calibrationModule.isUpdated("dht11")) {
        float calTemp = calibrationModule.getCalibratedValue("dht11", "temperature");
        float calHum = calibrationModule.getCalibratedValue("dht11", "humidity");
        
        Serial.print("DHT11 Calibrated - Temp: ");
        Serial.print(calTemp);
        Serial.print("°C, Humidity: ");
        Serial.print(calHum);
        Serial.println("%");
    }
    
    delay(3000);
}
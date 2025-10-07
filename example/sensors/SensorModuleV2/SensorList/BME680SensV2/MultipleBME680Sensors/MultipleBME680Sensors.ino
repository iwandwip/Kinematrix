#define ENABLE_SENSOR_MODULE_V2
#define ENABLE_SENSOR_BME680_V2
#define ENABLE_SENSOR_FILTER_V2
#define ENABLE_SENSOR_ALERT_SYSTEM_V2
#define ENABLE_SENSOR_UTILITY_V2
#include "Kinematrix.h"

SensorModuleV2 sensorModule;

void setup() {
    Serial.begin(115200);
    Serial.println("Multiple BME680SensV2 Example");
    
    BME680SensV2* indoor = new BME680SensV2();
    BME680SensV2* outdoor = new BME680SensV2(0x77);
    
    sensorModule.addSensor("indoor", indoor);
    sensorModule.addSensor("outdoor", outdoor);
    
    sensorModule.init();
    
    FilterParams kalmanParams;
    kalmanParams.kalman.processNoise = 0.1;
    kalmanParams.kalman.measurementNoise = 0.5;
    sensorModule.attachFilter("indoor", "temperature", FILTER_KALMAN, kalmanParams);
    sensorModule.attachFilter("indoor", "humidity", FILTER_KALMAN, kalmanParams);
    
    FilterParams movingAvgParams;
    movingAvgParams.movingAverage.windowSize = 5;
    sensorModule.attachFilter("outdoor", "pressure", FILTER_MOVING_AVERAGE, movingAvgParams);
    sensorModule.attachFilter("outdoor", "gas", FILTER_MOVING_AVERAGE, movingAvgParams);
    
    sensorModule.setThreshold("indoor", "temperature", 20.0, 28.0, ALERT_OUTSIDE);
    sensorModule.setThreshold("indoor", "humidity", 40.0, 70.0, ALERT_OUTSIDE);
    sensorModule.setThreshold("outdoor", "pressure", 950.0, 1050.0, ALERT_OUTSIDE);
    
    Serial.println("Multiple BME680 environmental sensors ready");
}

void loop() {
    sensorModule.update();
    
    if (sensorModule.isUpdated("indoor")) {
        float filteredTemp = sensorModule.getFilteredValue("indoor", "temperature");
        float filteredHum = sensorModule.getFilteredValue("indoor", "humidity");
        bool tempAlert = sensorModule.isAlertActive("indoor", "temperature");
        bool humAlert = sensorModule.isAlertActive("indoor", "humidity");
        
        Serial.println("=== Indoor BME680 ===");
        Serial.print("Temp: ");
        Serial.print(filteredTemp);
        Serial.print("°C (Alert: ");
        Serial.print(tempAlert ? "YES" : "NO");
        Serial.print("), Humidity: ");
        Serial.print(filteredHum);
        Serial.print("% (Alert: ");
        Serial.print(humAlert ? "YES" : "NO");
        Serial.print("), Pressure: ");
        Serial.print((float)sensorModule["indoor"]["pressure"]);
        Serial.print(" hPa, Gas: ");
        Serial.print((float)sensorModule["indoor"]["gas"]);
        Serial.println(" Ohms");
    }
    
    if (sensorModule.isUpdated("outdoor")) {
        float filteredPressure = sensorModule.getFilteredValue("outdoor", "pressure");
        float filteredGas = sensorModule.getFilteredValue("outdoor", "gas");
        bool pressureAlert = sensorModule.isAlertActive("outdoor", "pressure");
        
        Serial.println("=== Outdoor BME680 ===");
        Serial.print("Temp: ");
        Serial.print((float)sensorModule["outdoor"]["temperature"]);
        Serial.print("°C, Humidity: ");
        Serial.print((float)sensorModule["outdoor"]["humidity"]);
        Serial.print("%, Pressure: ");
        Serial.print(filteredPressure);
        Serial.print(" hPa (Alert: ");
        Serial.print(pressureAlert ? "YES" : "NO");
        Serial.print("), Gas: ");
        Serial.print(filteredGas);
        Serial.println(" Ohms");
    }
    
    delay(3000);
}
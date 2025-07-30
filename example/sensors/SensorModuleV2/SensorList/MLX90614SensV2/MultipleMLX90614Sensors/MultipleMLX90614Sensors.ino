#define ENABLE_SENSOR_MODULE_V2
#define ENABLE_SENSOR_MLX90614_V2
#define ENABLE_SENSOR_FILTER_V2
#define ENABLE_SENSOR_ALERT_SYSTEM_V2
#define ENABLE_SENSOR_UTILITY_V2
#include "Kinematrix.h"

SensorModuleV2 sensorModule;

void setup() {
    Serial.begin(115200);
    Serial.println("Multiple MLX90614SensV2 Example");
    
    sensorModule.addSensor("engine_temp", new MLX90614SensV2());
    sensorModule.addSensor("surface_temp", new MLX90614SensV2());
    sensorModule.addSensor("body_temp", new MLX90614SensV2());
    
    sensorModule.init();
    
    FilterParams kalmanParams;
    kalmanParams.kalman.processNoise = 0.1;
    kalmanParams.kalman.measurementNoise = 0.2;
    sensorModule.attachFilter("engine_temp", "objectTemp", FILTER_KALMAN, kalmanParams);
    sensorModule.attachFilter("surface_temp", "objectTemp", FILTER_KALMAN, kalmanParams);
    
    FilterParams movingAvgParams;
    movingAvgParams.movingAverage.windowSize = 8;
    sensorModule.attachFilter("body_temp", "objectTemp", FILTER_MOVING_AVERAGE, movingAvgParams);
    sensorModule.attachFilter("body_temp", "ambientTemp", FILTER_MOVING_AVERAGE, movingAvgParams);
    
    sensorModule.setThreshold("engine_temp", "objectTemp", 0.0, 80.0, ALERT_ABOVE);
    sensorModule.setThreshold("surface_temp", "objectTemp", 0.0, 60.0, ALERT_ABOVE);
    sensorModule.setThreshold("body_temp", "objectTemp", 35.0, 38.5, ALERT_OUTSIDE);
    
    Serial.println("Multiple infrared temperature sensors with filters and alerts ready");
}

void loop() {
    sensorModule.update();
    
    if (sensorModule.isUpdated("engine_temp")) {
        float filteredObjectTemp = sensorModule.getFilteredValue("engine_temp", "objectTemp");
        bool tempAlert = sensorModule.isAlertActive("engine_temp", "objectTemp");
        Serial.print("Engine - Object: ");
        Serial.print(filteredObjectTemp);
        Serial.print("°C, Ambient: ");
        Serial.print((float)sensorModule["engine_temp"]["ambientTemp"]);
        Serial.print("°C (Alert: ");
        Serial.print(tempAlert ? "YES" : "NO");
        Serial.println(")");
    }
    
    if (sensorModule.isUpdated("surface_temp")) {
        float filteredObjectTemp = sensorModule.getFilteredValue("surface_temp", "objectTemp");
        Serial.print("Surface - Object: ");
        Serial.print(filteredObjectTemp);
        Serial.print("°C, Ambient: ");
        Serial.print((float)sensorModule["surface_temp"]["ambientTemp"]);
        Serial.println("°C");
    }
    
    if (sensorModule.isUpdated("body_temp")) {
        float filteredObjectTemp = sensorModule.getFilteredValue("body_temp", "objectTemp");
        float filteredAmbientTemp = sensorModule.getFilteredValue("body_temp", "ambientTemp");
        bool bodyTempAlert = sensorModule.isAlertActive("body_temp", "objectTemp");
        Serial.print("Body - Object: ");
        Serial.print(filteredObjectTemp);
        Serial.print("°C, Ambient: ");
        Serial.print(filteredAmbientTemp);
        Serial.print("°C (Fever Alert: ");
        Serial.print(bodyTempAlert ? "YES" : "NO");
        Serial.println(")");
    }
    
    delay(1500);
}
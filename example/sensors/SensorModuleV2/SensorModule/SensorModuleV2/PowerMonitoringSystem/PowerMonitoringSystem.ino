#define ENABLE_SENSOR_MODULE_V2
#define ENABLE_SENSOR_ALERT_SYSTEM_V2
#define ENABLE_SENSOR_FILTER_V2
#define ENABLE_SENSOR_INA219_V2
#define ENABLE_SENSOR_ANALOG_V2
#define ENABLE_SENSOR_MLX90614_V2
#define ENABLE_SENSOR_UTILITY_V2
#include "Kinematrix.h"

SensorModuleV2 sensorModule;

void setup() {
    Serial.begin(115200);
    Serial.println("Power Monitoring System Example");
    
    sensorModule.addSensor("ina219_1", new INA219SensV2());
    sensorModule.addSensor("ina219_2", new INA219SensV2(0x41));
    sensorModule.addSensor("voltage1", new AnalogSensV2(A0, 5.0, 1023));
    sensorModule.addSensor("voltage2", new AnalogSensV2(A1, 3.3, 1023));
    sensorModule.addSensor("temp", new MLX90614SensV2());
    
    sensorModule.init();
    
    sensorModule.setThreshold("ina219_1", "current", 0.0, 3.0, ALERT_ABOVE);
    sensorModule.setThreshold("ina219_2", "current", 0.0, 3.0, ALERT_ABOVE);
    sensorModule.setThreshold("temp", "objectTemp", 0.0, 80.0, ALERT_ABOVE);
    
    FilterParams movingAvgParams;
    movingAvgParams.movingAverage.windowSize = 10;
    sensorModule.attachFilter("ina219_1", "power", FILTER_MOVING_AVERAGE, movingAvgParams);
    sensorModule.attachFilter("ina219_2", "power", FILTER_MOVING_AVERAGE, movingAvgParams);
    
    Serial.println("Power monitoring system initialized");
}

void loop() {
    sensorModule.update();
    
    if (sensorModule.isUpdated("ina219_1")) {
        float power1 = sensorModule.getFilteredValue("ina219_1", "power");
        float current1 = sensorModule["ina219_1"]["current"];
        Serial.print("INA219_1 - Power: ");
        Serial.print(power1);
        Serial.print("W, Current: ");
        Serial.print(current1);
        Serial.println("A");
    }
    
    if (sensorModule.isUpdated("ina219_2")) {
        float power2 = sensorModule.getFilteredValue("ina219_2", "power");
        float current2 = sensorModule["ina219_2"]["current"];
        Serial.print("INA219_2 - Power: ");
        Serial.print(power2);
        Serial.print("W, Current: ");
        Serial.print(current2);
        Serial.println("A");
    }
    
    if (sensorModule.isUpdated("temp")) {
        Serial.print("Temperature: ");
        Serial.print((float)sensorModule["temp"]["objectTemp"]);
        Serial.println("°C");
    }
    
    delay(1500);
}
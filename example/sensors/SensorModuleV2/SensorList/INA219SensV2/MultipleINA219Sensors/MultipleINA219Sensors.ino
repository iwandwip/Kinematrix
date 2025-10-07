#define ENABLE_SENSOR_MODULE_V2
#define ENABLE_SENSOR_INA219_V2
#define ENABLE_SENSOR_FILTER_V2
#define ENABLE_SENSOR_ALERT_SYSTEM_V2
#define ENABLE_SENSOR_UTILITY_V2
#include "Kinematrix.h"

SensorModuleV2 sensorModule;

void setup() {
    Serial.begin(115200);
    Serial.println("Multiple INA219SensV2 Example");
    
    sensorModule.addSensor("ina219_1", new INA219SensV2());
    sensorModule.addSensor("ina219_2", new INA219SensV2(0x41));
    sensorModule.addSensor("ina219_3", new INA219SensV2(0x44));
    
    sensorModule.init();
    
    FilterParams movingAvgParams;
    movingAvgParams.movingAverage.windowSize = 10;
    sensorModule.attachFilter("ina219_1", "power", FILTER_MOVING_AVERAGE, movingAvgParams);
    sensorModule.attachFilter("ina219_2", "power", FILTER_MOVING_AVERAGE, movingAvgParams);
    
    sensorModule.setThreshold("ina219_1", "current", 0.0, 3.0, ALERT_ABOVE);
    sensorModule.setThreshold("ina219_2", "current", 0.0, 3.0, ALERT_ABOVE);
    
    Serial.println("Multiple INA219 power monitors ready");
}

void loop() {
    sensorModule.update();
    
    if (sensorModule.isUpdated("ina219_1")) {
        float filteredPower = sensorModule.getFilteredValue("ina219_1", "power");
        bool currentAlert = sensorModule.isAlertActive("ina219_1", "current");
        
        Serial.print("INA219_1 - V: ");
        Serial.print((float)sensorModule["ina219_1"]["voltage"]);
        Serial.print("V, I: ");
        Serial.print((float)sensorModule["ina219_1"]["current"]);
        Serial.print("A, P: ");
        Serial.print(filteredPower);
        Serial.print("W (Alert: ");
        Serial.print(currentAlert ? "YES" : "NO");
        Serial.println(")");
    }
    
    if (sensorModule.isUpdated("ina219_2")) {
        float filteredPower = sensorModule.getFilteredValue("ina219_2", "power");
        Serial.print("INA219_2 - Power: ");
        Serial.print(filteredPower);
        Serial.println("W");
    }
    
    if (sensorModule.isUpdated("ina219_3")) {
        Serial.print("INA219_3 - Current: ");
        Serial.print((float)sensorModule["ina219_3"]["current"]);
        Serial.println("A");
    }
    
    delay(1500);
}
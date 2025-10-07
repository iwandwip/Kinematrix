#define ENABLE_SENSOR_MODULE_V2
#define ENABLE_SENSOR_ALERT_SYSTEM_V2
#define ENABLE_SENSOR_DHT_V2
#define ENABLE_SENSOR_ANALOG_V2
#define ENABLE_SENSOR_INA219_V2
#define ENABLE_SENSOR_UTILITY_V2
#include "Kinematrix.h"

SensorModuleV2 sensorModule;

void setup() {
    Serial.begin(115200);
    Serial.println("Alert System Example");
    
    sensorModule.addSensor("tempSensor", new DHTSensV2(2, DHT22));
    sensorModule.addSensor("voltage", new AnalogSensV2(A0, 5.0, 1023));
    sensorModule.addSensor("ina219", new INA219SensV2());
    
    sensorModule.init();
    
    sensorModule.setGlobalAlertCallback([](AlertInfo alertInfo) {
        Serial.print("ALERT: ");
        Serial.print(alertInfo.sensorName);
        Serial.print(".");
        Serial.print(alertInfo.valueKey);
        Serial.print(" = ");
        Serial.print(alertInfo.currentValue);
        Serial.print(" State: ");
        Serial.println(alertInfo.state == ALERT_ACTIVE ? "ACTIVE" : alertInfo.state == ALERT_ACKNOWLEDGED ? "ACKNOWLEDGED" : "INACTIVE");
    });
    
    sensorModule.setThreshold("tempSensor", "temperature", 20.0, 30.0, ALERT_OUTSIDE);
    sensorModule.setThreshold("voltage", "volt", 1.0, 4.0, ALERT_OUTSIDE);
    sensorModule.setThreshold("ina219", "current", 0.1, 2.0, ALERT_ABOVE);
    
    Serial.println("Alert thresholds configured");
}

void loop() {
    sensorModule.update();
    
    if (sensorModule.isUpdated("tempSensor")) {
        Serial.print("Temp: ");
        Serial.print((float)sensorModule["tempSensor"]["temperature"]);
        Serial.print("°C Alert: ");
        Serial.println(sensorModule.isAlertActive("tempSensor", "temperature") ? "YES" : "NO");
    }
    
    delay(1000);
}
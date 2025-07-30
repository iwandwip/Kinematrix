#define ENABLE_SENSOR_MODULE_V2
#define ENABLE_SENSOR_CUSTOM_TEMPLATE_V2
#define ENABLE_SENSOR_FILTER_V2
#define ENABLE_SENSOR_ALERT_SYSTEM_V2
#define ENABLE_SENSOR_UTILITY_V2
#include "Kinematrix.h"

SensorModuleV2 sensorModule;

void setup() {
    Serial.begin(115200);
    Serial.println("Multiple CustomSensorTemplateV2 Example");
    
    CustomSensorTemplateV2* pressureSensor = new CustomSensorTemplateV2(A0);
    pressureSensor->setCalibrationFactor(1.5);
    pressureSensor->setSensorMode(0);
    pressureSensor->addCustomSensorValue("pressure", "Pressure", "bar", 2, true);
    
    CustomSensorTemplateV2* tempSensor = new CustomSensorTemplateV2(A1);
    tempSensor->setCalibrationFactor(0.1);
    tempSensor->setSensorMode(1);
    tempSensor->addCustomSensorValue("temperature", "Temperature", "°C", 1, true);
    
    CustomSensorTemplateV2* flowSensor = new CustomSensorTemplateV2(A2);
    flowSensor->setCalibrationFactor(3.0);
    flowSensor->setSensorMode(0);
    flowSensor->addCustomSensorValue("flow", "Flow Rate", "L/min", 2, true);
    
    sensorModule.addSensor("pressure", pressureSensor);
    sensorModule.addSensor("temperature", tempSensor);
    sensorModule.addSensor("flow", flowSensor);
    
    sensorModule.init();
    
    FilterParams movingAvgParams;
    movingAvgParams.movingAverage.windowSize = 5;
    sensorModule.attachFilter("pressure", "value", FILTER_MOVING_AVERAGE, movingAvgParams);
    sensorModule.attachFilter("temperature", "value", FILTER_MOVING_AVERAGE, movingAvgParams);
    
    sensorModule.setThreshold("pressure", "value", 1.0, 10.0, ALERT_OUTSIDE);
    sensorModule.setThreshold("temperature", "value", 20.0, 50.0, ALERT_OUTSIDE);
    
    Serial.println("Multiple custom sensors with filters and alerts ready");
}

void loop() {
    sensorModule.update();
    
    if (sensorModule.isUpdated("pressure")) {
        float filteredPressure = sensorModule.getFilteredValue("pressure", "value");
        bool pressureAlert = sensorModule.isAlertActive("pressure", "value");
        Serial.print("Pressure - Raw: ");
        Serial.print((float)sensorModule["pressure"]["rawValue"]);
        Serial.print(", Filtered: ");
        Serial.print(filteredPressure);
        Serial.print(" bar (Alert: ");
        Serial.print(pressureAlert ? "YES" : "NO");
        Serial.println(")");
    }
    
    if (sensorModule.isUpdated("temperature")) {
        float filteredTemp = sensorModule.getFilteredValue("temperature", "value");
        Serial.print("Temperature - Filtered: ");
        Serial.print(filteredTemp);
        Serial.println("°C");
    }
    
    if (sensorModule.isUpdated("flow")) {
        Serial.print("Flow Rate - Value: ");
        Serial.print((float)sensorModule["flow"]["value"]);
        Serial.println(" L/min");
    }
    
    delay(1000);
}
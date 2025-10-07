#define ENABLE_SENSOR_MODULE_V2
#define ENABLE_SENSOR_CUSTOM_TEMPLATE_V2
#define ENABLE_SENSOR_UTILITY_V2
#include "Kinematrix.h"

CustomSensorTemplateV2 customSensor(A0);

void setup() {
    Serial.begin(115200);
    Serial.println("Basic CustomSensorTemplateV2 Example");
    
    JsonDocument doc;
    customSensor.setDocumentValue(&doc);
    customSensor.setDocument("custom");
    
    customSensor.setCalibrationFactor(2.0);
    customSensor.setSensorMode(1);
    customSensor.setUpdateInterval(500);
    
    customSensor.addCustomSensorValue("pressure", "Pressure", "kPa", 2, true);
    customSensor.addCustomSensorValue("flow", "Flow Rate", "L/min", 1, true);
    
    if (customSensor.init()) {
        Serial.println("Custom sensor initialized successfully");
    } else {
        Serial.println("Custom sensor initialization failed!");
    }
}

void loop() {
    if (customSensor.update()) {
        Serial.print("Raw Value: ");
        Serial.print((int)customSensor.getIntValue("rawValue"));
        Serial.print(", Processed Value: ");
        Serial.print((float)customSensor.getFloatValue("value"));
        Serial.print(", Pressure: ");
        Serial.print((float)customSensor.getFloatValue("pressure"));
        Serial.print(" kPa, Flow: ");
        Serial.print((float)customSensor.getFloatValue("flow"));
        Serial.println(" L/min");
    }
    
    delay(100);
}
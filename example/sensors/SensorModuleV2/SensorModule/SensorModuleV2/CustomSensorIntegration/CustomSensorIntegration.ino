#define ENABLE_SENSOR_MODULE_V2
#define ENABLE_SENSOR_ANALOG_V2
#define ENABLE_SENSOR_CUSTOM_TEMPLATE_V2
#define ENABLE_SENSOR_ABSTRACT_V2
#define ENABLE_SENSOR_UTILITY_V2
#include "Kinematrix.h"

SensorModuleV2 sensorModule;

void setup() {
    Serial.begin(115200);
    Serial.println("Custom Sensor Integration Example");
    
    AnalogSensV2* analogSensor = new AnalogSensV2(A0, 5.0, 1023, [](BaseSensV2* sensor, int rawValue, float voltageValue) {
        sensor->updateValue("custom1", rawValue * 2.5);
        sensor->updateValue("custom2", voltageValue * 1.2);
    });
    analogSensor->addCustomValue("custom1", "Custom Value 1", "units", 2, true);
    analogSensor->addCustomValue("custom2", "Custom Value 2", "V", 3, true);
    
    CustomSensorTemplateV2* customSensor = new CustomSensorTemplateV2(A1);
    customSensor->setCalibrationFactor(2.0);
    customSensor->setSensorMode(1);
    customSensor->addCustomSensorValue("pressure", "Pressure", "kPa", 2, true);
    
    sensorModule.addSensor("analogCustom", analogSensor);
    sensorModule.addSensor("customTemplate", customSensor);
    sensorModule.addSensor("abstract1", new AbstractSensV2(3, 10.0, 50.0));
    sensorModule.addSensor("abstract2", new AbstractSensV2(1, 25.5));
    
    sensorModule.init();
    
    Serial.println("Custom sensors initialized");
}

void loop() {
    sensorModule.update();
    
    if (sensorModule.isUpdated("analogCustom")) {
        Serial.print("Analog Custom - Raw: ");
        Serial.print((float)sensorModule["analogCustom"]["raw"]);
        Serial.print(", Custom1: ");
        Serial.print((float)sensorModule["analogCustom"]["custom1"]);
        Serial.print(", Custom2: ");
        Serial.println((float)sensorModule["analogCustom"]["custom2"]);
    }
    
    if (sensorModule.isUpdated("customTemplate")) {
        Serial.print("Custom Template - Value: ");
        Serial.println((float)sensorModule["customTemplate"]["value"]);
    }
    
    if (sensorModule.isUpdated("abstract1")) {
        Serial.print("Abstract1 - Value: ");
        Serial.print((float)sensorModule["abstract1"]["value"]);
        Serial.print(", Sine: ");
        Serial.println((float)sensorModule["abstract1"]["sine"]);
    }
    
    delay(1000);
}
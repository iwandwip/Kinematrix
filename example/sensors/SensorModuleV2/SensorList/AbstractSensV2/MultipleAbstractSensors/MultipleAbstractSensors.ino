#define ENABLE_SENSOR_MODULE_V2
#define ENABLE_SENSOR_ABSTRACT_V2
#define ENABLE_SENSOR_UTILITY_V2
#include "Kinematrix.h"

SensorModuleV2 sensorModule;

void setup() {
    Serial.begin(115200);
    Serial.println("Multiple AbstractSensV2 Example");
    
    sensorModule.addSensor("random", new AbstractSensV2(0));
    sensorModule.addSensor("fixed", new AbstractSensV2(1, 42.5));
    sensorModule.addSensor("range1", new AbstractSensV2(2, 100.0));
    sensorModule.addSensor("range2", new AbstractSensV2(3, 20.0, 80.0));
    
    sensorModule.init();
    
    AbstractSensV2* randomSensor = (AbstractSensV2*)sensorModule.getSensorByName("random");
    if (randomSensor) randomSensor->setUpdateInterval(500);
    
    AbstractSensV2* fixedSensor = (AbstractSensV2*)sensorModule.getSensorByName("fixed");
    if (fixedSensor) fixedSensor->setUpdateInterval(1000);
    
    Serial.println("Multiple abstract sensors with different modes initialized");
}

void loop() {
    sensorModule.update();
    
    if (sensorModule.isUpdated("random")) {
        Serial.print("Random Mode - Value: ");
        Serial.print((float)sensorModule["random"]["value"]);
        Serial.print(", Sine: ");
        Serial.println((float)sensorModule["random"]["sine"]);
    }
    
    if (sensorModule.isUpdated("fixed")) {
        Serial.print("Fixed Mode - Value: ");
        Serial.print((float)sensorModule["fixed"]["value"]);
        Serial.print(", Triangle: ");
        Serial.println((float)sensorModule["fixed"]["triangle"]);
    }
    
    if (sensorModule.isUpdated("range1")) {
        Serial.print("Range1 (0-100) - Value: ");
        Serial.println((float)sensorModule["range1"]["value"]);
    }
    
    if (sensorModule.isUpdated("range2")) {
        Serial.print("Range2 (20-80) - Value: ");
        Serial.println((float)sensorModule["range2"]["value"]);
    }
    
    delay(200);
}
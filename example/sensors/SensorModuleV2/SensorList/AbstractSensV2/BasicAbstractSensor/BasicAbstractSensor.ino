#define ENABLE_SENSOR_MODULE_V2
#define ENABLE_SENSOR_ABSTRACT_V2
#define ENABLE_SENSOR_UTILITY_V2
#include "Kinematrix.h"

AbstractSensV2 abstractSensor(0);

void setup() {
    Serial.begin(115200);
    Serial.println("Basic AbstractSensV2 Example");
    
    JsonDocument doc;
    abstractSensor.setDocumentValue(&doc);
    abstractSensor.setDocument("abstract");
    
    abstractSensor.init();
    abstractSensor.setUpdateInterval(1000);
    
    Serial.println("Abstract sensor initialized with random mode");
}

void loop() {
    if (abstractSensor.update()) {
        Serial.print("Value: ");
        Serial.print((float)abstractSensor.getFloatValue("value"));
        Serial.print(", Random: ");
        Serial.print((float)abstractSensor.getFloatValue("random"));
        Serial.print(", Sine: ");
        Serial.print((float)abstractSensor.getFloatValue("sine"));
        Serial.print(", Counter: ");
        Serial.println((int)abstractSensor.getIntValue("counter"));
    }
    
    delay(100);
}
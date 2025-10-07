#define ENABLE_SENSOR_MODULE_V2
#define ENABLE_SENSOR_ANALOG_V2
#define ENABLE_SENSOR_UTILITY_V2
#include "Kinematrix.h"

AnalogSensV2 analogSensor(A0, 5.0, 1023);

void setup() {
    Serial.begin(115200);
    Serial.println("Basic AnalogSensV2 Example");
    
    JsonDocument doc;
    analogSensor.setDocumentValue(&doc);
    analogSensor.setDocument("analog");
    
    analogSensor.init();
    Serial.println("Analog sensor initialized");
}

void loop() {
    if (analogSensor.update()) {
        Serial.print("Raw Value: ");
        Serial.print((int)analogSensor.getIntValue("raw"));
        Serial.print(", Voltage: ");
        Serial.print((float)analogSensor.getFloatValue("volt"));
        Serial.println("V");
    }
    
    delay(500);
}
#define ENABLE_SENSOR_MODULE_V2
#define ENABLE_SENSOR_INA219_V2
#define ENABLE_SENSOR_UTILITY_V2
#include "Kinematrix.h"

INA219SensV2 inaSensor;

void setup() {
    Serial.begin(115200);
    Serial.println("Basic INA219SensV2 Example");
    
    JsonDocument doc;
    inaSensor.setDocumentValue(&doc);
    inaSensor.setDocument("ina219");
    
    if (inaSensor.init()) {
        Serial.println("INA219 sensor initialized successfully");
    } else {
        Serial.println("INA219 sensor initialization failed!");
    }
}

void loop() {
    if (inaSensor.update()) {
        Serial.print("Voltage: ");
        Serial.print((float)inaSensor.getFloatValue("voltage"));
        Serial.print("V, Current: ");
        Serial.print((float)inaSensor.getFloatValue("current"));
        Serial.print("A, Power: ");
        Serial.print((float)inaSensor.getFloatValue("power"));
        Serial.println("W");
    }
    
    delay(1000);
}
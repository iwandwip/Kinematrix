#define ENABLE_SENSOR_MODULE_V2
#define ENABLE_SENSOR_MQ_V2
#define ENABLE_SENSOR_UTILITY_V2
#include "Kinematrix.h"

MQSensV2 mqSensor("Arduino", 5.0, 10, A0, "MQ-135");

void setup() {
    Serial.begin(115200);
    Serial.println("Basic MQSensV2 Example");
    
    JsonDocument doc;
    mqSensor.setDocumentValue(&doc);
    mqSensor.setDocument("mq135");
    
    if (mqSensor.init()) {
        Serial.println("MQ-135 sensor initialized successfully");
        Serial.println("Warming up... please wait 30 seconds");
        delay(30000);
    } else {
        Serial.println("MQ-135 sensor initialization failed!");
    }
}

void loop() {
    if (mqSensor.update()) {
        Serial.print("CO2: ");
        Serial.print((float)mqSensor.getFloatValue("co2"));
        Serial.print(" ppm, NH3: ");
        Serial.print((float)mqSensor.getFloatValue("nh3"));
        Serial.print(" ppm, Raw: ");
        Serial.println((int)mqSensor.getIntValue("raw"));
    }
    
    delay(2000);
}
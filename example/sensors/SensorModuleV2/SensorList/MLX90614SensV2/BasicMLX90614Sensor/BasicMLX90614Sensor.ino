#define ENABLE_SENSOR_MODULE_V2
#define ENABLE_SENSOR_MLX90614_V2
#define ENABLE_SENSOR_UTILITY_V2
#include "Kinematrix.h"

MLX90614SensV2 tempSensor;

void setup() {
    Serial.begin(115200);
    Serial.println("Basic MLX90614SensV2 Example");
    
    JsonDocument doc;
    tempSensor.setDocumentValue(&doc);
    tempSensor.setDocument("mlx90614");
    
    tempSensor.setUpdateInterval(500);
    
    if (tempSensor.init()) {
        Serial.println("MLX90614 infrared temperature sensor initialized successfully");
    } else {
        Serial.println("MLX90614 sensor initialization failed!");
    }
}

void loop() {
    if (tempSensor.update()) {
        Serial.print("Object Temperature: ");
        Serial.print((float)tempSensor.getFloatValue("objectTemp"));
        Serial.print("°C, Ambient Temperature: ");
        Serial.print((float)tempSensor.getFloatValue("ambientTemp"));
        Serial.println("°C");
    }
    
    delay(100);
}
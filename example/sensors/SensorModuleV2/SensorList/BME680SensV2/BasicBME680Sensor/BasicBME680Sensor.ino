#define ENABLE_SENSOR_MODULE_V2
#define ENABLE_SENSOR_BME680_V2
#define ENABLE_SENSOR_UTILITY_V2
#include "Kinematrix.h"

BME680SensV2 bme680Sensor;

void setup() {
    Serial.begin(115200);
    Serial.println("Basic BME680SensV2 Example");
    
    JsonDocument doc;
    bme680Sensor.setDocumentValue(&doc);
    bme680Sensor.setDocument("bme680");
    
    if (bme680Sensor.init()) {
        Serial.println("BME680 sensor initialized successfully");
    } else {
        Serial.println("BME680 sensor initialization failed!");
    }
}

void loop() {
    if (bme680Sensor.update()) {
        Serial.print("Temperature: ");
        Serial.print((float)bme680Sensor.getFloatValue("temperature"));
        Serial.print("°C, Humidity: ");
        Serial.print((float)bme680Sensor.getFloatValue("humidity"));
        Serial.print("%, Pressure: ");
        Serial.print((float)bme680Sensor.getFloatValue("pressure"));
        Serial.print(" hPa, Gas: ");
        Serial.print((float)bme680Sensor.getFloatValue("gas"));
        Serial.println(" Ohms");
    }
    
    delay(2000);
}
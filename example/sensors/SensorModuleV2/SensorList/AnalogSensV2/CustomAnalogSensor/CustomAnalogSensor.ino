#define ENABLE_SENSOR_MODULE_V2
#define ENABLE_SENSOR_ANALOG_V2
#define ENABLE_SENSOR_UTILITY_V2
#include "Kinematrix.h"

AnalogSensV2 customSensor(A0, 5.0, 1023, [](BaseSensV2* sensor, int rawValue, float voltageValue) {
    float pressure = voltageValue * 25.0;
    float temperature = (rawValue - 100) * 0.1;
    
    sensor->updateValue("pressure", pressure);
    sensor->updateValue("temperature", temperature);
});

void setup() {
    Serial.begin(115200);
    Serial.println("Custom AnalogSensV2 with Callback Example");
    
    customSensor.addCustomValue("pressure", "Pressure", "kPa", 2, true);
    customSensor.addCustomValue("temperature", "Temperature", "°C", 1, true);
    
    JsonDocument doc;
    customSensor.setDocumentValue(&doc);
    customSensor.setDocument("custom");
    
    customSensor.init();
    Serial.println("Custom analog sensor initialized");
}

void loop() {
    if (customSensor.update()) {
        Serial.print("Raw: ");
        Serial.print((int)customSensor.getIntValue("raw"));
        Serial.print(", Voltage: ");
        Serial.print((float)customSensor.getFloatValue("volt"));
        Serial.print("V, Pressure: ");
        Serial.print((float)customSensor.getFloatValue("pressure"));
        Serial.print("kPa, Temp: ");
        Serial.print((float)customSensor.getFloatValue("temperature"));
        Serial.println("°C");
    }
    
    delay(1000);
}
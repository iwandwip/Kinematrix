#define ENABLE_SENSOR_MODULE_V2
#define ENABLE_SENSOR_DHT_V2
#define ENABLE_SENSOR_ANALOG_V2
#define ENABLE_SENSOR_ABSTRACT_V2
#define ENABLE_SENSOR_UTILITY_V2
#include "Kinematrix.h"

SensorModuleV2 sensorModule;

void setup() {
    Serial.begin(115200);
    Serial.println("Basic SensorModuleV2 Example");
    
    sensorModule.addSensor("tempSensor", new DHTSensV2(2, DHT22));
    sensorModule.addSensor("analog", new AnalogSensV2(A0, 5.0, 1023));
    sensorModule.addSensor("abstract", new AbstractSensV2(0));
    
    sensorModule.init();
}

void loop() {
    sensorModule.update();
    
    if (sensorModule.isUpdated("tempSensor")) {
        Serial.print("Temperature: ");
        Serial.println((float)sensorModule["tempSensor"]["temperature"]);
    }
    
    if (sensorModule.isUpdated("analog")) {
        Serial.print("Voltage: ");
        Serial.println((float)sensorModule["analog"]["volt"]);
    }
    
    delay(1000);
}
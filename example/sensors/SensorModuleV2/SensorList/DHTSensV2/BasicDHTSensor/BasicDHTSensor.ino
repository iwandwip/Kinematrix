#define ENABLE_SENSOR_MODULE_V2
#define ENABLE_SENSOR_DHT_V2
#define ENABLE_SENSOR_UTILITY_V2
#include "Kinematrix.h"

DHTSensV2 dhtSensor(2, DHT22);

void setup() {
    Serial.begin(115200);
    Serial.println("Basic DHTSensV2 Example");
    
    JsonDocument doc;
    dhtSensor.setDocumentValue(&doc);
    dhtSensor.setDocument("dht");
    
    if (dhtSensor.init()) {
        Serial.println("DHT sensor initialized successfully");
    } else {
        Serial.println("DHT sensor initialization failed!");
    }
}

void loop() {
    if (dhtSensor.update()) {
        Serial.print("Temperature: ");
        Serial.print((float)dhtSensor.getFloatValue("temperature"));
        Serial.print("°C, Humidity: ");
        Serial.print((float)dhtSensor.getFloatValue("humidity"));
        Serial.print("%, Heat Index: ");
        Serial.print((float)dhtSensor.getFloatValue("heatIndex"));
        Serial.println("°C");
    }
    
    delay(2000);
}
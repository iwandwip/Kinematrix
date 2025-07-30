#define ENABLE_SENSOR_MODULE_V2
#define ENABLE_SENSOR_DHT_V2
#define ENABLE_SENSOR_FILTER_V2
#define ENABLE_SENSOR_ALERT_SYSTEM_V2
#define ENABLE_SENSOR_UTILITY_V2
#include "Kinematrix.h"

SensorModuleV2 sensorModule;

void setup() {
    Serial.begin(115200);
    Serial.println("Multiple DHTSensV2 Example");
    
    sensorModule.addSensor("dht22_1", new DHTSensV2(2, DHT22));
    sensorModule.addSensor("dht22_2", new DHTSensV2(3, DHT22));
    sensorModule.addSensor("dht11", new DHTSensV2(4, DHT11));
    
    sensorModule.init();
    
    FilterParams kalmanParams;
    kalmanParams.kalman.processNoise = 0.01;
    kalmanParams.kalman.measurementNoise = 0.1;
    sensorModule.attachFilter("dht22_1", "temperature", FILTER_KALMAN, kalmanParams);
    sensorModule.attachFilter("dht22_1", "humidity", FILTER_KALMAN, kalmanParams);
    
    sensorModule.setThreshold("dht22_1", "temperature", 20.0, 30.0, ALERT_OUTSIDE);
    sensorModule.setThreshold("dht22_2", "humidity", 40.0, 80.0, ALERT_OUTSIDE);
    
    Serial.println("Multiple DHT sensors with filters and alerts ready");
}

void loop() {
    sensorModule.update();
    
    if (sensorModule.isUpdated("dht22_1")) {
        float filteredTemp = sensorModule.getFilteredValue("dht22_1", "temperature");
        float filteredHum = sensorModule.getFilteredValue("dht22_1", "humidity");
        bool tempAlert = sensorModule.isAlertActive("dht22_1", "temperature");
        
        Serial.print("DHT22_1 - Temp: ");
        Serial.print(filteredTemp);
        Serial.print("°C (Alert: ");
        Serial.print(tempAlert ? "YES" : "NO");
        Serial.print("), Humidity: ");
        Serial.print(filteredHum);
        Serial.println("%");
    }
    
    if (sensorModule.isUpdated("dht22_2")) {
        Serial.print("DHT22_2 - Temp: ");
        Serial.print((float)sensorModule["dht22_2"]["temperature"]);
        Serial.print("°C, Humidity: ");
        Serial.print((float)sensorModule["dht22_2"]["humidity"]);
        Serial.println("%");
    }
    
    if (sensorModule.isUpdated("dht11")) {
        Serial.print("DHT11 - Temp: ");
        Serial.print((float)sensorModule["dht11"]["temperature"]);
        Serial.print("°C, Humidity: ");
        Serial.print((float)sensorModule["dht11"]["humidity"]);
        Serial.println("%");
    }
    
    delay(3000);
}
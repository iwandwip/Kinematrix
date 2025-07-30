#define ENABLE_SENSOR_MODULE_V2
#define ENABLE_SENSOR_FILTER_V2
#define ENABLE_SENSOR_DHT_V2
#define ENABLE_SENSOR_ANALOG_V2
#define ENABLE_SENSOR_BME680_V2
#define ENABLE_SENSOR_MQ_V2
#define ENABLE_SENSOR_UTILITY_V2
#include "Kinematrix.h"

SensorModuleV2 sensorModule;

void setup() {
    Serial.begin(115200);
    Serial.println("Multi Sensor with Filters Example");
    
    sensorModule.addSensor("dht", new DHTSensV2(2, DHT22));
    sensorModule.addSensor("analog1", new AnalogSensV2(A0, 5.0, 1023));
    sensorModule.addSensor("analog2", new AnalogSensV2(A1, 3.3, 1023));
    sensorModule.addSensor("bme680", new BME680SensV2());
    sensorModule.addSensor("mq135", new MQSensV2("Arduino", 5.0, 10, A2, "MQ-135"));
    
    sensorModule.init();
    
    FilterParams movingAvgParams;
    movingAvgParams.movingAverage.windowSize = 5;
    sensorModule.attachFilter("analog1", "volt", FILTER_MOVING_AVERAGE, movingAvgParams);
    
    FilterParams kalmanParams;
    kalmanParams.kalman.processNoise = 0.01;
    kalmanParams.kalman.measurementNoise = 0.1;
    sensorModule.attachFilter("dht", "temperature", FILTER_KALMAN, kalmanParams);
    
    Serial.println("Filters attached to sensors");
}

void loop() {
    sensorModule.update();
    
    if (sensorModule.isUpdated("dht")) {
        float temp = sensorModule.getFilteredValue("dht", "temperature");
        Serial.print("Filtered Temperature: ");
        Serial.println(temp);
    }
    
    if (sensorModule.isUpdated("analog1")) {
        float volt = sensorModule.getFilteredValue("analog1", "volt");
        Serial.print("Filtered Voltage: ");
        Serial.println(volt);
    }
    
    delay(2000);
}
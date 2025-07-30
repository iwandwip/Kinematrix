#define ENABLE_SENSOR_MODULE_V2
#define ENABLE_SENSOR_MQ_V2
#define ENABLE_SENSOR_FILTER_V2
#define ENABLE_SENSOR_ALERT_SYSTEM_V2
#define ENABLE_SENSOR_UTILITY_V2
#include "Kinematrix.h"

SensorModuleV2 sensorModule;

void setup() {
    Serial.begin(115200);
    Serial.println("Multiple MQSensV2 Example");
    
    sensorModule.addSensor("mq2", new MQSensV2("Arduino", 5.0, 10, A0, "MQ-2"));
    sensorModule.addSensor("mq135", new MQSensV2("Arduino", 5.0, 10, A1, "MQ-135"));
    sensorModule.addSensor("mq7", new MQSensV2("Arduino", 5.0, 10, A2, "MQ-7"));
    
    sensorModule.init();
    
    sensorModule.setThreshold("mq2", "lpg", 0.0, 1000.0, ALERT_ABOVE);
    sensorModule.setThreshold("mq135", "co2", 0.0, 1000.0, ALERT_ABOVE);
    sensorModule.setThreshold("mq7", "co", 0.0, 50.0, ALERT_ABOVE);
    
    Serial.println("Multiple MQ gas sensors with alerts ready");
    Serial.println("Warming up sensors... please wait 30 seconds");
    delay(30000);
}

void loop() {
    sensorModule.update();
    
    if (sensorModule.isUpdated("mq2")) {
        bool lpgAlert = sensorModule.isAlertActive("mq2", "lpg");
        Serial.print("MQ-2 - LPG: ");
        Serial.print((float)sensorModule["mq2"]["lpg"]);
        Serial.print(" ppm (Alert: ");
        Serial.print(lpgAlert ? "YES" : "NO");
        Serial.print("), Smoke: ");
        Serial.print((float)sensorModule["mq2"]["smoke"]);
        Serial.println(" ppm");
    }
    
    if (sensorModule.isUpdated("mq135")) {
        Serial.print("MQ-135 - CO2: ");
        Serial.print((float)sensorModule["mq135"]["co2"]);
        Serial.print(" ppm, NH3: ");
        Serial.print((float)sensorModule["mq135"]["nh3"]);
        Serial.println(" ppm");
    }
    
    if (sensorModule.isUpdated("mq7")) {
        Serial.print("MQ-7 - CO: ");
        Serial.print((float)sensorModule["mq7"]["co"]);
        Serial.println(" ppm");
    }
    
    delay(3000);
}
#define ENABLE_SENSOR_MODULE_V2
#define ENABLE_SENSOR_GP2Y_DUST_V2
#define ENABLE_SENSOR_FILTER_V2
#define ENABLE_SENSOR_ALERT_SYSTEM_V2
#define ENABLE_SENSOR_UTILITY_V2
#include "Kinematrix.h"

SensorModuleV2 sensorModule;

void setup() {
    Serial.begin(115200);
    Serial.println("Multiple GP2YDustSensV2 Example");
    
    sensorModule.addSensor("dust1", new GP2YDustSensV2(GP2Y1010AU0F, 7, A0));
    sensorModule.addSensor("dust2", new GP2YDustSensV2(GP2Y1010AU0F, 8, A1));
    sensorModule.addSensor("dust3", new GP2YDustSensV2(GP2Y1014AU0F, 9, A2));
    
    sensorModule.init();
    
    FilterParams movingAvgParams;
    movingAvgParams.movingAverage.windowSize = 10;
    sensorModule.attachFilter("dust1", "density", FILTER_MOVING_AVERAGE, movingAvgParams);
    sensorModule.attachFilter("dust2", "density", FILTER_MOVING_AVERAGE, movingAvgParams);
    
    FilterParams kalmanParams;
    kalmanParams.kalman.processNoise = 0.1;
    kalmanParams.kalman.measurementNoise = 0.5;
    sensorModule.attachFilter("dust3", "density", FILTER_KALMAN, kalmanParams);
    
    sensorModule.setThreshold("dust1", "density", 0.0, 50.0, ALERT_ABOVE);
    sensorModule.setThreshold("dust2", "density", 0.0, 75.0, ALERT_ABOVE);
    sensorModule.setThreshold("dust3", "density", 0.0, 100.0, ALERT_ABOVE);
    
    Serial.println("Multiple dust sensors with filters and alerts ready");
    Serial.println("Warming up sensors... please wait 30 seconds");
    delay(30000);
    Serial.println("All sensors ready for measurements");
}

void loop() {
    sensorModule.update();
    
    if (sensorModule.isUpdated("dust1")) {
        float filteredDensity = sensorModule.getFilteredValue("dust1", "density");
        bool dustAlert = sensorModule.isAlertActive("dust1", "density");
        Serial.print("Dust1 - Raw: ");
        Serial.print((float)sensorModule["dust1"]["density"]);
        Serial.print(" μg/m³, Filtered: ");
        Serial.print(filteredDensity);
        Serial.print(" μg/m³ (Alert: ");
        Serial.print(dustAlert ? "YES" : "NO");
        Serial.println(")");
    }
    
    if (sensorModule.isUpdated("dust2")) {
        float filteredDensity = sensorModule.getFilteredValue("dust2", "density");
        Serial.print("Dust2 - Filtered: ");
        Serial.print(filteredDensity);
        Serial.println(" μg/m³");
    }
    
    if (sensorModule.isUpdated("dust3")) {
        float filteredDensity = sensorModule.getFilteredValue("dust3", "density");
        Serial.print("Dust3 - Kalman Filtered: ");
        Serial.print(filteredDensity);
        Serial.println(" μg/m³");
    }
    
    delay(2000);
}
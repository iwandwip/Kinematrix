#define ENABLE_SENSOR_MODULE_V2
#define ENABLE_SENSOR_ANALOG_V2
#define ENABLE_SENSOR_FILTER_V2
#define ENABLE_SENSOR_ALERT_SYSTEM_V2
#define ENABLE_SENSOR_UTILITY_V2
#include "Kinematrix.h"

SensorModuleV2 sensorModule;

void setup() {
    Serial.begin(115200);
    Serial.println("Multiple AnalogSensV2 with SensorModuleV2 Example");
    
    sensorModule.addSensor("analog1", new AnalogSensV2(A0, 5.0, 1023));
    sensorModule.addSensor("analog2", new AnalogSensV2(A1, 3.3, 1023));
    sensorModule.addSensor("analog3", new AnalogSensV2(A2, 5.0, 4095));
    sensorModule.addSensor("analog4", new AnalogSensV2(A3, 3.3, 4095));
    
    sensorModule.init();
    
    FilterParams movingAvgParams;
    movingAvgParams.movingAverage.windowSize = 5;
    sensorModule.attachFilter("analog1", "volt", FILTER_MOVING_AVERAGE, movingAvgParams);
    sensorModule.attachFilter("analog2", "volt", FILTER_MOVING_AVERAGE, movingAvgParams);
    
    sensorModule.setThreshold("analog1", "volt", 1.0, 4.0, ALERT_OUTSIDE);
    sensorModule.setThreshold("analog2", "volt", 0.5, 2.8, ALERT_OUTSIDE);
    
    Serial.println("Multiple analog sensors initialized with filters and alerts");
}

void loop() {
    sensorModule.update();
    
    if (sensorModule.isUpdated("analog1")) {
        float filteredVolt = sensorModule.getFilteredValue("analog1", "volt");
        bool alertActive = sensorModule.isAlertActive("analog1", "volt");
        Serial.print("A1 - Raw: ");
        Serial.print((float)sensorModule["analog1"]["raw"]);
        Serial.print(", Filtered: ");
        Serial.print(filteredVolt);
        Serial.print("V, Alert: ");
        Serial.println(alertActive ? "YES" : "NO");
    }
    
    if (sensorModule.isUpdated("analog2")) {
        float filteredVolt = sensorModule.getFilteredValue("analog2", "volt");
        Serial.print("A2 - Filtered: ");
        Serial.print(filteredVolt);
        Serial.println("V");
    }
    
    if (sensorModule.isUpdated("analog3")) {
        Serial.print("A3 - Voltage: ");
        Serial.print((float)sensorModule["analog3"]["volt"]);
        Serial.println("V");
    }
    
    delay(800);
}
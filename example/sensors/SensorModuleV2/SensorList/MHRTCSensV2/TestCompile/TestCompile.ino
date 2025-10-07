#define ENABLE_SENSOR_MODULE_V2
#define ENABLE_SENSOR_MHRTC_V2
#include "Kinematrix.h"

SensorModuleV2 sensorModule;

void setup() {
    Serial.begin(115200);
    Serial.println("MHRTCSensV2 Compilation Test");
    
    sensorModule.addSensor("rtc", new MHRTCSensV2(4, 5, 6));
    sensorModule.init();
    
    Serial.println("Compilation successful!");
}

void loop() {
    sensorModule.update();
    
    if (sensorModule.isUpdated("rtc")) {
        int year = sensorModule.getValue<int>("rtc", "year");
        int month = sensorModule.getValue<int>("rtc", "month");
        int day = sensorModule.getValue<int>("rtc", "day");
        
        Serial.print("Date: ");
        Serial.print(year);
        Serial.print("/");
        Serial.print(month);
        Serial.print("/");
        Serial.println(day);
    }
    
    delay(1000);
}
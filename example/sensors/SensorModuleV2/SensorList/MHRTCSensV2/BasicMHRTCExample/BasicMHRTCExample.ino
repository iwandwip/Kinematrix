#define ENABLE_SENSOR_MODULE_V2
#define ENABLE_SENSOR_MHRTC_V2
#define ENABLE_SENSOR_UTILITY_V2
#include "Kinematrix.h"

SensorModuleV2 sensorModule;

void setup() {
    Serial.begin(115200);
    Serial.println("MHRTCSensV2 Basic Example");
    Serial.println("DS1302 RTC Module with SensorModuleV2");
    
    sensorModule.addSensor("rtc", new MHRTCSensV2(4, 5, 6));
    
    sensorModule.init();
    
    MHRTCSensV2* rtcSensor = (MHRTCSensV2*)sensorModule.getSensorByName("rtc");
    if (rtcSensor) {
        rtcSensor->setDateTime(24, 6, 22, 10, 30, 0, 7);
        rtcSensor->startClock();
        Serial.println("RTC initialized and time set");
    }
}

void loop() {
    sensorModule.update();
    
    if (sensorModule.isUpdated("rtc")) {
        Serial.println("=== RTC Data ===");
        Serial.print("Date: ");
        Serial.println(sensorModule.getValue<const char*>("rtc", "dateString"));
        Serial.print("Time: ");
        Serial.println(sensorModule.getValue<const char*>("rtc", "timeString"));
        Serial.print("DateTime: ");
        Serial.println(sensorModule.getValue<const char*>("rtc", "dateTimeString"));
        Serial.print("Timestamp: ");
        Serial.println(sensorModule.getValue<int>("rtc", "timestamp"));
        Serial.print("Running: ");
        Serial.println(sensorModule.getValue<int>("rtc", "isRunning") ? "Yes" : "No");
        Serial.println();
    }
    
    delay(1000);
}
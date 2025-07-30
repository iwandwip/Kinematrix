#define ENABLE_SENSOR_MODULE_V2
#define ENABLE_SENSOR_RTC_V2
#define ENABLE_SENSOR_RTC_DS3231_V2
#define ENABLE_SENSOR_UTILITY_V2
#include "Kinematrix.h"

RTCDS3231SensV2 rtcSensor;

void setup() {
    Serial.begin(115200);
    Serial.println("Basic RTC DS3231 SensV2 Example");
    
    JsonDocument doc;
    rtcSensor.setDocumentValue(&doc);
    rtcSensor.setDocument("rtc");
    
    rtcSensor.enableTemperature(true);
    
    if (rtcSensor.init()) {
        Serial.println("RTC DS3231 sensor initialized successfully");
    } else {
        Serial.println("RTC DS3231 sensor initialization failed!");
    }
}

void loop() {
    if (rtcSensor.update()) {
        Serial.print("Date: ");
        Serial.print((int)rtcSensor.getIntValue("day"));
        Serial.print("/");
        Serial.print((int)rtcSensor.getIntValue("month"));
        Serial.print("/");
        Serial.print((int)rtcSensor.getIntValue("year"));
        
        Serial.print(" Time: ");
        Serial.print((int)rtcSensor.getIntValue("hour"));
        Serial.print(":");
        Serial.print((int)rtcSensor.getIntValue("minute"));
        Serial.print(":");
        Serial.print((int)rtcSensor.getIntValue("second"));
        
        Serial.print(" Temp: ");
        Serial.print((float)rtcSensor.getFloatValue("temperature"));
        Serial.println("°C");
    }
    
    delay(1000);
}
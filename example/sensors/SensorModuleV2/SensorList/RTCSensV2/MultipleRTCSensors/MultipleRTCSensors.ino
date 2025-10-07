#define ENABLE_SENSOR_MODULE_V2
#define ENABLE_SENSOR_RTC_V2
#define ENABLE_SENSOR_RTC_DS3231_V2
#define ENABLE_SENSOR_RTC_DS1307_V2
#define ENABLE_SENSOR_RTC_PCF8523_V2
#define ENABLE_SENSOR_FILTER_V2
#define ENABLE_SENSOR_ALERT_SYSTEM_V2
#define ENABLE_SENSOR_UTILITY_V2
#include "Kinematrix.h"

SensorModuleV2 sensorModule;

void setup() {
    Serial.begin(115200);
    Serial.println("Multiple RTC Sensors Example");
    
    RTCDS3231SensV2* ds3231 = new RTCDS3231SensV2();
    ds3231->enableTemperature(true);
    sensorModule.addSensor("ds3231", ds3231);
    
    sensorModule.addSensor("ds1307", new RTCDS1307SensV2());
    sensorModule.addSensor("pcf8523", new RTCPCF8523SensV2());
    
    sensorModule.init();
    
    Serial.println("Multiple RTC sensors initialized");
}

void loop() {
    sensorModule.update();
    
    if (sensorModule.isUpdated("ds3231")) {
        Serial.print("DS3231 - ");
        Serial.print((int)sensorModule["ds3231"]["hour"]);
        Serial.print(":");
        Serial.print((int)sensorModule["ds3231"]["minute"]);
        Serial.print(":");
        Serial.print((float)sensorModule["ds3231"]["second"]);
        Serial.print(" Temp: ");
        Serial.print((float)sensorModule["ds3231"]["temperature"]);
        Serial.println("°C");
    }
    
    if (sensorModule.isUpdated("ds1307")) {
        Serial.print("DS1307 - ");
        Serial.print((int)sensorModule["ds1307"]["hour"]);
        Serial.print(":");
        Serial.print((int)sensorModule["ds1307"]["minute"]);
        Serial.print(":");
        Serial.println((int)sensorModule["ds1307"]["second"]);
    }
    
    if (sensorModule.isUpdated("pcf8523")) {
        Serial.print("PCF8523 - ");
        Serial.print((float)sensorModule["pcf8523"]["day"]);
        Serial.print("/");
        Serial.print((float)sensorModule["pcf8523"]["month"]);
        Serial.print("/");
        Serial.println((float)sensorModule["pcf8523"]["year"]);
    }
    
    delay(2000);
}
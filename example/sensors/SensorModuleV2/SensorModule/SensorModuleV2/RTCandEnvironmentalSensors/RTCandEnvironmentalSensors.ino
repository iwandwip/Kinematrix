#define ENABLE_SENSOR_MODULE_V2
#define ENABLE_SENSOR_RTC_DS3231_V2
#define ENABLE_SENSOR_BME680_V2
#define ENABLE_SENSOR_GP2Y_DUST_V2
#define ENABLE_SENSOR_MQ_V2
#define ENABLE_SENSOR_UTILITY_V2
#include "Kinematrix.h"

SensorModuleV2 sensorModule;

void setup() {
    Serial.begin(115200);
    Serial.println("RTC and Environmental Sensors Example");
    
    sensorModule.addSensor("rtc", new RTCDS3231SensV2());
    sensorModule.addSensor("bme680", new BME680SensV2());
    sensorModule.addSensor("dust", new GP2YDustSensV2(GP2Y1010AU0F, 7, A3));
    sensorModule.addSensor("mq2", new MQSensV2("Arduino", 5.0, 10, A2, "MQ-2"));
    
    sensorModule.init();
    
    RTCDS3231SensV2* rtc = (RTCDS3231SensV2*)sensorModule.getSensorByName("rtc");
    if (rtc) {
        rtc->enableTemperature(true);
    }
    
    Serial.println("RTC and environmental monitoring started");
}

void loop() {
    sensorModule.update();
    
    if (sensorModule.isUpdated("rtc")) {
        Serial.print("Time: ");
        Serial.print((int)sensorModule["rtc"]["hour"]);
        Serial.print(":");
        Serial.print((int)sensorModule["rtc"]["minute"]);
        Serial.print(":");
        Serial.println((int)sensorModule["rtc"]["second"]);
    }
    
    if (sensorModule.isUpdated("bme680")) {
        Serial.print("BME680 - Temp: ");
        Serial.print((float)sensorModule["bme680"]["temperature"]);
        Serial.print("°C, Humidity: ");
        Serial.print((float)sensorModule["bme680"]["humidity"]);
        Serial.println("%");
    }
    
    if (sensorModule.isUpdated("dust")) {
        Serial.print("Dust: ");
        Serial.print((float)sensorModule["dust"]["density"]);
        Serial.println(" μg/m³");
    }
    
    delay(2000);
}
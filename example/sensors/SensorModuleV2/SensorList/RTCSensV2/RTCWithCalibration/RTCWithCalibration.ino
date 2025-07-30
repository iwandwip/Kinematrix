#define ENABLE_SENSOR_MODULE_V2
#define ENABLE_SENSOR_CALIBRATION_MODULE_V2
#define ENABLE_INTERACTIVE_SERIAL_GENERAL_SENSOR_CALIBRATOR_V2
#define ENABLE_SENSOR_ALERT_SYSTEM_V2
#define ENABLE_SENSOR_FILTER_V2
#define ENABLE_SENSOR_UTILITY_V2
#define ENABLE_SENSOR_RTC_V2
#define ENABLE_SENSOR_RTC_DS3231_V2
#define ENABLE_SENSOR_RTC_DS1307_V2
#include "Kinematrix.h"

SensorCalibrationModuleV2 calibrationModule;

void setup() {
    Serial.begin(115200);
    Serial.println("RTC SensV2 with Calibration Example");
    
    RTCDS3231SensV2* ds3231 = new RTCDS3231SensV2();
    ds3231->enableTemperature(true);
    calibrationModule.addSensor("rtc_main", ds3231);
    
    calibrationModule.addSensor("rtc_backup", new RTCDS1307SensV2());
    
    calibrationModule.init();
    
    calibrationModule.addCalibrationEntry("rtc_main", "temperature");
    calibrationModule.addCalibrationEntry("rtc_main", "unixTime");
    calibrationModule.addCalibrationEntry("rtc_backup", "unixTime");
    
    calibrationModule.calibrateTwoPoint("rtc_main", "temperature", 25.0, 24.8, 30.0, 30.3);
    calibrationModule.calibrateOnePoint("rtc_main", "unixTime", 1640995200);
    calibrationModule.calibrateOnePoint("rtc_backup", "unixTime", 1640995200);
    
    Serial.println("RTC sensors with calibration ready");
}

void loop() {
    calibrationModule.update();
    
    if (calibrationModule.isUpdated("rtc_main")) {
        Serial.println("=== Main RTC DS3231 ===");
        Serial.print("Date: ");
        Serial.print((float)calibrationModule["rtc_main"]["day"]);
        Serial.print("/");
        Serial.print((float)calibrationModule["rtc_main"]["month"]);
        Serial.print("/");
        Serial.print((float)calibrationModule["rtc_main"]["year"]);
        
        Serial.print(" Time: ");
        Serial.print((int)calibrationModule["rtc_main"]["hour"]);
        Serial.print(":");
        Serial.print((int)calibrationModule["rtc_main"]["minute"]);
        Serial.print(":");
        Serial.println((int)calibrationModule["rtc_main"]["second"]);
        
        float rawTemp = calibrationModule["rtc_main"]["temperature"];
        float calTemp = calibrationModule.getCalibratedValue("rtc_main", "temperature");
        Serial.print("Temperature - Raw: ");
        Serial.print(rawTemp);
        Serial.print("°C, Cal: ");
        Serial.print(calTemp);
        Serial.println("°C");
        
        uint32_t rawUnixTime = (uint32_t)calibrationModule["rtc_main"]["unixTime"];
        uint32_t calUnixTime = calibrationModule.getCalibratedValue("rtc_main", "unixTime");
        Serial.print("Unix Time - Raw: ");
        Serial.print(rawUnixTime);
        Serial.print(", Cal: ");
        Serial.println(calUnixTime);
    }
    
    if (calibrationModule.isUpdated("rtc_backup")) {
        Serial.println("=== Backup RTC DS1307 ===");
        Serial.print("Time: ");
        Serial.print((int)calibrationModule["rtc_backup"]["hour"]);
        Serial.print(":");
        Serial.print((int)calibrationModule["rtc_backup"]["minute"]);
        Serial.print(":");
        Serial.println((int)calibrationModule["rtc_backup"]["second"]);
        
        uint32_t calUnixTime = calibrationModule.getCalibratedValue("rtc_backup", "unixTime");
        Serial.print("Calibrated Unix Time: ");
        Serial.println(calUnixTime);
    }
    
    delay(5000);
}
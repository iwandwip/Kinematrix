#define ENABLE_SENSOR_MODULE_V2
#define ENABLE_SENSOR_MHRTC_V2
#define ENABLE_SENSOR_ALERT_SYSTEM_V2
#define ENABLE_SENSOR_UTILITY_V2
#include "Kinematrix.h"

SensorModuleV2 sensorModule;

void printCommands();
void handleSerialCommands();

void alertCallback(AlertInfo alertInfo) {
    if (alertInfo.type == ALERT_OUTSIDE) {
        Serial.print("ALERT: Outside office hours! Current hour: ");
        Serial.println((int)alertInfo.currentValue);
    }
}

void setup() {
    Serial.begin(115200);
    Serial.println("MHRTCSensV2 Advanced Example");
    Serial.println("DS1302 RTC with Alerts and Commands");
    
    sensorModule.addSensor("rtc", new MHRTCSensV2(4, 5, 6));
    
    sensorModule.init();
    
    MHRTCSensV2* rtcSensor = (MHRTCSensV2*)sensorModule.getSensorByName("rtc");
    if (rtcSensor) {
        rtcSensor->setDateTime(24, 6, 22, 8, 0, 0, 7);
        rtcSensor->startClock();
    }
    
    sensorModule.setThreshold("rtc", "hour", 9, 17, ALERT_OUTSIDE);
    sensorModule.setGlobalAlertCallback(alertCallback);
    
    printCommands();
}

void loop() {
    sensorModule.update();
    
    if (sensorModule.isUpdated("rtc")) {
        MHRTCSensV2* rtcSensor = (MHRTCSensV2*)sensorModule.getSensorByName("rtc");
        Ds1302::DateTime dt = rtcSensor->getDateTime();
        
        Serial.print(rtcSensor->getFormattedDateTime("YYYY-MM-DD HH:mm:ss"));
        Serial.print(" | ");
        Serial.print(rtcSensor->getDayOfWeekName(dt.dow));
        Serial.print(" | ");
        Serial.print(rtcSensor->getMonthName(dt.month));
        Serial.print(" | Unix: ");
        Serial.println(rtcSensor->getUnixTimestamp());
    }
    
    handleSerialCommands();
    
    delay(1000);
}

void handleSerialCommands() {
    if (Serial.available()) {
        String command = Serial.readStringUntil('\n');
        command.trim();
        
        MHRTCSensV2* rtcSensor = (MHRTCSensV2*)sensorModule.getSensorByName("rtc");
        if (!rtcSensor) return;
        
        if (command == "help") {
            printCommands();
        } else if (command == "status") {
            Serial.println("=== RTC Status ===");
            sensorModule.debug("rtc", "year");
            Serial.print("Clock Running: ");
            Serial.println(rtcSensor->isClockRunning() ? "Yes" : "No");
        } else if (command == "start") {
            rtcSensor->startClock();
            Serial.println("Clock started");
        } else if (command == "stop") {
            rtcSensor->stopClock();
            Serial.println("Clock stopped");
        } else if (command.startsWith("set ")) {
            String params = command.substring(4);
            int year, month, day, hour, minute, second;
            if (sscanf(params.c_str(), "%d/%d/%d %d:%d:%d", &year, &month, &day, &hour, &minute, &second) == 6) {
                if (rtcSensor->setDateTime(year - 2000, month, day, hour, minute, second)) {
                    Serial.println("Time set successfully");
                } else {
                    Serial.println("Invalid time format");
                }
            } else {
                Serial.println("Format: set YYYY/MM/DD HH:MM:SS");
            }
        } else if (command == "format1") {
            Serial.println("Custom format 1: " + rtcSensor->getFormattedDateTime("DD/MM/YYYY HH:mm"));
        } else if (command == "format2") {
            Serial.println("Custom format 2: " + rtcSensor->getFormattedDateTime("YYYY-MM-DD"));
        } else if (command == "format3") {
            Serial.println("Custom format 3: " + rtcSensor->getFormattedDateTime("HH:mm:ss"));
        } else {
            Serial.println("Unknown command. Type 'help' for commands.");
        }
    }
}

void printCommands() {
    Serial.println("\n=== Available Commands ===");
    Serial.println("help      - Show this help");
    Serial.println("status    - Show RTC status");
    Serial.println("start     - Start RTC clock");
    Serial.println("stop      - Stop RTC clock");
    Serial.println("set       - Set time (Format: set YYYY/MM/DD HH:MM:SS)");
    Serial.println("format1   - Show custom date format 1");
    Serial.println("format2   - Show custom date format 2");
    Serial.println("format3   - Show custom time format");
    Serial.println("==========================\n");
}
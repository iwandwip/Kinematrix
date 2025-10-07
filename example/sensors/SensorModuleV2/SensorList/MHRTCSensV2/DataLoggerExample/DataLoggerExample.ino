#define ENABLE_SENSOR_MODULE_V2
#define ENABLE_SENSOR_MHRTC_V2
#define ENABLE_SENSOR_ANALOG_V2
#define ENABLE_SENSOR_DHT_V2
#define ENABLE_SENSOR_FILTER_V2
#define ENABLE_SENSOR_UTILITY_V2
#include "Kinematrix.h"

SensorModuleV2 sensorModule;

void logData();

void setup() {
    Serial.begin(115200);
    Serial.println("MHRTCSensV2 Data Logger Example");
    Serial.println("RTC with sensors for timestamped data logging");
    
    sensorModule.addSensor("rtc", new MHRTCSensV2(4, 5, 6));
    sensorModule.addSensor("temp", new DHTSensV2(2, DHT22));
    sensorModule.addSensor("light", new AnalogSensV2(A0, 5.0, 1023));
    
    sensorModule.init();
    
    MHRTCSensV2* rtcSensor = (MHRTCSensV2*)sensorModule.getSensorByName("rtc");
    if (rtcSensor) {
        rtcSensor->setDateTime(24, 6, 22, 10, 0, 0, 7);
        rtcSensor->startClock();
        Serial.println("RTC initialized");
    }
    
    FilterParams tempFilter;
    tempFilter.movingAverage.windowSize = 5;
    sensorModule.attachFilter("temp", "temperature", FILTER_MOVING_AVERAGE, tempFilter);
    
    FilterParams lightFilter;
    lightFilter.exponential.alpha = 0.2f;
    sensorModule.attachFilter("light", "volt", FILTER_EXPONENTIAL, lightFilter);
    
    Serial.println("CSV Header:");
    Serial.println("Timestamp,DateTime,Temperature,Humidity,Light_Voltage");
}

void loop() {
    sensorModule.update();
    
    static unsigned long lastLog = 0;
    if (millis() - lastLog >= 5000) {
        logData();
        lastLog = millis();
    }
    
    if (Serial.available()) {
        String command = Serial.readStringUntil('\n');
        command.trim();
        
        if (command == "debug") {
            Serial.println("=== Debug Information ===");
            sensorModule.debugAll(true);
        } else if (command == "time") {
            MHRTCSensV2* rtcSensor = (MHRTCSensV2*)sensorModule.getSensorByName("rtc");
            if (rtcSensor) {
                Serial.print("Current time: ");
                Serial.println(rtcSensor->getFormattedDateTime("YYYY-MM-DD HH:mm:ss"));
            }
        } else if (command == "header") {
            Serial.println("Timestamp,DateTime,Temperature,Humidity,Light_Voltage");
        }
    }
    
    delay(100);
}

void logData() {
    if (sensorModule.getSensorByName("rtc")->isUpdated() || 
        sensorModule.getSensorByName("temp")->isUpdated() || 
        sensorModule.getSensorByName("light")->isUpdated()) {
        
        uint32_t timestamp = sensorModule.getValue<int>("rtc", "timestamp");
        const char* dateTime = sensorModule.getValue<const char*>("rtc", "dateTimeString");
        
        float temperature = 0.0;
        float humidity = 0.0;
        if (sensorModule.hasFilter("temp", "temperature")) {
            temperature = sensorModule.getFilteredValue("temp", "temperature");
            humidity = sensorModule.getValue<float>("temp", "humidity");
        } else {
            temperature = sensorModule.getValue<float>("temp", "temperature");
            humidity = sensorModule.getValue<float>("temp", "humidity");
        }
        
        float lightVolt = 0.0;
        if (sensorModule.hasFilter("light", "volt")) {
            lightVolt = sensorModule.getFilteredValue("light", "volt");
        } else {
            lightVolt = sensorModule.getValue<float>("light", "volt");
        }
        
        Serial.print(timestamp);
        Serial.print(",");
        Serial.print(dateTime);
        Serial.print(",");
        Serial.print(temperature, 2);
        Serial.print(",");
        Serial.print(humidity, 2);
        Serial.print(",");
        Serial.println(lightVolt, 3);
    }
}
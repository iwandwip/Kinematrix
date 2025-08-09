# CeriaSensorDS3231

## Overview
CeriaSensorDS3231 provides a comprehensive wrapper for the DS3231 Real-Time Clock (RTC) module using the Adafruit RTClib library. This wrapper offers simplified access to DS3231 features including time management, temperature monitoring, alarms, and square wave generation.

## Features

### Core Functionality
- **High-Precision Timekeeping**: DS3231 with temperature-compensated crystal oscillator (TCXO)
- **Temperature Monitoring**: Built-in temperature sensor with ±3°C accuracy
- **Power Loss Detection**: Automatic detection of power failures
- **Battery Backup**: Maintains accurate time during power outages
- **I2C Communication**: Standard I2C interface (address 0x68)

### Advanced Features
- **Dual Alarms**: Two independent alarm systems with various trigger modes
- **Square Wave Generation**: Configurable output frequencies (1Hz, 1kHz, 4kHz, 8kHz)
- **Time Formatting**: Multiple string formats (12/24 hour, date formats)
- **Data Validation**: Input validation for date/time settings
- **Automatic Updates**: Configurable update intervals
- **Leap Year Handling**: Accurate leap year calculations

## Usage Examples

### Basic Usage
```cpp
#include "CeriaSensorDS3231.h"
using namespace CeriaDevOP;

CeriaSensorDS3231 rtc;

void setup() {
    Serial.begin(115200);
    Wire.begin();
    
    if (!rtc.begin()) {
        Serial.println("RTC not found!");
        while (1) delay(10);
    }
    
    // Check if power was lost
    if (rtc.isPowerLost()) {
        Serial.println("RTC lost power, setting compile time...");
        rtc.setToCompileTime();
    }
}

void loop() {
    rtc.update();
    
    if (rtc.hasNewData()) {
        Serial.println("Current time: " + rtc.getDateTimeString());
        Serial.println("Temperature: " + String(rtc.getTemperature(), 1) + "°C");
        Serial.println("Day: " + rtc.getDayName());
        Serial.println(rtc.getStatusString());
    }
    
    delay(1000);
}
```

### Advanced Time Management
```cpp
CeriaSensorDS3231 rtc;

void setup() {
    Serial.begin(115200);
    rtc.begin();
    
    // Set specific date and time
    rtc.setDateTime(2024, 1, 15, 14, 30, 0);  // Jan 15, 2024, 2:30:00 PM
    
    // Or set from string format
    rtc.setDateTime("2024-01-15 14:30:00");
    
    // Or set from Unix timestamp
    rtc.setDateTime(1705329000);  // Unix timestamp
    
    // Configure update interval
    rtc.setUpdateInterval(500);  // Update every 500ms
}

void loop() {
    rtc.update();
    
    if (rtc.hasNewData()) {
        // Get detailed time information
        DS3231TimeData timeData = rtc.getTime();
        
        Serial.println("=== Time Information ===");
        Serial.println("Date: " + rtc.getDateString());           // "2024-01-15"
        Serial.println("Time: " + rtc.getTimeString());           // "14:30:00"
        Serial.println("12h Time: " + rtc.getTimeString12Hour()); // "02:30:00 PM"
        Serial.println("Full: " + rtc.getRTCString());             // "2024-01-15 14:30:00 (Monday)"
        Serial.println("Unix: " + String(rtc.getUnixTime()));
        
        // Individual components
        Serial.println("Year: " + String(rtc.getYear()));
        Serial.println("Month: " + String(rtc.getMonth()) + " (" + rtc.getMonthName() + ")");
        Serial.println("Day: " + String(rtc.getDay()));
        Serial.println("Hour: " + String(rtc.getHour()));
        Serial.println("Minute: " + String(rtc.getMinute()));
        Serial.println("Second: " + String(rtc.getSecond()));
        Serial.println("Day of Week: " + rtc.getDayName());
    }
    
    delay(100);
}
```

### Alarm Configuration
```cpp
CeriaSensorDS3231 rtc;

void setup() {
    Serial.begin(115200);
    rtc.begin();
    
    // Set Alarm 1 - trigger at specific time daily
    rtc.setAlarm1(7, 30, 0, DS3231_A1_Hour);  // 7:30:00 AM daily
    
    // Set Alarm 2 - trigger every hour at 15 minutes past
    rtc.setAlarm2(0, 15, DS3231_A2_Minute);   // Every hour at :15
    
    // Configure square wave output for external interrupt
    rtc.setSqwMode(DS3231_SQW_1HZ);  // 1Hz square wave
    
    Serial.println("Alarms configured!");
    Serial.println("Alarm 1: Daily at 7:30 AM");
    Serial.println("Alarm 2: Hourly at :15");
}

void loop() {
    rtc.update();
    
    // Check for alarm triggers
    if (rtc.isAlarmTriggered(DS3231_ALARM_1)) {
        Serial.println("⏰ ALARM 1 TRIGGERED! Time for wake up!");
        rtc.clearAlarm(DS3231_ALARM_1);
    }
    
    if (rtc.isAlarmTriggered(DS3231_ALARM_2)) {
        Serial.println("⏰ ALARM 2 TRIGGERED! Hourly reminder!");
        rtc.clearAlarm(DS3231_ALARM_2);
    }
    
    if (rtc.hasNewData()) {
        Serial.println(rtc.getRTCString());
        
        // Show temperature every minute
        if (rtc.getSecond() == 0) {
            Serial.println("Temperature: " + String(rtc.getTemperature(), 2) + "°C");
        }
    }
    
    delay(100);
}
```

### Status Monitoring and Diagnostics
```cpp
void loop() {
    rtc.update();
    
    if (rtc.hasNewData()) {
        // Get comprehensive status
        DS3231Status status = rtc.getStatus();
        
        Serial.println("=== RTC Status ===");
        Serial.println("Power Lost: " + String(status.powerLost ? "YES" : "NO"));
        Serial.println("Oscillator: " + String(status.oscillatorRunning ? "Running" : "Stopped"));
        Serial.println("Temperature: " + String(status.temperature, 2) + "°C");
        Serial.println("SQW Mode: " + String(status.sqwMode));
        
        // Connection status
        if (rtc.isConnected()) {
            Serial.println("RTC: Connected");
        } else {
            Serial.println("RTC: Disconnected!");
        }
        
        // Full detailed status
        Serial.println(rtc.getFullStatusString());
    }
    
    delay(5000);
}
```

### Blocking Operations and Synchronization
```cpp
void demonstrateWaitFunctions() {
    Serial.println("Waiting for next second...");
    if (rtc.waitForSecondChange(2000)) {
        Serial.println("Second changed! Current: " + rtc.getTimeString());
    } else {
        Serial.println("Timeout waiting for second change");
    }
    
    Serial.println("Waiting for next minute...");
    if (rtc.waitForMinuteChange(61000)) {
        Serial.println("Minute changed! Current: " + rtc.getTimeString());
    } else {
        Serial.println("Timeout waiting for minute change");
    }
}
```

### Integration with CeriaDev Ecosystem
```cpp
#include "CeriaDevOP.h"

CeriaSensorDS3231 rtc;
CeriaSerial comm;

void setup() {
    Serial.begin(115200);
    Serial1.begin(115200);
    
    rtc.begin();
    comm.begin(Serial1, 115200);
    
    // Set update interval to match communication rate
    rtc.setUpdateInterval(1000);
    
    // Setup RTC callback for time sync requests
    comm.onReceive([](const String& key, const String& value) {
        if (key == "set_time") {
            rtc.setDateTime(value);
            Serial.println("Time updated from remote: " + value);
        } else if (key == "sync_time") {
            comm.send("current_time", rtc.getDateTimeString());
            comm.send("rtc_temp", rtc.getTemperature());
        }
    });
}

void loop() {
    rtc.update();
    comm.tick();
    
    // Send time data every minute
    static uint32_t lastTimeSend = 0;
    if (rtc.hasNewData() && rtc.getSecond() == 0 && (millis() - lastTimeSend) > 60000) {
        comm.send("rtc_datetime", rtc.getDateTimeString());
        comm.send("rtc_unix", rtc.getUnixTime());
        comm.send("rtc_temperature", rtc.getTemperature());
        comm.send("rtc_day", rtc.getDayName());
        
        lastTimeSend = millis();
    }
    
    // Alert on power loss
    if (rtc.isPowerLost()) {
        comm.send("rtc_alert", "POWER_LOST");
        Serial.println("⚠️  RTC Power Lost - Time may be incorrect!");
    }
    
    delay(10);
}
```

## API Reference

### Constructor
- `CeriaSensorDS3231()` - Initialize DS3231 wrapper

### Initialization
- `bool begin(TwoWire *wire = &Wire)` - Initialize RTC with I2C interface
- `bool isConnected(uint32_t timeoutMs = 5000)` - Check RTC connectivity

### Data Reading
- `void update()` - Read and process RTC data
- `bool hasNewData()` - Check for new data since last call
- `bool isPowerLost()` - Check if RTC lost power

### Time Access
- `DS3231TimeData getTime()` - Get complete time data structure
- `DS3231Status getStatus()` - Get complete status information
- Individual getters: `getYear()`, `getMonth()`, `getDay()`, `getHour()`, `getMinute()`, `getSecond()`, `getDayOfWeek()`, `getUnixTime()`

### Temperature
- `float getTemperature()` - Get temperature in Celsius (±3°C accuracy)

### String Formatting
- `String getDateString()` - Get date in "YYYY-MM-DD" format
- `String getTimeString()` - Get time in "HH:MM:SS" format (24h)
- `String getTimeString12Hour()` - Get time in "HH:MM:SS AM/PM" format
- `String getDateTimeString()` - Get combined date and time
- `String getRTCString()` - Get formatted date/time with day name
- `String getDayName()` - Get day of week name
- `String getMonthName()` - Get month name (short form)

### Time Setting
- `bool setDateTime(uint16_t year, uint8_t month, uint8_t day, uint8_t hour, uint8_t minute, uint8_t second)` - Set specific date/time
- `bool setDateTime(uint32_t unixTime)` - Set from Unix timestamp
- `bool setDateTime(const String &dateTime)` - Set from "YYYY-MM-DD HH:MM:SS" string
- `void setToCompileTime()` - Set to sketch compilation time

### Square Wave Configuration
- `void setSqwMode(DS3231SqwMode mode)` - Set square wave output mode
- `DS3231SqwMode getSqwMode()` - Get current square wave mode

### Alarm Management
- `bool setAlarm1(uint8_t hour, uint8_t minute, uint8_t second, Ds3231Alarm1Mode mode)` - Configure alarm 1
- `bool setAlarm2(uint8_t hour, uint8_t minute, Ds3231Alarm2Mode mode)` - Configure alarm 2
- `bool isAlarmTriggered(DS3231AlarmType alarm)` - Check if alarm triggered
- `void clearAlarm(DS3231AlarmType alarm)` - Clear alarm flag

### Configuration
- `void setUpdateInterval(uint32_t intervalMs)` - Set automatic update interval
- `void enableAutoSync(bool enable)` - Enable/disable automatic updates

### Status and Diagnostics
- `String getStatusString()` - Get basic status string
- `String getFullStatusString()` - Get detailed status information
- `void resetStatistics()` - Reset internal statistics
- `uint32_t getLastSyncTime()` - Get last update timestamp

### Blocking Operations
- `bool waitForSecondChange(uint32_t timeoutMs = 2000)` - Wait for second to change
- `bool waitForMinuteChange(uint32_t timeoutMs = 61000)` - Wait for minute to change

### Utility Functions
- `bool isLeapYear(uint16_t year)` - Check if year is leap year
- `uint8_t getDaysInMonth(uint8_t month, uint16_t year)` - Get days in month
- `uint32_t calculateUnixTime(...)` - Calculate Unix timestamp
- `bool validateDateTime(...)` - Validate date/time values

## Data Structures

### DS3231TimeData
```cpp
struct DS3231TimeData {
    uint16_t year;          // 2000-2100
    uint8_t month;          // 1-12
    uint8_t day;            // 1-31
    uint8_t hour;           // 0-23
    uint8_t minute;         // 0-59
    uint8_t second;         // 0-59
    uint8_t dayOfWeek;      // 0-6 (Sunday=0)
    uint32_t unixTime;      // Unix timestamp
    String dateString;      // "YYYY-MM-DD"
    String timeString;      // "HH:MM:SS"
    String dateTimeString;  // "YYYY-MM-DD HH:MM:SS"
};
```

### DS3231Status
```cpp
struct DS3231Status {
    bool powerLost;         // Power loss detected
    bool alarmTriggered;    // Any alarm triggered
    float temperature;      // Temperature in Celsius
    bool oscillatorRunning; // Oscillator status
    DS3231SqwMode sqwMode; // Current square wave mode
};
```

### Enumerations
```cpp
enum DS3231SqwMode {
    DS3231_SQW_OFF = 0,     // Square wave disabled
    DS3231_SQW_1HZ = 1,     // 1Hz output
    DS3231_SQW_1KHZ = 2,    // 1kHz output
    DS3231_SQW_4KHZ = 3,    // 4kHz output
    DS3231_SQW_8KHZ = 4     // 8kHz output
};

enum DS3231AlarmType {
    DS3231_ALARM_1 = 1,     // Alarm 1 only
    DS3231_ALARM_2 = 2,     // Alarm 2 only
    DS3231_ALARM_BOTH = 3   // Both alarms
};
```

## Technical Specifications

### Hardware Requirements
- **Module**: DS3231 RTC module
- **Interface**: I2C (SDA, SCL)
- **Address**: 0x68 (fixed)
- **Voltage**: 3.3V or 5V compatible
- **Backup**: CR2032 battery for timekeeping during power loss

### Performance Characteristics
- **Accuracy**: ±2ppm (±1 minute per year) with temperature compensation
- **Temperature Range**: -40°C to +85°C
- **Temperature Accuracy**: ±3°C
- **Battery Life**: 8-10 years (typical CR2032)
- **Update Rate**: Configurable (default: 1Hz)

### Memory Usage
- **RAM**: ~100 bytes per instance
- **Flash**: ~4KB compiled code
- **Dependencies**: RTClib, Wire libraries

## Platform Support
- **Arduino Uno/Nano**: Full support
- **Arduino Mega**: Full support  
- **ESP32**: Full support with any I2C pins
- **ESP8266**: Full support
- **Arduino Due**: Full support

## Integration Benefits
- **CeriaDev Ecosystem**: Seamless integration with CeriaSerial for time synchronization
- **Consistent API**: Follows CeriaDev wrapper patterns
- **Production Ready**: Comprehensive error handling and status monitoring
- **Educational Friendly**: Extensive examples and clear documentation

## Summary
CeriaSensorDS3231 provides enterprise-grade RTC functionality with:
- High-precision timekeeping with temperature compensation
- Comprehensive alarm and scheduling capabilities
- Built-in temperature monitoring
- Multiple time format outputs
- Robust error handling and status monitoring
- Integration-ready design for IoT and embedded applications

Perfect for applications requiring accurate timekeeping, scheduling, data logging, and time-sensitive operations in industrial, scientific, and IoT environments.
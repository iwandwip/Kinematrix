# CeriaModuleESP32NTP

## Overview
CeriaModuleESP32NTP provides a comprehensive wrapper for ESP32 Network Time Protocol (NTP) client functionality using Arduino IDE. This module offers high-precision time synchronization, timezone management, DST handling, and robust error handling specifically designed for ESP32 platforms.

## Features

### Core NTP Functionality
- **High-Precision Synchronization**: Millisecond accuracy time synchronization via NTP
- **Multiple NTP Servers**: Primary, secondary, and tertiary server support for redundancy
- **Automatic Retry**: Configurable retry mechanism for failed synchronization attempts
- **WiFi Integration**: Seamless integration with ESP32 WiFi connectivity
- **ESP32 Optimized**: Uses ESP32's built-in SNTP client with configTime() and lwip libraries

### Advanced Time Management
- **POSIX Timezone Support**: Full POSIX timezone string support with automatic DST handling
- **24 Common Timezones**: Pre-configured timezone strings for major world regions
- **Custom Timezone Configuration**: Support for custom GMT offset and DST settings
- **Multiple Time Formats**: 12/24 hour, ISO 8601, custom strftime formatting
- **Daylight Saving Time**: Automatic DST transitions based on timezone rules

### Monitoring and Diagnostics
- **Comprehensive Status Tracking**: Sync status, WiFi connectivity, error tracking
- **Statistics Collection**: Sync count, failure count, uptime monitoring
- **Event Callbacks**: Custom callbacks for sync completion and time updates
- **Connection Health**: WiFi and NTP server connectivity monitoring

## Usage Examples

### Basic Setup
```cpp
#include "CeriaModuleESP32NTP.h"
using namespace CeriaDevOP;

CeriaModuleESP32NTP ntp;

void setup() {
    Serial.begin(115200);
    
    // Connect to WiFi first
    WiFi.begin("YOUR_SSID", "YOUR_PASSWORD");
    while (WiFi.status() != WL_CONNECTED) {
        delay(1000);
        Serial.println("Connecting to WiFi...");
    }
    
    // Initialize NTP with timezone
    if (ntp.begin("WIB-7", "pool.ntp.org")) {  // Indonesia Western Time
        Serial.println("NTP initialized successfully!");
    } else {
        Serial.println("NTP initialization failed!");
    }
    
    // Wait for time synchronization
    Serial.println("Waiting for time sync...");
    if (ntp.waitForSync(30000)) {
        Serial.println("Time synchronized!");
        Serial.println("Current time: " + ntp.getDateTimeString());
    }
}

void loop() {
    ntp.update();
    
    if (ntp.isTimeValid()) {
        Serial.println("=== Current Time ===");
        Serial.println("Date: " + ntp.getDateString());           // "2024-08-08"
        Serial.println("Time: " + ntp.getTimeString());           // "15:30:45"
        Serial.println("12h: " + ntp.getTimeString12Hour());      // "03:30:45 PM"
        Serial.println("Full: " + ntp.getNTPString());            // "2024-08-08 15:30:45 WIB"
        Serial.println("Weekday: " + ntp.getWeekdayName());       // "Thursday"
        Serial.println("Timezone: " + ntp.getTimezoneString());   // "WIB"
        Serial.println("DST: " + String(ntp.isDST() ? "Yes" : "No"));
        Serial.println("Unix: " + String(ntp.getTimestamp()));
    }
    
    delay(10000);
}
```

### Advanced Configuration
```cpp
CeriaModuleESP32NTP ntp;

void setup() {
    Serial.begin(115200);
    connectToWiFi();
    
    // Advanced NTP configuration
    NTPConfig config;
    config.primaryServer = "id.pool.ntp.org";      // Indonesian NTP pool
    config.secondaryServer = "asia.pool.ntp.org";  // Asian NTP pool
    config.tertiaryServer = "time.google.com";     // Google Time
    config.timezone = "WIB-7";                     // Indonesia Western Time
    config.syncIntervalMs = 1800000;               // Sync every 30 minutes
    config.timeoutMs = 20000;                      // 20 second timeout
    config.autoRetry = true;                       // Enable auto retry
    config.maxRetries = 5;                         // Max 5 retry attempts
    
    if (ntp.begin(config)) {
        Serial.println("Advanced NTP configuration applied!");
        
        // Set sync mode
        ntp.setSyncMode(NTP_SYNC_PERIODIC);  // Periodic sync every 30 minutes
        
        // Set custom sync interval
        ntp.setSyncInterval(900000);  // 15 minutes
        
        // Enable auto sync
        ntp.enableAutoSync(true);
        
        Serial.println("Configuration: " + ntp.getConfigString());
    }
}

void loop() {
    ntp.update();
    
    // Display detailed status every 30 seconds
    static uint32_t lastStatusUpdate = 0;
    if (millis() - lastStatusUpdate > 30000) {
        Serial.println(ntp.getFullStatusString());
        lastStatusUpdate = millis();
    }
    
    delay(1000);
}
```

### Timezone Management
```cpp
void demonstrateTimezones() {
    Serial.println("=== Timezone Examples ===");
    
    // Set timezone using POSIX string
    ntp.setTimezone("JST-9");  // Japan Standard Time
    Serial.println("Japan: " + ntp.getDateTimeString());
    
    // Germany with DST
    ntp.setTimezone("CET-1CEST,M3.5.0/02,M10.5.0/03");
    Serial.println("Germany: " + ntp.getDateTimeString());
    
    // US Eastern Time with DST
    ntp.setTimezone("EST5EDT,M3.2.0/02,M11.1.0/02");
    Serial.println("US East: " + ntp.getDateTimeString());
    
    // Indonesia Central Time
    ntp.setTimezone("WITA-8");
    Serial.println("Indonesia Central: " + ntp.getDateTimeString());
    
    // Custom timezone with GMT offset
    ntp.setTimezone(8 * 3600, 0);  // GMT+8, no DST
    Serial.println("Custom GMT+8: " + ntp.getDateTimeString());
    
    // Use country helper
    String tz = ntp.findTimezoneByCountry("singapore");
    ntp.setTimezone(tz);
    Serial.println("Singapore: " + ntp.getDateTimeString());
    
    // Browse common timezones
    Serial.println("\n=== Available Timezones ===");
    for (uint8_t i = 0; i < ntp.getCommonTimezoneCount(); i++) {
        Serial.println(String(i) + ": " + ntp.getCommonTimezone(i));
    }
}
```

### Event Callbacks and Monitoring
```cpp
CeriaModuleESP32NTP ntp;

void setup() {
    Serial.begin(115200);
    connectToWiFi();
    
    // Setup event callbacks
    ntp.onSyncComplete([](bool success) {
        if (success) {
            Serial.println("✅ NTP Sync Successful!");
            Serial.println("Time: " + ntp.getNTPString());
        } else {
            Serial.println("❌ NTP Sync Failed!");
        }
    });
    
    ntp.onTimeUpdate([](const NTPTimeInfo &timeInfo) {
        if (timeInfo.second == 0) {  // Every minute
            Serial.println("⏰ Time Update: " + String(timeInfo.dateTimeString));
        }
    });
    
    ntp.begin("WIB-7", "pool.ntp.org");
}

void loop() {
    ntp.update();
    
    // Monitor connection status
    if (!ntp.isConnected()) {
        Serial.println("⚠️  NTP connection lost!");
    }
    
    // Display status every 5 minutes
    static uint32_t lastStatus = 0;
    if (millis() - lastStatus > 300000) {
        NTPStatus status = ntp.getStatus();
        
        Serial.println("=== NTP Status Report ===");
        Serial.println("Status: " + ntp.getStatusString());
        Serial.println("Sync Count: " + String(status.syncCount));
        Serial.println("Fail Count: " + String(status.failCount));
        Serial.println("Last Sync: " + String((millis() - status.lastSyncTime) / 1000) + "s ago");
        Serial.println("Uptime: " + String(status.uptime / 1000) + "s");
        
        if (status.lastError.length() > 0) {
            Serial.println("Last Error: " + status.lastError);
        }
        
        lastStatus = millis();
    }
    
    delay(1000);
}
```

### Time Formatting and Manipulation
```cpp
void demonstrateTimeFormatting() {
    if (!ntp.isTimeValid()) return;
    
    Serial.println("=== Time Formatting Examples ===");
    
    // Basic formats
    Serial.println("ISO Date: " + ntp.getDateString());              // "2024-08-08"
    Serial.println("24h Time: " + ntp.getTimeString());              // "15:30:45"
    Serial.println("12h Time: " + ntp.getTimeString12Hour());        // "03:30:45 PM"
    Serial.println("Full DateTime: " + ntp.getDateTimeString());     // "2024-08-08 15:30:45"
    
    // Custom strftime formatting
    Serial.println("Long Date: " + ntp.getFormattedTime("%A, %B %d, %Y"));           // "Thursday, August 08, 2024"
    Serial.println("Short Date: " + ntp.getFormattedTime("%a %b %d"));               // "Thu Aug 08"
    Serial.println("Time with TZ: " + ntp.getFormattedTime("%H:%M:%S %Z"));          // "15:30:45 WIB"
    Serial.println("ISO 8601: " + ntp.getFormattedTime("%Y-%m-%dT%H:%M:%S%z"));      // "2024-08-08T15:30:45+0700"
    Serial.println("US Format: " + ntp.getFormattedTime("%m/%d/%Y %I:%M %p"));       // "08/08/2024 03:30 PM"
    Serial.println("European: " + ntp.getFormattedTime("%d.%m.%Y %H:%M"));           // "08.08.2024 15:30"
    
    // Individual components
    Serial.println("Year: " + String(ntp.getYear()));
    Serial.println("Month: " + String(ntp.getMonth()) + " (" + ntp.getMonthName() + ")");
    Serial.println("Day: " + String(ntp.getDay()));
    Serial.println("Weekday: " + ntp.getWeekdayName() + " (" + String(ntp.getWeekday()) + ")");
    Serial.println("Hour: " + String(ntp.getHour()));
    Serial.println("Minute: " + String(ntp.getMinute()));
    Serial.println("Second: " + String(ntp.getSecond()));
    Serial.println("Unix Timestamp: " + String(ntp.getTimestamp()));
}
```

### Integration with CeriaDev Ecosystem
```cpp
#include "CeriaDevOP.h"

CeriaModuleESP32NTP ntp;
CeriaSerial comm;

void setup() {
    Serial.begin(115200);
    connectToWiFi();
    
    // Initialize NTP
    ntp.begin("WIB-7", "pool.ntp.org");
    ntp.enableAutoSync(true);
    ntp.setSyncInterval(3600000);  // 1 hour sync
    
    // Initialize CeriaSerial
    Serial1.begin(115200);
    comm.begin(Serial1, 115200);
    
    // Setup NTP callbacks
    ntp.onSyncComplete([](bool success) {
        if (success) {
            comm.send("ntp_sync", "success");
            comm.send("ntp_time", ntp.getDateTimeString());
        } else {
            comm.send("ntp_sync", "failed");
        }
    });
    
    // Setup communication handlers
    comm.onReceive([](const String& key, const String& value) {
        if (key == "get_time") {
            comm.send("current_time", ntp.getDateTimeString());
            comm.send("current_unix", ntp.getTimestamp());
            comm.send("timezone", ntp.getTimezoneString());
        } else if (key == "set_timezone") {
            ntp.setTimezone(value);
            comm.send("timezone_set", value);
        } else if (key == "sync_now") {
            ntp.forceSync();
            comm.send("sync_requested", "true");
        } else if (key == "ntp_status") {
            comm.send("ntp_status", ntp.getStatusString());
        }
    });
    
    Serial.println("NTP + CeriaSerial integration ready!");
}

void loop() {
    ntp.update();
    comm.tick();
    
    // Send periodic time updates
    static uint32_t lastTimeUpdate = 0;
    if (ntp.isTimeValid() && (millis() - lastTimeUpdate > 60000)) {  // Every minute
        comm.send("time_update", ntp.getDateTimeString());
        comm.send("unix_time", ntp.getTimestamp());
        comm.send("ntp_status", ntp.isConnected() ? "connected" : "disconnected");
        
        lastTimeUpdate = millis();
    }
    
    // Send daily sync report
    static uint8_t lastHour = 255;
    if (ntp.getHour() != lastHour) {
        if (ntp.getHour() == 0) {  // Midnight
            NTPStatus status = ntp.getStatus();
            comm.send("daily_report", String(status.syncCount) + "," + String(status.failCount));
        }
        lastHour = ntp.getHour();
    }
    
    delay(100);
}
```

## API Reference

### Constructor
- `CeriaModuleESP32NTP()` - Initialize ESP32 NTP module

### Initialization
- `bool begin(const String &timezone = "UTC0", const String &ntpServer = "pool.ntp.org")` - Basic initialization
- `bool begin(const NTPConfig &config)` - Advanced initialization with full configuration

### Core Operations
- `void update()` - Update NTP status and time information (call in loop())
- `bool isInitialized()` - Check if NTP is initialized
- `bool isTimeValid()` - Check if synchronized time is valid
- `bool hasNewTime()` - Check if time data has been updated

### Synchronization Control
- `bool startSync()` - Start NTP synchronization
- `bool stopSync()` - Stop NTP synchronization  
- `void forceSync()` - Force immediate synchronization
- `bool waitForSync(uint32_t timeoutMs = 30000)` - Block until sync completes

### Configuration
- `void setTimezone(const String &posixTZ)` - Set timezone using POSIX string
- `void setTimezone(long gmtOffsetSec, int daylightOffsetSec = 0)` - Set timezone using offsets
- `void setNTPServers(const String &primary, const String &secondary = "", const String &tertiary = "")` - Configure NTP servers
- `void setSyncInterval(uint32_t intervalMs)` - Set automatic sync interval
- `void setSyncMode(NTPSyncMode mode)` - Set sync mode (on-demand, periodic, aggressive)
- `void setTimeout(uint32_t timeoutMs)` - Set sync timeout
- `void enableAutoSync(bool enable = true)` - Enable/disable automatic synchronization
- `void setMaxRetries(uint8_t retries)` - Set maximum retry attempts
- `void setAutoRetry(bool enable)` - Enable/disable automatic retry

### Data Access
- `NTPTimeInfo getTime()` - Get complete time information structure
- `NTPStatus getStatus()` - Get complete status information
- `NTPConfig getConfig()` - Get current configuration
- Individual getters: `getYear()`, `getMonth()`, `getDay()`, `getHour()`, `getMinute()`, `getSecond()`, `getWeekday()`, `isDST()`, `getTimestamp()`, `getUptime()`

### String Formatting
- `String getDateString()` - Get date in "YYYY-MM-DD" format
- `String getTimeString()` - Get time in "HH:MM:SS" format (24h)
- `String getDateTimeString()` - Get combined date and time
- `String getTimeString12Hour()` - Get time in "HH:MM:SS AM/PM" format
- `String getTimezoneString()` - Get timezone abbreviation
- `String getWeekdayName()` - Get weekday name
- `String getMonthName()` - Get month name (short)
- `String getFormattedTime(const String &format)` - Custom strftime formatting
- `String getNTPString()` - Get formatted time with timezone

### Time Setting (Manual Override)
- `void setTime(time_t timestamp)` - Set system time from Unix timestamp
- `void setTime(uint16_t year, uint8_t month, uint8_t day, uint8_t hour, uint8_t minute, uint8_t second)` - Set specific date/time
- `void setTime(const String &dateTime)` - Set from "YYYY-MM-DD HH:MM:SS" string

### Timezone Utilities
- `String getCommonTimezone(uint8_t index)` - Get pre-configured timezone by index
- `uint8_t getCommonTimezoneCount()` - Get number of pre-configured timezones
- `String findTimezoneByCountry(const String &country)` - Find timezone by country name

### Event Callbacks
- `void onSyncComplete(NTPSyncCallback callback)` - Set callback for sync completion
- `void onTimeUpdate(NTPTimeCallback callback)` - Set callback for time updates

### Status and Diagnostics
- `String getStatusString()` - Get basic status string
- `String getFullStatusString()` - Get comprehensive status information
- `String getConfigString()` - Get configuration summary
- `bool isConnected(uint32_t timeoutMs = 5000)` - Check connection status
- `void reset()` - Reset NTP module
- `void resetStatistics()` - Reset statistics counters

### Static Utility Functions
- `static String timestampToString(time_t timestamp)` - Convert timestamp to string
- `static time_t stringToTimestamp(const String &dateTime)` - Convert string to timestamp
- `static bool validateTimezone(const String &timezone)` - Validate timezone string
- `static String getCurrentNTPServer()` - Get current NTP server
- `static uint32_t getLastSyncTime()` - Get last sync timestamp

## Data Structures

### NTPTimeInfo
```cpp
struct NTPTimeInfo {
    uint16_t year;          // 2024
    uint8_t month;          // 1-12
    uint8_t day;            // 1-31
    uint8_t hour;           // 0-23
    uint8_t minute;         // 0-59
    uint8_t second;         // 0-59
    uint8_t weekday;        // 0-6 (Sunday=0)
    uint16_t yearday;       // 1-366 (day of year)
    bool isDST;             // Daylight saving time active
    long timezone;          // Timezone offset in seconds
    time_t timestamp;       // Unix timestamp
    String dateString;      // "YYYY-MM-DD"
    String timeString;      // "HH:MM:SS"
    String dateTimeString;  // "YYYY-MM-DD HH:MM:SS"
    String timezoneString;  // "WIB", "UTC", etc.
};
```

### NTPStatus
```cpp
struct NTPStatus {
    NTPSyncStatus status;   // Current synchronization status
    bool isTimeValid;       // Time data is valid and synchronized
    bool isWiFiConnected;   // WiFi connection status
    uint32_t lastSyncTime;  // Last successful sync timestamp
    uint32_t syncCount;     // Number of successful syncs
    uint32_t failCount;     // Number of failed sync attempts
    uint32_t nextSyncTime;  // Next scheduled sync time
    String lastError;       // Last error message
    uint32_t uptime;        // System uptime in milliseconds
};
```

### NTPConfig
```cpp
struct NTPConfig {
    String primaryServer;    // Primary NTP server
    String secondaryServer;  // Secondary NTP server  
    String tertiaryServer;   // Tertiary NTP server
    String timezone;         // POSIX timezone string
    long gmtOffsetSec;      // GMT offset in seconds
    int daylightOffsetSec;  // DST offset in seconds
    uint32_t syncIntervalMs;// Sync interval in milliseconds
    uint32_t timeoutMs;     // Sync timeout in milliseconds
    bool autoRetry;         // Enable automatic retry
    uint8_t maxRetries;     // Maximum retry attempts
};
```

### Enumerations
```cpp
enum NTPSyncStatus {
    NTP_NOT_INITIALIZED = 0,    // Module not initialized
    NTP_WAITING_FOR_WIFI = 1,   // Waiting for WiFi connection
    NTP_INITIALIZING = 2,       // Initializing NTP client
    NTP_SYNCING = 3,           // Synchronization in progress
    NTP_SYNCHRONIZED = 4,       // Successfully synchronized
    NTP_SYNC_FAILED = 5,       // Synchronization failed
    NTP_WIFI_DISCONNECTED = 6   // WiFi disconnected
};

enum NTPSyncMode {
    NTP_SYNC_ON_DEMAND = 0,    // Manual sync only
    NTP_SYNC_PERIODIC = 1,     // Regular periodic sync
    NTP_SYNC_AGGRESSIVE = 2    // Frequent sync for high accuracy
};
```

## Pre-configured Timezones

The module includes 24 pre-configured timezone strings for common regions:

| Index | Region | POSIX Timezone String |
|-------|--------|----------------------|
| 0 | UTC | `UTC0` |
| 1 | London | `GMT0BST,M3.5.0/01,M10.5.0/02` |
| 2 | Germany/Paris | `CET-1CEST,M3.5.0/02,M10.5.0/03` |
| 3 | Helsinki | `EET-2EEST,M3.5.0/03,M10.5.0/04` |
| 4 | Moscow | `MSK-3` |
| 5 | India | `IST-5:30` |
| 6 | Japan | `JST-9` |
| 7 | Sydney | `AEST-10AEDT,M10.1.0/02,M4.1.0/03` |
| 8 | New Zealand | `NZST-12NZDT,M9.5.0/02,M4.1.0/03` |
| 15 | Indonesia Western | `WIB-7` |
| 16 | Indonesia Central | `WITA-8` |
| 17 | Indonesia Eastern | `WIT-9` |
| 11 | US Pacific | `PST8PDT,M3.2.0/02,M11.1.0/02` |
| 12 | US Mountain | `MST7MDT,M3.2.0/02,M11.1.0/02` |
| 13 | US Central | `CST6CDT,M3.2.0/02,M11.1.0/02` |
| 14 | US Eastern | `EST5EDT,M3.2.0/02,M11.1.0/02` |

## Technical Specifications

### Platform Requirements
- **Platform**: ESP32 only (uses ESP32-specific libraries)
- **Arduino IDE**: Compatible with ESP32 Arduino Core
- **Dependencies**: WiFi library, time.h, lwip/apps/sntp.h
- **Connectivity**: Requires WiFi internet connection

### Performance Characteristics  
- **Accuracy**: Millisecond-level precision with internet NTP servers
- **Sync Interval**: Configurable from 15 seconds to hours
- **Sync Time**: Typically 1-5 seconds depending on network
- **Memory Usage**: ~200 bytes RAM, ~6KB Flash
- **Timezone Support**: Full POSIX timezone strings with DST

### Network Requirements
- **Internet Access**: Required for NTP synchronization
- **NTP Ports**: UDP port 123 (standard NTP)
- **Fallback**: Multiple server support for redundancy
- **Timeout**: Configurable timeout handling

## ESP32 Specific Features

### Integration with ESP32 SDK
- **configTime()**: Uses ESP32 native time configuration
- **SNTP Client**: Built-on ESP32 SNTP implementation
- **Time Zone**: Automatic DST handling via POSIX strings
- **System Time**: Integrates with ESP32 system clock
- **Callbacks**: ESP32 SNTP callback notifications

### Platform Optimizations
- **ESP32 Macro Protection**: `#ifdef ESP32` ensures ESP32-only compilation
- **WiFi Integration**: Seamless WiFi status monitoring
- **Memory Efficient**: Uses ESP32's optimized time structures
- **Power Management**: Respects ESP32 sleep modes

## Error Handling

### Comprehensive Error States
- Connection timeout detection
- WiFi disconnection handling  
- NTP server failure recovery
- Automatic retry mechanisms
- Detailed error reporting

### Robust Recovery
- Automatic reconnection on WiFi restore
- Server fallback on primary failure
- Configurable retry attempts
- Graceful degradation on persistent failures

## Summary

CeriaModuleESP32NTP provides enterprise-grade NTP client functionality for ESP32 with:
- **High-precision time synchronization** via internet NTP servers
- **Comprehensive timezone support** with automatic DST handling
- **24 pre-configured timezones** for global applications
- **Multiple time formats** and custom formatting options
- **Robust error handling** with automatic recovery
- **Event-driven architecture** with sync and time callbacks
- **CeriaDev ecosystem integration** for IoT applications
- **ESP32 platform optimization** using native SDK features

Perfect for IoT applications requiring accurate timekeeping, data logging with timestamps, scheduling systems, and time-synchronized operations across distributed ESP32 devices.
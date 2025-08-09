# CeriaSensorGPS

## Overview
CeriaSensorGPS adalah wrapper sederhana dan powerful untuk TinyGPSPlus library yang menyediakan interface yang clean untuk GPS/GNSS modules. Dirancang untuk kemudahan penggunaan tanpa mengorbankan fungsionalitas essential.

## Features
- **Simple API**: Methods yang intuitif dan mudah digunakan
- **Multiple Serial Support**: Auto-detection untuk Serial, Serial1, Serial2
- **Connection Management**: Built-in timeout dan status checking
- **Data Validation**: Automatic fix detection dan validity checking
- **Distance Calculation**: Built-in great-circle distance calculation
- **String Formatting**: Human-readable output formats
- **Blocking Wait**: Optional wait-for-fix functionality

## Supported NMEA Sentences
- **GPRMC**: Recommended Minimum (location, speed, course, date/time)
- **GPGGA**: Global Positioning System Fix Data (location, altitude, satellites)
- **GNRMC/GNGGA**: GLONASS variants

## Basic Usage

### Initialization
```cpp
#include "CeriaSensorGPS.h"
using namespace CeriaDevOP;

CeriaSensorGPS gps;

void setup() {
    Serial.begin(115200);
    gps.begin(Serial1, 9600);  // GPS module on Serial1
}
```

### Reading GPS Data
```cpp
void loop() {
    gps.update();
    
    if (gps.hasNewData()) {
        if (gps.hasFix()) {
            Serial.println("=== GPS Data ===");
            Serial.println("Latitude: " + String(gps.getLatitude(), 6));
            Serial.println("Longitude: " + String(gps.getLongitude(), 6));
            Serial.println("Speed: " + String(gps.getSpeed()) + " km/h");
            Serial.println("Altitude: " + String(gps.getAltitude()) + " m");
            Serial.println("Satellites: " + String(gps.getSatellites()));
            Serial.println("Date: " + gps.getDate());
            Serial.println("Time: " + gps.getTime());
            Serial.println("Location: " + gps.getLocation());
        } else {
            Serial.println("No GPS fix - Satellites: " + String(gps.getSatellites()));
        }
    }
    
    delay(1000);
}
```

## API Reference

### Initialization Methods
```cpp
CeriaSensorGPS();                                    // Constructor
void begin(Stream &serial, unsigned long baud = 9600); // Initialize GPS
```

### Core Methods
```cpp
void update();                                       // Process incoming GPS data
bool hasNewData();                                   // Check for new data since last call
bool isConnected(uint32_t timeoutMs = 5000);        // Check GPS module connection
bool hasFix();                                       // Check if GPS has valid fix (3+ satellites)
```

### Location Methods
```cpp
double getLatitude();                                // Get latitude in decimal degrees
double getLongitude();                               // Get longitude in decimal degrees
bool isLocationValid();                              // Check if location data is valid
String getLocation();                                // Get "lat,lng" string
```

### Motion Methods
```cpp
double getSpeed();                                   // Get speed in km/h
double getAltitude();                                // Get altitude in meters
uint8_t getSatellites();                            // Get number of satellites in use
```

### Date/Time Methods
```cpp
String getDate();                                    // Get date as "DD/MM/YYYY"
String getTime();                                    // Get UTC time as "HH:MM:SS"
String getLocalTime();                               // Get local time with timezone offset

void setTimezone(int8_t offsetHours);                // Set timezone offset in hours (e.g., 7 for WIB)
void setTimezone(int8_t offsetHours, int8_t offsetMinutes); // Set timezone with hours and minutes
int8_t getTimezoneHours();                           // Get current timezone hours offset
int8_t getTimezoneMinutes();                         // Get current timezone minutes offset
```

### Utility Methods
```cpp
double distanceTo(double lat, double lng);           // Calculate distance to coordinates (meters)
bool waitForFix(uint32_t timeoutMs = 30000);        // Wait for GPS fix with timeout
```

## Usage Examples

### Basic GPS Tracker
```cpp
void setup() {
    Serial.begin(115200);
    gps.begin(Serial1, 9600);
    
    Serial.println("Waiting for GPS fix...");
    if (gps.waitForFix(60000)) {  // Wait max 60 seconds
        Serial.println("GPS fix acquired!");
    } else {
        Serial.println("No GPS fix after 60 seconds");
    }
}

void loop() {
    gps.update();
    
    if (gps.hasNewData() && gps.hasFix()) {
        Serial.println(gps.getDate() + " " + gps.getTime());
        Serial.println("Position: " + gps.getLocation());
        Serial.println("Speed: " + String(gps.getSpeed(), 1) + " km/h");
    }
    
    delay(1000);
}
```

### Distance Monitoring
```cpp
// Home coordinates
const double HOME_LAT = -6.200000;
const double HOME_LNG = 106.816666;

void loop() {
    gps.update();
    
    if (gps.hasNewData() && gps.hasFix()) {
        double distance = gps.distanceTo(HOME_LAT, HOME_LNG);
        
        Serial.println("Distance from home: " + String(distance/1000.0, 2) + " km");
        
        if (distance > 1000) {  // More than 1km from home
            Serial.println("You are far from home!");
        }
    }
    
    delay(5000);
}
```

### GPS Status Monitor
```cpp
void loop() {
    gps.update();
    
    Serial.print("GPS Status: ");
    if (!gps.isConnected()) {
        Serial.println("DISCONNECTED");
    } else if (!gps.hasFix()) {
        Serial.println("NO FIX (" + String(gps.getSatellites()) + " sats)");
    } else {
        Serial.println("FIXED (" + String(gps.getSatellites()) + " sats)");
        Serial.println("  Location: " + gps.getLocation());
        Serial.println("  Speed: " + String(gps.getSpeed()) + " km/h");
        Serial.println("  Altitude: " + String(gps.getAltitude()) + " m");
    }
    
    delay(2000);
}
```

### Timezone Usage Example
```cpp
void setup() {
    Serial.begin(115200);
    gps.begin(Serial1, 9600);
    
    // Set Indonesia timezone (GMT+7)
    gps.setTimezone(7);
    
    // Or set timezone with minutes (e.g., India GMT+5:30)
    // gps.setTimezone(5, 30);
}

void loop() {
    gps.update();
    
    if (gps.hasNewData() && gps.hasFix()) {
        Serial.println("=== Time Comparison ===");
        Serial.println("UTC Time: " + gps.getTime());              // 14:30:25
        Serial.println("Local Time (WIB): " + gps.getLocalTime()); // 21:30:25
        
        Serial.println("Timezone: GMT+" + String(gps.getTimezoneHours()));
        
        // Switch timezones dynamically
        gps.setTimezone(0);     // London time
        Serial.println("London Time: " + gps.getLocalTime());
        
        gps.setTimezone(9);     // Japan time
        Serial.println("Japan Time: " + gps.getLocalTime());
        
        gps.setTimezone(7);     // Back to WIB
        Serial.println("WIB Time: " + gps.getLocalTime());
    }
    
    delay(5000);
}
```

## Hardware Setup

### Typical GPS Module Connections
```
GPS Module    →    ESP32/Arduino
VCC           →    3.3V or 5V
GND           →    GND
TX            →    RX (Serial1 pin)
RX            →    TX (Serial1 pin)
```

### Serial Port Selection
```cpp
// Different serial ports
gps.begin(Serial, 9600);     // Hardware Serial (pins 0,1)
gps.begin(Serial1, 9600);    // Hardware Serial1 (ESP32: pins 9,10)
gps.begin(Serial2, 9600);    // Hardware Serial2 (ESP32: pins 16,17)
```

## Performance Notes

### Update Frequency
- Call `update()` frequently (every loop iteration)
- GPS modules typically update at 1Hz (once per second)
- `hasNewData()` returns true only when new valid sentence is processed

### Memory Usage
- Minimal memory overhead (~50 bytes)
- Inherits from TinyGPSPlus, no data duplication
- All original TinyGPSPlus methods remain accessible

### Power Considerations
- GPS modules consume 20-60mA during operation
- Consider power management for battery applications
- Some modules support sleep/standby modes

## Troubleshooting

### No GPS Fix
1. Check antenna connection and placement
2. Ensure clear sky view (outdoor testing)
3. Wait longer for initial fix (cold start can take 30+ seconds)
4. Verify baud rate (common: 9600, 38400, 115200)

### No Data Received
1. Check serial connections (TX→RX, RX→TX)
2. Verify power supply (3.3V or 5V depending on module)
3. Test with different baud rates
4. Check if `isConnected()` returns true

### Incorrect Readings
1. Verify NMEA sentence support on your GPS module
2. Check for electromagnetic interference
3. Ensure stable power supply
4. Consider different GPS module if persistent issues

## Integration with CeriaDev

### Enable in Project
```cpp
// In your enable.h or main file
#define ENABLE_CERIA_SENSOR_GPS
#include "CeriaSensorGPS.h"
```

### Platform Compatibility
- **ESP32**: Full support, multiple hardware serials
- **ESP8266**: Software serial or single hardware serial
- **Arduino Uno**: Software serial recommended
- **Arduino Mega**: Multiple hardware serials available

## TinyGPSPlus Base Class Reference

CeriaSensorGPS inherits dari TinyGPSPlus, sehingga semua methods dan properties original tetap tersedia. Berikut dokumentasi lengkap dari base class:

### Core TinyGPSPlus Objects

#### Location Object (`gps.location`)
```cpp
bool location.isValid();                             // Check if location data is valid
bool location.isUpdated();                           // Check if location was updated since last call
uint32_t location.age();                             // Age of location data in milliseconds
double location.lat();                               // Latitude in decimal degrees
double location.lng();                               // Longitude in decimal degrees
const RawDegrees& location.rawLat();                 // Raw latitude data (degrees + billionths)
const RawDegrees& location.rawLng();                 // Raw longitude data (degrees + billionths)
```

#### Date Object (`gps.date`)
```cpp
bool date.isValid();                                 // Check if date data is valid
bool date.isUpdated();                               // Check if date was updated since last call
uint32_t date.age();                                 // Age of date data in milliseconds
uint32_t date.value();                               // Date as DDMMYY
uint16_t date.year();                                // Full year (e.g., 2025)
uint8_t date.month();                                // Month (1-12)
uint8_t date.day();                                  // Day of month (1-31)
```

#### Time Object (`gps.time`)
```cpp
bool time.isValid();                                 // Check if time data is valid
bool time.isUpdated();                               // Check if time was updated since last call
uint32_t time.age();                                 // Age of time data in milliseconds
uint32_t time.value();                               // Time as HHMMSSCC (centiseconds)
uint8_t time.hour();                                 // Hour (0-23)
uint8_t time.minute();                               // Minute (0-59)
uint8_t time.second();                               // Second (0-59)
uint8_t time.centisecond();                          // Centisecond (0-99)
```

#### Speed Object (`gps.speed`)
```cpp
bool speed.isValid();                                // Check if speed data is valid
bool speed.isUpdated();                              // Check if speed was updated since last call
uint32_t speed.age();                                // Age of speed data in milliseconds
double speed.knots();                                // Speed in knots
double speed.mph();                                  // Speed in miles per hour
double speed.mps();                                  // Speed in meters per second
double speed.kmph();                                 // Speed in kilometers per hour
```

#### Course Object (`gps.course`)
```cpp
bool course.isValid();                               // Check if course data is valid
bool course.isUpdated();                             // Check if course was updated since last call
uint32_t course.age();                               // Age of course data in milliseconds
double course.deg();                                 // Course in degrees (0-360)
```

#### Altitude Object (`gps.altitude`)
```cpp
bool altitude.isValid();                             // Check if altitude data is valid
bool altitude.isUpdated();                           // Check if altitude was updated since last call
uint32_t altitude.age();                             // Age of altitude data in milliseconds
double altitude.meters();                            // Altitude in meters
double altitude.miles();                             // Altitude in miles
double altitude.kilometers();                        // Altitude in kilometers
double altitude.feet();                              // Altitude in feet
```

#### Satellites Object (`gps.satellites`)
```cpp
bool satellites.isValid();                           // Check if satellite data is valid
bool satellites.isUpdated();                         // Check if satellite data was updated since last call
uint32_t satellites.age();                           // Age of satellite data in milliseconds
uint32_t satellites.value();                         // Number of satellites in use
```

#### HDOP Object (`gps.hdop`)
```cpp
bool hdop.isValid();                                 // Check if HDOP data is valid
bool hdop.isUpdated();                               // Check if HDOP was updated since last call
uint32_t hdop.age();                                 // Age of HDOP data in milliseconds
double hdop.hdop();                                  // Horizontal Dilution of Precision
```

### TinyGPSPlus Core Methods

#### Processing Methods
```cpp
bool encode(char c);                                 // Process one character from GPS
TinyGPSPlus& operator<<(char c);                     // Stream operator for encoding
```

#### Statistics Methods
```cpp
uint32_t charsProcessed();                           // Total characters processed
uint32_t sentencesWithFix();                         // Number of sentences with valid fix
uint32_t failedChecksum();                           // Number of sentences with failed checksum
uint32_t passedChecksum();                           // Number of sentences with valid checksum
```

#### Utility Methods
```cpp
static double distanceBetween(double lat1, double long1, double lat2, double long2);
static double courseTo(double lat1, double long1, double lat2, double long2);
static const char* cardinal(double course);          // Convert degrees to cardinal direction
static const char* libraryVersion();                 // Get library version string
```

#### Low-Level Methods
```cpp
static int32_t parseDecimal(const char* term);       // Parse decimal value from NMEA term
static void parseDegrees(const char* term, RawDegrees& deg); // Parse degrees from NMEA
```

### Advanced Usage Examples

#### Accessing All GPS Data
```cpp
void displayCompleteGPSInfo() {
    gps.update();
    
    Serial.println("=== Complete GPS Information ===");
    
    // Location
    if (gps.location.isValid()) {
        Serial.println("Location: " + String(gps.location.lat(), 6) + ", " + String(gps.location.lng(), 6));
        Serial.println("Location age: " + String(gps.location.age()) + "ms");
    }
    
    // Date & Time
    if (gps.date.isValid() && gps.time.isValid()) {
        Serial.printf("Date/Time: %02d/%02d/%04d %02d:%02d:%02d\n", 
                     gps.date.day(), gps.date.month(), gps.date.year(),
                     gps.time.hour(), gps.time.minute(), gps.time.second());
    }
    
    // Speed & Course
    if (gps.speed.isValid()) {
        Serial.println("Speed: " + String(gps.speed.kmph()) + " km/h (" + 
                      String(gps.speed.knots()) + " knots)");
    }
    
    if (gps.course.isValid()) {
        Serial.println("Course: " + String(gps.course.deg()) + "° (" + 
                      String(TinyGPSPlus::cardinal(gps.course.deg())) + ")");
    }
    
    // Altitude
    if (gps.altitude.isValid()) {
        Serial.println("Altitude: " + String(gps.altitude.meters()) + "m (" + 
                      String(gps.altitude.feet()) + " feet)");
    }
    
    // Quality indicators
    if (gps.satellites.isValid()) {
        Serial.println("Satellites: " + String(gps.satellites.value()));
    }
    
    if (gps.hdop.isValid()) {
        Serial.println("HDOP: " + String(gps.hdop.hdop()));
    }
    
    // Statistics
    Serial.println("=== Statistics ===");
    Serial.println("Characters processed: " + String(gps.charsProcessed()));
    Serial.println("Sentences with fix: " + String(gps.sentencesWithFix()));
    Serial.println("Failed checksums: " + String(gps.failedChecksum()));
    Serial.println("Passed checksums: " + String(gps.passedChecksum()));
}
```

#### Custom NMEA Field Processing
```cpp
// Include TinyGPSCustom for custom field processing
#include "TinyGPS++.h"

// Create custom field objects
TinyGPSCustom fixQuality(gps, "GPGGA", 6);    // GPS fix quality
TinyGPSCustom activeMode(gps, "GPGSA", 2);    // Active/Manual mode

void setup() {
    Serial.begin(115200);
    gps.begin(Serial1, 9600);
}

void loop() {
    gps.update();
    
    // Check for updates in custom fields
    if (fixQuality.isUpdated()) {
        Serial.println("Fix Quality: " + String(fixQuality.value()));
        // 0 = No fix, 1 = GPS fix, 2 = DGPS fix
    }
    
    if (activeMode.isUpdated()) {
        Serial.println("Mode: " + String(activeMode.value()));
        // A = Automatic, M = Manual
    }
}
```

#### Navigation Calculations
```cpp
// Waypoint navigation example
struct Waypoint {
    double lat, lng;
    String name;
};

Waypoint waypoints[] = {
    {-6.200000, 106.816666, "Jakarta"},
    {-7.797068, 110.370529, "Yogyakarta"},
    {-8.650000, 115.216667, "Denpasar"}
};

void navigationExample() {
    gps.update();
    
    if (gps.location.isValid()) {
        double currentLat = gps.location.lat();
        double currentLng = gps.location.lng();
        
        Serial.println("=== Navigation to Waypoints ===");
        
        for (int i = 0; i < 3; i++) {
            double distance = TinyGPSPlus::distanceBetween(
                currentLat, currentLng, 
                waypoints[i].lat, waypoints[i].lng);
                
            double course = TinyGPSPlus::courseTo(
                currentLat, currentLng,
                waypoints[i].lat, waypoints[i].lng);
                
            Serial.println(waypoints[i].name + ":");
            Serial.println("  Distance: " + String(distance/1000.0, 2) + " km");
            Serial.println("  Bearing: " + String(course, 1) + "° (" + 
                          String(TinyGPSPlus::cardinal(course)) + ")");
        }
    }
}
```

#### Raw Data Access
```cpp
void displayRawData() {
    gps.update();
    
    if (gps.location.isValid()) {
        // Access raw degree data for maximum precision
        const RawDegrees& rawLat = gps.location.rawLat();
        const RawDegrees& rawLng = gps.location.rawLng();
        
        Serial.println("=== Raw Location Data ===");
        Serial.println("Latitude: " + String(rawLat.deg) + "° " + 
                      String(rawLat.billionths) + " billionths");
        Serial.println("Longitude: " + String(rawLng.deg) + "° " + 
                      String(rawLng.billionths) + " billionths");
        
        // Check for negative coordinates
        if (rawLat.negative) Serial.println("Latitude is South");
        if (rawLng.negative) Serial.println("Longitude is West");
    }
}
```

### Performance Monitoring
```cpp
void performanceMonitor() {
    static uint32_t lastCheck = 0;
    static uint32_t lastChars = 0;
    static uint32_t lastSentences = 0;
    
    if (millis() - lastCheck > 10000) {  // Every 10 seconds
        uint32_t currentChars = gps.charsProcessed();
        uint32_t currentSentences = gps.sentencesWithFix();
        
        uint32_t charsPerSecond = (currentChars - lastChars) / 10;
        uint32_t sentencesPerSecond = (currentSentences - lastSentences) / 10;
        
        Serial.println("=== GPS Performance ===");
        Serial.println("Characters/sec: " + String(charsPerSecond));
        Serial.println("Sentences/sec: " + String(sentencesPerSecond));
        Serial.println("Error rate: " + String(gps.failedChecksum() * 100.0 / gps.passedChecksum()) + "%");
        
        lastCheck = millis();
        lastChars = currentChars;
        lastSentences = currentSentences;
    }
}
```

CeriaSensorGPS provides clean, simple access to GPS functionality while maintaining all the power and flexibility of the underlying TinyGPSPlus library. You can mix and match wrapper methods dengan original TinyGPSPlus methods sesuai kebutuhan aplikasi Anda.
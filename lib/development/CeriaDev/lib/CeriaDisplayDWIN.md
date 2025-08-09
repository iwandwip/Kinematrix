# CeriaDisplayDWIN

## Overview

CeriaDisplayDWIN adalah wrapper lengkap untuk DWIN DGUS T5L HMI displays yang menyediakan API sederhana namun powerful untuk komunikasi dengan display touchscreen DWIN. Wrapper ini mengimplementasikan protokol komunikasi UART lengkap dengan fitur Variable Pointer (VP) management, event handling, dan integrasi seamless dengan ekosistem CeriaDev.

## Features

### Core Communication Features
- **T5L ASIC Protocol** - Implementasi lengkap protokol DWIN T5L DGUS
- **UART Communication** - Serial communication dengan frame validation
- **Variable Pointer Management** - Sistem VP untuk data exchange
- **Event-driven Architecture** - Real-time touch dan page change events
- **Error Handling** - Comprehensive error detection dan recovery
- **Connection Monitoring** - Auto-detection koneksi display dengan heartbeat

### Display Control Features  
- **Page Management** - Switch antar pages dengan callback support
- **Brightness Control** - Kontrol brightness display (0-100%)
- **Volume Control** - Audio volume management
- **Touch Event Processing** - Multi-touch support dengan coordinate tracking
- **Text Display** - Unicode text rendering support
- **Number Display** - Integer, float, dan formatted number display
- **Icon Management** - Dynamic icon switching
- **Progress Bars** - Percentage-based progress indicators

### Advanced Features
- **Multiple Data Types** - Support uint8/16/32, float, string, binary
- **Auto Upload Variables** - Automatic variable synchronization
- **RTC Support** - Real-time clock management
- **Sound Control** - Built-in sound playback
- **File Operations** - SD card file upload/download
- **Statistics Tracking** - Frame counting dan error monitoring
- **Debug Support** - Comprehensive debug output

## Basic Usage

### Hardware Setup
```
ESP32/Arduino    DWIN Display
GND         <--> GND
5V          <--> VCC
GPIO16(RX)  <--> TX
GPIO17(TX)  <--> RX
```

### Simple Initialization
```cpp
#include "CeriaDisplayDWIN.h"
using namespace CeriaDevOP;

CeriaDisplayDWIN display;

void setup() {
    Serial.begin(115200);
    Serial2.begin(115200);
    
    // Initialize display
    if (display.begin(Serial2, 115200)) {
        Serial.println("Display connected successfully");
    } else {
        Serial.println("Display connection failed");
    }
    
    // Set initial page
    display.setPage(0);
    display.setBrightness(80);
}

void loop() {
    display.update();
    
    // Check for touch events
    if (display.hasNewTouch()) {
        uint16_t vp = display.getTouchVP();
        Serial.println("Touched VP: 0x" + String(vp, HEX));
    }
    
    delay(10);
}
```

## API Reference

### Core Methods

#### Initialization
```cpp
bool begin(Stream &serial, uint32_t baud = 115200);
void update();                                       // Process incoming data
bool hasNewData();                                   // Check for new variable updates
bool isConnected(uint32_t timeoutMs = 5000);       // Check connection status
void setTimeout(uint32_t ms);                       // Set communication timeout
```

#### Display Control
```cpp
bool setPage(uint8_t page);                         // Switch to specific page
uint8_t getCurrentPage();                           // Get current page number
uint8_t getPreviousPage();                          // Get previous page number
bool setBrightness(uint8_t level);                  // Set brightness (0-100%)
uint8_t getBrightness();                           // Get current brightness
bool setVolume(uint8_t level);                      // Set volume (0-100%)
uint8_t getVolume();                               // Get current volume
```

#### Variable Pointer (VP) Operations
```cpp
// Write operations
bool writeVP(uint16_t vp, uint8_t value);
bool writeVP(uint16_t vp, uint16_t value);
bool writeVP(uint16_t vp, uint32_t value);
bool writeVP(uint16_t vp, int value);
bool writeVP(uint16_t vp, long value);
bool writeVP(uint16_t vp, float value);
bool writeVP(uint16_t vp, const String &value);
bool writeVP(uint16_t vp, const char *value);

// Read operations
bool readVP(uint16_t vp, DWINDataType type);
uint8_t getVPUint8(uint16_t vp);
uint16_t getVPUint16(uint16_t vp);
uint32_t getVPUint32(uint16_t vp);
float getVPFloat(uint16_t vp);
String getVPString(uint16_t vp);
```

#### Event Handling
```cpp
void setTouchCallback(DWINTouchCallback callback);
void setPageCallback(DWINPageCallback callback);
void setErrorCallback(DWINErrorCallback callback);
void setVariableCallback(DWINVariableCallback callback);

bool hasNewTouch();                                 // Check for touch events
bool hasNewPage();                                  // Check for page changes
DWINTouchData getLastTouch();                      // Get touch data
uint16_t getTouchVP();                             // Get touched VP address
uint16_t getTouchX();                              // Get touch X coordinate
uint16_t getTouchY();                              // Get touch Y coordinate
```

### Display Helper Methods

#### Text and Numbers
```cpp
bool setText(uint16_t vp, const String &text);     // Display text at VP
bool setNumber(uint16_t vp, int32_t number);       // Display number at VP
bool setFloat(uint16_t vp, float number, uint8_t decimals = 2); // Display float
bool setIcon(uint16_t vp, uint16_t iconId);        // Set icon at VP
bool setProgress(uint16_t vp, uint8_t percentage);  // Set progress bar
```

#### Variable Management
```cpp
bool registerVP(uint16_t vp, DWINDataType type, bool autoUpload = false);
bool unregisterVP(uint16_t vp);
bool isVPRegistered(uint16_t vp);
bool isVPUpdated(uint16_t vp);
uint32_t getVPLastUpdate(uint16_t vp);
bool enableAutoUpload(uint16_t vp);
bool disableAutoUpload(uint16_t vp);
```

### Advanced Features

#### RTC and Sound
```cpp
bool setRTC(uint16_t year, uint8_t month, uint8_t day, 
           uint8_t hour, uint8_t minute, uint8_t second);
bool getRTC();
bool playSound(uint8_t soundId);
```

#### File Operations
```cpp
bool uploadFile(const String &filename, uint8_t *data, uint32_t length);
bool downloadFile(const String &filename, uint8_t *buffer, uint32_t &length);
```

#### Utility Methods
```cpp
bool waitForConnection(uint32_t timeoutMs = 10000);
bool waitForTouch(uint32_t timeoutMs = 5000);
bool waitForVPUpdate(uint16_t vp, uint32_t timeoutMs = 3000);
bool waitForPageChange(uint32_t timeoutMs = 3000);

void getStats(uint32_t &sent, uint32_t &received, uint32_t &errors);
void resetStats();
String getDisplayString();
String getConnectionString();
String getDebugString();

void reset();                                       // Reset internal state
void factoryReset();                               // Factory reset display
```

## Usage Examples

### Example 1: Basic Display Control
```cpp
void displaySensorData() {
    display.setPage(1);                            // Switch to sensor page
    display.setText(0x1000, "Temperature");       // Label at VP 0x1000
    display.setFloat(0x1001, 25.6, 1);           // Value at VP 0x1001
    display.setText(0x1002, "°C");                // Unit at VP 0x1002
}
```

### Example 2: Touch Event Handling
```cpp
void setup() {
    display.begin(Serial2);
    display.setTouchCallback([](uint16_t vp, uint16_t value, DWINTouchEvent event) {
        Serial.println("Touch VP: 0x" + String(vp, HEX) + " Value: " + String(value));
        
        switch (vp) {
            case 0x2000: // Home button
                display.setPage(0);
                break;
            case 0x2001: // Settings button  
                display.setPage(2);
                break;
        }
    });
}

void loop() {
    display.update();
    
    if (display.hasNewTouch()) {
        DWINTouchData touch = display.getLastTouch();
        Serial.println("Touched at (" + String(touch.x) + "," + String(touch.y) + ")");
    }
}
```

### Example 3: Variable Management
```cpp
void setupVariables() {
    // Register variables with auto-upload
    display.registerVP(0x1000, DWIN_TYPE_FLOAT, true);    // Temperature sensor
    display.registerVP(0x1001, DWIN_TYPE_UINT16, true);   // Humidity sensor
    display.registerVP(0x1002, DWIN_TYPE_STRING, false);   // Status message
    
    display.setVariableCallback([](uint16_t vp, DWINDataType type, uint32_t value) {
        Serial.println("VP 0x" + String(vp, HEX) + " updated: " + String(value));
    });
}

void updateSensors() {
    if (display.isVPUpdated(0x1000)) {
        float temp = display.getVPFloat(0x1000);
        Serial.println("New temperature: " + String(temp, 2) + "°C");
    }
    
    if (display.isVPUpdated(0x1001)) {
        uint16_t humidity = display.getVPUint16(0x1001);
        Serial.println("New humidity: " + String(humidity) + "%");
    }
}
```

### Example 4: Page Management
```cpp
void setupPageCallbacks() {
    display.setPageCallback([](uint8_t newPage, uint8_t oldPage) {
        Serial.println("Page changed from " + String(oldPage) + " to " + String(newPage));
        
        switch (newPage) {
            case 0: // Home page
                initializeHomePage();
                break;
            case 1: // Sensor page
                initializeSensorPage();
                break;
            case 2: // Settings page
                initializeSettingsPage();
                break;
        }
    });
}

void initializeHomePage() {
    display.setText(0x0000, "Smart Home Controller");
    display.setText(0x0001, "Status: Online");
    display.setIcon(0x0002, 1); // Home icon
}
```

### Example 5: Error Handling
```cpp
void setupErrorHandling() {
    display.setErrorCallback([](DWINError error) {
        switch (error) {
            case DWIN_ERR_TIMEOUT:
                Serial.println("Display communication timeout");
                break;
            case DWIN_ERR_NO_CONNECTION:
                Serial.println("Display not connected");
                break;
            case DWIN_ERR_INVALID_FRAME:
                Serial.println("Invalid frame received");
                break;
            case DWIN_ERR_BUFFER_FULL:
                Serial.println("Buffer overflow");
                break;
        }
    });
}
```

### Example 6: Real-time Data Display
```cpp
void displayRealTimeData() {
    static unsigned long lastUpdate = 0;
    
    if (millis() - lastUpdate > 1000) { // Update every second
        // Update sensor readings
        display.setFloat(0x1000, readTemperature(), 1);
        display.setNumber(0x1001, readHumidity());
        display.setProgress(0x1002, calculateBatteryLevel());
        
        // Update status text
        if (display.isConnected()) {
            display.setText(0x1003, "Connected");
        } else {
            display.setText(0x1003, "Disconnected");
        }
        
        lastUpdate = millis();
    }
}
```

### Example 7: Integration with CeriaDev Ecosystem
```cpp
#include "CeriaSerial.h"
#include "CeriaDisplayDWIN.h"
using namespace CeriaDevOP;

CeriaSerial comm;
CeriaDisplayDWIN display;

void setup() {
    Serial.begin(115200);
    Serial1.begin(115200); // CeriaSerial
    Serial2.begin(115200); // Display
    
    comm.begin(Serial1);
    display.begin(Serial2);
    
    // Forward display events to serial
    display.setTouchCallback([](uint16_t vp, uint16_t value, DWINTouchEvent event) {
        comm.send("touch_vp", String(vp, HEX));
        comm.send("touch_value", value);
    });
    
    // Handle incoming serial data
    comm.onReceive([](const String& key, const String& value) {
        if (key == "temperature") {
            display.setFloat(0x1000, value.toFloat(), 1);
        } else if (key == "humidity") {
            display.setNumber(0x1001, value.toInt());
        } else if (key == "page") {
            display.setPage(value.toInt());
        }
    });
}

void loop() {
    comm.tick();
    display.update();
    delay(10);
}
```

## Hardware Setup

### Wiring Diagram
```
ESP32 DevKit v1          DWIN T5L Display
================         ================
3V3       -------------- 3.3V (Optional - for level shifting)
5V        -------------- VCC
GND       -------------- GND
GPIO16(RX2) ------------ TX
GPIO17(TX2) ------------ RX
```

### Platform-Specific Configuration

#### ESP32
```cpp
// Use Serial2 with custom pins
Serial2.begin(115200, SERIAL_8N1, 16, 17);
display.begin(Serial2, 115200);
```

#### ESP8266
```cpp
// Use SoftwareSerial atau Serial
#include <SoftwareSerial.h>
SoftwareSerial dwinSerial(D6, D7);
dwinSerial.begin(115200);
display.begin(dwinSerial, 115200);
```

#### Arduino Uno/Nano
```cpp
// Use SoftwareSerial
#include <SoftwareSerial.h>
SoftwareSerial dwinSerial(2, 3);
dwinSerial.begin(9600); // Lower baud rate for stability
display.begin(dwinSerial, 9600);
```

## Performance Notes

### Memory Usage
- **RAM Usage**: ~2KB untuk basic operations
- **Flash Usage**: ~15KB library code
- **Variable Storage**: 32 VP maximum dengan dynamic allocation

### Communication Performance
- **Baud Rate**: 115200 optimal untuk ESP32/ESP8266, 9600 untuk Arduino Uno
- **Frame Latency**: <10ms typical response time
- **Update Rate**: Recommended 10ms loop interval

### Optimization Tips
```cpp
// Batch variable updates
display.writeVP(0x1000, temperature);
display.writeVP(0x1001, humidity);
display.writeVP(0x1002, pressure);

// Use registered VPs untuk frequent access
display.registerVP(0x1000, DWIN_TYPE_FLOAT, true);

// Limit string updates untuk performance
if (statusChanged) {
    display.setText(0x2000, newStatus);
}
```

## Troubleshooting

### Common Issues

#### Display Not Responding
- Check wiring connections (TX/RX tidak terbalik)
- Verify baud rate matches (115200 default)
- Ensure adequate power supply (5V, min 500mA)
- Check ground connection

#### Touch Events Not Working
- Verify touch is enabled di DGUS project
- Check VP addresses match DGUS configuration
- Ensure touch callback is registered sebelum begin()

#### Variable Updates Failing
- Verify VP addresses exist di DGUS project
- Check data type compatibility
- Ensure VP is not read-only di DGUS

#### Communication Errors
```cpp
display.setErrorCallback([](DWINError error) {
    switch (error) {
        case DWIN_ERR_TIMEOUT:
            // Increase timeout atau check connections
            display.setTimeout(5000);
            break;
        case DWIN_ERR_INVALID_FRAME:
            // Check for electromagnetic interference
            break;
    }
});
```

### Debug Output
```cpp
void printDebugInfo() {
    Serial.println(display.getDebugString());
    Serial.println(display.getConnectionString());
    
    uint32_t sent, received, errors;
    display.getStats(sent, received, errors);
    Serial.println("Stats - S:" + String(sent) + " R:" + String(received) + " E:" + String(errors));
}
```

## Integration dengan CeriaDev

### Module Enable
Tambahkan ke `lib/enable.h`:
```cpp
#define ENABLE_CERIA_DISPLAY_DWIN
```

### Orchestration Integration
Tambahkan ke `KinematrixModules.h`:
```cpp
#ifdef ENABLE_CERIA_DISPLAY_DWIN
#include "../lib/development/CeriaDev/lib/CeriaDisplayDWIN.h"
#include "../lib/development/CeriaDev/lib/CeriaDisplayDWIN.cpp"
#endif
```

### Example Integration
Lihat `main.ino` untuk complete integration example dengan CeriaSerial, CeriaSensorGPS, dan CeriaSensorINA219.

## Advanced Configuration

### Custom Frame Timeout
```cpp
#define DWIN_FRAME_TIMEOUT 200  // Custom timeout di header file
```

### Buffer Size Optimization
```cpp
// Untuk memory-constrained devices
#define DWIN_MAX_VARIABLES 16   // Reduce dari 32 default
```

### Platform-Specific Optimizations
File sudah include platform detection untuk ESP32, ESP8266, dan Arduino dengan appropriate defaults.

## Conclusion

CeriaDisplayDWIN menyediakan comprehensive solution untuk DWIN T5L HMI display integration dengan Arduino/ESP32. Dengan API yang clean, error handling yang robust, dan integration seamless dengan CeriaDev ecosystem, wrapper ini memungkinkan rapid development dari advanced HMI applications.

Key benefits:
1. **Simple API** - Easy-to-use methods untuk common operations
2. **Complete Protocol** - Full T5L DGUS protocol implementation  
3. **Event-driven** - Real-time touch dan page change handling
4. **Error Resilient** - Comprehensive error detection dan recovery
5. **CeriaDev Ready** - Seamless integration dengan CeriaDev modules

Untuk advanced features dan customization, akses full T5L ASIC capabilities melalui direct VP operations dan custom command sending.